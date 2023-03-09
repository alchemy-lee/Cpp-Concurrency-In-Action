#include "../include/common.h"
#include "threadsafe_queue.h"


random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> distrib(1, 100);


threadsafe_queue<int> data_queue;


void data_preparation() {
    for (int i = 0; i < 10; ++i) {
        int data = distrib(gen);
        data_queue.push(data);
        cout << "produce data: " << data << endl;
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}


void data_processing() {
    for (int i = 0; i < 10; ++i) {
        auto data = data_queue.wait_and_pop();
        // 模拟耗时操作
        this_thread::sleep_for(chrono::milliseconds(50));
        cout << "consume data: " << *data << endl;
    }
}


int main() {
    thread t1(data_preparation);
    thread t2(data_processing);

    t1.join();
    t2.join();
}