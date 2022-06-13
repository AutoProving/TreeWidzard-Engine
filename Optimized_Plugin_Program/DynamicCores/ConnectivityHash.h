// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_GENERIC_H
#define DYNAMIC_GENERIC_H
#include "../Kernel/WitnessSet.h"
#include "../Kernel/DynamicCore.h"


using namespace std;
class ConnectivityHash_Witness;

typedef shared_ptr<ConnectivityHash_Witness> ConnectivityHash_WitnessPointer;
typedef shared_ptr<ConnectivityHash_Witness const> ConnectivityHash_WitnessPointerConst;

class ConnectivityHash_Witness: public Witness, public enable_shared_from_this<ConnectivityHash_Witness>{
public:
    bool is_equal_implementation(ConnectivityHash_WitnessPointerConst w) const;
    bool is_less_implementation(ConnectivityHash_WitnessPointerConst w) const;
    Witness& set_equal_implementation(ConnectivityHash_WitnessPointerConst w);
    ~ConnectivityHash_Witness();
    virtual bool is_equal(const Witness &rhs) const override;
    virtual bool is_less(const Witness &rhs) const override;
    virtual Witness& set_equal(const Witness &witness) override;
    virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap) const override;
    virtual void print() const override;
    string witnessInformation() const override;
    virtual void hash(Hasher &h) const override;
    //*****************************
    //*****************************
    set<set<unsigned>> partition;
    uint8_t tag = 0;
    //*****************************
    //*****************************
};

class ConnectivityHash_WitnessSet: public WitnessSetTypeTwo<ConnectivityHash_WitnessSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;

};

typedef shared_ptr<ConnectivityHash_WitnessSet> ConnectivityHash_WitnessSetPointer;

class ConnectivityHash_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(ConnectivityHash_WitnessPointer w_input, ConnectivityHash_WitnessPointer w_output);
    ConnectivityHash_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, ConnectivityHash_WitnessPointerConst w, ConnectivityHash_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, ConnectivityHash_WitnessPointerConst w, ConnectivityHash_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, ConnectivityHash_WitnessPointerConst w, ConnectivityHash_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, ConnectivityHash_WitnessPointerConst w1, ConnectivityHash_WitnessPointerConst w2, ConnectivityHash_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(ConnectivityHash_WitnessPointerConst w);
public:
    ConnectivityHash_DynamicCore();
    ~ConnectivityHash_DynamicCore(){};
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
