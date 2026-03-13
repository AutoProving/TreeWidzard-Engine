# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.18)
project(treewidzard LANGUAGES CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
  set(TREEWIDZARD_PATH_LIST_SEPARATOR ";")
else()
  set(TREEWIDZARD_PATH_LIST_SEPARATOR ":")
endif()

# Find BISON and FLEX
find_package(BISON 3.0 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

# Compiler options
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Optimization and debugging flags
add_compile_options(-O3 -g3)

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

# Define compile definitions
add_compile_definitions(YY_NO_INPUT=1 YY_NO_UNPUT=1)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define default paths for DPCores and SearchStrategies
set(TREEWIDZARD_DPCORES_DEFAULT_RAW "${CMAKE_CURRENT_BINARY_DIR}/DPCores")
if(DEFINED TREEWIDZARD_DPCORES_DEFAULT AND
   NOT "${TREEWIDZARD_DPCORES_DEFAULT}" STREQUAL "")
  string(APPEND TREEWIDZARD_DPCORES_DEFAULT_RAW
         "${TREEWIDZARD_PATH_LIST_SEPARATOR}${TREEWIDZARD_DPCORES_DEFAULT}")
endif()

set(TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT_RAW
    "${CMAKE_CURRENT_BINARY_DIR}/SearchStrategies")
if(DEFINED TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT AND
   NOT "${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT}" STREQUAL "")
  string(APPEND TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT_RAW
         "${TREEWIDZARD_PATH_LIST_SEPARATOR}${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT}")
endif()

string(REPLACE ";" "\\;" TREEWIDZARD_DPCORES_DEFAULT
       "${TREEWIDZARD_DPCORES_DEFAULT_RAW}")
string(REPLACE ";" "\\;" TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT
       "${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT_RAW}")

add_compile_definitions(TREEWIDZARD_DPCORES_DEFAULT="${TREEWIDZARD_DPCORES_DEFAULT}")
add_compile_definitions(TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT="${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT}")

# Set the environment variable for subprojects
set(ENV{TREEWIDZARD} ${PROJECT_SOURCE_DIR})

# Create directory for generated parser files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Parser/PropertyParser)

# Generate Parsers
bison_target(InputParser Parser/PropertyParser/input_parser.y ${CMAKE_CURRENT_BINARY_DIR}/Parser/PropertyParser/input_parser.cpp)
flex_target(InputLexer Parser/PropertyParser/input_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/Parser/PropertyParser/input_lexer.cpp)
add_flex_bison_dependency(InputLexer InputParser)

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
    ${CMAKE_CURRENT_BINARY_DIR}/Parser/PropertyParser # For generated headers
)

# Define the core library
# Define the core library as STATIC or SHARED based on BUILD_SHARED_LIBS
add_library(TreeWidzard-Core
    Controller/InputController.cpp
    Kernel/DynamicCore.cpp
    Kernel/Witness.cpp
    Kernel/WitnessSet.cpp
    Kernel/DynamicKernel.cpp
    Kernel/Flags.cpp
    Kernel/BreadthFirstTraversal.cpp
    Kernel/SearchStrategy.cpp
    Kernel/State.cpp
    Kernel/Bag.cpp
    Kernel/CertificateWriter.cpp
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
    src/TreeWidzard.cpp
    ${BISON_InputParser_OUTPUTS}
    ${FLEX_InputLexer_OUTPUTS}
)

# Standardize version and SO version
set_target_properties(TreeWidzard-Core PROPERTIES
    VERSION 0.1.0
    SOVERSION 0
)

# Installation Rules
include(GNUInstallDirs)

# Install the library
install(TARGETS TreeWidzard-Core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install Headers (Recursive pattern or list specific dirs)
install(DIRECTORY Kernel Conjecture TreeAutomaton Multigraph Controller
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/treewidzard/internal
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/treewidzard
    # Add other core source files here if any
)

target_include_directories(TreeWidzard-Core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/treewidzard>
)

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
    Controller/InputController.h
    Controller/CertificateChecker.cpp
    Controller/CertificateChecker.h
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
    # Parser files (generated ones are in Core lib)
    Controller/Parser/command_parser.hpp
    Controller/Parser/command_parser.cpp
    Controller/Parser/command_lexer.cpp
    ConcreteTreeDecomposition/ctd_parser.hpp
    ConcreteTreeDecomposition/ctd_parser.cpp
    ConcreteTreeDecomposition/ctd_lexer.cpp
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
