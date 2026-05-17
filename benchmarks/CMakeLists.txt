include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.8.4
)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(sgp_bench
    lru_cache_bench.cpp
)
target_link_libraries(sgp_bench PRIVATE
    sgp::sgp
    sgp_warnings
    benchmark::benchmark
    benchmark::benchmark_main
)
