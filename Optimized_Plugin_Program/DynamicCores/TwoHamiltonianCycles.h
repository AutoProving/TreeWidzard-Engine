// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_TWO_HAMILTONIAN_CYCLES_H
#define DYNAMIC_TWO_HAMILTONIAN_CYCLES_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class TwoHamiltonianCycles_Witness;
typedef shared_ptr<TwoHamiltonianCycles_Witness> TwoHamiltonianCycles_WitnessPointer;
typedef shared_ptr<TwoHamiltonianCycles_Witness const > TwoHamiltonianCycles_WitnessPointerConst;

class TwoHamiltonianCycles_Witness: public Witness, public enable_shared_from_this<TwoHamiltonianCycles_Witness>{
public:
    ~TwoHamiltonianCycles_Witness();
    bool is_equal_implementation(const TwoHamiltonianCycles_WitnessPointerConst w) const;
    bool is_less_implementation(const TwoHamiltonianCycles_WitnessPointerConst w) const;
    Witness& set_equal_implementation(TwoHamiltonianCycles_WitnessPointer w);
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
    virtual void print();
    string witnessInformation() override;
    //*****************************
    //*****************************
    std::vector<unsigned> degree[2];
    std::vector<unsigned> matching[2];
    bool closed[2] = {false, false};
    bool was_different = false;
    //*****************************
    //*****************************
};
class TwoHamiltonianCycles_WitnessSet : public WitnessSetTypeOne<TwoHamiltonianCycles_WitnessSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<TwoHamiltonianCycles_WitnessSet> TwoHamiltonianCycles_WitnessSetPointer ;


class TwoHamiltonianCycles_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(TwoHamiltonianCycles_WitnessPointer w_input, TwoHamiltonianCycles_WitnessPointer w_output);
    TwoHamiltonianCycles_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, TwoHamiltonianCycles_WitnessPointer w, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, TwoHamiltonianCycles_WitnessPointer w, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, TwoHamiltonianCycles_WitnessPointer w, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, TwoHamiltonianCycles_WitnessPointer w1, TwoHamiltonianCycles_WitnessPointer w2, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    TwoHamiltonianCycles_WitnessSetPointer  clean_implementation(TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(TwoHamiltonianCycles_WitnessPointer w);
public:
    unsigned parameter;
    TwoHamiltonianCycles_DynamicCore();
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
