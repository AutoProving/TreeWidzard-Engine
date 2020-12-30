// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CHROMATICNUMBER_LESSTHAN_H
#define CHROMATICNUMBER_LESSTHAN_H
#include "../Kernel/WitnessSet.h"
#include <map>
#include "../Kernel/DynamicCore.h"
using namespace std;

class ChromaticNumber_AtMost_Witness: public Witness, public enable_shared_from_this<ChromaticNumber_AtMost_Witness> {
    public:
        std::map<unsigned,unsigned> coloring; //The first coordinate of each pair is a vertex and the second a color. 
        virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap); 
        ChromaticNumber_AtMost_Witness(){};
        ~ChromaticNumber_AtMost_Witness(){};
        virtual bool is_equal(const Witness &rhs)const;
        virtual bool is_less(const Witness &lhs)const;
        virtual Witness& set_equal(Witness &rhs);
        virtual void print();
};

class ChromaticNumber_AtMost_WitnessSet : public WitnessSetTypeOne<ChromaticNumber_AtMost_WitnessSet>{

};
class ChromaticNumber_AtMost_DynamicCore: public DynamicCore{
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
