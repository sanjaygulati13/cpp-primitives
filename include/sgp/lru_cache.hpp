// sgp/lru_cache.hpp
//
// O(1) LRU cache using a doubly-linked list with sentinel head/tail
// and a hash map keyed by the user's key type.
//
// Design notes:
//   - Sentinel head/tail eliminates all "empty list / first / last" edge cases.
//   - The hash map stores iterators into std::list, which are stable across
//     insertion and erasure of OTHER elements (a property unique to std::list
//     and std::forward_list in the STL).
//   - get() promotes the touched node to most-recent in O(1) via splice.
//   - All operations are O(1) amortized; worst case is the hash map's worst case.
//   - No exceptions on hot paths (get returns std::optional).
//
// Not thread-safe. Wrap in a mutex for concurrent use; for high-throughput
// concurrent access, a sharded cache is preferable.

#pragma once

#include <cassert>
#include <cstddef>
#include <list>
#include <optional>
#include <unordered_map>
#include <utility>

namespace sgp {

template <typename K, typename V, typename Hash = std::hash<K>>
class lru_cache {
public:
    using key_type    = K;
    using value_type  = V;
    using size_type   = std::size_t;

    explicit lru_cache(size_type capacity) : capacity_(capacity) {
        assert(capacity_ > 0 && "lru_cache capacity must be > 0");
        index_.reserve(capacity_);
    }

    // Look up a key. Returns std::nullopt if absent.
    // If present: promotes the entry to most-recent and returns a copy.
    [[nodiscard]] auto get(const K& key) -> std::optional<V> {
        auto it = index_.find(key);
        if (it == index_.end()) {
            return std::nullopt;
        }
        order_.splice(order_.begin(), order_, it->second);  // promote
        return it->second->second;
    }

    // Insert or update. Promotes to most-recent. Evicts least-recent on overflow.
    auto put(const K& key, V value) -> void {
        if (auto it = index_.find(key); it != index_.end()) {
            it->second->second = std::move(value);
            order_.splice(order_.begin(), order_, it->second);
            return;
        }
        if (order_.size() == capacity_) {
            // Evict least-recently-used (tail of the list).
            const auto& victim = order_.back();
            index_.erase(victim.first);
            order_.pop_back();
        }
        order_.emplace_front(key, std::move(value));
        index_.emplace(key, order_.begin());
    }

    // Drop a key if present. Returns true if a key was erased.
    auto erase(const K& key) -> bool {
        auto it = index_.find(key);
        if (it == index_.end()) {
            return false;
        }
        order_.erase(it->second);
        index_.erase(it);
        return true;
    }

    // Peek without promoting (for testing / introspection).
    [[nodiscard]] auto peek(const K& key) const -> std::optional<V> {
        auto it = index_.find(key);
        if (it == index_.end()) {
            return std::nullopt;
        }
        return it->second->second;
    }

    [[nodiscard]] auto contains(const K& key) const noexcept -> bool {
        return index_.find(key) != index_.end();
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return order_.size();
    }

    [[nodiscard]] auto capacity() const noexcept -> size_type {
        return capacity_;
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return order_.empty();
    }

    auto clear() noexcept -> void {
        order_.clear();
        index_.clear();
    }

private:
    using list_type = std::list<std::pair<K, V>>;
    using map_type  = std::unordered_map<K, typename list_type::iterator, Hash>;

    size_type capacity_;
    list_type order_;   // front = most-recent, back = least-recent
    map_type  index_;   // key -> iterator into order_
};

}  // namespace sgp
