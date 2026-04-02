//
// Created by ryan tan on 2/4/26.
//

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

std::queue<std::string> pass;       // the pass between chef and waiter
std::mutex mtx;
std::condition_variable cv;

void chef() {
    std::vector<std::string> dishes = {"pasta", "steak", "salmon", "salad", "soup"};
    for (const auto& dish : dishes) {
        {
            std::lock_guard<std::mutex> lock(mtx); // lock the queue
            pass.push(dish);
            std::cout << "Chef cooked: " << dish << "\n";
        }
        cv.notify_one();   // lock on queue is released, waiter can access it
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // cooking takes time
    }
}

void waiter() {
    for (int i = 0; i < 5; ++i) {
        std::unique_lock<std::mutex> lock(mtx); // lock queue when waiter access it
        // waiter releases lock while sleeping, acquires lock once awake
        cv.wait(lock, [] { return !pass.empty(); });   // try removing this
        // locked portion once waiter acquires it
        std::string dish = pass.front();
        pass.pop();
        std::cout << "Waiter served: " << dish << "\n";
    }
}

int main() {
    std::thread t_chef(chef);
    std::thread t_waiter(waiter);

    t_chef.join();
    t_waiter.join();
}

