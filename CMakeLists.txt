cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
link_libraries(-lstdc++fs)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    # add_compile_options(-Wall -Wextra -pedantic -Werror)
    #add_compile_options(-Wall)
endif()
add_compile_options(-O2)
include_directories(Scripts)
link_libraries(stdc++fs)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

# add_compile_options(-fsanitize=address)
# add_compile_options(-fsanitize=undefined)
# add_link_options(-fsanitize=address)
# add_link_options(-fsanitize=undefined)

include_directories(DynamicCores)

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
        # DynamicCore files
        # DynamicCores/CliqueNumber_AtLeast.cpp
        # DynamicCores/CliqueNumber_AtLeast.h
        # DynamicCores/CliqueNumberSimpleGraphs_AtLeast.h
        # DynamicCores/CliqueNumberSimpleGraphs_AtLeast.cpp
        # DynamicCores/ChromaticNumber_AtMost.cpp
        # DynamicCores/ChromaticNumber_AtMost.h
        #	DynamicCores/MaxDegree_AtLeast.cpp
        # DynamicCores/MaxDegree_AtLeast.h
        # DynamicCores/HasMultipleEdges.cpp
        # DynamicCores/HasMultipleEdges.h
        #	DynamicCores/TriangleFreeWidthFour.cpp
        #	DynamicCores/EdgeConnected_AtMost.cpp
        #	DynamicCores/EdgeColoring_AtMost.cpp
        #	DynamicCores/Minor.cpp
        # DynamicCores/NoWhereZeroFlow_AtMost.cpp
        #	DynamicCores/AuxiliaryFunctions.h
        #	DynamicCores/HamiltonianCycle.cpp
        #	DynamicCores/MaxDegree_AtLeast.h
        #	DynamicCores/MaxDegree_AtLeast.cpp
        #	DynamicCores/MinDegree_AtMost.h
        #	DynamicCores/MinDegree_AtMost.cpp
        #	DynamicCores/CycleGirth_AtMost.h
        #	DynamicCores/CycleGirth_AtMost.cpp
        # DynamicCores/Mod.h
        # DynamicCores/Mod.cpp
        # DynamicCores/PathEdgeCover.h
        # DynamicCores/PathEdgeCover.cpp
        #	DynamicCores/VertexConnected_AtMost.cpp
        #	DynamicCores/VertexConnected_AtMost.h
        #	DynamicCores/HasCycle.h
        #	DynamicCores/HasCycle.cpp
        #   DynamicCores/ChromaticNumber_Min.h
        #  DynamicCores/ChromaticNumber_Min.cpp
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
        # SearchStrategy files
        #Search/Leveled/LeveledSetSearch.cpp
        #Search/Leveled/LeveledSetSearch.h
        #Search/Leveled/LeveledSetSearchPremise.cpp
        #Search/Leveled/LeveledSetSearchPremise.h
        #Search/BFS/BreadthFirstSearch.h
        #Search/BFS/BreadthFirstSearch.cpp
        #Search/BFS/BreadthFirstSearchStatistic.h
        #Search/BFS/BreadthFirstSearchStatistic.cpp
        #Search/RelabeledLeveledSetSearch.h
        #Search/RelabeledLeveledSetSearch.cpp
        #Search/BFS/RelabeledBreadthFirstSearch.h
        #Search/BFS/RelabeledBreadthFirstSearch.cpp
        #Search/BFS/RelabeledBreadthFirstSearch-Old.h
        #Search/BFS/RelabeledBreadthFirstSearch-Old.cpp
        #Search/BFS/IsomorphismBreadthFirstSearch.h
        #Search/BFS/IsomorphismBreadthFirstSearch.cpp
        #Search/Randomized/RandomLeveledSetSearch.h
        #Search/Randomized/RandomLeveledSetSearch.cpp
        #Search/Randomized/RandomLeveledSetSearchPremise.h
        #Search/Randomized/RandomLeveledSetSearchPremise.cpp
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
        BitVector/BitVector.cpp
        BitVector/BitVector.h
        BitVector/BitSet.cpp
        BitVector/BitSet.h
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
        TreeAutomaton/AbstractTreeDecomposition.h
        TreeAutomaton/AbstractTreeDecomposition.cpp
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


execute_process (
        COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/./buildAllPlugins.sh")

# Remove limit for stack (no longer needed for gr or td, but might be needed for big .atd files)
# ulimit -s unlimited
# execute_process(COMMAND bash -c "ulimit -s unlimited")
