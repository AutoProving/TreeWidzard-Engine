// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_TWO_HAMILTONIAN_CYCLES_H
#define DYNAMIC_TWO_HAMILTONIAN_CYCLES_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class TwoHamiltonianCycles_Witness_Old;
typedef shared_ptr<TwoHamiltonianCycles_Witness_Old> TwoHamiltonianCycles_Witness_OldPointer;
typedef shared_ptr<TwoHamiltonianCycles_Witness_Old const > TwoHamiltonianCycles_Witness_OldPointerConst;

class TwoHamiltonianCycles_Witness_Old: public Witness, public enable_shared_from_this<TwoHamiltonianCycles_Witness_Old>{
public:
    ~TwoHamiltonianCycles_Witness_Old();
    bool is_equal_implementation(TwoHamiltonianCycles_Witness_OldPointerConst w) const;
    bool is_less_implementation(TwoHamiltonianCycles_Witness_OldPointerConst w) const;
    Witness& set_equal_implementation(TwoHamiltonianCycles_Witness_OldPointerConst w);
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
class TwoHamiltonianCycles_Witness_OldSet : public WitnessSetTypeTwo<TwoHamiltonianCycles_Witness_OldSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<TwoHamiltonianCycles_Witness_OldSet> TwoHamiltonianCycles_Witness_OldSetPointer ;


class TwoHamiltonianCycles_DynamicCore_Old: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(TwoHamiltonianCycles_Witness_OldPointer w_input, TwoHamiltonianCycles_Witness_OldPointer w_output);
    TwoHamiltonianCycles_Witness_OldPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, TwoHamiltonianCycles_Witness_OldPointerConst w, TwoHamiltonianCycles_Witness_OldSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, TwoHamiltonianCycles_Witness_OldPointerConst w, TwoHamiltonianCycles_Witness_OldSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, TwoHamiltonianCycles_Witness_OldPointerConst w, TwoHamiltonianCycles_Witness_OldSetPointer witnessSet);
    void join_implementation(Bag &b, TwoHamiltonianCycles_Witness_OldPointerConst w1, TwoHamiltonianCycles_Witness_OldPointerConst w2, TwoHamiltonianCycles_Witness_OldSetPointer witnessSet);
    TwoHamiltonianCycles_Witness_OldSetPointer  clean_implementation(TwoHamiltonianCycles_Witness_OldSetPointer witnessSet);
    bool is_final_witness_implementation(TwoHamiltonianCycles_Witness_OldPointerConst w);
public:
    unsigned parameter;
    TwoHamiltonianCycles_DynamicCore_Old();
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
