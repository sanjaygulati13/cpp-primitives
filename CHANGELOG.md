# Changelog

All notable changes to `sgp` are documented here.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Planned
- `sgp::bounded_queue<T>` — thread-safe bounded MPMC queue (mutex + condvar)
- `sgp::thread_pool` — fixed-size pool with future-returning `submit`
- `sgp::type_erased_function<R(Args...)>` — educational `std::function` clone with SBO

## [0.1.0] — 2026-05-17

### Added
- `sgp::lru_cache<K, V>` — O(1) LRU cache, header-only
- CI: build (gcc + clang × Debug + Release), sanitizers (ASan/UBSan/TSan), clang-tidy, clang-format
- Catch2 unit tests
- google benchmark micro-benchmarks
- Runnable example
- `docs/lru_cache.md` design rationale
