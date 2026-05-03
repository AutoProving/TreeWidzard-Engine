// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef CURRENT_BREADTHFIRSTSEARCH_H
#define CURRENT_BREADTHFIRSTSEARCH_H
#include <cmath>
#include <mutex>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Kernel/SearchStrategy.h"
#include "Kernel/StateTree.h"
#include "TreeAutomaton/InstructiveTreeDecomposition.h"
#include "TreeAutomaton/ConcreteTreeDecomposition.h"
#include "TreeAutomaton/TreeAutomaton.h"

class ParallelBreadthFirstSearch : public SearchStrategy {
  private:
	using pset =
		std::unordered_set<State::ptr, State::ptr::Hash, State::ptr::Equal>;

	std::unordered_map<
		State::ptr,
		Transition<State::ptr, InstructiveTreeDecompositionNodeContent>,
		State::ptr::Hash,
		State::ptr::Equal>
		counterexampleTrace;
	std::mutex stateSetsLock;
	pset allStatesSet;
	pset intermediateStatesSet;
	pset newStatesSet;
	std::vector<State::ptr>
		newStatesVector; // This will make it easier to do parallel search
	bool noBFSDAG = false;
	int nThreads = 4;

  public:
	ParallelBreadthFirstSearch();
	ParallelBreadthFirstSearch(DynamicKernel *dynamicKernel,
							   Conjecture *conjecture, Flags *flags);
	~ParallelBreadthFirstSearch(){};
	InstructiveTreeDecomposition extractCounterExampleTerm(State::ptr state);
	////Extract State Tree//////////
	void extractCounterExampleStateTreeNode(State::ptr state,
											std::shared_ptr<StateTreeNode>);
	StateTree extractCounterExampleStateTree(State::ptr state);
	///////////////////////////////
	////Extract Run Tree///////////
	void extractCounterExampleRunNode(
		State::ptr state,
		std::shared_ptr<TermNode<
			RunNodeContent<State::ptr, InstructiveTreeDecompositionNodeContent>>>
			node);
	RunTree<State::ptr, InstructiveTreeDecompositionNodeContent>
	extractCounterExampleRun(State::ptr state);
	void search();
};

#endif // CURRENT_BREADTHFIRSTSEARCH_H
