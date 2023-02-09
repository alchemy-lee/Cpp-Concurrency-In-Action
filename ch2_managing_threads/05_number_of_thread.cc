#include <iostream>
#include <thread>
#include <numeric>
#include <vector>

using namespace std;


template<typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        cout << distance(first, last) << endl;
        result = accumulate(first, last, result);        
    }
};


template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    int length = distance(first, last);
    int min_per_thread = 25;
    int max_threads = (length + min_per_thread - 1) / min_per_thread;
    int hardware_threads = thread::hardware_concurrency();
    int num_threads = min(hardware_threads, max_threads);
    int block_size = length / num_threads;

    vector<T> results(num_threads);
    vector<thread> threads(num_threads - 1);
    Iterator block_start = first;
    for (int i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        advance(block_end, block_size);
        threads[i] = thread(accumulate_block<Iterator, T>(), 
            block_start, block_end, ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);

    for (auto &entry : threads) {
        entry.join();
    }
    return accumulate(results.begin(), results.end(), init);
}



int main() {
    cout << "hardware concurrency: " << thread::hardware_concurrency() << endl;


    vector<int> v;
    for (int i = 1; i <= 1008; ++i) {
        v.push_back(i);
    }
    const auto t1 = std::chrono::high_resolution_clock::now();
    int sum = parallel_accumulate(v.begin(), v.end(), 0);
    const auto t2 = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> ms = t2 - t1;
    cout << sum << "\t time: " << ms.count() << " ms\n";


}