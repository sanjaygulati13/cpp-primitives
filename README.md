# sgp — Small C++ Primitives, Done Right

[![CI](https://github.com/sanjaygulati13/cpp-primitives/actions/workflows/ci.yml/badge.svg)](https://github.com/sanjaygulati13/cpp-primitives/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Header-only C++20 library of carefully-engineered building blocks. Each primitive is small enough to read in one sitting, tested under ASan/UBSan/TSan, and benchmarked against the obvious alternatives.

## What's inside

| Primitive | Header | What it is | Status |
|---|---|---|---|
| `lru_cache<K,V>` | `<sgp/lru_cache.hpp>` | O(1) LRU cache (intrusive DLL + flat hash) | ✅ Ready |
| `bounded_queue<T>` | `<sgp/bounded_queue.hpp>` | Thread-safe bounded MPMC queue (mutex + condvar) | 🚧 Planned |
| `thread_pool` | `<sgp/thread_pool.hpp>` | Fixed-size pool with future-returning `submit` | 🚧 Planned |
| `type_erased_function<R(Args...)>` | `<sgp/function.hpp>` | Educational `std::function` clone, SBO-aware | 🚧 Planned |

## Why this exists

Most C++ "primitive" libraries are either gigantic (Boost) or unfit for production (LeetCode dumps). This is the in-between: **small enough to audit, real enough to use, modern enough to read.** I built it as a study artifact for staff-level C++ interviews, then kept it because the code is genuinely useful.

## Install

Header-only. Drop `include/sgp/` into your project, or via CMake `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
  sgp
  GIT_REPOSITORY https://github.com/sanjaygulati13/cpp-primitives.git
  GIT_TAG main
)
FetchContent_MakeAvailable(sgp)
target_link_libraries(my_app PRIVATE sgp::sgp)
```

## 30-second example

```cpp
#include <sgp/lru_cache.hpp>
#include <iostream>

int main() {
    sgp::lru_cache<int, std::string> cache{/*capacity=*/3};

    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    if (auto v = cache.get(1)) {
        std::cout << "1 → " << *v << "\n";  // promotes 1 to most-recent
    }

    cache.put(4, "four");  // evicts 2 (least-recent)
    std::cout << "size = " << cache.size() << "\n";  // 3
}
```

See `examples/` for more.

## Quality bar

- **C++20**, no warnings on `-Wall -Wextra -Wpedantic -Werror`
- All tests run under **AddressSanitizer + UndefinedBehaviorSanitizer**
- Multi-threaded primitives also run under **ThreadSanitizer**
- **clang-tidy** with `modernize-*`, `bugprone-*`, `performance-*`, `cppcoreguidelines-*`
- **clang-format** enforced in CI
- Benchmarks against `std::list`+`std::unordered_map` baseline (see `docs/lru_cache.md`)

## Build + test locally

```bash
git clone https://github.com/sanjaygulati13/cpp-primitives.git
cd cpp-primitives
cmake -B build -DSGP_BUILD_TESTS=ON -DSGP_BUILD_BENCHMARKS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Design philosophy

1. **Header-only, single-include per primitive.** No build complexity for users.
2. **No exceptions in hot paths.** Errors via `std::optional` or status codes.
3. **STL-compatible signatures.** Drop-in for educational use.
4. **One file per primitive.** Less than 200 lines wherever possible. Reads like a textbook.
5. **Tests document behavior.** Read `tests/lru_cache_test.cpp` to understand the contract.

## Repo layout

```
cpp-primitives/
├── include/sgp/        ← header-only library
├── tests/              ← Catch2 unit tests
├── benchmarks/         ← google benchmark micro-benchmarks
├── examples/           ← runnable usage examples
├── docs/               ← per-primitive design rationale + trade-offs
└── .github/workflows/  ← CI: build, sanitize, lint, format-check
```

## Contributing

This is a personal study project, not actively seeking contributions — but if you spot a bug or a clearer implementation, open an issue or PR. See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT. See [LICENSE](LICENSE).

## Why C++20?

C++20 gives concepts, ranges, three-way comparison, designated initializers — modernizations that materially improve primitive design. Backporting to C++17 is possible but I won't; if you need C++17, the implementations are simple enough to port yourself.

## Author

Sanjay Gulati ([sanjaygulati.dev](https://sanjaygulati.dev)). Building this as part of a 22-week study cycle. Companion blog posts on each primitive at [sanjaygulati.dev/blog](https://sanjaygulati.dev/blog).
