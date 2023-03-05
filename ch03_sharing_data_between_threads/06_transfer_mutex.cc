#include "../include/common.h"


// 可以将 std::defer_lock 传给 unique_lock，从而使 mutex 在完成构造时
// 处于无锁状态，等以后有需要时才在 std::unique_lock 对象（不是 mutex 对象）
// 上调用 lock() 获取锁，或把 std::unique_lock 对象交给 std::lock() 函数加锁
void unique_lock_test() {
    mutex m1, m2;
    // std::unique_lock 占用更多的空间，也比 std::lock_guard 略慢
    // unique_lock 对象可以传递给 lock() ，因为 unique_lock 提供了
    // lock()、try_lock() 和 unlock()成员函数。
    // 这些成员函数转发到底层 mutex 上的同名成员函数来完成工作，并更新
    // unique_lock 实例内部的一个标志，以表明 mutex 当前是否由该实例拥有
    // 这个标志是必要的，以确保 unlock 在析构函数中被正确调用
    unique_lock<mutex> lock_a(m1, defer_lock);
    unique_lock<mutex> lock_b(m2, defer_lock);
    lock(lock_a, lock_b);
}

// 如果右值，所有权的转移是自动的
// 对于左值，必须明确地进行转移，以避免意外地将所有权从一个变量中转移出去

// mutex 的转移有一种用途：准许函数锁定 mutex，然后把 mutex 
// 的归属权转移给函数调用者，好让他在同一个锁的保护下执行其他操作
unique_lock<mutex> get_lock() {
    mutex m1;
    unique_lock<mutex> lk(m1);
    cout << "prepare data" << endl;
    return lk;
}

void process_data() {
    unique_lock<mutex> lk(get_lock());
    cout << "process data" << endl;
}

int main() {
    unique_lock_test();


    process_data();

    // 和 mutex 一样，unique_lock 支持 lock 和 unlock 操作
    // unique_lock 的实例可以在被销毁前解锁。这就意味着，在执行流程的
    // 任何特定分支上，若某个锁显然没必要继续持有，我们则可手动解锁

    // 加锁时选用恰当的粒度，不仅事关锁定数据量的大小，还牵涉持锁时间
    // 以及持锁期间能执行什么操作
    // 一般地，若要执行某项操作，那我们应该只在所需的最短时间内持锁
}