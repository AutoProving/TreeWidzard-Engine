#ifndef TREEWIDZARD_PARALLELISOMORPHISMBREADTHFIRSTSEARCH_H
#define TREEWIDZARD_PARALLELISOMORPHISMBREADTHFIRSTSEARCH_H
#include <algorithm>
#include <cmath>

#include "Kernel/SearchStrategy.h"
#include "Kernel/StateTree.h"
#include "TreeAutomaton/ConcreteTreeDecomposition.h"
#include "TreeAutomaton/InstructiveTreeDecomposition.h"
#include "TreeAutomaton/TreeAutomaton.h"
#include "parallel_hashmap/phmap.h"

using namespace std;
using namespace std::chrono;

class ParallelIsomorphismBreadthFirstSearch : public SearchStrategy {
  private:
	using pset = phmap::parallel_flat_hash_set<
		State::ptr, State::ptr::Hash, State::ptr::Equal,
		phmap::priv::Allocator<State::ptr>, 4UL, std::mutex>;

	TreeAutomaton<State::ptr, InstructiveTreeDecompositionNodeContent>
		bfsDAG; // Constructs a DAG corresponding to the BFS.
	pset allStatesSet;
	pset intermediateStatesSet;
	pset newStatesSet;
	vector<State::ptr>
		newStatesVector; // This will make it easier to do parallel search
	bool noBFSDAG = false;
	int nThreads = 4;

  public:
	ParallelIsomorphismBreadthFirstSearch();
	ParallelIsomorphismBreadthFirstSearch(DynamicKernel *dynamicKernel,
										  Conjecture *conjecture, Flags *flags);
	~ParallelIsomorphismBreadthFirstSearch(){};
	InstructiveTreeDecomposition extractCounterExampleTerm();
	Term<RunNodeContent<State, InstructiveTreeDecompositionNodeContent>>
	extractCounterExampleRun();
	void search();
	map<unsigned, unsigned> relabeledMapGenerator(set<unsigned> bagElements);
	map<unsigned, unsigned> identityMap(
		unsigned k); // Returns the identity map with domain {1,...,k}
	map<unsigned, unsigned> identityMap(
		set<unsigned> bagSet); // Returns the identity map on bagSet.
	bool nextPermutation(map<unsigned, unsigned> &m);
	State::ptr canonicalState(State::ptr state);
	void extractRunTreeNode(
		shared_ptr<TermNode<RunNodeContent<
			State::ptr, InstructiveTreeDecompositionNodeContent>>>
			wrongRunNode,
		shared_ptr<TermNode<RunNodeContent<
			State::ptr, InstructiveTreeDecompositionNodeContent>>>
			correctedRunNode,
		map<unsigned, unsigned> &m);
	RunTree<State::ptr, InstructiveTreeDecompositionNodeContent> extractRunTree(
		State::ptr state);
};

#endif // TREEWIDZARD_PARALLELISOMORPHISMBREADTHFIRSTSEARCH_H
