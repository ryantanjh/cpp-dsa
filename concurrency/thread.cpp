//
// Created by ryan tan on 2/4/26.
//
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex cout_mtx;   // protects std::cout from interleaved output

void worker(int id, int iterations) {
    // each thread does 3 iterations of printing work
    for (int i = 0; i < iterations; ++i) {
        // try removing this to see what happens. it ensures that each thread only goes to cout one at a time
        std::lock_guard<std::mutex> lock(cout_mtx);
        std::cout << "Thread " << id << ": iteration " << i << "\n";
    }
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) { //
        threads.emplace_back(worker, i, 3);   // create vector of 3 threads (must use emplace_back)
    }

    for (auto& t : threads) {
        t.join();   // wait for all threads to finish
    }

    std::cout << "All threads done\n";
}