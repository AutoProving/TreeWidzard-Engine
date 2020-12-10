# Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
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
include_directories(.)
link_libraries(stdc++fs)
include_directories(DynamicCores)

add_executable(
	# Executable file name
		dp
	# Main file
		main.cpp
	# DynamicCore files
		 #   DynamicCores/CliqueNumber_AtLeast.cpp
		#DynamicCores/CliqueNumber_AtLeast.h
		#  DynamicCores/ChromaticNumber_AtMost.cpp
		#   DynamicCores/ChromaticNumber_AtMost.h
		#	DynamicCores/MaxDegree_AtLeast.cpp
		#	DynamicCores/MaxDegree_AtLeast.h
		#   DynamicCores/HasMultipleEdges.cpp
		#   DynamicCores/HasMultipleEdges.h
		#	DynamicCores/TriangleFreeWidthFour.cpp
			DynamicCores/EdgeConnected_AtMost.cpp

	# Kernel files
        Kernel/Witness.cpp
        Kernel/Witness.h
        Kernel/WitnessSet.cpp
        Kernel/WitnessSet.h
        Kernel/PropertyAssignment.cpp
        Kernel/PropertyAssignment.h
        Kernel/Bag.cpp
        Kernel/Bag.h
		Kernel/DynamicCore.cpp
		Kernel/DynamicCore.h
        Kernel/DynamicKernel.cpp
        Kernel/DynamicKernel.h
        Kernel/Conjecture.cpp
        Kernel/Conjecture.h
        Kernel/Flags.cpp
        Kernel/Flags.h
		Kernel/SearchStrategy.cpp
		Kernel/SearchStrategy.h
        Kernel/State.cpp
        Kernel/State.h
        Kernel/Width.h
        Kernel/Width.cpp
        Kernel/ConcreteTreeDecomposition.h
		Kernel/ConcreteTreeDecomposition.cpp
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
		#Search/LeveledSetSearch.cpp
		#Search/LeveledSetSearch.h
		#Search/LeveledSetSearchPremise.cpp
		#Search/LeveledSetSearchPremise.h
	# Parser files
        Parser/lexer.cpp
        Parser/parser.cpp
        Parser/parser.hpp
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
		Translation/PACE/Parser/gr_lexer.cpp
		Translation/PACE/Parser/td_lexer.cpp
		Translation/PACE/Parser/td_parser.cpp
		Translation/PACE/Parser/td_parser.hpp
		Translation/TreeAutomaton/term_parser.cpp
		Translation/TreeAutomaton/term_parser.cpp
		Translation/TreeAutomaton/term_lexer.cpp
	# TreeAutomaton
		TreeAutomaton/TreeAutomaton.cpp
)
target_link_libraries(dp ${CMAKE_DL_LIBS})
