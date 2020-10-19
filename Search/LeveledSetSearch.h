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
	vector<set<State::ptr> > setAllStates;
	vector<set<State::ptr> > setIntermediateStates;
	vector<set<State::ptr> > setNewStates;
	vector<vector<set<State::ptr> > > leveledSetAllStates;
  public:
    LeveledSetSearch();
	LeveledSetSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
	~LeveledSetSearch(){};
	static unsigned bagSetToNumber(set<unsigned> bagSet, unsigned width);
	virtual pair<bool, ConcreteTreeDecomposition> search();
	shared_ptr<CTDNodeNew> extractCTDNode(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr> > > &leveledSetAllStates);
	ConcreteTreeDecomposition extractCTDDecomposition(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr> > > &leveledSetAllStates);
	shared_ptr<StateTreeNode> extractStateTreeNode(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr> > > &leveledSetAllStates,
		bool tree_width);
	StateTree extractStateTreeDecomposition(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr> > > &leveledSetAllStates,
		bool tree_width);
};

#endif // CURRENT_LEVELEDSETSEARCH_H
