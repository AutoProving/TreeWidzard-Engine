// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_CORE_H
#define DYNAMIC_CORE_H

#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include "Bag.h"
#include "WitnessSet.h"
#include "Witness.h"

using namespace std;
class DynamicCore{
public:
    shared_ptr<WitnessSet> initialWitnessSet;
    virtual shared_ptr<WitnessSet> createInitialWitnessSet();
    virtual shared_ptr<WitnessSet> intro_v(unsigned i, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> intro_e(unsigned i, unsigned j, Bag &b , Witness &witness);
    virtual shared_ptr<WitnessSet> forget_v(unsigned i, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> join(Bag &b, Witness &witness1, Witness &witness2);
    virtual bool is_final_witness(Witness &witness);
    virtual shared_ptr<WitnessSet> clean(shared_ptr<WitnessSet> witnessSet);
    shared_ptr<WitnessSet> intro_v(unsigned i, Bag &b, WitnessSet &witnessSet);
    shared_ptr<WitnessSet> intro_e(unsigned i, unsigned j, Bag &b, WitnessSet &witnessSet);
    shared_ptr<WitnessSet> forget_v(unsigned i, Bag &b, WitnessSet &witnessSet);
    shared_ptr<WitnessSet> join(Bag &b, WitnessSet &witnessSet1, WitnessSet &witnessSet2);
    shared_ptr<WitnessSet> get_initialSet(); // returns initialSet
    bool is_final_set_witness(Bag &b,WitnessSet &witnessSet);
};

#endif
