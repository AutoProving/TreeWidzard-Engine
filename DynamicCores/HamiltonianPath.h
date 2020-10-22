// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_HAMILTONIAN_PATH_H
#define DYNAMIC_HAMILTONIAN_PATH_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class HamiltonianPath_Witness;

typedef shared_ptr<HamiltonianPath_Witness> HamiltonianPath_WitnessPointer;
typedef shared_ptr<HamiltonianPath_Witness const> HamiltonianPath_WitnessPointerConst;
class HamiltonianPath_Witness: public Witness, public enable_shared_from_this<HamiltonianPath_Witness>{
public:
    bool is_equal_implementation(const HamiltonianPath_WitnessPointerConst w) const;
    bool is_less_implementation(const HamiltonianPath_WitnessPointerConst w) const;
    Witness& set_equal_implementation(HamiltonianPath_WitnessPointer w);
    ~HamiltonianPath_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    std::set<unsigned> degree_0, degree_1, degree_2;
    std::map<unsigned, unsigned> matching;
    //*****************************
    //*****************************
};

class HamiltonianPath_WitnessSet : public WitnessSetTypeOne<HamiltonianPath_WitnessSet>{

};

typedef shared_ptr<HamiltonianPath_WitnessSet> HamiltonianPath_WitnessSetPointer ;

class HamiltonianPath_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(HamiltonianPath_WitnessPointer w_input, HamiltonianPath_WitnessPointer w_output);
    HamiltonianPath_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, HamiltonianPath_WitnessPointer w, HamiltonianPath_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, HamiltonianPath_WitnessPointer w, HamiltonianPath_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, HamiltonianPath_WitnessPointer w, HamiltonianPath_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, HamiltonianPath_WitnessPointer w1, HamiltonianPath_WitnessPointer w2, HamiltonianPath_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(HamiltonianPath_WitnessPointer w);
public:
    unsigned parameter;
    HamiltonianPath_DynamicCore();
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