cmake_minimum_required(VERSION 3.18)
project(SimpleMPSTest LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if we're on Apple platform
if(APPLE)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(MPS_FRAMEWORK MetalPerformanceShaders REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    
    message(STATUS "Found Metal framework: ${METAL_FRAMEWORK}")
    message(STATUS "Found MPS framework: ${MPS_FRAMEWORK}")
    message(STATUS "Found Foundation framework: ${FOUNDATION_FRAMEWORK}")
    
    # Create simple MPS test executable
    add_executable(simple_mps_test simple_mps_test.cpp)
    
    # Set Objective-C++ compilation for Apple frameworks
    set_source_files_properties(simple_mps_test.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
    
    # Link Apple frameworks
    target_link_libraries(simple_mps_test 
        ${METAL_FRAMEWORK}
        ${MPS_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
    )
    
    # Compiler flags for Apple Silicon
    target_compile_definitions(simple_mps_test PRIVATE __APPLE__)
    target_compile_options(simple_mps_test PRIVATE -Wall -Wextra -O2)
    
else()
    message(STATUS "Not on Apple platform - creating dummy test")
    add_executable(simple_mps_test simple_mps_test.cpp)
    target_compile_options(simple_mps_test PRIVATE -Wall -Wextra -O2)
endif()