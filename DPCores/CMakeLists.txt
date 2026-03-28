# DPCores/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(TreeWidzard-DPCores LANGUAGES CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler options
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
# Remove or comment out the following line if 'Scripts' does not exist
# include_directories(Scripts)

# Instead, include necessary directories based on your project structure
include_directories(
    ${PROJECT_SOURCE_DIR}/../Controller
    ${PROJECT_SOURCE_DIR}/../Kernel
    ${PROJECT_SOURCE_DIR}/../Parser/PropertyParser
    ${PROJECT_SOURCE_DIR}/../Controller/Parser
    ${PROJECT_SOURCE_DIR}/../ConcreteTreeDecomposition
    ${PROJECT_SOURCE_DIR}/../Translation/PACE/Parser
    ${PROJECT_SOURCE_DIR}/../Translation/TreeAutomaton
    # Add other directories as needed
)

# Set shared library prefix to empty (optional)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Find Threads package
find_package(Threads REQUIRED)

# Gather all .cpp files in Source/
file(GLOB files "Source/*.cpp")

foreach(file ${files})
    # Corrected from NAME_WLE to NAME_WE
    get_filename_component(core_name "${file}" NAME_WE)
    message("Adding core: ${core_name}")

    # Add shared library
    add_library(${core_name} SHARED ${file})

    # Specify include directories for the target
    target_include_directories(${core_name} PRIVATE
        $ENV{TREEWIDZARD}            # Ensure this environment variable is correctly set
        ${PROJECT_SOURCE_DIR}/../Controller
        ${PROJECT_SOURCE_DIR}/../Kernel
        ${PROJECT_SOURCE_DIR}/../Parser/PropertyParser
        ${PROJECT_SOURCE_DIR}/../Controller/Parser
        ${PROJECT_SOURCE_DIR}/../ConcreteTreeDecomposition
        ${PROJECT_SOURCE_DIR}/../Translation/PACE/Parser
        ${PROJECT_SOURCE_DIR}/../Translation/TreeAutomaton
        # Add other directories as needed
    )

    # Link against the core library and Threads
    target_link_libraries(${core_name} PRIVATE
        TreeWidzard-Core
        Threads::Threads
    )

    if(DEFINED TREEWIDZARD_PLUGIN_INSTALL_RPATH)
        set_target_properties(${core_name} PROPERTIES
            INSTALL_RPATH "${TREEWIDZARD_PLUGIN_INSTALL_RPATH}"
        )
    endif()

    install(TARGETS ${core_name}
        LIBRARY DESTINATION ${TREEWIDZARD_INSTALL_DPCORES_DIR}
        RUNTIME DESTINATION ${TREEWIDZARD_INSTALL_DPCORES_DIR}
    )
endforeach()
