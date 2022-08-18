// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CURRENT_BREADTHFIRSTSEARCH_H
#define CURRENT_BREADTHFIRSTSEARCH_H
#include <math.h>
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "../../Kernel/SearchStrategy.h"
#include "../../Kernel/StateTree.h"
#include "../../TreeAutomaton/TreeAutomaton.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"

using namespace std;
using namespace std::chrono;

class BreadthFirstSearch : public SearchStrategy {
  private:
	TreeAutomaton<State::ptr,AbstractTreeDecompositionNodeContent> bfsDAG; // Constructs a DAG corresponding to the BFS.
	set<State::ptr> allStatesSet;
	set<State::ptr> intermediateStatesSet;
	set<State::ptr>  newStatesSet; 
	vector<State::ptr> newStatesVector; // This will make it easier to do parallel search 
  public:
    BreadthFirstSearch();
	BreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
	~BreadthFirstSearch(){};
    AbstractTreeDecomposition extractCounterExampleTerm(State::ptr state);
    ////Extract State Tree//////////
    void extractCounterExampleStateTreeNode(State::ptr state,shared_ptr<StateTreeNode>);
    StateTree extractCounterExampleStateTree(State::ptr state);
    ///////////////////////////////
    ////Extract Run Tree///////////
    void extractCounterExampleRunNode(State::ptr state, shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> >> node);
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> extractCounterExampleRun(State::ptr state);
    void search();
};

#endif // CURRENT_BREADTHFIRSTSEARCH_H
