// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef EDGECONNNECTED_ATMOST_H
#define EDGECONNNECTED_ATMOST_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <algorithm>

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
    shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
    virtual void print();
    string witnessInformation() override;
    //*****************************
    //*****************************
    // Specific Attributes
    bool found; // True if a disconnecting set has been found. 
    bool processed; // Indicates that a partition has been processed or not.
    unsigned size ; // Size of the disconnected edges.
    set<pair<unsigned ,unsigned >> disconnectingEdges; // Set of edges that when removed will disconnect the graph.
    set<set<unsigned >> partition; // Partition of the vertices.
    // Auxiliary functions
    void removeIncidentEdges(unsigned i); // remove all incident edges to "i" from disconnectingEdges.
    // The following function returns a pair of bools. The first is true if the partition has at least two cells where {i} is one of the cells.
    // Therefore, if we delete i, we will have at least two connected components. 
    // the second is true when {i} is the only cell of the partition. This means that that component represented is connected but 
    // has been completely forgotten. Therefore the introduction of any new vertex gives rise to a disconnected graph. 
    friend pair<bool,bool> removeVertexFromPartition(unsigned  i, set<set<unsigned >> &partition);
    // The next function is used to collapse cells of a partition after adding an edge between two cells. 
    friend void addEdgeToPartition(unsigned i, unsigned j, set<set<unsigned >> &partition);
    // The next function merges two partitions by collapsing cells appropriately whenever there is an intersection. 
    friend set<set<unsigned >> mergePartitions(set<set<unsigned >> &partition1, set<set<unsigned >> &partition2);
    //*****************************
    //*****************************
};

class EdgeConnected_AtMost_WitnessSet : public WitnessSetTypeOne<EdgeConnected_AtMost_WitnessSet> {

    //************************
    //************************
    //Do not define anything. All functions and members are derived from WitnessTypeOne.
    //************************
    //************************
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
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
    shared_ptr<WitnessSet> clean_implementation(EdgeConnected_AtMost_WitnessSetPointer witnessSet);
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
