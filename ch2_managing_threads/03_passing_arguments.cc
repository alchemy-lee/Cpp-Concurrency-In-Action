#include <iostream>
#include <thread>
using namespace std;

void f(int i, const string &s) {
    cout << i << " size of s: " << s.size() << endl;
}

void oops(int p) {
    char buffer[1024];
    for (int i = 0; i < p; ++i) {
        buffer[i] = 'a';
    }
    thread t(f, 3, buffer);
    t.detach();
}

void not_oops(int p) {
    char buffer[1024];
    for (int i = 0; i < p; ++i) {
        buffer[i] = 'a';
    }
    thread t(f, 3, string(buffer));
    t.detach();
}

void g(int i, string &s) {
    cout << i << " size of s: " << s.size() << endl;
}

// void oops_again() {
//     string s = "hello";
//     thread t(g, 3, s);
//     t.join();
// }

void not_oops_again() {
    string s = "hello";
    thread t(g, 3, ref(s));
    t.join();
}

class X {
public:
    void do_work() {
        cout << "do_work in X" << endl;
    }

    void do_work_with_para(int i) {
        cout << "do_work_with_para in X, " << i << endl;
    }
};

void process_big_object(std::unique_ptr<string> p) {
    cout << *p << endl;
}

int main() {
    // 线程具有内部存储空间，参数会按照默认方式先复制到该处，新创建的执行线程才能直接访问它们
    // 然后，这些副本被当成临时变量，以右值形式传给新线程上的函数或可调用对象
    // 即便函数的相关参数按设想应该是引用，上述过程依然会发生
    thread t(f, 3, "hello");
    t.join();

    // 如果传给 f 的参数是指针类型，这一过程会产生重大影响
    // thread 的构造函数原样复制所提供的值，在新线程调用 f 时才会将 buffer 转换成 string 类型
    // 而此时函数 oops 可能已经退出，导致局部数组被销毁
    // 在 m1 pro mbp 上进行试验，最终 f 只输出了大约 720 个字符 'a'，而不是 1023 个
    oops(1023);
    this_thread::sleep_for(chrono::milliseconds(10));

    // 使用显式转换避免悬空指针
    not_oops(1023);
    this_thread::sleep_for(chrono::milliseconds(10));

    // 为了应对 move-only 类型，线程库的内部代码会把参数副本（位于新线程的内部存储空间）以右值的形式传递
    // 此时如果参数是非常量引用，直接传递会报错，因为不能将右值赋值给非常量引用
    // oops_again();
    // 需要使用 std::ref 进行包装
    not_oops_again();

    X x;
    // 若要将某个类的成员函数设定为线程函数，需要传入一个指向该成员函数的指针作为 thread 的第一个参数
    // 然后传入对象的指针作为第二个参数，下面的代码调用 x.do_work()
    thread t2(&X::do_work, &x);
    t2.join();

    // 还能为成员函数提供参数：若给 thread 增添第 3 个参数，则它会传入成员函数作为第 1 个参数
    thread t3(&X::do_work_with_para, &x, 3);
    t3.join();

    // 如果参数类型只能被移动，不能被复制(例如 unique_ptr)，向 thread 传递参数时要使用 std::move
    unique_ptr<string> p(new string("hello"));
    thread t4(process_big_object, std::move(p));
    t4.join();
}
