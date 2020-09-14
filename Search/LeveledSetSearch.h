// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CURRENT_LEVELEDSETSEARCH_H
#define CURRENT_LEVELEDSETSEARCH_H
#include "../Kernel/SearchStrategy.h"
#include "../Kernel/StateTree.h"
#include "../Kernel/ConcreteTreeDecomposition.h"
#include <math.h>

using namespace std;
using namespace std::chrono;

class LeveledSetSearch : public SearchStrategy{
private:

    vector<set<shared_ptr<State>, compare_less > > setAllStates;
    vector<set<shared_ptr<State>, compare_less > > setIntermediateStates;
    vector<set<shared_ptr<State>, compare_less > > setNewStates;
    vector<vector<set<shared_ptr<State>, compare_less > > > leveledSetAllStates;
public:
    LeveledSetSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
    static unsigned bagSetToNumber(set<unsigned> bagSet,unsigned width);
    virtual pair<bool,ConcreteTreeDecomposition> search();
    shared_ptr<CTDNodeNew> extractCTDNode(unsigned level, shared_ptr<State> s, vector<vector<set<shared_ptr<State>, compare_less > > > &leveledSetAllStates);
    ConcreteTreeDecomposition extractCTDDecomposition(unsigned level, shared_ptr<State> s, vector<vector<set<shared_ptr<State>, compare_less > > > &leveledSetAllStates);
    shared_ptr<StateTreeNode> extractStateTreeNode(unsigned level, shared_ptr<State> s, vector<vector<set<shared_ptr<State>, compare_less> > > &leveledSetAllStates, bool tree_width);
    StateTree extractStateTreeDecomposition(unsigned level, shared_ptr<State> s, vector<vector<set<shared_ptr<State>, compare_less> > > &leveledSetAllStates, bool tree_width);
};


#endif //CURRENT_LEVELEDSETSEARCH_H
