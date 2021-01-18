// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef TREEWIDZARD_IsomorphismBreadthFirstSearch_H
#define TREEWIDZARD_IsomorphismBreadthFirstSearch_H
#include <math.h>
#include <algorithm>
#include "../../Kernel/ConcreteTreeDecomposition.h"
#include "../../Kernel/SearchStrategy.h"
#include "../../Kernel/StateTree.h"
#include "../../TreeAutomaton/TreeAutomaton.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"


using namespace std;
using namespace std::chrono;

class IsomorphismBreadthFirstSearch : public SearchStrategy {
  private:
	TreeAutomaton<State::ptr,AbstractTreeDecompositionNodeContent> bfsDAG; // Constructs a DAG corresponding to the BFS.
	// The vector has width+2 positions. Index 0 contains states with an empty bag, and index i contains states with bags of size i.
	// Recall that for width "w", the bag has "w+1" elements.
	vector<set<State::ptr>> allStatesSet;
	vector<set<State::ptr>>  newStatesSet;
	vector<vector<State::ptr>> newStatesVector; // This will make it easier to do parallel search
  public:
    IsomorphismBreadthFirstSearch();
	IsomorphismBreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
	~IsomorphismBreadthFirstSearch(){};
	AbstractTreeDecomposition extractCounterExampleTerm(); 	
	Term<RunNodeContent<State,AbstractTreeDecompositionNodeContent> > extractCounterExampleRun();
    void search();
    map<unsigned ,unsigned > relabeledMapGenerator(set<unsigned > bagElements);
    map<unsigned ,unsigned > identityMap(unsigned k); // Returns the identity map with domain {1,...,k}
    map<unsigned ,unsigned > identityMap(set<unsigned > bagSet); // Returns the identity map on bagSet.
    bool nextPermutation(map<unsigned ,unsigned > &m);
    State::ptr canonicalState(State::ptr state);
    void extractRunTreeNode(shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> >> wrongRunNode,
                            shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>> correctedRunNode, map<unsigned ,unsigned > &m);
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> extractRunTree(State::ptr state);

};

#endif // CURRENT_IsomorphismBreadthFirstSearch_H
