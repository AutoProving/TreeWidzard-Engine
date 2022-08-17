// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef RANDOMLEVELEDSETSEARCHPREMISE_H
#define RANDOMLEVELEDSETSEARCHPREMISE_H

#include "../../Kernel/SearchStrategy.h"
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"
#include <math.h>

class RandomLeveledSetSearchPremise: public SearchStrategy{
    private:
        vector<pair<State::ptr,string> > generatedVector;
        map<State::ptr, unsigned> mapState;
        vector<set<State::ptr > > setAllStates;
        vector<set<State::ptr > > setNewStates;
        vector<vector<set<State::ptr > > > leveledSetAllStates;
    public:
        RandomLeveledSetSearchPremise();
        RandomLeveledSetSearchPremise(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
        static unsigned bagSetToNumber(set<unsigned> bagSet,unsigned width);
        void search();
        shared_ptr<TermNode<ConcreteNode>> extractCTDNode(unsigned index);
        ConcreteTreeDecomposition extractCTDDecomposition();
//        shared_ptr<StateTreeNode> extractStateTreeNode(unsigned index);
//        StateTree extractStateTreeDecomposition();

};



#endif
