// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CURRENT_LEVELEDSETSEARCH_H
#define CURRENT_LEVELEDSETSEARCH_H
#include <math.h>
#include "../Kernel/ConcreteTreeDecomposition.h"
#include "../Kernel/SearchStrategy.h"
#include "../Kernel/StateTree.h"

using namespace std;
using namespace std::chrono;

class LeveledSetSearch : public SearchStrategy {
  private:
	vector<set<State::ptr, compareLessState> > setAllStates;
	vector<set<State::ptr, compareLessState> > setIntermediateStates;
	vector<set<State::ptr, compareLessState> > setNewStates;
	vector<vector<set<State::ptr, compareLessState> > > leveledSetAllStates;

  public:
	LeveledSetSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture,
					 Flags *flags);
	static unsigned bagSetToNumber(set<unsigned> bagSet, unsigned width);
	virtual pair<bool, ConcreteTreeDecomposition> search();
	shared_ptr<CTDNodeNew> extractCTDNode(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr, compareLessState> > >
			&leveledSetAllStates);
	ConcreteTreeDecomposition extractCTDDecomposition(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr, compareLessState> > >
			&leveledSetAllStates);
	shared_ptr<StateTreeNode> extractStateTreeNode(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr, compareLessState> > >
			&leveledSetAllStates,
		bool tree_width);
	StateTree extractStateTreeDecomposition(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr, compareLessState> > >
			&leveledSetAllStates,
		bool tree_width);
};

#endif // CURRENT_LEVELEDSETSEARCH_H
