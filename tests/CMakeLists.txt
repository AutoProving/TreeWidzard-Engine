cmake_minimum_required(VERSION 3.10)

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)

if(GTest_FOUND)
    # Include test directories
    include_directories(${PROJECT_SOURCE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/tests)

    # Test sources
    set(TEST_SOURCES
        tests/test_conjecture.cpp
        tests/test_dynamic_core.cpp
        tests/test_tree_decomposition.cpp
        tests/test_search_strategies.cpp
        tests/test_integration.cpp
    )

    # Create test executable
    add_executable(run_tests ${TEST_SOURCES})
    
    # Link test executable
    target_link_libraries(run_tests 
        TreeWidzard-Core
        GTest::GTest 
        GTest::Main
        ${CMAKE_DL_LIBS}
        Threads::Threads
    )

    # Add tests to CTest
    add_test(NAME TreeWidzardTests COMMAND run_tests)
    
    # Custom test target
    add_custom_target(test_all
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        DEPENDS run_tests
    )
    
else()
    message(WARNING "Google Test not found. Tests will not be built.")
    message(STATUS "To install Google Test on Ubuntu: sudo apt-get install libgtest-dev")
    message(STATUS "To install Google Test on macOS: brew install googletest")
endif()

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