// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef TREEWIDZARD_RelabeledBREADTHFIRSTSEARCH_H
#define TREEWIDZARD_RelabeledBREADTHFIRSTSEARCH_H
#include <algorithm>
#include <cmath>

#include "../../Kernel/SearchStrategy.h"
#include "../../Kernel/StateTree.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "../../TreeAutomaton/TreeAutomaton.h"

using namespace std;
using namespace std::chrono;

class RelabeledBreadthFirstSearch : public SearchStrategy {
 private:
  TreeAutomaton<State::ptr, AbstractTreeDecompositionNodeContent>
      bfsDAG;  // Constructs a DAG corresponding to the BFS.
  // The vector has width+2 positions. Index 0 contains states with an empty
  // bag, and index i contains states with bags of size i. Recall that for width
  // "w", the bag has "w+1" elements.
  vector<set<State::ptr>> allStatesSet;
  vector<set<State::ptr>> newStatesSet;
  vector<vector<State::ptr>>
      newStatesVector;  // This will make it easier to do parallel search
  bool noBFSDAG = false;
  
 public:
  RelabeledBreadthFirstSearch();
  RelabeledBreadthFirstSearch(DynamicKernel *dynamicKernel,
                              Conjecture *conjecture, Flags *flags);
  ~RelabeledBreadthFirstSearch(){};
  AbstractTreeDecomposition extractCounterExampleTerm();
  Term<RunNodeContent<State, AbstractTreeDecompositionNodeContent>>
  extractCounterExampleRun();
  void search();
  map<unsigned, unsigned> relabeledMapGenerator(set<unsigned> bagElements);
  map<unsigned, unsigned> identityMap(
      unsigned k);  // Returns the identity map with domain {1,...,k}
  map<unsigned, unsigned> identityMap(
      set<unsigned> bagSet);  // Returns the identity map on bagSet.
  bool nextPermutation(map<unsigned, unsigned> &m);
  void extractRunTreeNode(
      shared_ptr<TermNode<
          RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>>
          relabeledNode,
      shared_ptr<TermNode<
          RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>>
          node,
      map<unsigned, unsigned> &m);
  RunTree<State::ptr, AbstractTreeDecompositionNodeContent> extractRunTree(
      State::ptr state);
};

#endif  // CURRENT_RelabeledBREADTHFIRSTSEARCH_H
