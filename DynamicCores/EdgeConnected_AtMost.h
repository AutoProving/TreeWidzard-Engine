// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

/////////////////////////////////////////////////////////////////////////////
// Before starting to work on your core, please create a new branch, and rename
// these files according to the instructions in the file README-GenericCore.txt
/////////////////////////////////////////////////////////////////////////////

#ifndef EDGECONNNECTED_ATMOST_H
#define EDGECONNNECTED_ATMOST_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class EdgeConnected_AtMost_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<EdgeConnected_AtMost_Witness> EdgeConnected_AtMost_WitnessPointer;
typedef shared_ptr<EdgeConnected_AtMost_Witness const > EdgeConnected_AtMost_WitnessPointerConst;

class EdgeConnected_AtMost_Witness: public Witness, public enable_shared_from_this<EdgeConnected_AtMost_Witness>{
public:
    bool is_equal_implementation(const EdgeConnected_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const EdgeConnected_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(EdgeConnected_AtMost_WitnessPointer w);
    ~EdgeConnected_AtMost_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your witness.
    bool found; // Shows a disconnectedness has been found.
    bool processed; // Indicates that a partition has been processed or not.
    set<set<unsigned >> partition; // Partition of the vertices.
    set<pair<unsigned ,unsigned >> disconnectedEdges; // Set of edges are considered as a disconnected edges.
    unsigned size ; // Size of the disconnected edges.
    void removeIncidentEdges(unsigned i); // remove all incident edges to "i" from disconnectedEdges.
    friend pair<bool,bool> removeVertexFromPartition(unsigned  i, set<set<unsigned >> &partition);
    friend void addEdgeToPartition(unsigned i, unsigned j, set,set<unsigned >> &partition);
    //*****************************
    //*****************************
};

class EdgeConnected_AtMost_WitnessSet : public WitnessSetTypeOne<EdgeConnected_AtMost_WitnessSet> {

    //************************
    //************************
    //Do not define anything. All functions and members are derived from WitnessTypeOne.
    //************************
    //************************
};

// Pointer type for Generic_Operator_WitnessSet.
typedef shared_ptr<EdgeConnected_AtMost_WitnessSet> EdgeConnected_AtMost_WitnessSetPointer;

class EdgeConnected_AtMost_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(EdgeConnected_AtMost_WitnessPointer w_input, EdgeConnected_AtMost_WitnessPointer w_output);
    EdgeConnected_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, EdgeConnected_AtMost_WitnessPointer w1, EdgeConnected_AtMost_WitnessPointer w2, EdgeConnected_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(EdgeConnected_AtMost_WitnessPointer w);
public:
    unsigned parameter;
    EdgeConnected_AtMost_DynamicCore();
    EdgeConnected_AtMost_DynamicCore(unsigned parameter);
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