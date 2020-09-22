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
	vector<set<StatePointer, compareLessState> > setAllStates;
	vector<set<StatePointer, compareLessState> > setIntermediateStates;
	vector<set<StatePointer, compareLessState> > setNewStates;
	vector<vector<set<StatePointer, compareLessState> > > leveledSetAllStates;

  public:
	LeveledSetSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture,
					 Flags *flags);
	static unsigned bagSetToNumber(set<unsigned> bagSet, unsigned width);
	virtual pair<bool, ConcreteTreeDecomposition> search();
	shared_ptr<CTDNodeNew> extractCTDNode(
		unsigned level, StatePointer s,
		vector<vector<set<StatePointer, compareLessState> > >
			&leveledSetAllStates);
	ConcreteTreeDecomposition extractCTDDecomposition(
		unsigned level, StatePointer s,
		vector<vector<set<StatePointer, compareLessState> > >
			&leveledSetAllStates);
	shared_ptr<StateTreeNode> extractStateTreeNode(
		unsigned level, StatePointer s,
		vector<vector<set<StatePointer, compareLessState> > >
			&leveledSetAllStates,
		bool tree_width);
	StateTree extractStateTreeDecomposition(
		unsigned level, StatePointer s,
		vector<vector<set<StatePointer, compareLessState> > >
			&leveledSetAllStates,
		bool tree_width);
};

#endif // CURRENT_LEVELEDSETSEARCH_H
