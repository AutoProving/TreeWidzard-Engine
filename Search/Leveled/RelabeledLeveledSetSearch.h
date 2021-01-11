// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef TREEWIDZARD_RELABELEDLEVELEDSETSEARCH_H
#define TREEWIDZARD_RELABELEDLEVELEDSETSEARCH_H
#include <math.h>
#include "../Kernel/ConcreteTreeDecomposition.h"
#include "../Kernel/SearchStrategy.h"
#include "../Kernel/StateTree.h"

using namespace std;
using namespace std::chrono;

class RelabeledLeveledSetSearch : public SearchStrategy {
  private:
	vector<set<State::ptr> > setAllStates;
	vector<set<State::ptr> > setIntermediateStates;
	vector<set<State::ptr> > setNewStates;
	vector<vector<set<State::ptr> > > leveledSetAllStates;
  public:
    RelabeledLeveledSetSearch();
	RelabeledLeveledSetSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
	~RelabeledLeveledSetSearch(){};
	static unsigned bagSetToNumber(set<unsigned> bagSet, unsigned width);
	void search();
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
	map<unsigned ,unsigned > relabeledMapGenerator(set<unsigned > bagElements);
};

#endif TREEWIDZARD_RELABELEDLEVELEDSETSEARCH_H
