#include <iostream>
#include <thread>

using namespace std;

void hello() {
    cout << "hello" << endl;
}


thread::id master_id;


void f() {
    if (this_thread::get_id() == master_id) {
        cout << "This is master thread." << endl;
    } else {
        cout << "This is slave thread." << endl;
    }
}


int main() {
    // 每个线程都内建了唯一的线程 ID，它有两种获取方法
    //    在与线程关联的 thread 对象上调用成员函数 get_id()
    thread t1(hello);
    auto id1 = t1.get_id();
    cout << id1 << endl;
    t1.join();

    //    当前线程的 ID 可以通过调用 this_thread::get_id() 获得
    auto id2 = this_thread::get_id();
    cout << id2 << endl;
    
    // 如果 thread 对象没有关联任何执行线程，返回的 id 对象按默认构造方式生成，表示“线程不存在”
    thread t3;
    auto id3 = t3.get_id();
    cout << id3 << endl;

    // id 类型支持完整的比较运算操作，比较运算符就所有不相等的值确立了全序（total order）关系
    cout << (id1 == id2) << endl;
    cout << (id1 == id1) << endl; 

    // 标准库支持 std::hash<std::thread::id>，因此 id 的值也可以用作无序关联容器的键值


    // thread::id 实例常用于识别线程，以判断它是否需要执行某项操作
    master_id = this_thread::get_id();
    thread t4(f);
    f();

    t4.join();
}