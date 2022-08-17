// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CHROMATICNUMBER_LESSTHAN_H
#define CHROMATICNUMBER_LESSTHAN_H
#include "../Kernel/WitnessSet.h"
#include <map>
#include "../Kernel/DynamicCore.h"
using namespace std;

class ChromaticNumber_AtMost_Witness;

typedef shared_ptr<ChromaticNumber_AtMost_Witness> ChromaticNumber_AtMost_WitnessPointer;
typedef shared_ptr<ChromaticNumber_AtMost_Witness const> ChromaticNumber_AtMost_WitnessPointerConst;
class ChromaticNumber_AtMost_Witness: public Witness, public enable_shared_from_this<ChromaticNumber_AtMost_Witness> {
public:
    //std::map<unsigned,unsigned> coloring; //The first coordinate of each pair is a vertex and the second a color.
    set<set<unsigned>> partialColoring;
    //unsigned numberUsedColors;
    ChromaticNumber_AtMost_Witness(){};
    ~ChromaticNumber_AtMost_Witness(){};
    bool is_equal_implementation(const ChromaticNumber_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const ChromaticNumber_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(ChromaticNumber_AtMost_WitnessPointer w);
    virtual bool is_equal(const Witness &rhs)const;
    virtual bool is_less(const Witness &lhs)const;
    virtual Witness& set_equal(Witness &rhs);
    virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);
    virtual void print();
    string witnessInformation() override;
};

class ChromaticNumber_AtMost_WitnessSet : public WitnessSetTypeOne<ChromaticNumber_AtMost_WitnessSet>{
public:
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};
typedef shared_ptr<ChromaticNumber_AtMost_WitnessSet> ChromaticNumber_AtMost_WitnessSetPointer;

class ChromaticNumber_AtMost_DynamicCore: public DynamicCore{
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(ChromaticNumber_AtMost_WitnessPointer w_input, 
            ChromaticNumber_AtMost_WitnessPointer w_output);
    ChromaticNumber_AtMost_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, ChromaticNumber_AtMost_WitnessPointer w,
            ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, ChromaticNumber_AtMost_WitnessPointer w,
            ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, ChromaticNumber_AtMost_WitnessPointer w,
            ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, ChromaticNumber_AtMost_WitnessPointer w1,
            ChromaticNumber_AtMost_WitnessPointer w2, ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(ChromaticNumber_AtMost_WitnessPointer w);
    ChromaticNumber_AtMost_WitnessSetPointer clean_implementation(ChromaticNumber_AtMost_WitnessSetPointer witnessSet);


public:
        unsigned k;
        ChromaticNumber_AtMost_DynamicCore();
        ChromaticNumber_AtMost_DynamicCore(unsigned k);
        ~ChromaticNumber_AtMost_DynamicCore(){};
        void  createInitialWitnessSet();
        WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
        WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
        WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
        WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
        WitnessSetPointer clean(WitnessSetPointer witnessSet);
        bool is_final_witness(Witness &witness);

};

#endif
