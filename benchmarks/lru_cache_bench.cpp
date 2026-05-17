// Micro-benchmarks for sgp::lru_cache.
// Run: ./build/benchmarks/sgp_bench

#include <sgp/lru_cache.hpp>

#include <random>
#include <vector>

#include <benchmark/benchmark.h>

namespace {

constexpr int kSeed = 12345;

// Sequential inserts (no evictions).
static void BM_PutSequential(benchmark::State& state) {
    const auto n = state.range(0);
    for (auto _ : state) {
        sgp::lru_cache<int, int> c{static_cast<std::size_t>(n)};
        for (int i = 0; i < n; ++i) {
            c.put(i, i);
        }
        benchmark::DoNotOptimize(c);
    }
    state.SetItemsProcessed(state.iterations() * n);
}
BENCHMARK(BM_PutSequential)->Range(1024, 1 << 16);

// Sequential inserts that overflow (every put after capacity evicts).
static void BM_PutWithEviction(benchmark::State& state) {
    const auto capacity = state.range(0);
    const auto inserts = capacity * 4;
    for (auto _ : state) {
        sgp::lru_cache<int, int> c{static_cast<std::size_t>(capacity)};
        for (int i = 0; i < inserts; ++i) {
            c.put(i, i);
        }
        benchmark::DoNotOptimize(c);
    }
    state.SetItemsProcessed(state.iterations() * inserts);
}
BENCHMARK(BM_PutWithEviction)->Range(1024, 1 << 14);

// Random gets on a half-full cache. Mix of hits and misses.
static void BM_GetMixed(benchmark::State& state) {
    const auto capacity = state.range(0);
    sgp::lru_cache<int, int> c{static_cast<std::size_t>(capacity)};
    for (int i = 0; i < capacity; ++i) {
        c.put(i, i);
    }

    std::mt19937 rng{kSeed};
    std::uniform_int_distribution<int> dist{0, capacity * 2};  // 50% miss rate
    std::vector<int> queries(8192);
    for (auto& q : queries) {
        q = dist(rng);
    }

    std::size_t idx = 0;
    for (auto _ : state) {
        auto v = c.get(queries[idx++ & 8191]);
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetMixed)->Range(1024, 1 << 16);

}  // namespace
