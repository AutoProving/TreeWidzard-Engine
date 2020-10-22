// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef EDGECOLORING_ATMOST_H
#define EDGECOLORING_ATMOST_H
#include "../Kernel/Bag.h"
#include "../Kernel/Witness.h"
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class EdgeColoring_AtMost_Witness;

typedef shared_ptr<EdgeColoring_AtMost_Witness> EdgeColoring_AtMost_WitnessPointer;
typedef shared_ptr<EdgeColoring_AtMost_Witness const> EdgeColoring_AtMost_WitnessPointerConst;
class EdgeColoring_AtMost_Witness: public Witness, public enable_shared_from_this<EdgeColoring_AtMost_Witness>{

public:
    bool is_equal_implementation(const EdgeColoring_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const EdgeColoring_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(EdgeColoring_AtMost_WitnessPointer w);
    ~EdgeColoring_AtMost_Witness();
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &rhs)const;
    virtual Witness& set_equal(Witness &witness);
    virtual void print();
    //*****************************
    //*****************************
    //Specific functions:
    void print_pair(pair<unsigned,unsigned> edge);// This method prints a pair with the format (pair.first, pair.second).
    unsigned next_available_edge_number(); // This method gets the next available edge number in the availableEdgeNumbers map of this witness so that
    // a new edge does not overwrite one that is already represented.

    //Attributes:
    map<unsigned,unsigned> availableEdgeNumbers; // Each edge is represented by a number. This map records the number of times each edge is used.
    // The first coordinate is the number representing the edge. The second coordinate can be 1 or 2.
    // <a,2> indicates that edge "a" is being used by two vertices in the bag. <a,1> indicates the edge "a"
    // is being used by two vertices. If "a" is used by no vertex in the bag then no pair is assigned to it.
    map<unsigned, map<unsigned,unsigned>> edgesPerVertex; // This map records the edges incident with each vertex together with their respective colors.
    // The first coordinate is the vertex number. The second is a map from edge numbers to colors.
    //*****************************
    //*****************************
};

class EdgeColoring_AtMost_WitnessSet: public WitnessSetTypeOne<EdgeColoring_AtMost_WitnessSet>{

};

typedef shared_ptr<EdgeColoring_AtMost_WitnessSet> EdgeColoring_AtMost_WitnessSetPointer ;

class EdgeColoring_AtMost_DynamicCore: public DynamicCore{
    void createInitialWitnessSet_implementation();
    void copyWitness(EdgeColoring_AtMost_WitnessPointer w_input, EdgeColoring_AtMost_WitnessPointer w_output);
    EdgeColoring_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, EdgeColoring_AtMost_WitnessPointer w, EdgeColoring_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, EdgeColoring_AtMost_WitnessPointer w, EdgeColoring_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, EdgeColoring_AtMost_WitnessPointer w, EdgeColoring_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, EdgeColoring_AtMost_WitnessPointer w1, EdgeColoring_AtMost_WitnessPointer w2, EdgeColoring_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(EdgeColoring_AtMost_WitnessPointer w);
public:
    unsigned parameter;
    EdgeColoring_AtMost_DynamicCore();
    EdgeColoring_AtMost_DynamicCore(unsigned parameter);
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b,Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
    virtual bool is_final_witness(Witness &witness);
    //*****************************
    //*****************************
    // Only change this part. Here you can define attributes and functions that are specific to your core.
    //*****************************
    //*****************************
};

#endif