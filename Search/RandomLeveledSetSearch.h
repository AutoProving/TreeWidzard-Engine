// Copyright 2020 Mateus de Oliveira Oliveira and Farhad Vadiee.
// We acknowledge support from the Research Council of Norway in the context of the project
// AutoProving: Automated Theorem Proving From the Mindset of Parameterized Complexity Theory
// (Project Number: 288761, Project Period: 2019-2023).
#ifndef RANDOMLEVELEDSETSEARCH_H
#define RANDOMLEVELEDSETSEARCH_H
#include "../Kernel/SearchStrategy.h"
#include "../Kernel/StateTree.h"
#include <math.h>

class RandomLeveledSetSearch: public SearchStrategy{
    private:
        vector<pair<State::ptr,string> > generatedVector;
        map<State::ptr, unsigned> mapState;
        vector<set<State::ptr > > setAllStates;
        vector<set<State::ptr > > setNewStates;
        vector<vector<set<State::ptr > > > leveledSetAllStates;
    public:
        RandomLeveledSetSearch();
        RandomLeveledSetSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
        static unsigned bagSetToNumber(set<unsigned> bagSet,unsigned width);
        virtual pair<bool,ConcreteTreeDecomposition> search();
        shared_ptr<CTDNodeNew> extractCTDNode(unsigned index);
        ConcreteTreeDecomposition extractCTDDecomposition();
        shared_ptr<StateTreeNode> extractStateTreeNode(unsigned index);
        StateTree extractStateTreeDecomposition();

};



#endif
