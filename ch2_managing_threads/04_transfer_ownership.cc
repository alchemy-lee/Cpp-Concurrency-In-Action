#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>

using namespace std;


void some_function() {

}

void some_other_function() {

}

int main() {
    // thread 类的每个实例都负责管控一个执行线程。因为 thread 类的实例能够移动（movable）却
    //   不能复制（not copyable），故此线程的归属权可以在实例之间转移
    // 对于任一特定的执行线程，任何时候都只有唯一的 thread 对象与之关联

    thread t1(some_function);
    thread t2 = move(t1);
    t1 = thread(some_other_function);
    thread t3;
    t3 = move(t2);
    // t1 = move(t3);

    t1.join();
    t3.join();
}