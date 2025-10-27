# GPU Integration for TreeWidzard
# Add GPU support to the existing CMake configuration

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

# GPU source files
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

# Create GPU acceleration library
if(TREEWIDZARD_CUDA_SUPPORT OR TREEWIDZARD_OPENCL_SUPPORT)
    add_library(TreeWidzard-GPU ${GPU_SOURCES} ${GPU_HEADERS})
    
    # Link with GPU libraries
    target_link_libraries(TreeWidzard-GPU ${GPU_LIBRARIES})
    
    # Include directories
    target_include_directories(TreeWidzard-GPU PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CUDA_INCLUDE_DIRS}
    )
    
    # Preprocessor definitions
    if(TREEWIDZARD_CUDA_SUPPORT)
        target_compile_definitions(TreeWidzard-GPU PUBLIC TREEWIDZARD_CUDA_ENABLED)
    endif()
    
    if(TREEWIDZARD_OPENCL_SUPPORT)
        target_compile_definitions(TreeWidzard-GPU PUBLIC TREEWIDZARD_OPENCL_ENABLED)
    endif()
    
    # Link main executable with GPU library
    target_link_libraries(treewidzard TreeWidzard-GPU)
    
    message(STATUS "GPU acceleration enabled")
    
    # Optional: Create GPU benchmark executable
    add_executable(treewidzard-gpu-benchmark 
        examples/GPUBenchmark.cpp
        ${GPU_SOURCES}
    )
    target_link_libraries(treewidzard-gpu-benchmark 
        TreeWidzard-Core 
        TreeWidzard-GPU
        ${GPU_LIBRARIES}
    )
    
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
if(TREEWIDZARD_CUDA_SUPPORT AND BUILD_TESTING)
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
    add_test(NAME GPUTests COMMAND gpu-tests)
endif()

# Configuration summary
message(STATUS "TreeWidzard GPU Configuration:")
message(STATUS "  CUDA Support: ${TREEWIDZARD_CUDA_SUPPORT}")
message(STATUS "  OpenCL Support: ${TREEWIDZARD_OPENCL_SUPPORT}")
if(TREEWIDZARD_CUDA_SUPPORT)
    message(STATUS "  CUDA Architectures: ${CMAKE_CUDA_ARCHITECTURES}")
endif()