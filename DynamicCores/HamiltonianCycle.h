// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_HAMILTONIAN_CYCLE_H
#define DYNAMIC_HAMILTONIAN_CYCLE_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class HamiltonianCycle_Witness: public Witness, public enable_shared_from_this<HamiltonianCycle_Witness>{
public:
    ~HamiltonianCycle_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    std::set<unsigned> degree_0, degree_1, degree_2;
    std::map<unsigned, unsigned> matching;
    bool closed = false;
    //*****************************
    //*****************************
};
class HamiltonianCycle_WitnessSet : public WitnessSetTypeOne<HamiltonianCycle_WitnessSet>{


};

class HamiltonianCycle_DynamicCore: public DynamicCore{
public:
    unsigned parameter;
    HamiltonianCycle_DynamicCore();
    void createInitialWitnessSet();
    virtual shared_ptr<WitnessSet> intro_v(unsigned i, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> forget_v(unsigned i, Bag &b,Witness &witness);
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