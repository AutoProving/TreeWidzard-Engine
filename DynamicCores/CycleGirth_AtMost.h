// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CYCLEGIRTH_ATMOST_H
#define CYCLEGIRTH_ATMOST_H

#include "../Kernel/Bag.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/Witness.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class CycleGirth_AtMost_Witness;

typedef shared_ptr<CycleGirth_AtMost_Witness> CycleGirth_AtMost_WitnessPointer;
typedef shared_ptr<CycleGirth_AtMost_Witness const> CycleGirth_AtMost_WitnessPointerConst;

class CycleGirth_AtMost_Witness : public Witness, public enable_shared_from_this<CycleGirth_AtMost_Witness> {
public:
    bool is_equal_implementation(const CycleGirth_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const CycleGirth_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(CycleGirth_AtMost_WitnessPointer w);
    ~CycleGirth_AtMost_Witness();
    virtual bool is_equal(const Witness &rhs) const;
    virtual bool is_less(const Witness &rhs) const;
    virtual Witness &set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    unsigned current_size = 0;
    set<unsigned> degree_0 = set<unsigned>();
    set<unsigned> degree_2 = set<unsigned>();
    map<unsigned, unsigned> extremity = map<unsigned, unsigned>();
    bool found_cycle = false;
    //*****************************
    //*****************************
};

class CycleGirth_AtMost_WitnessSet : public WitnessSetTypeOne<CycleGirth_AtMost_WitnessSet>{

    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<CycleGirth_AtMost_WitnessSet> CycleGirth_AtMost_WitnessSetPointer;

class CycleGirth_AtMost_DynamicCore : public DynamicCore {
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(CycleGirth_AtMost_WitnessPointer w_input, CycleGirth_AtMost_WitnessPointer w_output);
    CycleGirth_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, CycleGirth_AtMost_WitnessPointer w, CycleGirth_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, CycleGirth_AtMost_WitnessPointer w, CycleGirth_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, CycleGirth_AtMost_WitnessPointer w, CycleGirth_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, CycleGirth_AtMost_WitnessPointer w1, CycleGirth_AtMost_WitnessPointer w2, CycleGirth_AtMost_WitnessSetPointer witnessSet);
    CycleGirth_AtMost_WitnessSetPointer clean_implementation(CycleGirth_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(CycleGirth_AtMost_WitnessPointer w);
public:
    unsigned parameter;
    CycleGirth_AtMost_DynamicCore();
    CycleGirth_AtMost_DynamicCore(unsigned parameter);
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    virtual WitnessSetPointer clean(shared_ptr<WitnessSet> witnessSet);
    virtual bool is_final_witness(Witness &witness);
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    //*****************************
    //*****************************
};
#endif