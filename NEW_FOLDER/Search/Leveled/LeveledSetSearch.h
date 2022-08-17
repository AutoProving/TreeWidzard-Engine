// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CURRENT_LEVELEDSETSEARCH_H
#define CURRENT_LEVELEDSETSEARCH_H
#include <math.h>
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"
#include "../../Kernel/SearchStrategy.h"

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
	void search();
	shared_ptr<TermNode<ConcreteNode>> extractCTDNode(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr> > > &leveledSetAllStates);
	ConcreteTreeDecomposition extractCTDDecomposition(
		unsigned level, State::ptr s,
		vector<vector<set<State::ptr> > > &leveledSetAllStates);
//	shared_ptr<RunTr> extractStateTreeNode(
//		unsigned level, State::ptr s,
//		vector<vector<set<State::ptr> > > &leveledSetAllStates,
//		bool tree_width);
//	StateTree extractStateTreeDecomposition(
//		unsigned level, State::ptr s,
//		vector<vector<set<State::ptr> > > &leveledSetAllStates,
//		bool tree_width);
};

#endif // CURRENT_LEVELEDSETSEARCH_H
