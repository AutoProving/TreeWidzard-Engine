#ifndef TREEWIDZARD_NVertices_AtMost_H
#define TREEWIDZARD_NVertices_AtMost_H

#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <iostream>
using namespace std;
class NVertices_AtMost_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<NVertices_AtMost_Witness> NVertices_AtMost_WitnessPointer;
typedef shared_ptr<NVertices_AtMost_Witness const > NVertices_AtMost_WitnessPointerConst;
//Defines the type of a witness for the core MaxDegree_GreaterThanOrEqual
class NVertices_AtMost_Witness:public Witness, public enable_shared_from_this<NVertices_AtMost_Witness>{
public:
    int nVertices; // Keep the remainder NVertices_AtMost of number of vertices which have been introduced  until the witness creation.
    bool is_equal_implementation(const NVertices_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const NVertices_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(NVertices_AtMost_WitnessPointer w);
    NVertices_AtMost_Witness(); // Constructor
    ~NVertices_AtMost_Witness();
    virtual void print();
    string witnessInformation() override; // Same as print, but instead of prinitng returns the string of information. 
    virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);
    //////////////////////////////////////////////////////////////////////////////
    virtual bool is_equal(const Witness &rhs) const; //Comparator
    virtual bool is_less(const Witness &rhs) const;  //Comparator
    virtual Witness& set_equal(Witness &witness); //Assignment operator


};

class NVertices_AtMost_WitnessSet : public WitnessSetTypeOne<NVertices_AtMost_WitnessSet>{
public:
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

// Pointer type for Generic_Operator_WitnessSet.
typedef shared_ptr<NVertices_AtMost_WitnessSet> NVertices_AtMost_WitnessSetPointer ;

//Defines the dynamic core for NVertices_AtMost
class NVertices_AtMost_DynamicCore: public DynamicCore {
    // The core accepts a decomposition if the number of vertices in the graph 
    // is equal to remainder NVertices_AtMost divsor
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(NVertices_AtMost_WitnessPointer w_input, NVertices_AtMost_WitnessPointer w_output);
    NVertices_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, NVertices_AtMost_WitnessPointer w, NVertices_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, NVertices_AtMost_WitnessPointer w, NVertices_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, NVertices_AtMost_WitnessPointer w, NVertices_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, NVertices_AtMost_WitnessPointer w1, NVertices_AtMost_WitnessPointer w2, NVertices_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(NVertices_AtMost_WitnessPointer w);
    shared_ptr<WitnessSet> clean_implementation(NVertices_AtMost_WitnessSetPointer witnessSet);
public:
    int nVertices;
    NVertices_AtMost_DynamicCore();
    NVertices_AtMost_DynamicCore(unsigned nVertices); 
    ~NVertices_AtMost_DynamicCore(){};
    void createInitialWitnessSet();
    WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    bool is_final_witness(Witness &witness);
};



#endif //TREEWIDZARD_NVertices_AtMost_H
