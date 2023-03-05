#include "../include/common.h"


// 防范死锁的建议通常是，始终按相同顺序对两个 mutex 加锁
// 若我们总是先锁 mutex A再锁 mutex B，则永远不会发生死锁

// 然而考虑一个函数，其操作同一个类的两个实例，互相交换它们的内部数据
// 此时如果按照固定顺序对传入的两个参数加锁，有可能导致死锁


class some_big_object { 
private:
    int i;
public:
    some_big_object(int i) : i(i) { }

    friend ostream& operator<<(ostream &os, const some_big_object& s) {
        os << s.i;
        return os;
    }

    friend void swap (some_big_object &lhs, some_big_object &rhs) {
        std::swap(lhs.i, rhs.i);
        cout << "swap: " << lhs.i << " " << rhs.i << endl;
    }
};



class X {
private:
    some_big_object &some_detail;
    mutex m;

public:
    X(some_big_object &sd) : some_detail(sd) { }

    friend void swap(X &lhs, X &rhs) {
        if (&lhs == &rhs) {
            return;
        }
        // std::lock() 函数可以同时锁住多个 mutex
        // 假如 std::lock() 函数在其中一个 mutex 上成功获取了锁，但它
        //  试图在另一个 mutex 上获取锁时却有异常抛出，那么第一个锁就会自动释放
        lock(lhs.m, rhs.m);
        // std::adopt_lock 对象指明 mutex 已被锁住，lock_guard 实例
        //  应当接收锁的归属权，不得在构造函数内试图另行加锁
        lock_guard<mutex> lock_a(lhs.m, adopt_lock);
        lock_guard<mutex> lock_b(rhs.m, adopt_lock);
        swap(lhs.some_detail, rhs.some_detail);
    }

    friend void swap_std17(X &lhs, X &rhs) {
        if (&lhs == &rhs) {
            return;
        }
        // C++17 还进一步提供了新的 RAII 类模板 std::scoped_lock<>
        // std:: scoped_lock<> 和 std::lock_guard<> 完全等价，只不过
        // 前者是可变参数模板（variadic template），接收各种互斥型别作为
        // 模板参数列表，还以多个互斥对象作为构造函数的参数列表
        scoped_lock guard(lhs.m, rhs.m);
        swap(lhs.some_detail, rhs.some_detail);
    }
};


int main() {
    some_big_object s_a(3), s_b(5);
    cout << s_a << " " << s_b << endl;
    X a(s_a), b(s_b);
    swap(a, b);
    cout << s_a << " " << s_b << endl;

    swap_std17(a, b);
    cout << s_a << " " << s_b << endl;
    
       
}