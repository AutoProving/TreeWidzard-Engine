// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_GENERIC_H
#define DYNAMIC_GENERIC_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <tuple>
#include "../Multigraph/MultiGraph.h"

using namespace std;
class Minor_Witness;
// Pointer type for Minor_Witness
typedef shared_ptr<Minor_Witness> Minor_WitnessPointer;
typedef shared_ptr<Minor_Witness const > Minor_WitnessPointerConst;

class Minor_Witness: public Witness, public enable_shared_from_this<Minor_Witness>{
public:
    bool is_equal_implementation(const Minor_WitnessPointerConst w) const;
    bool is_less_implementation(const Minor_WitnessPointerConst w) const;
    Witness& set_equal_implementation(Minor_WitnessPointer w);
    ~Minor_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    bool found;
    vector<tuple<set<unsigned >, set<set<unsigned >>,bool> > partitions;
    vector<bool> edgeFoundVector;
    //*****************************
    //*****************************
};

class Minor_WitnessSet : public WitnessSetTypeOne<Minor_WitnessSet> {
    //************************
    //************************
    //Do not define anything. All functions and memebers are derived from WitnessTypeOne.
    //************************
    //************************
};

// Pointer type for Minor_WitnessSet.
typedef shared_ptr<Minor_WitnessSet> Minor_WitnessSetPointer;

class Minor_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(Minor_WitnessPointer w_input, Minor_WitnessPointer w_output);
    Minor_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, Minor_WitnessPointer w1, Minor_WitnessPointer w2, Minor_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(Minor_WitnessPointer w);
public:
    MultiGraph multigraph;
    Minor_DynamicCore();
    Minor_DynamicCore(MultiGraph multiGraph);
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

