#include "../include/common.h"

// 保护甚少更新的数据结构
// 更新完成，数据结构方可重新被多线程并发访问

using dns_entry = std::string;

class dns_cache {
public:
    dns_entry find_entry(const string& domain) const {
        // 读锁，使用 shared_lock，多个线程能够同时锁住同一个 shared_mutex
        // 共享锁仅有一个限制，即假设它已被某些线程所持有，若别的线程试图
        // 获取排他锁，就会发生阻塞，直到那些线程全都释放该共享锁
        shared_lock<shared_mutex> lk(entry_mutex);
        auto it = entries.find(domain);
        return (it == entries.end()) ? dns_entry() : it->second;
    }

    void update_or_add_entry(const string& domain, const dns_entry& dns_detail) {
        // 写锁，使用 lock_guard 或 unique_lock
        // 如果任一线程持有排他锁，那么其他线程全都无法获取共享锁或排他锁，
        // 直到持锁线程将排他锁释放为止
        lock_guard<shared_mutex> lk(entry_mutex);
        entries[domain] = dns_detail;
    }

private:
    map<string, dns_entry> entries;
    mutable shared_mutex entry_mutex;
};



int main() {
    dns_cache cache;
    cache.update_or_add_entry("baidu.com", "1.1.1.1");

    auto res = cache.find_entry("baidu.com");
    cout << res << endl;

    // 对于 std::mutex，如果一个线程试图锁定一个它已经拥有的 mutex，
    // 会导致未定义的行为

    // C++ 标准库为此提供了 std::recursive_mutex，其工作方式与 std::mutex
    // 相似，不同之处是，其允许同一线程对某 mutex 的同一实例多次加锁
    // 我们必须先释放全部的锁，才可以让另一个线程锁住该 mutex
    // 例如，若我们对它调用了 3 次 lock()，就必须调用 3 次 unlock()
    // 只要正确地使用 lock_guard<std::recursive_mutex> 和
    // unique_lock<std::recursive_mutex>，它们便会处理好递归锁的余下细节
}