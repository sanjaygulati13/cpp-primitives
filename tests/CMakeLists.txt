include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
)
FetchContent_MakeAvailable(Catch2)

add_executable(sgp_tests
    lru_cache_test.cpp
)
target_link_libraries(sgp_tests PRIVATE
    sgp::sgp
    sgp_warnings
    sgp_sanitizers
    Catch2::Catch2WithMain
)

include(CTest)
include(${catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(sgp_tests)
