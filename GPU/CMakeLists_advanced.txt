cmake_minimum_required(VERSION 3.18)
project(AdvancedMPSDemo LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include TreeWidzard core components
set(TREEWIDZARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Check if we're on Apple platform
if(APPLE)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(MPS_FRAMEWORK MetalPerformanceShaders REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    
    message(STATUS "Building Advanced MPS Demo for Apple Silicon")
    message(STATUS "Found Metal framework: ${METAL_FRAMEWORK}")
    message(STATUS "Found MPS framework: ${MPS_FRAMEWORK}")
    message(STATUS "Found Foundation framework: ${FOUNDATION_FRAMEWORK}")
    
    # TreeWidzard core library (minimal for demo)
    add_library(TreeWidzard-Core-Advanced STATIC
        ${TREEWIDZARD_ROOT}/Kernel/Bag.cpp
        ${TREEWIDZARD_ROOT}/Kernel/State.cpp
        ${TREEWIDZARD_ROOT}/Kernel/Witness.cpp
        ${TREEWIDZARD_ROOT}/Kernel/WitnessSet.cpp
        ${TREEWIDZARD_ROOT}/Kernel/SearchStrategy.cpp
    )
    
    target_include_directories(TreeWidzard-Core-Advanced PUBLIC ${TREEWIDZARD_ROOT})
    target_compile_options(TreeWidzard-Core-Advanced PRIVATE -Wall -Wextra -O2)
    
    # Advanced MPS library
    add_library(TreeWidzard-AdvancedMPS STATIC
        AdvancedMPSAcceleration.cpp
    )
    
    target_include_directories(TreeWidzard-AdvancedMPS PUBLIC 
        ${TREEWIDZARD_ROOT}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Set Objective-C++ compilation for MPS files
    set_source_files_properties(AdvancedMPSAcceleration.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
    
    target_link_libraries(TreeWidzard-AdvancedMPS 
        TreeWidzard-Core-Advanced
        ${METAL_FRAMEWORK}
        ${MPS_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
    )
    
    target_compile_definitions(TreeWidzard-AdvancedMPS PRIVATE __APPLE__)
    target_compile_options(TreeWidzard-AdvancedMPS PRIVATE -Wall -Wextra -O2)
    
    # Advanced MPS demo executable
    add_executable(advanced_mps_demo advanced_mps_demo.cpp)
    
    # Set Objective-C++ compilation for demo file too
    set_source_files_properties(advanced_mps_demo.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
    
    target_link_libraries(advanced_mps_demo 
        TreeWidzard-AdvancedMPS
        TreeWidzard-Core-Advanced
        ${METAL_FRAMEWORK}
        ${MPS_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
    )
    
    target_compile_definitions(advanced_mps_demo PRIVATE __APPLE__)
    target_compile_options(advanced_mps_demo PRIVATE -Wall -Wextra -O2)
    
    # Copy Metal shader files to build directory
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/chromatic_number.metal 
                   ${CMAKE_CURRENT_BINARY_DIR}/chromatic_number.metal COPYONLY)
    
else()
    message(STATUS "Not on Apple platform - creating CPU-only demo")
    
    # TreeWidzard core library
    add_library(TreeWidzard-Core-Advanced STATIC
        ${TREEWIDZARD_ROOT}/Kernel/Bag.cpp
        ${TREEWIDZARD_ROOT}/Kernel/State.cpp
        ${TREEWIDZARD_ROOT}/Kernel/Witness.cpp
        ${TREEWIDZARD_ROOT}/Kernel/WitnessSet.cpp
        ${TREEWIDZARD_ROOT}/Kernel/SearchStrategy.cpp
    )
    
    target_include_directories(TreeWidzard-Core-Advanced PUBLIC ${TREEWIDZARD_ROOT})
    target_compile_options(TreeWidzard-Core-Advanced PRIVATE -Wall -Wextra -O2)
    
    # MPS library (CPU fallback only)
    add_library(TreeWidzard-AdvancedMPS STATIC
        AdvancedMPSAcceleration.cpp
    )
    
    target_include_directories(TreeWidzard-AdvancedMPS PUBLIC 
        ${TREEWIDZARD_ROOT}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    target_link_libraries(TreeWidzard-AdvancedMPS TreeWidzard-Core-Advanced)
    target_compile_options(TreeWidzard-AdvancedMPS PRIVATE -Wall -Wextra -O2)
    
    # Demo executable
    add_executable(advanced_mps_demo advanced_mps_demo.cpp)
    target_link_libraries(advanced_mps_demo 
        TreeWidzard-AdvancedMPS 
        TreeWidzard-Core-Advanced
    )
    target_compile_options(advanced_mps_demo PRIVATE -Wall -Wextra -O2)
    
endif()

# Print configuration summary
message(STATUS "Advanced MPS Demo Configuration:")
message(STATUS "  - C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  - Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  - Platform: ${CMAKE_SYSTEM_NAME}")
if(APPLE)
    message(STATUS "  - Apple Silicon GPU: ENABLED")
    message(STATUS "  - Metal Shaders: AVAILABLE")
else()
    message(STATUS "  - Apple Silicon GPU: DISABLED (not on Apple platform)")
endif()