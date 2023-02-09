#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello" << std::endl;
}

class background_task {
public:
    void operator()() const {
        std::cout << "background_task" << std::endl;
    }
};

struct func {     
    int& i;
    func(int& i_):i(i_){}
    void operator()() {
        for(unsigned j=0;j<1000000;++j) {
            // 可能访问悬空引用
            ++i;
            if (i % 1000 == 0) {
                std::cout << i << std::endl;
            }
        }
    }
};

void oops() {
    int a = 0;
    func f(a);
    std::thread t(f);
    t.detach();
    // 线程 t 可能还在运行
}

int main() {
    // start a thread
    std::thread t(hello);
    t.join();

    // pass an instance of a class with a function call operator
    background_task f;
    std::thread t2(f);
    t2.join();

    // 声明了一个函数，而不是创建了一个 thread
    // std::thread t3(background_task());

    // 使用 uniform initialization 语法声明变量
    std::thread t4{background_task()};
    t4.join();

    // lambda 表达式
    std::thread t5([] {
        std::cout << "lambda" << std::endl;
    });
    t5.join();
    
    // 需要在线程对象销毁之前做出连接或分离的决定，此时线程本身可能已经结束了
    // 如果线程对象销毁之前还没有做出决定，线程就会终止(thread 的析构函数会调用 terminate())

    // 分离后的线程可能在线程对象销毁之后还在继续运行，它只有在线程函数返回之后才停止运行
    // 如果不等待线程结束，则需要确保线程访问的数据直到线程结束之前都是有效的
    oops();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // 解决方案：令线程函数自包含，将数据复制到新线程内部
}