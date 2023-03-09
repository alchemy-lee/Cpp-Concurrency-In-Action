#include "../include/common.h"

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> distrib(1, 100);

// 一个条件变量与一个事件或 condition 相关联，一个或多个线程可以等待该条件被满足
// 当一个线程确定条件得到满足时，它可以通知一个或多个等待条件变量的线程，
// 以唤醒它们，让它们继续处理

// stl 提供了 condition_variable 和 condition_variable_any
// 它们都需要与一个 mutex 一起工作，以便提供适当的同步；
// 前者通常与 std::unique_lock 一起工作，而后者可以与任何符合 mutex 的
// 最低标准的东西一起工作，因此后缀为 _any
// condition_variable_any 更通用，有可能在大小、性能或操作系统资源方面产生
// 额外的成本

mutex m;
queue<int> data_queue;
condition_variable data_cond;


void data_preparation() {
    for (int i = 0; i < 10; ++i) {
        int data = distrib(gen);
        {
            lock_guard<mutex> lk(m);
            data_queue.push(data);
            cout << "produce data: " << data << endl;
        }
        // 对 mutex 解锁之后再通知条件变量
        data_cond.notify_one();
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}


void data_processing() {
    for (int i = 0; i < 10; ++i) {
        unique_lock<mutex> lk(m);
        // 如果条件没用满足，wait() 解锁 mutex，并令线程进入阻塞状态或等待状态
        // 当线程被 notify_one 唤醒后，重新在 mutex 上获取锁，再次查验条件：
        // 若条件成立，则从 wait() 函数返回，而 mutex 仍被锁住

        // 在调用 wait() 的过程中，可能会对条件进行多次检查
        // 检查条件的函数最好不要有其他副作用（例如修改全局变量）
        data_cond.wait(lk, []{ 
            cout << "check condition" << endl;
            return !data_queue.empty(); 
        });
        int data = data_queue.front();
        data_queue.pop();
        lk.unlock();

        // 模拟耗时操作
        this_thread::sleep_for(chrono::milliseconds(50));
        cout << "consume data: " << data << endl;
    }
}


int main() {
    thread t1(data_preparation);
    thread t2(data_processing);

    t1.join();
    t2.join();
}