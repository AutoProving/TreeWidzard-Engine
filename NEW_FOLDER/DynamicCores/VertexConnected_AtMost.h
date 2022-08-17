// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef VERTEXCONNECTED_ATMOST_H
#define VERTEXCONNECTED_ATMOST_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class VertexConnected_AtMost_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<VertexConnected_AtMost_Witness> VertexConnected_AtMost_WitnessPointer;
typedef shared_ptr<VertexConnected_AtMost_Witness const > VertexConnected_AtMost_WitnessPointerConst;

class VertexConnected_AtMost_Witness: public Witness, public enable_shared_from_this<VertexConnected_AtMost_Witness>{
public:

    bool is_equal_implementation(const VertexConnected_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const VertexConnected_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(VertexConnected_AtMost_WitnessPointer w);
    VertexConnected_AtMost_Witness(); // Constructor
    ~VertexConnected_AtMost_Witness() override;
    virtual void print();
    string witnessInformation() override;
    virtual bool is_equal(const Witness &rhs) const; //Comparator
    virtual bool is_less(const Witness &rhs) const;  //Comparator
    virtual Witness& set_equal(Witness &witness); //Assignment operator
    virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);

    //*****************************
    //*****************************
    bool found = false;
    unsigned sizeOfDisconnectedSet = 0;
    // This is the set of vertices that when removed from the graph will make the graph disconnected
    set<unsigned> disconnectingSet;
    bool partitionProcessed = false;
    // This is the partition that is build by pretending that the vertices in disconnectingSet do not exist
    set<set<unsigned>> partition;
    //*****************************
    //*****************************

};

class VertexConnected_AtMost_WitnessSet: public WitnessSetTypeOne<VertexConnected_AtMost_WitnessSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<VertexConnected_AtMost_WitnessSet> VertexConnected_AtMost_WitnessSetPointer;


class VertexConnected_AtMost_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(VertexConnected_AtMost_WitnessPointer w_input, VertexConnected_AtMost_WitnessPointer w_output);
    VertexConnected_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, VertexConnected_AtMost_WitnessPointer w, VertexConnected_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, VertexConnected_AtMost_WitnessPointer w, VertexConnected_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, VertexConnected_AtMost_WitnessPointer w, VertexConnected_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, VertexConnected_AtMost_WitnessPointer w1, VertexConnected_AtMost_WitnessPointer w2, VertexConnected_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(VertexConnected_AtMost_WitnessPointer w);
    shared_ptr<WitnessSet> clean_implementation(VertexConnected_AtMost_WitnessSetPointer witnessSet);
public:
    unsigned parameter; 
    VertexConnected_AtMost_DynamicCore();
    VertexConnected_AtMost_DynamicCore(unsigned parameter);
    ~VertexConnected_AtMost_DynamicCore(){};
    void createInitialWitnessSet();
    WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness) override;
    WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) override;
    WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness) override;
    WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2) override;
    WitnessSetPointer clean(WitnessSetPointer witnessSet) override;
    bool is_final_witness(Witness &witness) override;//*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    virtual bool has_common_vertices(set<unsigned> disconnectingSet, set<set<unsigned>> partition);
    //*****************************
    //*****************************
};


#endif
