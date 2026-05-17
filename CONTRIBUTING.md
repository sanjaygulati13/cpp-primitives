# Contributing

This is primarily a personal study repo, but bug reports and clarifying PRs are welcome.

## Bug reports

Open an issue with:
- A minimal repro (≤ 30 lines of code that triggers the bug)
- Expected vs actual behavior
- Compiler + version, OS, build configuration (sanitizers on/off)

## Pull requests

Before opening a PR, please run locally:

```bash
cmake -B build -DSGP_BUILD_TESTS=ON -DSGP_BUILD_EXAMPLES=ON \
               -DSGP_ENABLE_ASAN=ON -DSGP_ENABLE_UBSAN=ON
cmake --build build
ctest --test-dir build --output-on-failure
clang-format --dry-run --Werror include/**/*.hpp tests/*.cpp
```

CI will reject PRs that:
- Don't compile with `-Wall -Wextra -Wpedantic -Werror`
- Fail any test
- Trigger ASan / UBSan / TSan
- Trigger any clang-tidy check in `.clang-tidy`
- Aren't `clang-format`-clean

## New primitive checklist

Adding a new primitive (`include/sgp/foo.hpp`)? You also need:

- `tests/foo_test.cpp` — full Catch2 test coverage
- `benchmarks/foo_bench.cpp` — at least one micro-benchmark
- `examples/foo_example.cpp` — 30-line runnable example
- `docs/foo.md` — design rationale doc following the `lru_cache.md` shape
- README.md update — add row to the primitives table

## Style

Follow `.clang-format`. The `.clang-tidy` config is the source of truth for naming conventions and code patterns.

Keep each primitive header < 200 lines wherever practical. If it doesn't fit, split it.
