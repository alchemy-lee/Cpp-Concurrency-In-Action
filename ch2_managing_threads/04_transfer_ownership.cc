#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>

using namespace std;


void some_function() {
    cout << "some_function" << endl;
}

void some_other_function(int i) {
    cout << "some_other_function: " << i << endl; 
}

// 从函数内部返回 thread 对象
thread f() {
    return thread(some_function);
}

thread g() {
    thread t(some_other_function, 5);
    return t;
}

// 将 thread 对象传递给函数
void h(thread t) {
    if (t.joinable()) {
        t.join();
    }
}

struct func {     
    int& i;
    func(int& i_):i(i_){}
    void operator()() {
        for(unsigned j=0;j<1000000;++j) {
            // 可能访问悬空引用
            ++i;
            if (i % 10000 == 0) {
                std::cout << i << std::endl;
            }
        }
    }
};

// 在离开其对象所在的作用域前，确保线程已经完结
class scoped_thread {
    thread t;

public:
    explicit scoped_thread(thread t_) : t(move(t_)) {
        if (!t.joinable()) {
            throw logic_error("No thread");
        }
    }

    ~scoped_thread() {
        t.join();
    }

    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator=(scoped_thread const&) = delete;

};

int main() {
    // thread 类的每个实例都负责管控一个执行线程。因为 thread 类的实例能够移动（movable）却
    //   不能复制（not copyable），故此线程的归属权可以在实例之间转移
    // 对于任一特定的执行线程，任何时候都只有唯一的 thread 对象与之关联

    thread t1(some_function);
    thread t2 = move(t1);
    t1 = thread(some_other_function, 3);
    thread t3;
    t3 = move(t2);
    // t1 = move(t3);   // 该赋值会使程序终止，因为 t1 已经管控了一个执行线程
    t1.join();
    t3.join();

    // 从函数内部返回 thread 对象 
    thread t4 = f();
    t4.join();
    thread t5 = g();
    t5.join();

    // 将 thread 对象按右值传递给函数
    h(thread(some_function));
    thread t6(some_other_function, 7);
    h(move(t6));


    // 在离开其对象所在的作用域前，确保线程已经完结
    int a = 0;
    scoped_thread t7{thread(func(a))};

}