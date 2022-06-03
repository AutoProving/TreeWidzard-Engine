// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_HAMILTONIAN_CYCLE_H
#define DYNAMIC_HAMILTONIAN_CYCLE_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class HamiltonianCycle_Witness;
typedef shared_ptr<HamiltonianCycle_Witness> HamiltonianCycle_WitnessPointer;
typedef shared_ptr<HamiltonianCycle_Witness const > HamiltonianCycle_WitnessPointerConst;

class HamiltonianCycle_Witness: public Witness, public enable_shared_from_this<HamiltonianCycle_Witness>{
public:
    ~HamiltonianCycle_Witness();
    bool is_equal_implementation(HamiltonianCycle_WitnessPointerConst w) const;
    bool is_less_implementation(HamiltonianCycle_WitnessPointerConst w) const;
    Witness& set_equal_implementation(HamiltonianCycle_WitnessPointerConst w);
    virtual bool is_equal(const Witness &rhs) const;
    virtual bool is_less(const Witness &rhs) const;
    virtual Witness& set_equal(const Witness &witness);
    shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) const override;
    virtual void print();
    string witnessInformation() const override;
    //*****************************
    //*****************************
    std::set<unsigned> degree_0, degree_1, degree_2;
    std::map<unsigned, unsigned> matching;
    bool closed = false;
    //*****************************
    //*****************************
};
class HamiltonianCycle_WitnessSet : public WitnessSetTypeTwo<HamiltonianCycle_WitnessSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<HamiltonianCycle_WitnessSet> HamiltonianCycle_WitnessSetPointer ;


class HamiltonianCycle_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(HamiltonianCycle_WitnessPointer w_input, HamiltonianCycle_WitnessPointer w_output);
    HamiltonianCycle_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, HamiltonianCycle_WitnessPointerConst w, HamiltonianCycle_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, HamiltonianCycle_WitnessPointerConst w, HamiltonianCycle_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, HamiltonianCycle_WitnessPointerConst w, HamiltonianCycle_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, HamiltonianCycle_WitnessPointerConst w1, HamiltonianCycle_WitnessPointerConst w2, HamiltonianCycle_WitnessSetPointer witnessSet);
    HamiltonianCycle_WitnessSetPointer  clean_implementation(HamiltonianCycle_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(HamiltonianCycle_WitnessPointerConst w);
public:
    unsigned parameter;
    HamiltonianCycle_DynamicCore();
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, const Witness &witness) override;
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness) override;
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b, const Witness &witness) override;
    virtual WitnessSetPointer join(Bag &b, const Witness &witness1, const Witness &witness2) override;
    virtual WitnessSetPointer clean(WitnessSetPointer witnessSet) override;
    virtual bool is_final_witness(const Witness &witness) override;
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    //*****************************
    //*****************************
};


#endif
