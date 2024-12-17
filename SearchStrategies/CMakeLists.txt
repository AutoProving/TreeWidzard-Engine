# cmake_minimum_required(VERSION 3.0.0)
# set(CMAKE_CXX_STANDARD 20)
# # link_libraries(-lstdc++fs)

# if (MSVC)
#   # warning level 4 and all warnings as errors
#   add_compile_options(/W4 /WX)
# else()
#   add_compile_options(-Werror)
#   add_compile_options(-Wall -Wextra -pedantic)
# endif()
# include_directories(Scripts)
# # link_libraries(stdc++fs)

# project("TreeWidzard-SearchStrategies")

# message("Targeting TreeWidzard source at: $ENV{TREEWIDZARD}")

# set(CMAKE_SHARED_LIBRARY_PREFIX "")
# file(GLOB files
#   "Source/*.cpp"
#   "Source/**/*.cpp"
# )
# foreach(file ${files})
#   get_filename_component(search_strategy_name "${file}" NAME_WLE)
#   message("Adding search strategy: ${search_strategy_name}")
#   add_library(${search_strategy_name} SHARED ${file})
#   target_include_directories(${search_strategy_name} PRIVATE $ENV{TREEWIDZARD})
# endforeach()

######
cmake_minimum_required(VERSION 3.10)
project(TreeWidzard-SearchStrategies)

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
    ${PROJECT_SOURCE_DIR}/../Conjecture
    ${PROJECT_SOURCE_DIR}/../TreeAutomaton
    ${PROJECT_SOURCE_DIR}/../Multigraph
    # Add other directories as needed
)

# Set shared library prefix to empty (optional)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Find Threads package
find_package(Threads REQUIRED)

# Gather all .cpp files in Source/
file(GLOB files "Source/*.cpp" "Source/**/*.cpp"
)

foreach(file ${files})
    # Corrected from NAME_WLE to NAME_WE
    get_filename_component(search_strategy_name "${file}" NAME_WE)
    message("Adding search strategy: ${search_strategy_name}")

    # Add shared library
    add_library(${search_strategy_name} SHARED ${file})

    # Specify include directories for the target
    target_include_directories(${search_strategy_name} PRIVATE
        $ENV{TREEWIDZARD}            # Ensure this environment variable is correctly set
        ${PROJECT_SOURCE_DIR}/../Controller
        ${PROJECT_SOURCE_DIR}/../Kernel
        ${PROJECT_SOURCE_DIR}/../Parser/PropertyParser
        ${PROJECT_SOURCE_DIR}/../Controller/Parser
        ${PROJECT_SOURCE_DIR}/../ConcreteTreeDecomposition
        ${PROJECT_SOURCE_DIR}/../Translation/PACE/Parser
        ${PROJECT_SOURCE_DIR}/../Translation/TreeAutomaton
        ${PROJECT_SOURCE_DIR}/../Conjecture
        ${PROJECT_SOURCE_DIR}/../TreeAutomaton
        ${PROJECT_SOURCE_DIR}/../Multigraph


        # Add other directories as needed
    )

    # Link against the core library and Threads
    target_link_libraries(${search_strategy_name} PRIVATE
        TreeWidzard-Core
        Threads::Threads
    )
endforeach()
