#include "../include/common.h"
#include "hierarchical_mutex.h"

// hierarchical_mutex 之间不可能发生死锁，
// 因为 mutex 自身已经被强制限定了加锁次序
hierarchical_mutex high_level_mutex(10000);
hierarchical_mutex low_level_mutex(5000);
hierarchical_mutex mid_level_mutex(7000);


int low_level_stuff() {
    cout << "low level stuff" << endl;
    return 3;
}

int low_level_func() {
    // 用户自定义的类型也能与 lock_guard 结合使用，
    // 因其实现了 3 个成员函数: lock()、unlock() 和 try_lock()
    lock_guard<hierarchical_mutex> lk(low_level_mutex);
    return low_level_stuff(); 
}


void high_level_stuff(int param) {
    cout << "high level stuff, get param: " << param << endl;
}

void high_level_func() {
    lock_guard<hierarchical_mutex> lk(high_level_mutex);
    high_level_stuff(low_level_func());
}


void thread_a() {
    try {
        high_level_func();
    } catch(const std::exception& e) {
        std::cerr << "exception in thread a: " << e.what() << '\n';
    } 
}



void mid_level_stuff() {
    cout << "mid level stuff" << endl;
    high_level_func();
}

void mid_level_func() {
    lock_guard<hierarchical_mutex> lk(mid_level_mutex);
    mid_level_stuff();
}

void thread_b() {
    try {
        mid_level_func();
    } catch(const std::exception& e) {
        std::cerr << "exception in thread b: " << e.what() << '\n';
    } 
}

int main() {
    // 为了避免死锁，创建线程的函数也要对线程执行 join
    thread t1(thread_a);
    t1.join();


    thread t2(thread_b);
    t2.join();

    
}