cmake_minimum_required(VERSION 3.20)
project(sgp
    VERSION 0.1.0
    DESCRIPTION "Small C++ primitives, done right"
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Export compile_commands.json for clangd / clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(SGP_BUILD_TESTS         "Build unit tests"            OFF)
option(SGP_BUILD_BENCHMARKS    "Build micro-benchmarks"      OFF)
option(SGP_BUILD_EXAMPLES      "Build runnable examples"     OFF)
option(SGP_ENABLE_ASAN         "Enable AddressSanitizer"     OFF)
option(SGP_ENABLE_UBSAN        "Enable UB Sanitizer"         OFF)
option(SGP_ENABLE_TSAN         "Enable ThreadSanitizer"      OFF)
option(SGP_WARNINGS_AS_ERRORS  "Treat warnings as errors"    OFF)

# ============================================================
# The header-only library target
# ============================================================
add_library(sgp INTERFACE)
add_library(sgp::sgp ALIAS sgp)

target_include_directories(sgp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(sgp INTERFACE cxx_std_20)

# ============================================================
# Shared compile / sanitizer flags for tests / benchmarks / examples
# ============================================================
add_library(sgp_warnings INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(sgp_warnings INTERFACE
        -Wall -Wextra -Wpedantic
        -Wshadow -Wnon-virtual-dtor -Wold-style-cast
        -Wcast-align -Wunused -Woverloaded-virtual
        -Wconversion -Wsign-conversion
        -Wnull-dereference -Wdouble-promotion
        -Wformat=2 -Wimplicit-fallthrough
    )
    if(SGP_WARNINGS_AS_ERRORS)
        target_compile_options(sgp_warnings INTERFACE -Werror)
    endif()
endif()

# Sanitizer flags
add_library(sgp_sanitizers INTERFACE)
if(SGP_ENABLE_ASAN)
    target_compile_options(sgp_sanitizers INTERFACE -fsanitize=address -fno-omit-frame-pointer -g -O1)
    target_link_options(sgp_sanitizers INTERFACE -fsanitize=address)
endif()
if(SGP_ENABLE_UBSAN)
    target_compile_options(sgp_sanitizers INTERFACE -fsanitize=undefined -fno-omit-frame-pointer -g)
    target_link_options(sgp_sanitizers INTERFACE -fsanitize=undefined)
endif()
if(SGP_ENABLE_TSAN)
    # TSan is incompatible with ASan; choose one
    target_compile_options(sgp_sanitizers INTERFACE -fsanitize=thread -g -O1)
    target_link_options(sgp_sanitizers INTERFACE -fsanitize=thread)
endif()

# ============================================================
# Subdirectories
# ============================================================
if(SGP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SGP_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(SGP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ============================================================
# Install
# ============================================================
include(GNUInstallDirs)
install(TARGETS sgp EXPORT sgpTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT sgpTargets
    FILE sgpConfig.cmake
    NAMESPACE sgp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sgp
)
