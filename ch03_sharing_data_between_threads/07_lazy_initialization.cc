#include "../include/common.h"

// 为了并发访问，有时共享数据仅需在初始化过程中受到保护，之后再也无须进行显式的
// 同步操作。这可能是因为共享数据一旦创建就处于只读状态，原本可能发生的同步问题
// 遂不复存在；也可能是因为后续操作已为共享数据施加了必要的隐式保护

// 假定共享数据本身就能安全地被并发访问，则仅有初始化过程需要保护

shared_ptr<string> resource_ptr;


mutex resource_mutex;
void lazy_init_mutex() {
    // 每个线程都必须在 mutex 上轮候，等待检查数据是否已经完成初始化
    unique_lock<mutex> lk(resource_mutex);
    if (!resource_ptr) {
        resource_ptr.reset(new string);
    }
    lk.unlock();

    resource_ptr->append("mutex ");
    cout << __func__ << ": " << *resource_ptr << endl;
}


// 不能使用 double checked locking，因为它有可能诱发恶性条件竞争
// https://stackoverflow.com/questions/8570903/explain-race-condition-in-double-checked-locking

// 在这种情况下（取决于.reset和!的实现），当线程 1 在初始化 resource_ptr 
// 的过程中执行到一半，然后被暂停/切换时，可能会出现问题。此时线程 2 走过来，
// 执行第一次检查，看到指针不是空的，并跳过锁/完全初始化的检查。然后，它使用
// 了部分初始化的对象（可能导致坏事发生）。然后线程 1 回来并完成了初始化，
// 但这已经太晚了。

// 部分初始化的 resource_ptr 是可能的，因为 CPU 被允许重新排序指令（只要
// 它不改变单线程的行为）。因此，虽然代码看起来应该完全初始化对象，然后将其
// 分配给resource_ptr，但优化后的汇编代码可能会做一些完全不同的事情，而且
// CPU 也不能保证按照二进制文件中指定的顺序运行汇编指令!

// 当涉及多个线程时，内存围栏（锁）是保证事情按正确顺序发生的唯一方法。
void undefind_behaviour_with_double_checked_locking() {
    if (!resource_ptr) {
        lock_guard<mutex> lk(resource_mutex);
        if (!resource_ptr) {
            resource_ptr.reset(new string);
        }
    }

    resource_ptr->append("double_checked ");
    cout << __func__ << ": " << *resource_ptr << endl;
}


// 也可以使用 call_once 解决上述问题
// 相比显式使用 mutex，std::call_once() 函数的额外开销往往更低，
// 特别是在初始化已经完成的情况下
// 如果在 class 的成员函数中调用 call_once，还需要传入 this 指针作为参数
// 例如：std::call_once(connection_init_flag,&X::open_connection,this);
once_flag resource_flag;
void init_resource() {
    resource_ptr.reset(new string);
}

void lazy_init_call_once() {
    call_once(resource_flag, init_resource);

    resource_ptr->append("call_once ");
    cout << __func__ << ": " << *resource_ptr << endl;    
}


// Mayer's Singleton
class SingletonClass {
public:
    // C++11 规定初始化只会在某一线程上单独发生，在初始化完成之前，
    // 其他线程不会越过静态数据的声明而继续运行
    static SingletonClass& Instance() {
        static SingletonClass instance;
        return instance;
    }
    

private:
    SingletonClass()= default;
    ~SingletonClass()= default;
    SingletonClass(const SingletonClass&)= delete;
    SingletonClass& operator=(const SingletonClass&)= delete;
};

int main() {
    lazy_init_mutex();

    lazy_init_call_once();
    lazy_init_mutex();
}
