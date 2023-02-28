#include "../include/common.h"

list<int> l;
mutex m;

// lock_guard
void add_to_list(vector<int> &v) {
    lock_guard<mutex> guard(m);
    for (auto i : v) {
        l.push_back(i);
        this_thread::sleep_for(chrono::milliseconds(200));
    }
    PRINT_LIST_ELEMENTS(l, "global list: ");
}

void list_contains(vector<int> &v) {
    lock_guard<mutex> guard(m);
    for (auto i : v) {
        bool ex = find(l.begin(), l.end(), i) != l.end();
        string s = ex ? "true" : "false";
        cout << "existence of " << i << ": " << s << endl;
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

int main() {
    // How to avoid problematic race conditions:
    //   1. lock-based programming, wrap data structure with a protection
    //      mechanism
    //   2. lock-free programming
    //   3. software transactional memory, handle the updates to the data 
    //      structure as a transaction


    // use std::mutex 调用成员函数 lock() 对其加锁，调用 unlock() 解锁
    // 不推荐直接调用成员函数的做法，需要在退出函数的每条代码路径上都调用 unlock()，
    //   包括由于异常导致退出的路径
    // 标准库提供了类模板 std::lock_guard<>，针对互斥类融合实现了 RAII 手法：
    //   在构造时给 mutex 加锁，在析构时解锁，从而保证 mutex 总被正确解锁
    vector<int> v{1, 3, 5, 7, 9};
    thread t1(add_to_list, std::ref(v));
    thread t2(list_contains, std::ref(v));
    t1.join();
    t2.join();
    // 大多数场景下不使用全局变量，而是将 mutex 与受保护的数据组成一个类。
    //   既能清楚表明它们互相联系，还能封装函数以增强保护
    // 如果成员函数返回指向共享数据的指针或引用，那么即便成员函数全都按良好、有序的方式
    //   锁定 mutex，仍然无济于事，因为保护已被打破
    // 若利用 mutex 保护共享数据，则需谨慎设计程序接口
}