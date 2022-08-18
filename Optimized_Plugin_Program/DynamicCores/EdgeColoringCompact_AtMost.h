// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef EDGECOLORINGCOMPACT_ATMOST_H
#define EDGECOLORINGCOMPACT_ATMOST_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
//#include <list>

using namespace std;
class EdgeColoringCompact_AtMost_Witness;

typedef shared_ptr<EdgeColoringCompact_AtMost_Witness> EdgeColoringCompact_AtMost_WitnessPointer;
typedef shared_ptr<EdgeColoringCompact_AtMost_Witness const> EdgeColoringCompact_AtMost_WitnessPointerConst;
class EdgeColoringCompact_AtMost_Witness: public Witness, public enable_shared_from_this<EdgeColoringCompact_AtMost_Witness>{

public:
    bool is_equal_implementation(const EdgeColoringCompact_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const EdgeColoringCompact_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(EdgeColoringCompact_AtMost_WitnessPointer w);
    ~EdgeColoringCompact_AtMost_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
    virtual void print();
    string witnessInformation() override;
    //*****************************
    //*****************************
    //Attributes:
    multiset<set<unsigned> > colorIncidence; // A set with at most $k$ elements (where k is the maximum number of colors) a,b together in a set means that 
	    				// there is a color such that an edge incident with a and an edge incident with b have the same color
    //*****************************
    //*****************************
};

class EdgeColoringCompact_AtMost_WitnessSet: public WitnessSetTypeOne<EdgeColoringCompact_AtMost_WitnessSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<EdgeColoringCompact_AtMost_WitnessSet> EdgeColoringCompact_AtMost_WitnessSetPointer ;

class EdgeColoringCompact_AtMost_DynamicCore: public DynamicCore{
    void createInitialWitnessSet_implementation();
    void copyWitness(EdgeColoringCompact_AtMost_WitnessPointer w_input, EdgeColoringCompact_AtMost_WitnessPointer w_output);
    EdgeColoringCompact_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, EdgeColoringCompact_AtMost_WitnessPointer w, EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, EdgeColoringCompact_AtMost_WitnessPointer w, EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, EdgeColoringCompact_AtMost_WitnessPointer w, EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, EdgeColoringCompact_AtMost_WitnessPointer w1, EdgeColoringCompact_AtMost_WitnessPointer w2, EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet);
    EdgeColoringCompact_AtMost_WitnessSetPointer clean_implementation(EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(EdgeColoringCompact_AtMost_WitnessPointer w);
public:
    unsigned parameter;
    EdgeColoringCompact_AtMost_DynamicCore();
    EdgeColoringCompact_AtMost_DynamicCore(unsigned parameter);
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b,Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
    virtual bool is_final_witness(Witness &witness);
    //*****************************
    //*****************************
    // Used in the join function 
    void constructCombinations(multiset<set<unsigned> > m_1, multiset<set<unsigned> > m_2, multiset<set<unsigned> > result, set<multiset<set<unsigned> > > &L); 
    //*****************************
    //*****************************
};

#endif
