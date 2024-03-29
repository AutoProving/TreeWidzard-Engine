cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
link_libraries(-lstdc++fs)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    # add_compile_options(-Werror)
    add_compile_options(-Wall -Wextra -pedantic)
endif()
add_compile_options(-O3)
add_compile_options(-g3)
link_libraries(stdc++fs)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

# add_compile_options(-O0)
# add_compile_options(-g3)
# add_compile_options(-fsanitize=address)
# add_compile_options(-fsanitize=undefined)
# add_compile_options(-fno-sanitize-recover)
# add_link_options(-fsanitize=address)
# add_link_options(-fsanitize=undefined)
# add_link_options(-fno-sanitize-recover)
# add_compile_options(-D_GLIBCXX_DEBUG)
# add_compile_options(-D_GLIBCXX_DEBUG_PEDANTIC)

# add_compile_options(-fsanitize=thread)
# add_compile_options(-fno-sanitize-recover)
# add_link_options(-fsanitize=thread)

# stops lexer from creating some unused methods
add_compile_definitions(YY_NO_INPUT=1)
add_compile_definitions(YY_NO_UNPUT=1)

set(TREEWIDZARD_DPCORES_DEFAULT ${CMAKE_CURRENT_BINARY_DIR}/DPCores:${TREEWIDZARD_DPCORES_DEFAULT})
set(TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT ${CMAKE_CURRENT_BINARY_DIR}/SearchStrategies:${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT})
add_compile_definitions(TREEWIDZARD_DPCORES_DEFAULT="${TREEWIDZARD_DPCORES_DEFAULT}")
add_compile_definitions(TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT="${TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT}")

project("treewidzard")
add_executable(
        # Executable file name
        treewidzard
        # Main file
        Controller/SearchController.h
        Controller/SearchController.cpp
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
        # Command Parser
        Controller/Parser/command_parser.hpp
        Controller/Parser/command_parser.cpp
        Controller/Parser/command_lexer.cpp
        # Input File Parser
        Parser/PropertyParser/input_lexer.cpp
        Parser/PropertyParser/input_parser.cpp
        Parser/PropertyParser/input_parser.hpp
        # ConcreteTreeDecomposition
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
        TreeAutomaton/RunTree.cpp TreeAutomaton/RunTree.h
        TreeAutomaton/ConcreteTreeDecomposition.cpp
        TreeAutomaton/ConcreteTreeDecomposition.h
        TreeAutomaton/DecompositionTree.h
        TreeAutomaton/DecompositionTree.cpp

)

# -pthread is a compiler flag for parallel.
SET(CMAKE_PTHREAD_FLAG "-pthread")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CMAKE_PTHREAD_FLAG}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(treewidzard ${CMAKE_DL_LIBS})

# Remove limit for stack (no longer needed for gr or td, but might be needed for big .atd files)
# ulimit -s unlimited
# execute_process(COMMAND bash -c "ulimit -s unlimited")

set(ENV{TREEWIDZARD} ${PROJECT_SOURCE_DIR})
add_subdirectory("DPCores")
add_subdirectory("SearchStrategies")

