#include "../include/common.h"
#include "threadsafe_stack.h"
// 设计一个线程安全的 stack


// stl 里的 stack 不是线程安全的，例如：
// t1     t2
// top
//        top
// pop
//        pop
// 会导致丢失数据，有一个元素未经处理就被 pop
void f() {
    stack<int> s;
    if (!s.empty()) {
        int v = s.top();
        s.pop();
        cout << v << endl;
    }
}
// 可以将 top 和 pop 结合在同一个操作中：pop 删除的同时返回元素
// 然而在向调用者复制数据的过程中有可能抛出异常。万一弹出的元素已从栈上移除，
//    但复制却不成功（例如内存不足），就会令数据丢失

threadsafe_stack<int> s;

void g() {
    try {
        while (!s.empty()) {
            auto p = s.pop();
            // 模拟一个耗时操作
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    } 
}



int main() {
    auto create_thread = [](int n) {
        vector<thread> threads(n);
        for (int i = 0; i < n; ++i) {
            threads[i] = thread(g);
        }
        for (auto &t : threads) {
            t.join();
        }
    };

    // single thread
    for (int i = 0; i < 100; ++i) {
        s.push(i);
    }
    eval_time([&]{ create_thread(1); }, "single thread time: ");

    cout << "stack is empty: " << s.empty() << endl;

    // two thread
    for (int i = 0; i < 100; ++i) {
        s.push(i);
    }
    eval_time([&]{ create_thread(2); }, "two thread time: ");

    cout << "stack is empty: " << s.empty() << endl;

    // four thread
    for (int i = 0; i < 100; ++i) {
        s.push(i);
    }
    eval_time([&]{ create_thread(4); }, "four thread time: ");
}