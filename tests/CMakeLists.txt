cmake_minimum_required(VERSION 3.10)

# Fetch GoogleTest using FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Include test directories
include_directories(${PROJECT_SOURCE_DIR})

# Test sources
set(TEST_SOURCES
    test_conjecture.cpp
)

# Create test executable
add_executable(run_tests ${TEST_SOURCES})

# Link test executable
target_link_libraries(run_tests 
    TreeWidzard-Core
    GTest::gtest_main
    ${CMAKE_DL_LIBS}
    Threads::Threads
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(run_tests)


# Performance benchmarks (optional)
find_package(benchmark QUIET)
if(benchmark_FOUND)
    add_executable(benchmarks
        tests/benchmarks/benchmark_dp_cores.cpp
        tests/benchmarks/benchmark_search_strategies.cpp
    )
    
    target_link_libraries(benchmarks
        TreeWidzard-Core
        benchmark::benchmark
        ${CMAKE_DL_LIBS}
        Threads::Threads
    )
endif()