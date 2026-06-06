#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include <chrono>

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

// ─── MemPool ──────────────────────────────────────────────────────────────────
template<typename T>
class MemPool final {
private:
    struct ObjectBlock {
        T    object_;       // must be FIRST member (see ASSERT in constructor)
        bool is_free_ = true;
    };

    std::vector<ObjectBlock> store_;
    size_t next_free_index_ = 0;
    size_t num_free_;       // track free slot count to know when pool is full

    void updateNextFreeIndex() noexcept {
        if (num_free_ == 0) return;  // pool is full, nothing to scan for
        const auto initial_free_index = next_free_index_;
        while (!store_[next_free_index_].is_free_) {
            ++next_free_index_;
            if (UNLIKELY(next_free_index_ == store_.size()))
                next_free_index_ = 0;
            if (UNLIKELY(next_free_index_ == initial_free_index))
                ASSERT(false, "MemPool is out of space");
        }
    }

public:
    explicit MemPool(size_t num_elems)
            : store_(num_elems, {T(), true}), num_free_(num_elems) {
        // Verify object_ is the first member of ObjectBlock so that
        // a T* and its enclosing ObjectBlock* share the same address.
        // deallocate() relies on this to cast T* back to ObjectBlock*.
        ASSERT(
                reinterpret_cast<const ObjectBlock*>(&(store_[0].object_)) == &(store_[0]),
                "T object_ must be the first member of ObjectBlock"
        );
    }

    MemPool()                              = delete;
    MemPool(const MemPool&)                = delete;
    MemPool(MemPool&&)                     = delete;
    MemPool& operator=(const MemPool&)     = delete;
    MemPool& operator=(MemPool&&)          = delete;

    template<typename... Args>
    T* allocate(Args&&... args) noexcept {
        ASSERT(num_free_ > 0, "MemPool is out of space");

        auto* obj_block = &(store_[next_free_index_]);
        ASSERT(obj_block->is_free_, "ObjectBlock is not free");

        T* ret = &(obj_block->object_);
        ret = new(ret) T(std::forward<Args>(args)...);  // placement new: no heap alloc
        obj_block->is_free_ = false;
        --num_free_;

        updateNextFreeIndex();
        return ret;
    }

    void deallocate(const T* elem) noexcept {
        const auto* block_ptr = reinterpret_cast<const ObjectBlock*>(elem);
        const auto elem_index = block_ptr - &store_[0];

        ASSERT(elem_index >= 0 && static_cast<size_t>(elem_index) < store_.size(),
               "Element does not belong to this MemPool");
        ASSERT(!store_[elem_index].is_free_, "Expected slot to be in-use");

        store_[elem_index].is_free_ = true;
        ++num_free_;

        // If we were full before this deallocate, next_free_index_ is stale.
        // Point it at the slot we just freed so the next allocate() finds it.
        if (num_free_ == 1)
            next_free_index_ = static_cast<size_t>(elem_index);
    }
};

// ─── Example struct ───────────────────────────────────────────────────────────
struct Order {
    double price;
    int    qty;
    int    id;

    Order() : price(0), qty(0), id(0) {}
    Order(double p, int q, int i) : price(p), qty(q), id(i) {}
};

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    // Pool with 5 slots. One heap allocation here — never again.
    MemPool<Order> pool(5);

    std::cout << "=== filling all 5 slots ===\n";

    Order* a = pool.allocate(100.0, 10, 1);
    Order* b = pool.allocate(200.0, 20, 2);
    Order* c = pool.allocate(300.0, 30, 3);
    Order* d = pool.allocate(400.0, 40, 4);
    Order* e = pool.allocate(500.0, 50, 5);

    std::cout << "order a: price=" << a->price << " qty=" << a->qty << " addr=" << (void*)a << "\n";
    std::cout << "order b: price=" << b->price << " qty=" << b->qty << " addr=" << (void*)b << "\n";
    std::cout << "order c: price=" << c->price << " qty=" << c->qty << " addr=" << (void*)c << "\n";
    std::cout << "order d: price=" << d->price << " qty=" << d->qty << " addr=" << (void*)d << "\n";
    std::cout << "order e: price=" << e->price << " qty=" << e->qty << " addr=" << (void*)e << "\n";

    // Pool is now full. Free two slots in the middle.
    std::cout << "\n=== deallocating b (slot 1) and d (slot 3) ===\n";
    pool.deallocate(b);
    pool.deallocate(d);
    std::cout << "b and d slots are now free\n";

    // With no fresh slots available, the next two allocations must reuse b and d.
    std::cout << "\n=== allocating 2 new orders (only freed slots available) ===\n";
    Order* f = pool.allocate(600.0, 60, 6);
    Order* g = pool.allocate(700.0, 70, 7);

    std::cout << "order f: price=" << f->price << " qty=" << f->qty << " addr=" << (void*)f << "\n";
    std::cout << "order g: price=" << g->price << " qty=" << g->qty << " addr=" << (void*)g << "\n";

    std::cout << "\nf reuses b's slot: " << (f == b ? "yes" : "no") << "\n";
    std::cout << "g reuses d's slot: " << (g == d ? "yes" : "no") << "\n";

    // ─── Benchmark: MemPool vs heap — worst-case latency ───────────────────────
    // In low-latency systems, we care about tail latency (worst case), not average.
    // The heap can occasionally trigger syscalls (mmap/brk) causing huge spikes.
    constexpr int NUM_ITERS = 1'000'000;
    MemPool<Order> bench_pool(NUM_ITERS);

    long pool_max_ns = 0, heap_max_ns = 0;
    long pool_total_ns = 0, heap_total_ns = 0;

    // MemPool: allocate all, track per-allocation latency
    std::vector<Order*> pool_ptrs(NUM_ITERS);
    for (int i = 0; i < NUM_ITERS; ++i) {
        auto t1 = std::chrono::high_resolution_clock::now();
        pool_ptrs[i] = bench_pool.allocate(100.0, i, i);
        auto t2 = std::chrono::high_resolution_clock::now();
        long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        pool_total_ns += ns;
        if (ns > pool_max_ns) pool_max_ns = ns;
    }
    for (int i = 0; i < NUM_ITERS; ++i)
        bench_pool.deallocate(pool_ptrs[i]);

    // Heap: allocate all, track per-allocation latency
    std::vector<Order*> heap_ptrs(NUM_ITERS);
    for (int i = 0; i < NUM_ITERS; ++i) {
        auto t1 = std::chrono::high_resolution_clock::now();
        heap_ptrs[i] = new Order(100.0, i, i);
        auto t2 = std::chrono::high_resolution_clock::now();
        long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        heap_total_ns += ns;
        if (ns > heap_max_ns) heap_max_ns = ns;
    }
    for (int i = 0; i < NUM_ITERS; ++i)
        delete heap_ptrs[i];

    std::cout << "\n=== benchmark: " << NUM_ITERS << " allocations ===\n";
    std::cout << "            Avg (ns)    Worst (ns)\n";
    std::cout << "MemPool :   " << pool_total_ns / NUM_ITERS << "           " << pool_max_ns << "\n";
    std::cout << "Heap    :   " << heap_total_ns / NUM_ITERS << "           " << heap_max_ns << "\n";
    std::cout << "\nWorst-case: MemPool is " << 100.0 * (heap_max_ns - pool_max_ns) / heap_max_ns << "% faster than heap\n";

    // ─── Allocating when pool is full ─────────────────────────────────────────
    MemPool<Order> full_pool(2);
    full_pool.allocate(1.0, 1, 1);
    full_pool.allocate(2.0, 2, 2);

    std::cout << "\n=== attempting to allocate when pool is full ===\n";
    std::cout << "expecting ASSERT failure...\n";
    full_pool.allocate(3.0, 3, 3);  // this will abort

    return 0;
}