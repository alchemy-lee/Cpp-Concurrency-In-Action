#include "../include/common.h"

class some_data {
    int a;

public:
    some_data() : a(0) {}

    void do_something() {
        ++a;
        cout << "do_something: " << a << endl;        
    }
};


class data_wrapper {
    some_data data;
    mutex m;

public:
    template<typename Function>
    void process_data(Function func) {
        lock_guard<mutex> l(m);
        // 将共享数据作为参数传递给函数指针
        func(data);
    }
};


some_data *unprotected;

void malicious_funcsion(some_data &protected_data) {
    unprotected = &protected_data;
}


int main() {
    data_wrapper x;
    x.process_data(malicious_funcsion);
    // bypass the protection
    unprotected->do_something();

    // 不得向锁所在的作用域之外传递指向共享数据指针和引用，无论是通过函数返回值
    //   将它们保存到对外可见的内存，还是将它们作为参数传递给使用者提供的函数
}
