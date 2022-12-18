#include <iostream>
#include <thread>
using namespace std;

class thread_guard {
    thread &t;
public:
    explicit thread_guard(thread &t_) : t(t_) {}
    
    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }

    // 禁止拷贝复制操作，防止多次调用析构函数
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

void hello() {
    std::cout << "Hello" << std::endl;
}

int main() {
    // 调用 join() 会清空线程关联的所有存储空间
    // 对于一个给定的线程，不能调用两次 join()

    // 如果主线程在启动新线程之后有异常抛出，而 join() 尚未执行，则该 join() 调用会被略过
    // 因此出现异常后同样需要调用 join()

    // 可以使用 try/catch 块，在 catch 中调用 join()，但这要求全部可能的退出路径都必须保证这种先后次序
    // 因此可以使用 RAII 方法
    thread t(hello);
    // g 的析构先于 t
    thread_guard g(t);

    // 若不需要等待线程结束，我们可以将其分离，从而防止异常引发的安全问题
    // detach 操作会切断线程和 thread 对象间的关联
    // 这样，thread 对象在销毁时肯定不调用 terminate()，即便线程还在后台运行，也不会调用

    // 调用 detach() 会令线程在后台运行，遂无法与之直接通信
    // 然而分离的线程确实仍在后台运行，其归属权和控制权都转移给 C++运行时库，
    // 由此保证，一旦线程退出，与之关联的资源都会被正确回收。
    thread t2(hello);
    t2.detach();
    cout << t2.joinable() << endl;

    // 只有当 t.joinable()返回 true 时，才能调用 t.detach()
}
