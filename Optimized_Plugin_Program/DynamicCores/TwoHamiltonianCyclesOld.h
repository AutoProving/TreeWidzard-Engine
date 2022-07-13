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
    bool is_equal_implementation(TwoHamiltonianCycles_WitnessPointerConst w) const;
    bool is_less_implementation(TwoHamiltonianCycles_WitnessPointerConst w) const;
    Witness& set_equal_implementation(TwoHamiltonianCycles_WitnessPointerConst w);
    virtual bool is_equal(const Witness &rhs) const override;
    virtual void hash(Hasher &h) const override;
    virtual bool is_less(const Witness &rhs) const override;
    virtual Witness& set_equal(const Witness &witness);
    shared_ptr<Witness> relabel(const map<unsigned int, unsigned int> &relabelingMap) const override;
    virtual void print() const override;
    string witnessInformation() const override;
    //*****************************
    //*****************************
    struct VertexInfo {
      unsigned degree = -1U;
      unsigned match = -1U;
      bool operator<(const VertexInfo &other) const {
        return tie(degree, match) < tie(other.degree, other.match);
      }
      bool operator==(const VertexInfo &other) const {
        return tie(degree, match) == tie(other.degree, other.match);
      }
    };
    vector<VertexInfo> info[2];
    bool closed[2] = {false, false};
    bool was_different = false;
    //*****************************
    //*****************************
};
class TwoHamiltonianCycles_WitnessSet : public WitnessSetTypeTwo<TwoHamiltonianCycles_WitnessSet>{
    shared_ptr<WitnessSetBase> createEmptyWitnessSet() override;
};

typedef shared_ptr<TwoHamiltonianCycles_WitnessSet> TwoHamiltonianCycles_WitnessSetPointer ;


class TwoHamiltonianCycles_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(TwoHamiltonianCycles_WitnessPointer w_input, TwoHamiltonianCycles_WitnessPointer w_output);
    TwoHamiltonianCycles_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, TwoHamiltonianCycles_WitnessPointerConst w, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, TwoHamiltonianCycles_WitnessPointerConst w, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, TwoHamiltonianCycles_WitnessPointerConst w, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, TwoHamiltonianCycles_WitnessPointerConst w1, TwoHamiltonianCycles_WitnessPointerConst w2, TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    TwoHamiltonianCycles_WitnessSetPointer  clean_implementation(TwoHamiltonianCycles_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(TwoHamiltonianCycles_WitnessPointerConst w);
public:
    unsigned parameter;
    TwoHamiltonianCycles_DynamicCore();
    void createInitialWitnessSet();
    virtual WitnessSetBasePointer intro_v(unsigned i, Bag &b, const Witness &witness) override;
    virtual WitnessSetBasePointer intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness) override;
    virtual WitnessSetBasePointer forget_v(unsigned i, Bag &b, const Witness &witness) override;
    virtual WitnessSetBasePointer join(Bag &b, const Witness &witness1, const Witness &witness2) override;
    virtual WitnessSetBasePointer clean(WitnessSetBasePointer witnessSet) override;
    virtual bool is_final_witness(const Witness &witness) override;
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    //*****************************
    //*****************************
};


#endif
