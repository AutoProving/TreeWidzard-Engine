#ifndef TREEWIDZARD_IsomorphismBreadthFirstSearch_H
#define TREEWIDZARD_IsomorphismBreadthFirstSearch_H
#include <algorithm>
#include <cmath>

#include "Kernel/SearchStrategy.h"
#include "Kernel/StateTree.h"
#include "TreeAutomaton/ConcreteTreeDecomposition.h"
#include "TreeAutomaton/InstructiveTreeDecomposition.h"
#include "TreeAutomaton/TreeAutomaton.h"

using namespace std;
using namespace std::chrono;

class IsomorphismBreadthFirstSearch : public SearchStrategy {
  private:
	TreeAutomaton<State::ptr, InstructiveTreeDecompositionNodeContent>
		bfsDAG; // Constructs a DAG corresponding to the BFS.
	set<State::ptr> allStatesSet;
	set<State::ptr> intermediateStatesSet;
	set<State::ptr> newStatesSet;
	vector<State::ptr>
		newStatesVector; // This will make it easier to do parallel search
	bool noBFSDAG = false;

  public:
	IsomorphismBreadthFirstSearch();
	IsomorphismBreadthFirstSearch(DynamicKernel *dynamicKernel,
								  Conjecture *conjecture, Flags *flags);
	~IsomorphismBreadthFirstSearch(){};
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

#endif // CURRENT_IsomorphismBreadthFirstSearch_H
