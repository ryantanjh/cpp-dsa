#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <thread>
#include <string>
#include <chrono>
#include <cstdlib>
#include <ctime>

// ─── ASSERT macro ─────────────────────────────────────────────────────────────
#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "ASSERT failed: " << (msg) << "\n" \
                      << "  at " << __FILE__ << ":" << __LINE__ << "\n"; \
            std::abort(); \
        } \
    } while (0)

#define UNLIKELY(x) __builtin_expect(!!(x), 0)

// ─── LFQueue ──────────────────────────────────────────────────────────────────
template<typename T>
class LFQueue final {
private:
    std::vector<T>       store_;
    std::atomic<size_t>  next_write_index_ = {0};
    std::atomic<size_t>  next_read_index_  = {0};
    std::atomic<size_t>  num_elements_     = {0};

public:
    explicit LFQueue(size_t num_elems) : store_(num_elems, T()) {}

    LFQueue()                          = delete;
    LFQueue(const LFQueue&)            = delete;
    LFQueue(LFQueue&&)                 = delete;
    LFQueue& operator=(const LFQueue&) = delete;
    LFQueue& operator=(LFQueue&&)      = delete;

    T* getNextToWriteTo() noexcept {
        return &store_[next_write_index_];
    }

    void updateWriteIndex() noexcept {
        next_write_index_ = (next_write_index_ + 1) % store_.size();
        ++num_elements_;
    }

    const T* getNextToRead() const noexcept {
        return (next_read_index_ == next_write_index_)
               ? nullptr
               : &store_[next_read_index_];
    }

    void updateReadIndex() noexcept {
        next_read_index_ = (next_read_index_ + 1) % store_.size();
        ASSERT(num_elements_ != 0, "Tried to read from empty queue");
        --num_elements_;
    }

    size_t size() const noexcept { return num_elements_.load(); }
};

// ─── LogType and LogElement ───────────────────────────────────────────────────
// Each log call is broken into individual LogElements — one per character or
// primitive value. The type_ tag tells the logger thread how to format it.
// Using a union means every LogElement is the same size regardless of what
// it holds, which is what allows us to store them in a fixed LFQueue.
enum class LogType : int8_t {
    CHAR = 0, INTEGER = 1, LONG_INTEGER = 2, LONG_LONG_INTEGER = 3,
    UNSIGNED_INTEGER = 4, UNSIGNED_LONG_INTEGER = 5,
    UNSIGNED_LONG_LONG_INTEGER = 6, FLOAT = 7, DOUBLE = 8
};

struct LogElement {
    LogType type_ = LogType::CHAR;
    union {
        char               c;
        int                i;
        long               l;
        long long          ll;
        unsigned           u;
        unsigned long      ul;
        unsigned long long ull;
        float              f;
        double             d;
    } u_;
};

// ─── Logger ───────────────────────────────────────────────────────────────────
constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;  // 8 million slots

class Logger final {
private:
    const std::string     file_name_;
    std::ofstream         file_;
    LFQueue<LogElement>   queue_;
    std::atomic<bool>     running_ = {true};
    std::thread*          logger_thread_ = nullptr;

    // ── Logger thread: drains the queue and writes to disk ───────────────────
    // This is the only place that does string formatting and file I/O.
    // The hot path never touches the disk — it just pushes LogElements.
    void flushQueue() noexcept {
        while (running_) {
            for (auto next = queue_.getNextToRead();
                 queue_.size() && next;
                 next = queue_.getNextToRead())
            {
                switch (next->type_) {
                    case LogType::CHAR:                      file_ << next->u_.c;   break;
                    case LogType::INTEGER:                   file_ << next->u_.i;   break;
                    case LogType::LONG_INTEGER:              file_ << next->u_.l;   break;
                    case LogType::LONG_LONG_INTEGER:         file_ << next->u_.ll;  break;
                    case LogType::UNSIGNED_INTEGER:          file_ << next->u_.u;   break;
                    case LogType::UNSIGNED_LONG_INTEGER:     file_ << next->u_.ul;  break;
                    case LogType::UNSIGNED_LONG_LONG_INTEGER:file_ << next->u_.ull; break;
                    case LogType::FLOAT:                     file_ << next->u_.f;   break;
                    case LogType::DOUBLE:                    file_ << next->u_.d;   break;
                }
                queue_.updateReadIndex();
            }
            // Sleep 1ms between drains to avoid burning CPU in a tight spin loop.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // ── Hot path: push a single LogElement onto the queue ────────────────────
    void pushValue(const LogElement& elem) noexcept {
        *queue_.getNextToWriteTo() = elem;
        queue_.updateWriteIndex();
    }

    // Overloads for each type — the hot path calls these directly.
    // Each one just wraps the value in a LogElement and pushes it.
    void pushValue(char value) noexcept {
        pushValue(LogElement{LogType::CHAR, {.c = value}});
    }
    // Walk the C-string and push each character individually.
    void pushValue(const char* value) noexcept {
        while (*value) { pushValue(*value); ++value; }
    }
    void pushValue(const std::string& value) noexcept {
        pushValue(value.c_str());
    }
    void pushValue(int value) noexcept {
        pushValue(LogElement{LogType::INTEGER, {.i = value}});
    }
    void pushValue(long value) noexcept {
        pushValue(LogElement{LogType::LONG_INTEGER, {.l = value}});
    }
    void pushValue(long long value) noexcept {
        pushValue(LogElement{LogType::LONG_LONG_INTEGER, {.ll = value}});
    }
    void pushValue(unsigned value) noexcept {
        pushValue(LogElement{LogType::UNSIGNED_INTEGER, {.u = value}});
    }
    void pushValue(unsigned long value) noexcept {
        pushValue(LogElement{LogType::UNSIGNED_LONG_INTEGER, {.ul = value}});
    }
    void pushValue(unsigned long long value) noexcept {
        pushValue(LogElement{LogType::UNSIGNED_LONG_LONG_INTEGER, {.ull = value}});
    }
    void pushValue(float value) noexcept {
        pushValue(LogElement{LogType::FLOAT, {.f = value}});
    }
    void pushValue(double value) noexcept {
        pushValue(LogElement{LogType::DOUBLE, {.d = value}});
    }

public:
    explicit Logger(const std::string& file_name)
            : file_name_(file_name), queue_(LOG_QUEUE_SIZE)
    {
        file_.open(file_name);
        ASSERT(file_.is_open(), "Could not open log file: " + file_name);

        // Spawn the logger thread. It runs flushQueue() in the background
        // for the lifetime of this Logger object.
        logger_thread_ = new std::thread([this] { flushQueue(); });
        ASSERT(logger_thread_ != nullptr, "Failed to start logger thread");
        std::cout << "[logger] writing to " << file_name << "\n";
    }

    ~Logger() {
        std::cerr << "[logger] flushing and closing " << file_name_ << "\n";
        // Wait until the queue is fully drained before shutting down.
        while (queue_.size()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        running_ = false;
        logger_thread_->join();
        delete logger_thread_;
        file_.close();
    }

    Logger()                           = delete;
    Logger(const Logger&)              = delete;
    Logger(Logger&&)                   = delete;
    Logger& operator=(const Logger&)   = delete;
    Logger& operator=(Logger&&)        = delete;

    // ── log(): the public API ─────────────────────────────────────────────────
    // Walks the format string. Literal characters are pushed as CHAR elements.
    // Each % is replaced by the next argument, pushed as its typed LogElement.
    // Example: logger.log("price:% qty:%\n", 42.5, 10)
    //   pushes: 'p','r','i','c','e',':',DOUBLE(42.5),' ','q','t','y',':',INT(10),'\n'

    // Base case: no substitutions left — push the remaining literal characters.
    void log(const char* s) noexcept {
        while (*s) {
            if (*s == '%') {
                // %% in the format string means a literal '%'
                if (UNLIKELY(*(s + 1) == '%')) { ++s; }
                else { ASSERT(false, "Missing arguments to log()"); }
            }
            pushValue(*s++);
        }
    }

    // Recursive case: consume one argument per % encountered.
    template<typename T, typename... A>
    void log(const char* s, const T& value, A&&... args) noexcept {
        while (*s) {
            if (*s == '%') {
                if (UNLIKELY(*(s + 1) == '%')) {
                    ++s;  // escaped %%, treat as literal
                } else {
                    pushValue(value);         // push this argument
                    log(s + 1, args...);      // recurse for the rest
                    return;
                }
            }
            pushValue(*s++);
        }
        ASSERT(false, "Extra arguments provided to log()");
    }
};

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    Logger logger("example.log");

    // These calls return almost instantly — they just push LogElements onto
    // the lock-free queue. The background thread does the actual file writing.
    char        c  = 'X';
    int         i  = 42;
    long        l  = 123456L;
    float       f  = 3.14f;
    double      d  = 2.71828;
    const char* cs = "a C-string";
    std::string ss = "a std::string";

    logger.log("--- basic types ---\n");
    logger.log("char:    %\n", c);
    logger.log("int:     %\n", i);
    logger.log("long:    %\n", l);
    logger.log("float:   %\n", f);
    logger.log("double:  %\n", d);
    logger.log("cstring: %\n", cs);
    logger.log("string:  %\n", ss);

    logger.log("\n--- multiple args ---\n");
    logger.log("order: symbol=% price=% qty=% side=%\n",
               std::string("AAPL"), 189.50, 100, 'B');

    logger.log("\n--- escaped percent ---\n");
    logger.log("fill rate: 98%% (escaped)\n");

    // Simulate a hot loop — log 5 market updates as fast as possible.
    logger.log("\n--- simulated market data loop ---\n");
    for (int tick = 1; tick <= 5; ++tick) {
        double price = 100.0 + tick * 0.25;
        int    qty   = tick * 10;
        logger.log("tick=% price=% qty=%\n", tick, price, qty);
    }

    // Logger destructor waits for the queue to drain, then closes the file.
    // Output will appear in example.log in the current directory.
    std::cout << "[main] done logging. destructor will flush and close.\n";
    return 0;
}