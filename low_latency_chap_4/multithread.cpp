#include <iostream>
#include <atomic>
#include <thread>
#include <pthread.h>
#include <sched.h>
#include <string>
#include <chrono>

using std::string;
using std::atomic;
using std::thread;
using std::cerr;

// ─── 1. Pin a thread to a specific CPU core ───────────────────────────────────
// Returns true on success, false if the OS rejected the request (e.g. invalid core).
inline auto setThreadCore(int core_id) noexcept -> bool {
    cpu_set_t cpuset;           // step 1: declare a bitmask of all cores
    CPU_ZERO(&cpuset);          // step 2: clear it (all cores = OFF)
    CPU_SET(core_id, &cpuset);  // step 3: flip ON just the one core we want
    // step 4: tell the OS "this thread may only run on cores in cpuset"
    return (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0);
}

// ─── 2. Create and launch a thread ────────────────────────────────────────────
// T      = callable type (function, lambda, functor)
// A...   = zero or more argument types
// Returns a heap-allocated std::thread* the caller must join() and delete.
// Returns nullptr if the thread failed to start (e.g. bad core_id).
template<typename T, typename... A>
inline auto createAndStartThread(int core_id, const string& name,
                                 T&& func, A&&... args) noexcept -> thread* {
    // Two atomics for signalling between this thread and the new one.
    // atomic<bool> makes them safe to read/write from two threads simultaneously.
    atomic<bool> running(false), failed(false);

    // [&] captures all local variables by reference so the lambda can set them.
    auto thread_body = [&] {
        if (core_id >= 0 && !setThreadCore(core_id)) {
            cerr << "[" << name << "] failed to pin to core " << core_id << "\n";
            failed = true;
            return;
        }
        std::cout << "[" << name << "] Started. Core requested: " << core_id << "\n";
        running = true;
        // std::forward preserves the original value category (lvalue/rvalue).
        // The ... unpacks all args: func(arg1, arg2, arg3, ...)
        std::forward<T>(func)(std::forward<A>(args)...);
        std::cout << "[" << name << "] Finished.\n";
    };

    // Hand thread_body to a real OS thread and start it immediately.
    auto t = new thread(thread_body);

    // Spin here until the new thread sets running=true or failed=true.
    // Without this we might return before the thread has even started.
    while (!running && !failed) {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(100ms);  // check every 100ms, don't burn CPU
    }

    if (failed) {
        t->join();   // wait for thread to fully exit before we delete it
        delete t;
        return (thread*)nullptr;
    }

    // All good. Return the thread handle so the caller can join() later.
    // join() means "wait here until this thread finishes before moving on",
    // which prevents the program from exiting while the thread is still running.
    return t;
}

// ─── Example functions that will run on threads ───────────────────────────────

void addNumbers(int a, int b) {
    std::cout << "  addNumbers: " << a << " + " << b << " = " << (a + b) << "\n";
}

void countSlowly(const string& label, int from, int to) {
    for (int i = from; i <= to; ++i) {
        std::cout << "  [" << label << "] count: " << i << "\n";
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(500ms);
    }
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== main: starting threads ===\n\n";

    // core_id = -1  → skip affinity, OS decides which core to use
    // Runs addNumbers(10, 32) on the new thread.
    auto t1 = createAndStartThread(-1, "t1-add", addNumbers, 10, 32);

    // core_id = 0  → pin to core 0
    // Runs countSlowly("counter", 1, 4) on the new thread.
    auto t2 = createAndStartThread(0, "t2-count", countSlowly, string("counter"), 1, 4);

    // core_id = 99  → almost certainly invalid, triggers the failed path.
    auto t3 = createAndStartThread(99, "t3-bad", addNumbers, 1, 1);

    // Check which threads actually launched.
    if (!t1) std::cerr << "t1 failed to start\n";
    if (!t2) std::cerr << "t2 failed to start\n";
    if (!t3) std::cerr << "t3 failed to start (expected — core 99 doesn't exist)\n";

    std::cout << "\n=== main: waiting for threads to finish ===\n\n";

    // join() blocks main until that thread finishes.
    // Never delete a thread without joining first — the OS thread may still be running.
    if (t1) { t1->join(); delete t1; }
    if (t2) { t2->join(); delete t2; }
    // t3 was already cleaned up inside createAndStartThread when failed=true.

    std::cout << "\n=== main: all done ===\n";
    return 0;
}
