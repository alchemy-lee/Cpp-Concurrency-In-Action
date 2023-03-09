#ifndef CPP_LEARNING_COMMON_H
#define CPP_LEARNING_COMMON_H

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <memory>
#include <iterator>
#include <functional>
#include <fstream>
#include <thread>
#include <mutex>
#include <stack>
#include <shared_mutex>
#include <random>

using namespace std;

template <typename T>
inline void PRINT_LIST_ELEMENTS (const T& coll, const std::string& optcstr="") {
    std::cout << optcstr;
    for (auto elem : coll) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

template <typename T>
inline void PRINT_MAPPED_ELEMENTS (const T& coll, const std::string& optcstr="") {
    std::cout << optcstr;
    for (auto elem : coll) {
        std::cout << "[" << elem.first << ", " << elem.second << "] ";
    }
    std::cout << std::endl;
}

template <typename F>
inline void eval_time(F fun, string desc="time: ") {
    const auto t1 = std::chrono::high_resolution_clock::now();
    fun();
    const auto t2 = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> ms = t2 - t1;
    std::cout << desc << ms.count() << " ms\n";
}

// function<void(Trie*)> dfs = [&](Trie* node) {
//     if (node->ref != -1) {
//         ans.push_back(folder[node->ref]);
//         return;
//     }
//     for (auto& [_, child] : node->children) {
//         dfs(child);
//     }
// };

#endif //CPP_LEARNING_COMMON_H