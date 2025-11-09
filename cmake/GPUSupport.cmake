# GPU Integration for TreeWidzard
# Add GPU support to the existing CMake configuration

# Detect platform and configure appropriate GPU support
# Allow users to force CPU-only builds and skip GPU detection entirely
if(DEFINED TREEWIDZARD_FORCE_CPU AND TREEWIDZARD_FORCE_CPU)
    message(STATUS "Forcing CPU-only build: skipping GPU detection")
    set(TREEWIDZARD_MPS_SUPPORT OFF)
    set(TREEWIDZARD_CUDA_SUPPORT OFF)
    set(TREEWIDZARD_OPENCL_SUPPORT OFF)
    set(GPU_LIBRARIES "")
    set(GPU_SOURCES "")
    set(GPU_HEADERS "")
    return()
endif()

if(APPLE)
    message(STATUS "Apple platform detected - configuring Metal/MPS support")
    
    # Find Metal framework
    find_library(METAL_FRAMEWORK Metal)
    find_library(MPS_FRAMEWORK MetalPerformanceShaders)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    
    if(METAL_FRAMEWORK AND MPS_FRAMEWORK AND FOUNDATION_FRAMEWORK)
        message(STATUS "Metal frameworks found - MPS acceleration available")
        set(TREEWIDZARD_MPS_SUPPORT ON)
        set(GPU_LIBRARIES ${METAL_FRAMEWORK} ${MPS_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
        set(GPU_COMPILE_DEFINITIONS TREEWIDZARD_MPS_ENABLED)
        
        # Add MPS source files
        set(GPU_SOURCES
            GPU/GPUAcceleration.cpp
            GPU/GPUSearchStrategy.cpp
            GPU/MPSAcceleration.cpp
            GPU/MPSSearchStrategy.cpp
        )
        
        set(GPU_HEADERS
            GPU/GPUAcceleration.h
            GPU/GPUSearchStrategy.h
            GPU/MPSAcceleration.h
            GPU/MPSSearchStrategy.h
        )
        
        # Set Objective-C++ for MPS files
        set_source_files_properties(
            GPU/MPSAcceleration.cpp
            GPU/MPSSearchStrategy.cpp
            PROPERTIES
            COMPILE_FLAGS "-x objective-c++"
        )
        
    else()
        message(STATUS "Metal frameworks not found - MPS acceleration disabled")
        set(TREEWIDZARD_MPS_SUPPORT OFF)
    endif()
    
else()
    # Non-Apple platforms - try CUDA and OpenCL
    message(STATUS "Non-Apple platform - configuring CUDA/OpenCL support")
    
    # Check for CUDA
    find_package(CUDA QUIET)
    if(CUDA_FOUND)
        enable_language(CUDA)
        message(STATUS "CUDA found: Version ${CUDA_VERSION}")
        set(TREEWIDZARD_CUDA_SUPPORT ON)
        
        # CUDA compilation flags
        set(CMAKE_CUDA_STANDARD 17)
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -O3 -use_fast_math")
        
        # GPU architecture support (adjust based on target hardware)
        set(CMAKE_CUDA_ARCHITECTURES "60;70;75;80;86")
        
    else()
        message(STATUS "CUDA not found - GPU acceleration disabled")
        set(TREEWIDZARD_CUDA_SUPPORT OFF)
    endif()

    # Check for OpenCL
    find_package(OpenCL QUIET)
    if(OpenCL_FOUND)
        message(STATUS "OpenCL found: Version ${OpenCL_VERSION}")
        set(TREEWIDZARD_OPENCL_SUPPORT ON)
    else()
        message(STATUS "OpenCL not found - OpenCL acceleration disabled")
        set(TREEWIDZARD_OPENCL_SUPPORT OFF)
    endif()

    # Standard GPU source files for non-Apple platforms
    set(GPU_SOURCES
        GPU/GPUAcceleration.cpp
        GPU/GPUSearchStrategy.cpp
    )

    set(GPU_HEADERS
        GPU/GPUAcceleration.h
        GPU/GPUSearchStrategy.h
    )
    
    # CUDA kernels (if CUDA is available)
    if(TREEWIDZARD_CUDA_SUPPORT)
        set(CUDA_SOURCES
            GPU/kernels/witness_kernels.cu
            GPU/kernels/dp_kernels.cu
        )
        
        # Create CUDA library
        add_library(TreeWidzard-CUDA ${CUDA_SOURCES})
        target_link_libraries(TreeWidzard-CUDA ${CUDA_LIBRARIES})
        
        # Set CUDA properties
        set_property(TARGET TreeWidzard-CUDA PROPERTY CUDA_STANDARD 17)
        set_property(TARGET TreeWidzard-CUDA PROPERTY CUDA_ARCHITECTURES ${CMAKE_CUDA_ARCHITECTURES})
        
        list(APPEND GPU_LIBRARIES TreeWidzard-CUDA)
    endif()

    # OpenCL library (if available)
    if(TREEWIDZARD_OPENCL_SUPPORT)
        list(APPEND GPU_LIBRARIES OpenCL::OpenCL)
    endif()
    
    # Set compile definitions for non-Apple platforms
    set(GPU_COMPILE_DEFINITIONS "")
    if(TREEWIDZARD_CUDA_SUPPORT)
        list(APPEND GPU_COMPILE_DEFINITIONS TREEWIDZARD_CUDA_ENABLED)
    endif()
    if(TREEWIDZARD_OPENCL_SUPPORT)
        list(APPEND GPU_COMPILE_DEFINITIONS TREEWIDZARD_OPENCL_ENABLED)
    endif()
    
endif()

# Create GPU acceleration library
if(TREEWIDZARD_CUDA_SUPPORT OR TREEWIDZARD_OPENCL_SUPPORT OR TREEWIDZARD_MPS_SUPPORT)
    add_library(TreeWidzard-GPU ${GPU_SOURCES} ${GPU_HEADERS})
    
    # Link with GPU libraries
    target_link_libraries(TreeWidzard-GPU ${GPU_LIBRARIES})
    
    # Include directories
    target_include_directories(TreeWidzard-GPU PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CUDA_INCLUDE_DIRS}
    )
    
    # Preprocessor definitions
    target_compile_definitions(TreeWidzard-GPU PUBLIC ${GPU_COMPILE_DEFINITIONS})
    
    if(TREEWIDZARD_MPS_SUPPORT)
        message(STATUS "MPS acceleration library created for Apple Silicon")
    else()
        message(STATUS "CUDA/OpenCL acceleration library created")
    endif()
    
    # Optional: Create GPU benchmark executable
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples/GPUBenchmark.cpp)
        add_executable(treewidzard-gpu-benchmark 
            examples/GPUBenchmark.cpp
            ${GPU_SOURCES}
        )
        target_link_libraries(treewidzard-gpu-benchmark 
            TreeWidzard-Core 
            TreeWidzard-GPU
            ${GPU_LIBRARIES}
        )
        
        # Set Objective-C++ for Apple platforms
        if(APPLE AND TREEWIDZARD_MPS_SUPPORT)
            set_source_files_properties(
                examples/GPUBenchmark.cpp
                PROPERTIES
                COMPILE_FLAGS "-x objective-c++"
            )
        endif()
    endif()
    
else()
    message(STATUS "No GPU libraries found - building CPU-only version")
endif()

# Installation
if(TARGET TreeWidzard-GPU)
    install(TARGETS TreeWidzard-GPU
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )
    
    install(FILES ${GPU_HEADERS}
        DESTINATION include/TreeWidzard/GPU
    )
endif()

# Testing
if((TREEWIDZARD_CUDA_SUPPORT OR TREEWIDZARD_MPS_SUPPORT) AND BUILD_TESTING)
    add_executable(gpu-tests
        tests/GPUTests.cpp
        ${GPU_SOURCES}
    )
    target_link_libraries(gpu-tests 
        TreeWidzard-Core
        TreeWidzard-GPU
        ${GPU_LIBRARIES}
        gtest
        gtest_main
    )
    
    # Set Objective-C++ for Apple platforms
    if(APPLE AND TREEWIDZARD_MPS_SUPPORT)
        set_source_files_properties(
            tests/GPUTests.cpp
            PROPERTIES
            COMPILE_FLAGS "-x objective-c++"
        )
    endif()
    
    add_test(NAME GPUTests COMMAND gpu-tests)
endif()

# Configuration summary
message(STATUS "TreeWidzard GPU Configuration:")
if(APPLE)
    message(STATUS "  Platform: Apple Silicon")
    message(STATUS "  MPS Support: ${TREEWIDZARD_MPS_SUPPORT}")
    if(TREEWIDZARD_MPS_SUPPORT)
        message(STATUS "  Metal Framework: Found")
        message(STATUS "  MPS Framework: Found")
    endif()
else()
    message(STATUS "  Platform: Non-Apple")
    message(STATUS "  CUDA Support: ${TREEWIDZARD_CUDA_SUPPORT}")
    message(STATUS "  OpenCL Support: ${TREEWIDZARD_OPENCL_SUPPORT}")
    if(TREEWIDZARD_CUDA_SUPPORT)
        message(STATUS "  CUDA Architectures: ${CMAKE_CUDA_ARCHITECTURES}")
    endif()
endif()
