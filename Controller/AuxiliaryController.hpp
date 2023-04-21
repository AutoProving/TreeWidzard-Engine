#ifndef TREEWIDZARD_AUXILIARYCONTROLLER_HPP
#define TREEWIDZARD_AUXILIARYCONTROLLER_HPP
#include <fstream>
#include <iostream>
inline void show_manual() {
	// random settings (not implemented in this version) [-random <seed> <intro
	// vertex probability> <forget vertex probability> <ping number>]
	//
	/*
	Random Search Parameters:
	  seed:   The seed that initializes the random generator. This is an
			  unsigned int. intro vertex probability: The probability of
	introducing a new vertex. This is a float between 0 and 1. forget vertex
	probability: The probability of forgetting a vertex. This is a float between
	0 and 1. ping number: At each "ping number" times the program prints the
	number of iterations executed so far. This parameter only has an effect if
	the -pl flag is on.
	*/

	std::cout << R"___(    Searching for a Counterexample:
        ./treewidzard -atp [Width] [Flag Options] <search method> <property file>

    [Width]: tw|pw = number
                example: tw = 2 (The program will search graphs with tree-width at most 2)
                example: pw = 2 (The program will search graphs with path-width at most 2)
    Flag Options:
        -pl          print loop
        -ps          print states
        -st          print state tree decomposition if a counterexample is found
        -pdbgn       print directed bipartite graph in NAUTY format
        -no-bfs-dag  disable creation of the bfs dag, to speed up the search (NOT WORKING IN THIS VERSION)
        -nthreads    number of threads to use in parallel
        -files       write the counter example into files. Files: ITD and Augmented ITD,

    Parsing a Tree Decomposition:
        1- For PACE Format
            ./treewidzard -modelcheck PACE [Flag Options] <property file> <graph file> <tree decomposition file>
        2- For ITD Decomposition Verify
            ./treewidzard -modelcheck ITD [Flag Options] <property file> <instructive tree decomposition file>
     Flag Options:
            -files     write the counter example into files. Files: ITD and Augmented ITD, RunTree, Graph File, Graph in GML Format, Graph in PACE Format, Tree Decomposition
)___";
	exit(20);
}

#endif // TREEWIDZARD_AUXILIARYCONTROLLER_HPP
