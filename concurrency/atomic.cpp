//
// Created by ryan tan on 2/4/26.
//
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
// atomic — lock-free thread-safe operations, but only works for simple types
// Much faster than mutex
std::atomic<int> counter{0};

void worker() {
    for (int j = 0; j < 10000; ++j) {
        ++counter; // increment, then return
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) t.join();
    std::cout << "Counter: " << counter << "\n";
}