#ifndef TREEWIDZARD_Mod_H
#define TREEWIDZARD_Mod_H

#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <iostream>
using namespace std;
class Mod_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<Mod_Witness> Mod_WitnessPointer;
typedef shared_ptr<Mod_Witness const > Mod_WitnessPointerConst;
//Defines the type of a witness for the core MaxDegree_GreaterThanOrEqual
class Mod_Witness:public Witness, public enable_shared_from_this<Mod_Witness>{
public:
    int remainder; // Keep the remainder Mod of number of vertices which have been introduced  until the witness creation.
    bool is_equal_implementation(const Mod_WitnessPointerConst w) const;
    bool is_less_implementation(const Mod_WitnessPointerConst w) const;
    Witness& set_equal_implementation(Mod_WitnessPointer w);
    Mod_Witness(); // Constructor
    ~Mod_Witness();
    virtual void print();
    string witnessInformation() override; // Same as print, but instead of prinitng returns the string of information. 
    virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);
    //////////////////////////////////////////////////////////////////////////////
    virtual bool is_equal(const Witness &rhs) const; //Comparator
    virtual bool is_less(const Witness &rhs) const;  //Comparator
    virtual Witness& set_equal(Witness &witness); //Assignment operator


};

class Mod_WitnessSet : public WitnessSetTypeOne<Mod_WitnessSet>{
public:
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

// Pointer type for Generic_Operator_WitnessSet.
typedef shared_ptr<Mod_WitnessSet> Mod_WitnessSetPointer ;

//Defines the dynamic core for Mod
class Mod_DynamicCore: public DynamicCore {
    // The core accepts a decomposition if the number of vertices in the graph 
    // is equal to remainder Mod divsor
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(Mod_WitnessPointer w_input, Mod_WitnessPointer w_output);
    Mod_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, Mod_WitnessPointer w, Mod_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, Mod_WitnessPointer w, Mod_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, Mod_WitnessPointer w, Mod_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, Mod_WitnessPointer w1, Mod_WitnessPointer w2, Mod_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(Mod_WitnessPointer w);
    shared_ptr<WitnessSet> clean_implementation(Mod_WitnessSetPointer witnessSet);
public:
    int divisor;
    int remainder;
    Mod_DynamicCore();
    Mod_DynamicCore(vector<int> parameters);
    ~Mod_DynamicCore(){};
    void createInitialWitnessSet();
    WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    bool is_final_witness(Witness &witness);
};



#endif //TREEWIDZARD_Mod_H
