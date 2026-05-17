# `sgp::lru_cache` — Design Rationale

A small, fast, header-only LRU cache. ~120 lines including comments.

## Contract

- `O(1)` amortized for `get`, `put`, `erase`, `peek`, `contains`, `size`
- `get` promotes the touched key to most-recent
- `put` of an existing key updates value AND promotes
- `peek` does NOT promote (introspection-only)
- Capacity is fixed at construction time
- Not thread-safe (wrap externally; sharded variants are a future primitive)

## Data structure

Two structures glued together:

```
order_ : std::list<pair<K,V>>           // doubly-linked list, front = MRU
index_ : unordered_map<K, iterator>     // key -> iterator into order_
```

Why both?

- **`std::list`** gives O(1) splice (the promotion operation) AND iterators that stay valid through insertion/erasure of *other* elements. This iterator-stability property is rare in the STL — `std::vector` invalidates iterators on any insert; `std::deque` invalidates on insert at non-ends; only `std::list` / `std::forward_list` guarantee what we need here.
- **`std::unordered_map`** gives O(1) average lookup from key to the list node.

The combination is what makes both operations O(1).

## The promotion mechanism

The critical line:

```cpp
order_.splice(order_.begin(), order_, it->second);
```

This moves the node pointed to by `it->second` to the front of the list **without invalidating any iterator** (the same `it->second` is still valid after splicing). `splice` is one of `std::list`'s superpowers — it's O(1) and iterator-preserving.

## Trade-offs vs alternatives

| Approach | Pros | Cons |
|---|---|---|
| **`std::list` + `unordered_map`** (this one) | Simple, O(1), iterator-stable | Two allocations per insert; cache-unfriendly |
| **Intrusive DLL** (e.g., Boost.Intrusive) | Single allocation; better locality | More boilerplate; only marginally faster in practice |
| **Open-addressed table with slot generation counter** | No DLL at all; very cache-friendly | Complex; eviction policy choice is harder |
| **`std::vector<entry> + free list`** | Single allocation, contiguous | Need handle-based "iterators"; reallocation triggers all-handle update |

For a study primitive that should be READ in one sitting, the `std::list` + `unordered_map` approach is the clear winner. For a production cache serving millions of QPS, a custom intrusive design is justified.

## Why not `std::list<std::pair<K,V>>` keyed by reference?

You can't key an `unordered_map` by reference (it would need stable storage), and a key-by-value duplicates the key — which is OK for small keys but wasteful for large ones.

For very large keys, a refined design stores the key only in the list node, and the map keys by `string_view` / similar lightweight handle. Future work.

## Why `std::optional<V>` for `get` return?

Three alternatives considered:

1. **`V* get(...)`** — returns raw pointer, null if missing. Loses type safety; caller must remember to null-check. The pointer's lifetime is also brittle (invalidated on next mutation).
2. **`V& get(...) throw`** — throws on miss. Forces caller into a try/catch for the common case of "may be missing" — exceptions are for exceptional cases, not for expected ones.
3. **`std::optional<V> get(...)`** — type-safe absence, no exceptions, copy semantics make lifetime trivially safe.

Option 3 is the modern C++ idiom for "may not exist."

The cost is a copy of `V`. For large value types this is non-trivial; a future variant could return `std::optional<std::reference_wrapper<V>>` for in-place access (with the caveat that the reference is invalidated by any subsequent mutation).

## Why no thread safety built-in?

Two reasons:

1. **Single responsibility.** This is a cache, not a synchronization primitive.
2. **Locking strategy is workload-dependent.** Some uses want a single mutex; others want sharding; others want lock-free. Bundling one choice would make the primitive worse for everyone else.

The standard wrapping pattern:

```cpp
class thread_safe_lru {
    mutable std::mutex m_;
    sgp::lru_cache<K, V> c_;
public:
    auto get(const K& k) {
        std::lock_guard lk{m_};
        return c_.get(k);
    }
    // ...
};
```

For high-contention workloads, see the planned `sharded_lru` primitive.

## Benchmarks

(Run `cmake -B build -DSGP_BUILD_BENCHMARKS=ON --build build` and `./build/benchmarks/sgp_bench` to reproduce.)

Expected order of magnitude on a 2024 laptop:

| Operation | Throughput |
|---|---|
| `put` sequential (no eviction), 64K items | ~10M ops/sec |
| `put` with eviction (50% replacement), 16K capacity | ~6M ops/sec |
| `get` mixed (50% hit), 16K capacity | ~15M ops/sec |

The `std::list` allocation overhead dominates `put`. A future `sgp::pmr::lru_cache` variant using a `std::pmr::monotonic_buffer_resource` would likely double the `put` throughput.

## Common bugs in handmade LRU implementations

If you implement an LRU yourself for an interview, these are the most common bugs (avoid by testing against the test suite here):

1. **Forgetting to promote on `put` of an existing key.** Easy to miss in a 30-min interview.
2. **Iterator invalidation.** Using anything other than `std::list` or implementing the DLL by hand without sentinel head/tail.
3. **Off-by-one in eviction.** Checking `size() > capacity` AFTER the insert vs `size() == capacity` BEFORE.
4. **Capacity of 1 edge case.** Every insert past the first triggers eviction; some implementations crash here.
5. **Self-mutation in iterator traversal.** Eviction during put can confuse a naive `for` loop.

See `tests/lru_cache_test.cpp` — there's a test for each of these.

## Future work

- `pmr::lru_cache` variant using `std::pmr` for allocator control
- `sharded_lru` for high-concurrency workloads
- `weighted_lru` (eviction based on entry size, not just count)
- `lfu_cache` as a peer primitive
