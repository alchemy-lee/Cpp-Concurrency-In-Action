#include "../include/common.h"

class hierarchical_mutex {
    mutex internal_mutex;
    unsigned long const hierarchy_value;
    unsigned long previous_hierarchy_value;
    // 为了存储当前层级编号，hierarchical_mutex 的实现使用了线程专属的局部变量
    // 类的所有实例都能读取该变量，但它的值因不同线程而异。
    // 可以独立检测各线程的行为，各 mutex 都能判断是否允许当前线程对其加锁。
    static thread_local unsigned long this_thread_hierarchy_value;

    void check_for_hierarchy_violation() {
        if (this_thread_hierarchy_value <= hierarchy_value) {
            throw logic_error("mutex hierarchy violated");
        }
    }

    void update_hierarchy_value() {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

public:
    explicit hierarchical_mutex(unsigned long value) :
        hierarchy_value(value),
        previous_hierarchy_value(0) { }

    void lock() {
        check_for_hierarchy_violation();
        internal_mutex.lock();
        update_hierarchy_value();
    }

    void unlock() {
        if (this_thread_hierarchy_value != hierarchy_value) {
            throw logic_error("mutex hierarchy violated");
        }
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }

    bool try_lock() {
        check_for_hierarchy_violation();
        if (!internal_mutex.try_lock()) {
            return false;
        }
        update_hierarchy_value();
        return true;
    }
};

// 初始化为最大值，每个线程都有自己的 this_thread_hierarchy_value 副本
thread_local unsigned long 
    hierarchical_mutex::this_thread_hierarchy_value(INT64_MAX);