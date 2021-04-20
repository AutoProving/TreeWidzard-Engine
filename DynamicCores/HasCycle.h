// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_GENERIC_H
#define DYNAMIC_GENERIC_H

#include "../Kernel/Bag.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/Witness.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class HasCycle_Witness;

// Pointer type for HasCycle_Witness
typedef shared_ptr<HasCycle_Witness> HasCycle_WitnessPointer;
typedef shared_ptr<HasCycle_Witness const> HasCycle_WitnessPointerConst;

class HasCycle_Witness : public Witness, public enable_shared_from_this<HasCycle_Witness> {
public:
    bool is_equal_implementation(const HasCycle_WitnessPointerConst w) const;
    bool is_less_implementation(const HasCycle_WitnessPointerConst w) const;
    Witness &set_equal_implementation(HasCycle_WitnessPointer w);
    ~HasCycle_Witness();
    virtual bool is_equal(const Witness &rhs) const;
    virtual bool is_less(const Witness &rhs) const;
    virtual Witness &set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    //bool is_odd = false;
    set<unsigned> degree_0 = set<unsigned>();
    set<unsigned> degree_2 = set<unsigned>();
    map<unsigned, unsigned> extremity = map<unsigned, unsigned>();
    bool found_cycle = false;
    //*****************************
    //*****************************
};

class HasCycle_WitnessSet : public WitnessSetTypeOne<HasCycle_WitnessSet> {
    //************************
    //************************
    //Do not define anything. All functions and memebers are derived from WitnessTypeOne.
    //************************
    //************************
};

// Pointer type for HasCycle_WitnessSet.
typedef shared_ptr<HasCycle_WitnessSet> HasCycle_WitnessSetPointer;

class HasCycle_DynamicCore : public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(HasCycle_WitnessPointer w_input, HasCycle_WitnessPointer w_output);
    HasCycle_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, HasCycle_WitnessPointer w, HasCycle_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, HasCycle_WitnessPointer w, HasCycle_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i, unsigned j, Bag &b, HasCycle_WitnessPointer w, HasCycle_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, HasCycle_WitnessPointer w1, HasCycle_WitnessPointer w2, HasCycle_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(HasCycle_WitnessPointer w);

public:
    HasCycle_DynamicCore();
    void createInitialWitnessSet();
    virtual shared_ptr<WitnessSet> intro_v(unsigned i, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> forget_v(unsigned i, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> join(Bag &b, Witness &witness1, Witness &witness2);
    virtual shared_ptr<WitnessSet> clean(shared_ptr<WitnessSet> witnessSet);
    virtual bool is_final_witness(Witness &witness);
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    //*****************************
    //*****************************
};

#endif