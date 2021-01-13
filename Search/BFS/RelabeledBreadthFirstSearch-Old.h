// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef TREEWIDZARD_RelabeledBREADTHFIRSTSEARCH_Old_H
#define TREEWIDZARD_RelabeledBREADTHFIRSTSEARCH_Old_H
#include <math.h>
#include "../../Kernel/ConcreteTreeDecomposition.h"
#include "../../Kernel/SearchStrategy.h"
#include "../../Kernel/StateTree.h"
#include "../../TreeAutomaton/TreeAutomaton.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"


using namespace std;
using namespace std::chrono;

class RelabeledBreadthFirstSearch_Old : public SearchStrategy {
  private:
	TreeAutomaton<State,AbstractTreeDecompositionNodeContent> bfsDAG; // Constructs a DAG corresponding to the BFS.
	set<State::ptr> allStatesSet;
	set<State::ptr> intermediateStatesSet;
	set<State::ptr>  newStatesSet; 
	vector<State::ptr> newStatesVector; // This will make it easier to do parallel search 
  public:
    RelabeledBreadthFirstSearch_Old();
	RelabeledBreadthFirstSearch_Old(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
	~RelabeledBreadthFirstSearch_Old(){};
	AbstractTreeDecomposition extractCounterExampleTerm(); 	
	Term<RunNodeContent<State,AbstractTreeDecompositionNodeContent> > extractCounterExampleRun();
    void search();
    map<unsigned ,unsigned > relabeledMapGenerator(set<unsigned > bagElements);

};

#endif // CURRENT_RelabeledBREADTHFIRSTSEARCH_Old_H
