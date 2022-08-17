// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CHROMATICNUMBER_LESSTHAN_H
#define CHROMATICNUMBER_LESSTHAN_H
#include "../Kernel/WitnessSet.h"
#include <map>
#include "../Kernel/DynamicCore.h"
using namespace std;

class ChromaticNumber_Min_Witness;

typedef shared_ptr<ChromaticNumber_Min_Witness> ChromaticNumber_Min_WitnessPointer;
typedef shared_ptr<ChromaticNumber_Min_Witness const> ChromaticNumber_Min_WitnessPointerConst;
class ChromaticNumber_Min_Witness: public Witness, public enable_shared_from_this<ChromaticNumber_Min_Witness> {
    public:
        //std::map<unsigned,unsigned> coloring; //The first coordinate of each pair is a vertex and the second a color.
        set<set<unsigned>> partialColoring;
        unsigned numberUsedColors; //maximum number of cells in partial coloring
        ChromaticNumber_Min_Witness(){};
        ~ChromaticNumber_Min_Witness(){};
        bool is_equal_implementation(const ChromaticNumber_Min_WitnessPointerConst w) const;
        bool is_less_implementation(const ChromaticNumber_Min_WitnessPointerConst w) const;
        Witness& set_equal_implementation(ChromaticNumber_Min_WitnessPointer w);
        virtual bool is_equal(const Witness &rhs)const;
        virtual bool is_less(const Witness &lhs)const;
        virtual Witness& set_equal(Witness &rhs);
        virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);
        virtual void print();
        string witnessInformation() override;
};

class ChromaticNumber_Min_WitnessSet : public WitnessSetTypeOne<ChromaticNumber_Min_WitnessSet>{
    public:
        shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};
typedef shared_ptr<ChromaticNumber_Min_WitnessSet> ChromaticNumber_Min_WitnessSetPointer;

class ChromaticNumber_Min_DynamicCore: public DynamicCore{
    private:
        void createInitialWitnessSet_implementation();
        void copyWitness(ChromaticNumber_Min_WitnessPointer w_input,
                ChromaticNumber_Min_WitnessPointer w_output);
        ChromaticNumber_Min_WitnessPointer createWitness();
        void intro_v_implementation(unsigned i, Bag &b, ChromaticNumber_Min_WitnessPointer w,
                ChromaticNumber_Min_WitnessSetPointer witnessSet);
        void forget_v_implementation(unsigned i, Bag &b, ChromaticNumber_Min_WitnessPointer w,
                ChromaticNumber_Min_WitnessSetPointer witnessSet);
        void intro_e_implementation(unsigned i,unsigned j, Bag &b, ChromaticNumber_Min_WitnessPointer w,
                ChromaticNumber_Min_WitnessSetPointer witnessSet);
        void join_implementation(Bag &b, ChromaticNumber_Min_WitnessPointer w1,
                ChromaticNumber_Min_WitnessPointer w2, ChromaticNumber_Min_WitnessSetPointer witnessSet);
        bool is_final_witness_implementation(ChromaticNumber_Min_WitnessPointer w);
        int weight_implementation(ChromaticNumber_Min_WitnessPointer w);
        ChromaticNumber_Min_WitnessSetPointer clean_implementation(ChromaticNumber_Min_WitnessSetPointer witnessSet);
    public:
        ChromaticNumber_Min_DynamicCore();
        ~ChromaticNumber_Min_DynamicCore(){};
        void  createInitialWitnessSet();
        WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
        WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
        WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
        WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
        WitnessSetPointer clean(WitnessSetPointer witnessSet);
        bool is_final_witness(Witness &witness);
        int weight(Witness &witness);
};

#endif
