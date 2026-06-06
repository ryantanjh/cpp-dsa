#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <string>
#include <cstdlib>

// ─── ASSERT macro ─────────────────────────────────────────────────────────────
#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "ASSERT failed: " << (msg) << "\n" \
                      << "  at " << __FILE__ << ":" << __LINE__ << "\n"; \
            std::abort(); \
        } \
    } while (0)

// ─── LFQueue ──────────────────────────────────────────────────────────────────
// SPSC lock-free queue backed by a ring buffer.
// Producer owns next_write_index_. Consumer owns next_read_index_.
// They never write to the same index, so no lock is needed.
template<typename T>
class LFQueue final {
private:
    std::vector<T>        store_;
    std::atomic<size_t>   next_write_index_ = {0};
    std::atomic<size_t>   next_read_index_  = {0};
    std::atomic<size_t>   num_elements_     = {0};

public:
    explicit LFQueue(size_t num_elems) : store_(num_elems, T()) {}

    LFQueue()                              = delete;
    LFQueue(const LFQueue&)                = delete;
    LFQueue(LFQueue&&)                     = delete;
    LFQueue& operator=(const LFQueue&)     = delete;
    LFQueue& operator=(LFQueue&&)          = delete;

    // ── Producer side ─────────────────────────────────────────────────────────

    // Returns a pointer to the next slot the producer should write into.
    // The producer writes directly into this slot, then calls updateWriteIndex().
    T* getNextToWriteTo() noexcept {
        return &store_[next_write_index_];
    }

    // Commits the write: advances the write index and increments element count.
    // Only call this AFTER you have finished writing into the slot.
    void updateWriteIndex() noexcept {
        next_write_index_ = (next_write_index_ + 1) % store_.size();
        ++num_elements_;
    }

    // ── Consumer side ─────────────────────────────────────────────────────────

    // Returns a pointer to the next element to read, or nullptr if queue is empty.
    // Queue is empty when read index == write index (consumer has caught up).
    const T* getNextToRead() const noexcept {
        return (next_read_index_ == next_write_index_)
               ? nullptr
               : &store_[next_read_index_];
    }

    // Commits the read: advances the read index and decrements element count.
    // Only call this AFTER you have finished using the element.
    void updateReadIndex() noexcept {
        next_read_index_ = (next_read_index_ + 1) % store_.size();
        ASSERT(num_elements_ != 0, "Tried to read from an empty queue");
        --num_elements_;
    }

    size_t size() const noexcept {
        return num_elements_.load();
    }
};

// ─── Market data struct ───────────────────────────────────────────────────────
struct MarketUpdate {
    int    id;
    double price;
    int    qty;
};

// ─── Consumer ─────────────────────────────────────────────────────────────────
// Runs on a background thread. Waits 2 seconds to let the producer build up a
// backlog, then drains the queue one element at a time.
void consumer(LFQueue<MarketUpdate>* queue) {
    using namespace std::literals::chrono_literals;

    std::cout << "[consumer] starting — waiting 2s to let producer build backlog...\n";
    std::this_thread::sleep_for(2s);

    while (queue->size()) {
        const auto* update = queue->getNextToRead();
        if (!update) continue;

        std::cout << "[consumer] read update #" << update->id
                  << " price=" << update->price
                  << " qty="   << update->qty
                  << "  (queue size now: " << queue->size() - 1 << ")\n";

        queue->updateReadIndex();
        std::this_thread::sleep_for(200ms);  // consumer is slower than producer
    }

    std::cout << "[consumer] queue drained. exiting.\n";
}

// ─── Main / Producer ──────────────────────────────────────────────────────────
int main() {
    // Pre-allocate 64 slots — the only heap allocation that ever happens.
    LFQueue<MarketUpdate> queue(64);

    // Spawn the consumer on a background thread.
    std::thread consumer_thread(consumer, &queue);

    std::cout << "[producer] pushing 10 market updates...\n\n";

    // Producer pushes 10 updates at 500ms intervals.
    // Consumer starts 2s late and reads slower, so the queue builds up first.
    for (int i = 1; i <= 10; ++i) {
        MarketUpdate update{i, 100.0 * i, i * 10};

        // Step 1: get a pointer to the next free slot and write into it directly.
        *queue.getNextToWriteTo() = update;

        // Step 2: commit — makes the element visible to the consumer.
        queue.updateWriteIndex();

        std::cout << "[producer] pushed update #" << update.id
                  << " price=" << update.price
                  << " qty="   << update.qty
                  << "  (queue size now: " << queue.size() << ")\n";

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(500ms);
    }

    std::cout << "\n[producer] done pushing. waiting for consumer to finish...\n\n";

    // Block main until the consumer thread has fully drained the queue.
    consumer_thread.join();

    std::cout << "[main] all done.\n";
    return 0;
}