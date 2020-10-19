// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef MAXDEGREE_GREATERTHAN_H
#define MAXDEGREE_GREATERTHAN_H
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <iostream>
#include <map>

using namespace std;

//Defines the type of a witness for the core MaxDegree_GreaterThanOrEqual
class MaxDegree_AtLeast_Witness:public Witness, public enable_shared_from_this<MaxDegree_AtLeast_Witness>{
    public:
        map<unsigned,unsigned> degreeCounter; // Counts the number neighbors seen so far for each vertex of the bag.
        bool found; // Set to true if and only if  a vertex of degree at least maxDeg has been found.
        MaxDegree_AtLeast_Witness(); // Constructor
        ~MaxDegree_AtLeast_Witness();
        virtual void print();
        virtual bool is_equal(const Witness &rhs) const; //Comparator
        virtual bool is_less(const Witness &rhs) const;  //Comparator
        virtual Witness& set_equal(Witness &witness); //Assignment operator
};

class MaxDegree_AtLeast_WitnessSet : public WitnessSetTypeOne<MaxDegree_AtLeast_WitnessSet>{

};

//Defines the dynamic core for MaxDegree_AtLeast
class MaxDegree_AtLeast_DynamicCore: public DynamicCore {
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
