# Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 11)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    # add_compile_options(-Wall -Wextra -pedantic -Werror)
    #add_compile_options(-Wall)
	# optimize
    add_compile_options(-O2)
endif()

include_directories(.)

include_directories(DynamicCores)

add_executable(
	# Executable file name
		dp
	# Main file
		main.cpp
	# DynamicCore files
		DynamicCores/CliqueNumber_AtLeast.cpp
		DynamicCores/CliqueNumber_AtLeast.h
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
	# Multigraph files
		Multigraph/MultiGraph.cpp
		Multigraph/MultiGraph.h
	# SearchStrategy files
		Search/LeveledSetSearch.cpp
		Search/LeveledSetSearch.h
	# Parser files
        Parser/lexer.cpp
        Parser/parser.cpp
        Parser/parser.hpp

)
