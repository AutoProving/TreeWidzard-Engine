// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CURRENT_BREADTHFIRSTSEARCH_H
#define CURRENT_BREADTHFIRSTSEARCH_H
#include <cmath>
#include <set>
#include <vector>

#include "../../Kernel/SearchStrategy.h"
#include "../../Kernel/StateTree.h"
#include "../../LockedSet/locked_set.hpp"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "../../TreeAutomaton/TreeAutomaton.h"

class SlightlyParallelBreadthFirstSearch : public SearchStrategy {
 private:
  TreeAutomaton<State::ptr, AbstractTreeDecompositionNodeContent>
      bfsDAG;  // Constructs a DAG corresponding to the BFS.
  locked_set<State::ptr> allStatesSet;
  locked_set<State::ptr> intermediateStatesSet;
  locked_set<State::ptr> newStatesSet;
  std::vector<State::ptr>
      newStatesVector;  // This will make it easier to do parallel search
  bool noBFSDAG = false;
  int nThreads = 4;

 public:
  SlightlyParallelBreadthFirstSearch();
  SlightlyParallelBreadthFirstSearch(DynamicKernel *dynamicKernel,
                                     Conjecture *conjecture, Flags *flags);
  ~SlightlyParallelBreadthFirstSearch(){};
  AbstractTreeDecomposition extractCounterExampleTerm(State::ptr state);
  ////Extract State Tree//////////
  void extractCounterExampleStateTreeNode(State::ptr state,
                                          std::shared_ptr<StateTreeNode>);
  StateTree extractCounterExampleStateTree(State::ptr state);
  ///////////////////////////////
  ////Extract Run Tree///////////
  void extractCounterExampleRunNode(
      State::ptr state,
      std::shared_ptr<TermNode<
          RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>>
          node);
  RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
  extractCounterExampleRun(State::ptr state);
  void search();
};

#endif  // CURRENT_BREADTHFIRSTSEARCH_H
