// Minimal example: use sgp::lru_cache to memoize an expensive function.

#include <sgp/lru_cache.hpp>

#include <iostream>
#include <string>

// Pretend this is expensive (DB lookup, network call, etc.).
static auto fetch_user_name(int user_id) -> std::string {
    return "user_" + std::to_string(user_id);
}

auto main() -> int {
    sgp::lru_cache<int, std::string> cache{/*capacity=*/100};

    auto get_name = [&](int id) -> std::string {
        if (auto cached = cache.get(id)) {
            return *cached;
        }
        auto name = fetch_user_name(id);
        cache.put(id, name);
        return name;
    };

    std::cout << get_name(42) << "\n";  // miss -> fetch -> cache
    std::cout << get_name(42) << "\n";  // hit  -> direct from cache
    std::cout << "cache size: " << cache.size() << "\n";

    for (int i = 0; i < 150; ++i) {
        get_name(i);
    }
    std::cout << "after 150 inserts: " << cache.size() << " (capped at " << cache.capacity()
              << ")\n";
}
