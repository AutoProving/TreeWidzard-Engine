// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "DynamicCore.h"


WitnessSetPointer DynamicCore::getInitialSet() {
    return initialWitnessSet;
}

void DynamicCore::insertIntoInitialWitnessSet(WitnessPointer w) {
	initialWitnessSet->insert(w);
}

WitnessSetPointer DynamicCore::intro_v(unsigned i, Bag &b, WitnessSet &witnessSet)
{
    WitnessSetPointer aux(new WitnessSet); // aux initializes with empty set
	for (WitnessPointer temp : witnessSet) {
        aux->union_set_witness(intro_v(i,b,*temp));
    }
    return clean(aux);
}

WitnessSetPointer DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, WitnessSet &witnessSet) {
    WitnessSetPointer aux(new WitnessSet); // aux initializes with empty set
	for (WitnessPointer temp : witnessSet) {
        aux->union_set_witness(intro_e(i,j, b,*temp));
    }
    return clean(aux);
}

WitnessSetPointer DynamicCore::forget_v(unsigned i, Bag &b, WitnessSet &witnessSet) {
    WitnessSetPointer aux(new WitnessSet); // aux initializes with empty set
	for (WitnessPointer temp : witnessSet) {
        aux->union_set_witness(forget_v(i,b,*temp));
    }
    return clean(aux);
}

WitnessSetPointer DynamicCore::join(Bag &b, WitnessSet &witnessSet1, WitnessSet &witnessSet2) {
    WitnessSetPointer aux(new WitnessSet);
	for (WitnessPointer temp1 : witnessSet1) {
		for (WitnessPointer temp2 : witnessSet2) {
            aux->union_set_witness(join(b,*temp1, *temp2));
        }
    }
    return clean(aux);
}

bool DynamicCore::is_final_set_witness(Bag &b, WitnessSet &witnessSet) {
    bool flag = false;
	for (WitnessPointer temp : witnessSet) {
        if(is_final_witness(*temp)){
            return true;
        }
    }
    return flag;
}

WitnessSetPointer DynamicCore::clean(WitnessSetPointer witnessSet) {
    return witnessSet;
}

WitnessSetPointer DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    cout<<"ERROR: DynamicCore::intro_v";
    exit(20);
}

WitnessSetPointer DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    cout<<"ERROR: DynamicCore::intro_e";
    exit(20);}

WitnessSetPointer DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    cout<<"ERROR: DynamicCore::forget_v";
    exit(20);}

WitnessSetPointer DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    cout<<"ERROR: DynamicCore::join";
    exit(20);
}

bool DynamicCore::is_final_witness(Witness &witness) {
    cout<<"ERROR: DynamicCore::is_final_witness";
    exit(20);
}

void DynamicCore::createInitialWitnessSet() {
    cout<<"ERROR: DynamicCore::createInitialWitnessSet";
    exit(20);
}


