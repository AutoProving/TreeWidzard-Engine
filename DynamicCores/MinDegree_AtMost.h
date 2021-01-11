// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef MAXDEGREE_GREATERTHAN_H
#define MAXDEGREE_GREATERTHAN_H
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <iostream>
#include <map>

using namespace std;
class MinDegree_AtMost_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<MinDegree_AtMost_Witness> MinDegree_AtMost_WitnessPointer;
typedef shared_ptr<MinDegree_AtMost_Witness const > MinDegree_AtMost_WitnessPointerConst;
//Defines the type of a witness for the core MaxDegree_GreaterThanOrEqual
class MinDegree_AtMost_Witness:public Witness, public enable_shared_from_this<MinDegree_AtMost_Witness>{
    public:
        map<unsigned,unsigned> degreeCounter; // Counts the number neighbors seen so far for each vertex of the bag.
        bool found; // Set to true if and only if  a vertex of degree at least maxDeg has been found.
        bool is_equal_implementation(const MinDegree_AtMost_WitnessPointerConst w) const;
        bool is_less_implementation(const MinDegree_AtMost_WitnessPointerConst w) const;
        Witness& set_equal_implementation(MinDegree_AtMost_WitnessPointer w);
        MinDegree_AtMost_Witness(); // Constructor
        ~MinDegree_AtMost_Witness();
        virtual void print();
        string witnessInformation() override;
        virtual bool is_equal(const Witness &rhs) const; //Comparator
        virtual bool is_less(const Witness &rhs) const;  //Comparator
        virtual Witness& set_equal(Witness &witness); //Assignment operator
        virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);

};

class MinDegree_AtMost_WitnessSet : public WitnessSetTypeOne<MinDegree_AtMost_WitnessSet>{
public:
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

// Pointer type for Generic_Operator_WitnessSet.
typedef shared_ptr<MinDegree_AtMost_WitnessSet> MinDegree_AtMost_WitnessSetPointer ;

//Defines the dynamic core for MinDegree_AtMost
class MinDegree_AtMost_DynamicCore: public DynamicCore {
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(MinDegree_AtMost_WitnessPointer w_input, MinDegree_AtMost_WitnessPointer w_output);
    MinDegree_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, MinDegree_AtMost_WitnessPointer w, MinDegree_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, MinDegree_AtMost_WitnessPointer w, MinDegree_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, MinDegree_AtMost_WitnessPointer w, MinDegree_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, MinDegree_AtMost_WitnessPointer w1, MinDegree_AtMost_WitnessPointer w2, MinDegree_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(MinDegree_AtMost_WitnessPointer w);
    shared_ptr<WitnessSet> clean_implementation(MinDegree_AtMost_WitnessSetPointer witnessSet);
public:
    unsigned parameter;
    MinDegree_AtMost_DynamicCore();
    MinDegree_AtMost_DynamicCore(unsigned maxDegree);
    ~MinDegree_AtMost_DynamicCore(){};
    void createInitialWitnessSet();
    WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    bool is_final_witness(Witness &witness);
};

#endif
