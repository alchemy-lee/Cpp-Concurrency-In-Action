#ifndef THREADSAFE_STACK_H
#define THREADSAFE_STACK_H

#include "../include/common.h"

// 线程安全的 stack

struct empty_stack : exception {
    const char* what() const noexcept {
        return "Empty stack!";
    }
};


template<typename T>
class threadsafe_stack {
private:
    stack<T> data;
    mutable mutex m;
public:
    threadsafe_stack() {}
    threadsafe_stack(const threadsafe_stack &other) {
        lock_guard<mutex> lock(other.m);
        data = other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;
    void push(T new_value) {
        lock_guard<mutex> lock(m);
        data.push(std::move(new_value));
    }

    // 使用 shared_ptr 返回 value
    shared_ptr<T> pop() {
        lock_guard<mutex> lock(m);
        if (data.empty()) {
            throw empty_stack();
        }
        // Allocate return value before modifying stack
        shared_ptr<T> const res(make_shared<T>(data.top()));
        data.pop();
        return res;
    }

    // 将 value 保存到一个引用中
    void pop(T &value) {
        lock_guard<mutex> lock(m);
        if (data.empty()) {
            throw empty_stack();
        }
        value = data.top();
        data.pop();
    }

    bool empty() const {
        lock_guard<mutex> lock(m);
        return data.empty();
    }
};


#endif 