// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef INDEPENDENTSET_ATLEAST_h
#define INDEPENDENTSET_ATLEAST_h
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class IndependentSet_AtLeast_Witness;

typedef shared_ptr<IndependentSet_AtLeast_Witness> IndependentSet_AtLeast_WitnessPointer;
typedef shared_ptr<IndependentSet_AtLeast_Witness const> IndependentSet_AtLeast_WitnessPointerConst;

class IndependentSet_AtLeast_Witness: public Witness, public enable_shared_from_this<IndependentSet_AtLeast_Witness>{
public:
    bool is_equal_implementation(const IndependentSet_AtLeast_WitnessPointerConst w) const;
    bool is_less_implementation(const IndependentSet_AtLeast_WitnessPointerConst w) const;
    Witness& set_equal_implementation(IndependentSet_AtLeast_WitnessPointer w);
    ~IndependentSet_AtLeast_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your witness.
    set<unsigned> partialIndSet;
    unsigned size = 0;
    bool found = false;
    //*****************************
    //*****************************
};
class IndependentSet_AtLeast_WitnessSet : public WitnessSetTypeOne<IndependentSet_AtLeast_WitnessSet>{
 shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<IndependentSet_AtLeast_WitnessSet> IndependentSet_AtLeast_WitnessSetPointer;
class IndependentSet_AtLeast_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(IndependentSet_AtLeast_WitnessPointer w_input, IndependentSet_AtLeast_WitnessPointer w_output);
    IndependentSet_AtLeast_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, IndependentSet_AtLeast_WitnessPointer w, IndependentSet_AtLeast_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, IndependentSet_AtLeast_WitnessPointer w, IndependentSet_AtLeast_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, IndependentSet_AtLeast_WitnessPointer w, IndependentSet_AtLeast_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, IndependentSet_AtLeast_WitnessPointer w1, IndependentSet_AtLeast_WitnessPointer w2, IndependentSet_AtLeast_WitnessSetPointer witnessSet);
    WitnessSetPointer clean_implementation(IndependentSet_AtLeast_WitnessSetPointer  witnessSet);
    bool is_final_witness_implementation(IndependentSet_AtLeast_WitnessPointer w);
public:
    unsigned parameter;
    IndependentSet_AtLeast_DynamicCore();
    IndependentSet_AtLeast_DynamicCore(unsigned parameter);
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b,Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
    virtual bool is_final_witness(Witness &witness);
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    //*****************************
    //*****************************
};

#endif