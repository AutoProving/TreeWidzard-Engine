// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef MAXDEGREE_GREATERTHAN_H
#define MAXDEGREE_GREATERTHAN_H
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <iostream>
#include <map>

using namespace std;
class MaxDegree_AtLeast_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<MaxDegree_AtLeast_Witness> MaxDegree_AtLeast_WitnessPointer;
typedef shared_ptr<MaxDegree_AtLeast_Witness const > MaxDegree_AtLeast_WitnessPointerConst;
//Defines the type of a witness for the core MaxDegree_GreaterThanOrEqual
class MaxDegree_AtLeast_Witness:public Witness, public enable_shared_from_this<MaxDegree_AtLeast_Witness>{
    public:
        map<unsigned,unsigned> degreeCounter; // Counts the number neighbors seen so far for each vertex of the bag.
        bool found; // Set to true if and only if  a vertex of degree at least maxDeg has been found.
        bool is_equal_implementation(const MaxDegree_AtLeast_WitnessPointerConst w) const;
        bool is_less_implementation(const MaxDegree_AtLeast_WitnessPointerConst w) const;
        Witness& set_equal_implementation(MaxDegree_AtLeast_WitnessPointer w);
        MaxDegree_AtLeast_Witness(); // Constructor
        ~MaxDegree_AtLeast_Witness();
        virtual void print();
        string witnessInformation() override;
        virtual bool is_equal(const Witness &rhs) const; //Comparator
        virtual bool is_less(const Witness &rhs) const;  //Comparator
        virtual Witness& set_equal(Witness &witness); //Assignment operator
        virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);

};

class MaxDegree_AtLeast_WitnessSet : public WitnessSetTypeOne<MaxDegree_AtLeast_WitnessSet>{
public:
   // shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

// Pointer type for Generic_Operator_WitnessSet.
typedef shared_ptr<MaxDegree_AtLeast_WitnessSet> MaxDegree_AtLeast_WitnessSetPointer ;

//Defines the dynamic core for MaxDegree_AtLeast
class MaxDegree_AtLeast_DynamicCore: public DynamicCore {
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(MaxDegree_AtLeast_WitnessPointer w_input, MaxDegree_AtLeast_WitnessPointer w_output);
    MaxDegree_AtLeast_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, MaxDegree_AtLeast_WitnessPointer w, MaxDegree_AtLeast_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, MaxDegree_AtLeast_WitnessPointer w, MaxDegree_AtLeast_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, MaxDegree_AtLeast_WitnessPointer w, MaxDegree_AtLeast_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, MaxDegree_AtLeast_WitnessPointer w1, MaxDegree_AtLeast_WitnessPointer w2, MaxDegree_AtLeast_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(MaxDegree_AtLeast_WitnessPointer w);
    shared_ptr<WitnessSet> clean_implementation(MaxDegree_AtLeast_WitnessSetPointer witnessSet);
public:
    unsigned maxDegree;
    MaxDegree_AtLeast_DynamicCore();
    MaxDegree_AtLeast_DynamicCore(unsigned maxDegree);
    ~MaxDegree_AtLeast_DynamicCore(){};
    void createInitialWitnessSet();
    WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    bool is_final_witness(Witness &witness);
};

#endif
