// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CURRENT_LEVELEDSETSEARCHPREMISE_H
#define CURRENT_LEVELEDSETSEARCHPREMISE_H
#include "../../Kernel/SearchStrategy.h"
#include "../../Kernel/StateTree.h"
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"
#include <math.h>

using namespace std;
using namespace std::chrono;

class LeveledSetSearchPremise : public SearchStrategy{
private:
    vector<set<State::ptr> > setAllStates;
    vector<set<State::ptr> > setIntermediateStates;
    vector<set<State::ptr> >  setNewStates;
    vector< vector<set<State::ptr> > > leveledSetAllStates;
public:
    LeveledSetSearchPremise();
    LeveledSetSearchPremise(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
    ~LeveledSetSearchPremise(){};
    static unsigned bagSetToNumber(set<unsigned> bagSet,unsigned width);
    void search();
    shared_ptr<TermNode<ConcreteNode> > extractCTDNode(unsigned level, State::ptr s,
            vector< vector<set<State::ptr> > >  &leveledSetAllStates);
    ConcreteTreeDecomposition extractCTDDecomposition(
            unsigned level,State::ptr s, 
            vector<vector<set<State::ptr> > > &leveledSetAllStates);
//    shared_ptr<StateTreeNode> extractStateTreeNode(unsigned level, State::ptr s,
//            vector<vector<set<State::ptr> > > &leveledSetAllStates,
//            bool tree_width);
//
//    StateTree extractStateTreeDecomposition(unsigned level, State::ptr s,
//            vector<vector<set<State::ptr> > > &leveledSetAllStates,
//            bool tree_width);
};
#endif //CURRENT_LEVELEDSETSEARCHPREMISE_H
