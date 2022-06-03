// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_GENERIC_H
#define DYNAMIC_GENERIC_H
#include "../Kernel/WitnessSet.h"
#include "../Kernel/DynamicCore.h"


using namespace std;
class Connectivity_Witness;

typedef shared_ptr<Connectivity_Witness> Connectivity_WitnessPointer;
typedef shared_ptr<Connectivity_Witness const> Connectivity_WitnessPointerConst;

class Connectivity_Witness: public Witness, public enable_shared_from_this<Connectivity_Witness>{
public:
    bool is_equal_implementation(Connectivity_WitnessPointerConst w) const;
    bool is_less_implementation(Connectivity_WitnessPointerConst w) const;
    Witness& set_equal_implementation(Connectivity_WitnessPointerConst w);
    ~Connectivity_Witness();
    virtual bool is_equal(const Witness &rhs) const override;
    virtual bool is_less(const Witness &rhs) const override;
    virtual Witness& set_equal(const Witness &witness) override;
    virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap) const override;
    virtual void print() const override;
    string witnessInformation() const override;
    //*****************************
    //*****************************
    set<set<unsigned>> partition;
    uint8_t tag = 0;
    //*****************************
    //*****************************
};

class Connectivity_WitnessSet: public WitnessSetTypeTwo<Connectivity_WitnessSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;

};

typedef shared_ptr<Connectivity_WitnessSet> Connectivity_WitnessSetPointer;

class Connectivity_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(Connectivity_WitnessPointer w_input, Connectivity_WitnessPointer w_output);
    Connectivity_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, Connectivity_WitnessPointerConst w, Connectivity_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, Connectivity_WitnessPointerConst w, Connectivity_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, Connectivity_WitnessPointerConst w, Connectivity_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, Connectivity_WitnessPointerConst w1, Connectivity_WitnessPointerConst w2, Connectivity_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(Connectivity_WitnessPointerConst w);
public:
    Connectivity_DynamicCore();
    ~Connectivity_DynamicCore(){};
    void createInitialWitnessSet();
    WitnessSetPointer intro_v(unsigned i, Bag &b, const Witness &witness);
    WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness);
    WitnessSetPointer forget_v(unsigned i, Bag &b, const Witness &witness);
    WitnessSetPointer join(Bag &b, const Witness &witness1, const Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    bool is_final_witness(const Witness &witness);
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    //*****************************
    //*****************************
};


#endif
