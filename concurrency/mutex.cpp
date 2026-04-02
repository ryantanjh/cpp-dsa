//
// Created by ryan tan on 2/4/26.
//
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

class ThreadSafeCounter {
    mutable std::mutex mtx_; // mutex object contains lock() and unlock()
    // we wrap lock and unlock around the shared state (multiple threads can access that state) to prevent race conditions
    // lock_guard essentially does the same, just short form . it also ensures to release the lock, preventing deadlock
    int count_ = 0;

public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx_); // try removing this. wont count to 10000
        ++count_;
    }

    int get() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return count_;
    }
};

int main() {
    ThreadSafeCounter counter;
    std::vector<std::thread> threads;

    // 10 threads each incrementing 10000 times
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&counter]() {
            for (int j = 0; j < 10000; ++j) {
                counter.increment();
            }
        });
    }

    for (auto& t : threads) t.join();

    std::cout << "Final count: " << counter.get() << "\n";   // exactly 100000
}