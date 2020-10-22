// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef VERTEXCOVER_ATMOST_H
#define VERTEXCOVER_ATMOST_H

#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class VertexCover_AtMost_Witness;

typedef shared_ptr<VertexCover_AtMost_Witness> VertexCover_AtMost_WitnessPointer;
typedef shared_ptr<VertexCover_AtMost_Witness const> VertexCover_AtMost_WitnessPointerConst;

class VertexCover_AtMost_Witness : public Witness, public enable_shared_from_this<VertexCover_AtMost_Witness>
{
public:
    bool is_equal_implementation(const VertexCover_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const VertexCover_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(VertexCover_AtMost_WitnessPointer w);
    VertexCover_AtMost_Witness() = default;
    ~VertexCover_AtMost_Witness();
    virtual bool is_equal(const Witness &rhs) const;
    virtual bool is_less(const Witness &rhs) const;
    virtual Witness &set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    set<unsigned> partialCover;
    unsigned int cost;
    //*****************************
    //*****************************
};
class VertexCover_AtMost_WitnessSet : public WitnessSetTypeOne<VertexCover_AtMost_WitnessSet>{

};

typedef shared_ptr<VertexCover_AtMost_WitnessSet> VertexCover_AtMost_WitnessSetPointer;

class VertexCover_AtMost_DynamicCore : public DynamicCore
{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(VertexCover_AtMost_WitnessPointer w_input, VertexCover_AtMost_WitnessPointer w_output);
    VertexCover_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, VertexCover_AtMost_WitnessPointer w, VertexCover_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, VertexCover_AtMost_WitnessPointer w, VertexCover_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, VertexCover_AtMost_WitnessPointer w, VertexCover_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, VertexCover_AtMost_WitnessPointer w1, VertexCover_AtMost_WitnessPointer w2, VertexCover_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(VertexCover_AtMost_WitnessPointer w);
public:
    VertexCover_AtMost_DynamicCore();
    VertexCover_AtMost_DynamicCore(unsigned parameter);
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
    virtual bool is_final_witness(Witness &witness);

    //*****************************
    //*****************************
    unsigned int parameter;
    //*****************************
    //*****************************
};

#endif