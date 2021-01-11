// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef MINOR_H
#define MINOR_H
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
    bool is_equal_implementation(Minor_WitnessPointerConst w) const;
    bool is_less_implementation(Minor_WitnessPointerConst w) const;
    Witness& set_equal_implementation(Minor_WitnessPointer w);
    ~Minor_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
    virtual void print();
    string witnessInformation() override;
    //*****************************
    //******BEGIN**ATTRIBUTES*******
    //*****************************
    bool found; // Shows that a minor has been found or not. If it is true it means a minor is found, otherwise, not.
    // Each coordinate of the next vector corresponds to a vertex of the pattern graph, and contains a tuple encoding 
    // a connected subgraph of the host graph.
    // The first coordinate corresponds to the vertex set of the component. The second coordinate, is a partition of this 
    // set, and is used to keep track of connectivity as the decomposition is read. The third component is a boolean flag
    // that is true if the component has been completely processed. 
    vector<tuple<set<unsigned >, set<set<unsigned >>,bool> > partitions;
    // Each coordinate of the next vector corresponds to an edge of the pattern graph, and contains a pair a pair reprenenting a The following vector has one coordinate for each edge of the graph. The 
    //vector<pair<unsigned,bool>> edgeFoundVector;
    set<unsigned int> foundEdges; // i belongs to the set if the edge labeled i has been found. 
    //*****************************
    //*******END**ATTRIBUTES*******
    //*****************************
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

class Minor_WitnessSet : public WitnessSetTypeOne<Minor_WitnessSet> {
    //************************
    //************************
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
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
    shared_ptr<WitnessSet> clean_implementation(Minor_WitnessSetPointer witnessSet);
    bool is_minor_found(Minor_WitnessPointer);

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

