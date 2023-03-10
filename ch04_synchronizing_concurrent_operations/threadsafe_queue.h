#include "../include/common.h"

template<typename T>
class threadsafe_queue {
private: 
    mutable mutex m;
    queue<T> data_queue;
    condition_variable data_cond;

public:
    threadsafe_queue() = default;

    threadsafe_queue(const threadsafe_queue& other) {
        lock_guard<mutex> lk(other.m);
        data_queue = other.data_queue;
    }

    void push(T v) {
        lock_guard<mutex> lk(m);
        data_queue.push(v);
        // 此方式并不能确定会通知到具体哪个线程，甚至不能保证正好有线程在
        // 等待通知，因为可能负责数据处理的全部线程都尚未完工
        data_cond.notify_one();
    }

    // wait_and_pop() 等待直到有一个值可以检索
    void wait_and_pop(T& value) {
        unique_lock<mutex> lk(m);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        value = data_queue.front();
        data_queue.pop();
    }

    shared_ptr<T> wait_and_pop() {
        unique_lock<mutex> lk(m);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        shared_ptr<T> res(make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    // try_pop() 试图从队列中弹出值，但总是立即返回，即使队列中没有值

    // try_pop() 的第一个重载将检索到的值保存在引用中，所以它可以使用返回值
    // 来表明状态；如果它检索到一个值，则返回 true，否则返回false
    bool try_pop(T& value) {
        lock_guard<mutex> lk(m);
        if (data_queue.empty()) {
            return false;
        }
        value = data_queue.front();
        data_queue.pop();
        return true;
    }

    // 第二个重载直接返回检索到的值。如果没有检索到，返回的指针为 NULL
    shared_ptr<T> try_pop() {
        lock_guard<mutex> lk(m);
        if (data_queue.empty()) {
            return shared_ptr<T>();
        }
        shared_ptr<T> res(make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const {
        lock_guard<mutex> lk(m);
        return data_queue.empty();
    }
};