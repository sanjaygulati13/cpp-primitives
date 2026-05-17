include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
)
FetchContent_MakeAvailable(Catch2)

# Treat Catch2 headers as SYSTEM so warnings from inside them don't bubble up
# under -Werror. Belt-and-suspenders with the curated warning set in the root.
get_target_property(_catch2_includes Catch2::Catch2 INTERFACE_INCLUDE_DIRECTORIES)
if(_catch2_includes)
    set_target_properties(Catch2 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_catch2_includes}")
endif()

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
