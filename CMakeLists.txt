# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(treewidzard)

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

# Optimization and debugging flags
add_compile_options(-O3 -g3)

# Linker flags (choose one based on your needs)
# Avoid setting both "-static" and "-rdynamic" as they may conflict
# set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

# Define compile definitions
add_compile_definitions(YY_NO_INPUT=1 YY_NO_UNPUT=1)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define default paths for DPCores and SearchStrategies
set(TREEWIDZARD_DPCORES_DEFAULT ${CMAKE_CURRENT_BINARY_DIR}/DPCores:${TREEWIDZARD_DPCORES_DEFAULT})
set(TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT ${CMAKE_CURRENT_BINARY_DIR}/SearchStrategies:${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT})
add_compile_definitions(TREEWIDZARD_DPCORES_DEFAULT="${TREEWIDZARD_DPCORES_DEFAULT}")
add_compile_definitions(TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT="${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT}")

# Set the environment variable for subprojects
set(ENV{TREEWIDZARD} ${PROJECT_SOURCE_DIR})

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/Controller
    ${PROJECT_SOURCE_DIR}/Kernel
    ${PROJECT_SOURCE_DIR}/Parser/PropertyParser
    ${PROJECT_SOURCE_DIR}/Controller/Parser
    ${PROJECT_SOURCE_DIR}/ConcreteTreeDecomposition
    ${PROJECT_SOURCE_DIR}/Translation/PACE/Parser
    ${PROJECT_SOURCE_DIR}/Translation/TreeAutomaton
    ${PROJECT_SOURCE_DIR}/Conjecture
    ${PROJECT_SOURCE_DIR}/TreeAutomaton
    ${PROJECT_SOURCE_DIR}/Multigraph
    ${PROJECT_SOURCE_DIR}/Performance

    # Add other directories as needed
)

# Define the core library
add_library(TreeWidzard-Core STATIC
    Kernel/DynamicCore.cpp
    Kernel/Witness.cpp
    Kernel/WitnessSet.cpp
    Kernel/DynamicKernel.cpp
    Kernel/Flags.cpp
    Kernel/SearchStrategy.cpp
    Kernel/State.cpp
    Kernel/Bag.cpp
    Kernel/Width.cpp
    Kernel/StateTree.cpp
    Kernel/DynamicCoreHandler.cpp
    Kernel/SearchStrategyHandler.cpp
    Conjecture/Conjecture.cpp
    Conjecture/PropertyAssignment.cpp
    TreeAutomaton/TreeAutomaton.cpp
    TreeAutomaton/InstructiveTreeDecomposition.cpp
    TreeAutomaton/ConcreteTreeDecomposition.cpp
    TreeAutomaton/DecompositionTree.cpp
    TreeAutomaton/Term.cpp
    TreeAutomaton/RunTree.cpp
    Multigraph/MultiGraph.cpp
    # Add other core source files here if any
)

# Specify include directories for the core library
# Already included via main include_directories

# Find and link Threads (if needed)
find_package(Threads REQUIRED)

# Find NUMA library for performance optimization
find_library(NUMA_LIBRARY numa)
if(NUMA_LIBRARY)
    message(STATUS "Found NUMA library: ${NUMA_LIBRARY}")
    add_compile_definitions(NUMA_AVAILABLE=1)
    set(NUMA_LIBRARIES ${NUMA_LIBRARY})
else()
    message(STATUS "NUMA library not found - proceeding without NUMA optimization")
    set(NUMA_LIBRARIES "")
endif()

target_link_libraries(TreeWidzard-Core PRIVATE Threads::Threads ${NUMA_LIBRARIES})

# Define the executable and link against the core library
add_executable(treewidzard
    Controller/SearchController.cpp
    Controller/SearchController.h
    Controller/ParseController.cpp
    Controller/ParseController.h
    Controller/InputController.cpp
    Controller/InputController.h
    Controller/AuxiliaryController.hpp
    main.cpp
    # Kernel files 
    Kernel/CoreWrapper.h
    Kernel/Witness.cpp
    Kernel/Witness.h
    Kernel/WitnessSet.cpp
    Kernel/WitnessSet.h
    Kernel/WitnessWrapper.h
    Kernel/PropertyAssignment.cpp
    Kernel/PropertyAssignment.h
    Kernel/Bag.cpp
    Kernel/Bag.h
    Kernel/DynamicCore.cpp
    Kernel/DynamicCore.h
    Kernel/DynamicKernel.cpp
    Kernel/DynamicKernel.h
    #Kernel/Conjecture.cpp
    #Kernel/Conjecture.h
    Kernel/Flags.cpp
    Kernel/Flags.h
    Kernel/SearchStrategy.cpp
    Kernel/SearchStrategy.h
    Kernel/State.cpp
    Kernel/State.h
    Kernel/Width.h
    Kernel/Width.cpp
    Kernel/StateTree.cpp
    Kernel/StateTree.h
    Kernel/DynamicCoreHandler.cpp
    Kernel/DynamicCoreHandler.h
    Kernel/SearchStrategyHandler.cpp
    Kernel/SearchStrategyHandler.h
    # Multigraph files
    Multigraph/MultiGraph.cpp
    Multigraph/MultiGraph.h
    # Conjecture
    Conjecture/Conjecture.cpp
    Conjecture/Conjecture.h
    Conjecture/PropertyAssignment.cpp
    Conjecture/PropertyAssignment.h
    # Parser files
    Controller/Parser/command_parser.hpp
    Controller/Parser/command_parser.cpp
    Controller/Parser/command_lexer.cpp
    Parser/PropertyParser/input_lexer.cpp
    Parser/PropertyParser/input_parser.cpp
    Parser/PropertyParser/input_parser.hpp
    ConcreteTreeDecomposition/ctd_parser.hpp
    ConcreteTreeDecomposition/ctd_parser.cpp
    ConcreteTreeDecomposition/ctd_lexer.cpp
    # BitVector files
    #BitVector/BitVector.cpp
    #BitVector/BitVector.h
    #BitVector/BitSet.cpp
    #BitVector/BitSet.h
    # Translation
    Translation/PACE/TreeDecompositionPACE.cpp
    Translation/PACE/TreeDecompositionPACE.h
    Translation/PACE/WitnessTreePACE.cpp
    Translation/PACE/WitnessTreePACE.h
    Translation/PACE/Parser/gr_parser.cpp
    Translation/PACE/Parser/gr_parser.hpp
    Translation/PACE/Parser/td_parser.cpp
    Translation/PACE/Parser/td_parser.hpp
    Translation/PACE/Parser/atd_lexer.cpp
    Translation/PACE/Parser/atd_parser.cpp
    Translation/PACE/Parser/atd_parser.hpp
    Translation/TreeAutomaton/term_parser.cpp
    Translation/TreeAutomaton/term_parser.cpp
    Translation/TreeAutomaton/term_lexer.cpp
    # TreeAutomaton
    TreeAutomaton/TreeAutomaton.h
    TreeAutomaton/TreeAutomaton.cpp
    TreeAutomaton/Term.h
    TreeAutomaton/Term.cpp
    TreeAutomaton/InstructiveTreeDecomposition.h
    TreeAutomaton/InstructiveTreeDecomposition.cpp
    TreeAutomaton/RunTree.cpp
    TreeAutomaton/RunTree.h
    TreeAutomaton/ConcreteTreeDecomposition.cpp
    TreeAutomaton/ConcreteTreeDecomposition.h
    TreeAutomaton/DecompositionTree.h
    TreeAutomaton/DecompositionTree.cpp
)

# Link the executable against the core library and Threads
target_link_libraries(treewidzard PRIVATE TreeWidzard-Core ${CMAKE_DL_LIBS} Threads::Threads ${NUMA_LIBRARIES})

# Add subdirectories for DPCores and SearchStrategies
add_subdirectory("DPCores")
add_subdirectory("SearchStrategies")

# Add testing subdirectory if tests are enabled
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_subdirectory("tests")
endif()
