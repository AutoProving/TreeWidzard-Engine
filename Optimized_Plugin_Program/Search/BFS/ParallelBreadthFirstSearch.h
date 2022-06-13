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
#include "../../parallel_hashmap/phmap.h"

class ParallelBreadthFirstSearch : public SearchStrategy {
 private:
  struct KeyHash {
    const uint64_t seed_ = 0;
    uint64_t operator()(const State::ptr &ptr) const {
      Hasher h(seed_);
      ptr->hash(h);
      return h.get();
    }
  };

  struct KeyEqual {
    uint64_t operator()(const State::ptr &l, const State::ptr &r) const {
      return *l == *r;
    }
  };

  using pset = phmap::parallel_flat_hash_set<State::ptr, KeyHash, KeyEqual,
                                             phmap::priv::Allocator<State::ptr>,
                                             4UL, std::mutex>;

  TreeAutomaton<State::ptr, AbstractTreeDecompositionNodeContent>
      bfsDAG;  // Constructs a DAG corresponding to the BFS.
  pset allStatesSet;
  pset intermediateStatesSet;
  pset newStatesSet;
  std::vector<State::ptr>
      newStatesVector;  // This will make it easier to do parallel search
 public:
  ParallelBreadthFirstSearch();
  ParallelBreadthFirstSearch(DynamicKernel *dynamicKernel,
                             Conjecture *conjecture, Flags *flags);
  ~ParallelBreadthFirstSearch(){};
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
