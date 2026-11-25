cmake_minimum_required(VERSION 3.10)
project(HashMapProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release (optimised) so benchmark timings are meaningful
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# The hash map itself, shared by all three programs
add_library(hashmap src/HashMap.cpp)
target_include_directories(hashmap PUBLIC include)

# Demo program (main.cpp)
add_executable(demo "src/main.cpp")
target_link_libraries(demo PRIVATE hashmap)

# Unit tests
add_executable(tests tests/tests.cpp)
target_link_libraries(tests PRIVATE hashmap)

# Performance benchmark
add_executable(benchmark benchmark/benchmark.cpp)
target_link_libraries(benchmark PRIVATE hashmap)

enable_testing()
add_test(NAME hashmap_tests COMMAND tests)
