// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_GENERIC_H
#define DYNAMIC_GENERIC_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class Connectivity_Witness;

typedef shared_ptr<Connectivity_Witness> Connectivity_WitnessPointer;
typedef shared_ptr<Connectivity_Witness const> Connectivity_WitnessPointerConst;

class Connectivity_Witness: public Witness, public enable_shared_from_this<Connectivity_Witness>{
public:
    bool is_equal_implementation(const Connectivity_WitnessPointerConst w) const;
    bool is_less_implementation(const Connectivity_WitnessPointerConst w) const;
    Witness& set_equal_implementation(Connectivity_WitnessPointer w);
    ~Connectivity_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    set<set<unsigned>> partition;
    //*****************************
    //*****************************
};

class Connectivity_WitnessSet: public WitnessSetTypeOne<Connectivity_WitnessSet>{

};

typedef shared_ptr<Connectivity_WitnessSet> Connectivity_WitnessSetPointer;

class Connectivity_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(Connectivity_WitnessPointer w_input, Connectivity_WitnessPointer w_output);
    Connectivity_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, Connectivity_WitnessPointer w, Connectivity_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, Connectivity_WitnessPointer w, Connectivity_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, Connectivity_WitnessPointer w, Connectivity_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, Connectivity_WitnessPointer w1, Connectivity_WitnessPointer w2, Connectivity_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(Connectivity_WitnessPointer w);
public:
    unsigned parameter;
    Connectivity_DynamicCore();
    Connectivity_DynamicCore(unsigned parameter);
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