// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef NoWhereZeroFlow_AtMost_H
#define NoWhereZeroFlow_AtMost_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include "../Multigraph/MultiGraph.h"

using namespace std;
class NoWhereZeroFlow_AtMost_Witness;
// Pointer type for NoWhereZeroFlow_AtMost_Witness
typedef shared_ptr<NoWhereZeroFlow_AtMost_Witness> NoWhereZeroFlow_AtMost_WitnessPointer;
typedef shared_ptr<NoWhereZeroFlow_AtMost_Witness const > NoWhereZeroFlow_AtMost_WitnessPointerConst;

class NoWhereZeroFlow_AtMost_Witness: public Witness, public enable_shared_from_this<NoWhereZeroFlow_AtMost_Witness>{
public:
    bool is_equal_implementation(NoWhereZeroFlow_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(NoWhereZeroFlow_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(NoWhereZeroFlow_AtMost_WitnessPointer w);
    ~NoWhereZeroFlow_AtMost_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
    virtual void print();
    //*****************************
    //******BEGIN**ATTRIBUTES*******
    //*****************************
    //The unique atribute is a map that sends each vertex to the current sum
    //of the flows around those vertices. The sum will be updated once an edge 
    //is introduced 
    map<unsigned ,unsigned > flowSum;
    //*****************************
    //*******END**ATTRIBUTES*******
    //*****************************
    //*****************************
    //*****************************
};

class NoWhereZeroFlow_AtMost_WitnessSet : public WitnessSetTypeOne<NoWhereZeroFlow_AtMost_WitnessSet> {
    //************************
    //************************
    //Do not define anything. All functions and members are derived from WitnessTypeOne in this case.
    //************************
    //************************
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

// Pointer type for NoWhereZeroFlow_AtMost_WitnessSet.
typedef shared_ptr<NoWhereZeroFlow_AtMost_WitnessSet> NoWhereZeroFlow_AtMost_WitnessSetPointer;

class NoWhereZeroFlow_AtMost_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(NoWhereZeroFlow_AtMost_WitnessPointer w_input, NoWhereZeroFlow_AtMost_WitnessPointer w_output);
    NoWhereZeroFlow_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w1, NoWhereZeroFlow_AtMost_WitnessPointer w2, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(NoWhereZeroFlow_AtMost_WitnessPointer w);
    shared_ptr<WitnessSet> clean_implementation(NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet);

public:
    unsigned parameter;
    NoWhereZeroFlow_AtMost_DynamicCore();
    NoWhereZeroFlow_AtMost_DynamicCore(unsigned parameter);
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

