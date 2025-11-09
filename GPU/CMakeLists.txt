cmake_minimum_required(VERSION 3.18)
project(ComprehensiveBenchmark LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include TreeWidzard core components
set(TREEWIDZARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(APPLE)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(MPS_FRAMEWORK MetalPerformanceShaders REQUIRED) 
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    
    message(STATUS "Building Comprehensive Benchmark for Apple Silicon")
    
    # TreeWidzard minimal core (what we need for benchmarking)
    add_library(TreeWidzard-Benchmark-Core STATIC
        ${TREEWIDZARD_ROOT}/Kernel/Bag.cpp
        ${TREEWIDZARD_ROOT}/Kernel/State.cpp
    )
    
    target_include_directories(TreeWidzard-Benchmark-Core PUBLIC ${TREEWIDZARD_ROOT})
    target_compile_options(TreeWidzard-Benchmark-Core PRIVATE -Wall -Wextra -O3)
    
    # Apple GPU interface library (reuse from previous)
    add_library(AppleGPUInterface STATIC apple_gpu_interface.mm)
    set_source_files_properties(apple_gpu_interface.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
    target_link_libraries(AppleGPUInterface
        ${METAL_FRAMEWORK}
        ${MPS_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
    )
    target_compile_definitions(AppleGPUInterface PRIVATE __APPLE__)
    
    # Comprehensive benchmark executable (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/comprehensive_benchmark.cpp)
        add_executable(comprehensive_benchmark comprehensive_benchmark.cpp)
        target_link_libraries(comprehensive_benchmark 
            AppleGPUInterface 
            TreeWidzard-Benchmark-Core
        )
        target_compile_definitions(comprehensive_benchmark PRIVATE __APPLE__)
        target_compile_options(comprehensive_benchmark PRIVATE -Wall -Wextra -O3)
    else()
        message(STATUS "Skipping comprehensive_benchmark: source not found")
    endif()

    # Real problem benchmark executable (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/real_problem_benchmark.cpp)
        add_executable(real_problem_benchmark real_problem_benchmark.cpp)
        target_link_libraries(real_problem_benchmark 
            AppleGPUInterface 
            TreeWidzard-Benchmark-Core
        )
        target_compile_definitions(real_problem_benchmark PRIVATE __APPLE__)
        target_compile_options(real_problem_benchmark PRIVATE -Wall -Wextra -O3)
    else()
        message(STATUS "Skipping real_problem_benchmark: source not found")
    endif()

    # Pathwidth 5+ large scale benchmark (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pathwidth5_benchmark.cpp)
        add_executable(pathwidth5_benchmark pathwidth5_benchmark.cpp)
        target_link_libraries(pathwidth5_benchmark 
            AppleGPUInterface 
            TreeWidzard-Benchmark-Core
        )
        target_compile_definitions(pathwidth5_benchmark PRIVATE __APPLE__)
        target_compile_options(pathwidth5_benchmark PRIVATE -Wall -Wextra -O3)
    else()
        message(STATUS "Skipping pathwidth5_benchmark: source not found")
    endif()

    # Pathwidth 5 test (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pathwidth5_test.cpp)
        add_executable(pathwidth5_test pathwidth5_test.cpp)
        target_link_libraries(pathwidth5_test 
            TreeWidzard-Benchmark-Core
        )
        target_compile_options(pathwidth5_test PRIVATE -Wall -Wextra -O3)
    else()
        message(STATUS "Skipping pathwidth5_test: source not found")
    endif()

    # Pathwidth 5 ATP test (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pathwidth5_atp_test.cpp)
        add_executable(pathwidth5_atp_test pathwidth5_atp_test.cpp)
        target_link_libraries(pathwidth5_atp_test 
            TreeWidzard-Benchmark-Core
        )
        target_compile_options(pathwidth5_atp_test PRIVATE -Wall -Wextra -O3)
    else()
        message(STATUS "Skipping pathwidth5_atp_test: source not found")
    endif()

    # ATP GPU vs CPU benchmark for pathwidth 5 (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/atp_pathwidth5_simple.cpp)
        add_executable(atp_pathwidth5_benchmark atp_pathwidth5_simple.cpp)
        target_link_libraries(atp_pathwidth5_benchmark 
            TreeWidzard-Benchmark-Core
        )
        target_compile_options(atp_pathwidth5_benchmark PRIVATE -Wall -Wextra -O3)
    else()
        message(STATUS "Skipping atp_pathwidth5_benchmark: source not found")
    endif()

    # ATP GPU vs CPU benchmark for pathwidth 6 (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/atp_pathwidth6_benchmark.cpp)
        add_executable(atp_pathwidth6_benchmark atp_pathwidth6_benchmark.cpp)
        target_link_libraries(atp_pathwidth6_benchmark 
            TreeWidzard-Benchmark-Core
        )
        target_compile_options(atp_pathwidth6_benchmark PRIVATE -Wall -Wextra -O3)
    else()
        message(STATUS "Skipping atp_pathwidth6_benchmark: source not found")
    endif()

    # Simple demo (optional)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/simple_advanced_demo.cpp)
        add_executable(simple_advanced_demo simple_advanced_demo.cpp)
        target_link_libraries(simple_advanced_demo AppleGPUInterface)
        target_compile_definitions(simple_advanced_demo PRIVATE __APPLE__)
        target_compile_options(simple_advanced_demo PRIVATE -Wall -Wextra -O2)
    else()
        message(STATUS "Skipping simple_advanced_demo: source not found")
    endif()

elseif(TREEWIDZARD_CUDA_SUPPORT)
    # ...existing code for CUDA targets...
else()
    message(STATUS "TREEWIDZARD_CUDA_SUPPORT is OFF - skipping GPU/CUDA targets")
endif()

message(STATUS "Comprehensive Benchmark Configuration Complete")
message(STATUS "Available targets:")
message(STATUS "  - comprehensive_benchmark: Full GPU vs CPU comparison")
message(STATUS "  - simple_advanced_demo: Simple demo from previous test")
