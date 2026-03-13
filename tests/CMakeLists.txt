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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)

# Unit test sources
set(UNIT_TEST_SOURCES
    unit/test_bag_operations.cpp
    unit/test_core_support.cpp
    unit/test_witness_cache.cpp
)

# Integration test sources
set(INTEGRATION_TEST_SOURCES
    integration/test_atp_workflow.cpp
    integration/test_certificates.cpp
    integration/test_public_api.cpp
)

# Original test
set(LEGACY_TEST_SOURCES
    test_conjecture.cpp
)

# Create unit test executable
add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests 
    TreeWidzard-Core
    GTest::gtest_main
    ${CMAKE_DL_LIBS}
    Threads::Threads
)

# Create integration test executable
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests 
    TreeWidzard-Core
    GTest::gtest_main
    ${CMAKE_DL_LIBS}
    Threads::Threads
)

# Integration tests invoke `treewidzard.sh`, which depends on the main executable
# and the dynamically loaded search strategy plugins being built.
add_dependencies(integration_tests
    treewidzard
    BreadthFirstSearch
    IsomorphismBreadthFirstSearch
    ParallelBreadthFirstSearch
    ParallelIsomorphismBreadthFirstSearch
)

# Create legacy test executable (keep existing tests)
add_executable(run_tests ${LEGACY_TEST_SOURCES})
target_link_libraries(run_tests 
    TreeWidzard-Core
    GTest::gtest_main
    ${CMAKE_DL_LIBS}
    Threads::Threads
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)
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
