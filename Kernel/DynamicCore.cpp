// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "DynamicCore.h"


shared_ptr<WitnessSet> DynamicCore::get_initialSet() {
    return this->initialWitnessSet;
}

shared_ptr<WitnessSet> DynamicCore::intro_v(unsigned i, Bag &b, WitnessSet &witnessSet)
{
    shared_ptr<WitnessSet> aux(new WitnessSet); // aux initializes with empty set
    for (set<shared_ptr<Witness>>::iterator it = witnessSet.container.begin();  it != witnessSet.container.end() ; ++it){
        shared_ptr<Witness> temp = *it;
        aux->union_set_witness(intro_v(i,b,*temp));
    }
    return clean(aux);
}

shared_ptr<WitnessSet> DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, WitnessSet &witnessSet) {
    shared_ptr<WitnessSet> aux(new WitnessSet); // aux initializes with empty set
    for (set<shared_ptr<Witness>>::iterator it = witnessSet.container.begin(); it != witnessSet.container.end(); ++it){
        shared_ptr<Witness> temp = *it;
        aux->union_set_witness(intro_e(i,j, b,*temp));
    }
    return clean(aux);
}

shared_ptr<WitnessSet> DynamicCore::forget_v(unsigned i, Bag &b, WitnessSet &witnessSet) {
    shared_ptr<WitnessSet> aux(new WitnessSet); // aux initializes with empty set
    for (set<shared_ptr<Witness>>::iterator it = witnessSet.container.begin(); it!=witnessSet.container.end(); ++it){
        shared_ptr<Witness> temp = *it;
        aux->union_set_witness(forget_v(i,b,*temp));
    }
    return clean(aux);
}

shared_ptr<WitnessSet> DynamicCore::join(Bag &b, WitnessSet &witnessSet1, WitnessSet &witnessSet2) {
    shared_ptr<WitnessSet> aux(new WitnessSet);
    for (set<shared_ptr<Witness>>::iterator it = witnessSet1.container.begin(); it!=witnessSet1.container.end(); ++it){
        shared_ptr<Witness> temp1 = *it;
        for (set<shared_ptr<Witness>>::iterator itr = witnessSet2.container.begin(); itr!=witnessSet2.container.end(); ++itr){
            shared_ptr<Witness> temp2 = *itr;
            aux->union_set_witness(join(b,*temp1, *temp2));
        }
    }
    return clean(aux);
}

bool DynamicCore::is_final_set_witness(Bag &b, WitnessSet &witnessSet) {
    bool flag = false;
    for(set<shared_ptr<Witness>>::iterator it = witnessSet.container.begin(); it!=witnessSet.container.end(); ++it){
        shared_ptr<Witness> temp = *it;
        if(is_final_witness(*temp)){
            return true;
        }
    }
    return flag;
}

shared_ptr<WitnessSet> DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    return witnessSet;
}

shared_ptr<WitnessSet> DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    cout<<"ERROR: DynamicCore::intro_v";
    exit(20);
}

shared_ptr<WitnessSet> DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    cout<<"ERROR: DynamicCore::intro_e";
    exit(20);}

shared_ptr<WitnessSet> DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    cout<<"ERROR: DynamicCore::forget_v";
    exit(20);}

shared_ptr<WitnessSet> DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    cout<<"ERROR: DynamicCore::join";
    exit(20);
}

bool DynamicCore::is_final_witness(Witness &witness) {
    cout<<"ERROR: DynamicCore::is_final_witness";
    exit(20);
}

shared_ptr<WitnessSet> DynamicCore::createInitialWitnessSet() {
    cout<<"ERROR: DynamicCore::createInitialWitnessSet";
    exit(20);
}


