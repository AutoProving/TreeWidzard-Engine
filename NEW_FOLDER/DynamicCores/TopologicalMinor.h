// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef TopologicalMinor_H
#define TopologicalMinor_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <tuple>
#include "../Multigraph/MultiGraph.h"

using namespace std;
class TopologicalMinor_Witness;
// Pointer type for TopologicalMinor_Witness
typedef shared_ptr<TopologicalMinor_Witness> TopologicalMinor_WitnessPointer;
typedef shared_ptr<TopologicalMinor_Witness const > TopologicalMinor_WitnessPointerConst;

class TopologicalMinor_Witness: public Witness, public enable_shared_from_this<TopologicalMinor_Witness>{
public:
    bool is_equal_implementation(const TopologicalMinor_WitnessPointerConst w) const;
    bool is_less_implementation(const TopologicalMinor_WitnessPointerConst w) const;
    Witness& set_equal_implementation(TopologicalMinor_WitnessPointer w);
    ~TopologicalMinor_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    bool found; // Shows that a TopologicalMinor has been found or not. If it is true it means a TopologicalMinor is found, otherwise, not.
    // Each index of this vector correspond to an edge of the multigraph, and each coordinate is a tuple
    vector<tuple<set<unsigned >, set<set<unsigned >>,bool> > partitions;
    // The following function returns a pair of bools. The first is true if the partition has at least two cells where {i} is one of the cells.
    // Therefore, if we delete i, we will have at least two connected components.
    // the second is true when {i} is the only cell of the partition. This means that that component represented is connected but
    // has been completely forgotten. Therefore the introduction of any new vertex gives rise to a disconnected graph.
    pair<bool,bool> removeVertexFromPartition(unsigned  i, set<set<unsigned >> &partition);
    // The next function is used to collapse cells of a partition after adding an edge between two cells.
    void addEdgeToPartition(unsigned i, unsigned j, set<set<unsigned >> &partition);
    // The next function merges two partitions by collapsing cells appropriately whenever there is an intersection.
    set<set<unsigned >> mergePartitions(set<set<unsigned >> &partition1, set<set<unsigned >> &partition2);
    //*****************************
    //*****************************
};

class TopologicalMinor_WitnessSet : public WitnessSetTypeOne<TopologicalMinor_WitnessSet> {
    //************************
    //************************
    //Do not define anything. All functions and memebers are derived from WitnessTypeOne.
    //************************
    //************************
};

// Pointer type for TopologicalMinor_WitnessSet.
typedef shared_ptr<TopologicalMinor_WitnessSet> TopologicalMinor_WitnessSetPointer;

class TopologicalMinor_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(TopologicalMinor_WitnessPointer w_input, TopologicalMinor_WitnessPointer w_output);
    TopologicalMinor_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, TopologicalMinor_WitnessPointer w, TopologicalMinor_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, TopologicalMinor_WitnessPointer w, TopologicalMinor_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, TopologicalMinor_WitnessPointer w, TopologicalMinor_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, TopologicalMinor_WitnessPointer w1, TopologicalMinor_WitnessPointer w2, TopologicalMinor_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(TopologicalMinor_WitnessPointer w);
public:
    MultiGraph multigraph;
    TopologicalMinor_DynamicCore();
    TopologicalMinor_DynamicCore(MultiGraph multiGraph);
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

