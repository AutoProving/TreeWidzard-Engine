#ifndef HASMULTIPLEEDGES_H
#define HASMULTIPLEEDGES_H

#include "../Kernel/WitnessSet.h"
#include "../Kernel/DynamicCore.h"
#include <algorithm>
class HasMultipleEdges_Witness;

typedef shared_ptr<HasMultipleEdges_Witness> HasMultipleEdges_WitnessPointer;
typedef shared_ptr<HasMultipleEdges_Witness const> HasMultipleEdges_WitnessPointerConst;
class HasMultipleEdges_Witness: public Witness, public enable_shared_from_this<HasMultipleEdges_Witness>{
public:
    std::set<pair<unsigned,unsigned>> edgeSet;
    bool found;
    HasMultipleEdges_Witness(){};
    ~HasMultipleEdges_Witness(){};
    virtual bool is_equal(const Witness &rhs) const;
    virtual bool is_less(const Witness &rhs) const;
    virtual Witness& set_equal(Witness &rhs);
    shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
    virtual void print();
    string witnessInformation() override;
};

class HasMultipleEdges_WitnessSet : public WitnessSetTypeOne<HasMultipleEdges_WitnessSet>{

};

class HasMultipleEdges_DynamicCore: public DynamicCore{
public:
    HasMultipleEdges_DynamicCore();
    ~HasMultipleEdges_DynamicCore(){};
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b,Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    virtual bool is_final_witness(Witness &witness);
};

#endif
