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

	std::cout << R"___(TreeWidzard

Usage:
  ./treewidzard --help
  ./treewidzard -atp <tw|pw>=<k> [flags] <search method> <property file>
  ./treewidzard -modelcheck PACE [flags] <property file> <graph file> <tree decomposition file>
  ./treewidzard -modelcheck ITD [flags] <property file> <instructive tree decomposition file>

Search methods:
  BreadthFirstSearch
  IsomorphismBreadthFirstSearch
  ParallelBreadthFirstSearch
  ParallelIsomorphismBreadthFirstSearch

ATP flags:
  -pl              print loop progress
  -ps              print discovered states
  -st              print the counterexample state tree (BreadthFirstSearch variants)
  -premise         enable premise pruning
  -pdbgn           write the directed bipartite graph in NAUTY format
  -no-bfs-dag      disable BFS DAG creation
  -nthreads <N>    set the number of threads for parallel search methods only
  -files           write counterexample artifacts to files

Model checking flags:
  -files           write generated artifacts to files

Examples:
  ./treewidzard -atp tw=4 -pl ParallelBreadthFirstSearch examples/system_paper/atp/five_colorable.txt
  ./treewidzard -modelcheck PACE examples/system_paper/model_checking/three_colorable.txt examples/system_paper/model_checking/diamond.gr examples/system_paper/model_checking/diamond.td
  ./treewidzard -modelcheck ITD examples/system_paper/model_checking/three_colorable.txt examples/system_paper/model_checking/diamond.itd
)___";
	exit(0);
}

#endif // TREEWIDZARD_AUXILIARYCONTROLLER_HPP
