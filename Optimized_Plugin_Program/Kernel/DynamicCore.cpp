// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "DynamicCore.h"
#include "../DynamicCores/EdgeConnected_AtMost.h"
WitnessSetPointer DynamicCore::getInitialSet() {
    return initialWitnessSet;
}

void DynamicCore::insertIntoInitialWitnessSet(WitnessPointer w) {
	initialWitnessSet->insert(w);
}

WitnessSetPointer DynamicCore::intro_v(unsigned i, Bag &b,WitnessSetPointer witnessSet) {
	WitnessSetPointer aux = witnessSet->createEmptyWitnessSet(); // aux initializes with empty set
	for (auto temp : *witnessSet) {
        aux->union_set_witness(intro_v(i, b, *temp));
    }
	return clean(aux);
}

WitnessSetPointer DynamicCore::intro_e(unsigned i, unsigned j, Bag &b,WitnessSetPointer witnessSet) {
	WitnessSetPointer aux = witnessSet->createEmptyWitnessSet(); // aux initializes with empty set
	WitnessSet t = *aux;
	for (auto temp : *witnessSet) {
		aux->union_set_witness(intro_e(i, j, b, *temp));
	}

	return clean(aux);
}

WitnessSetPointer DynamicCore::forget_v(unsigned i, Bag &b,WitnessSetPointer witnessSet) {
	WitnessSetPointer aux = witnessSet->createEmptyWitnessSet(); // aux initializes with empty set
	for (auto temp : *witnessSet) {
		aux->union_set_witness(forget_v(i, b, *temp));
	}
    return clean(aux);
}

WitnessSetPointer DynamicCore::join(Bag &b, WitnessSetPointer witnessSet1,WitnessSetPointer witnessSet2) {
	WitnessSetPointer aux = witnessSet1->createEmptyWitnessSet();
	for (auto temp1 : *witnessSet1) {
		for (auto temp2 : *witnessSet2) {
			aux->union_set_witness(join(b, *temp1, *temp2));
		}
	}
    return clean(aux);
}

bool DynamicCore::is_final_set_witness(Bag &b, WitnessSetPointer witnessSet) {
	bool flag = false;
	for (auto temp : *witnessSet) {
		if (is_final_witness(*temp)) {
			return true;
		}
	}
	return flag;
}


WitnessSetPointer DynamicCore::clean(WitnessSetPointer witnessSet) {
	// By default, if this function is not implemented by a derived class
	// this function simply returns a pointer to the input witness set.
	cout<< "generic clean is called"<<endl;
	return witnessSet;
}

int DynamicCore::weight(WitnessSetPointer witnessSet) {
	set<int> values;
	for(auto temp : *witnessSet){
        values.insert(weight(*temp) );
	}
    string coreType = getAttributeValue("CoreType");
    if(coreType=="NULL" or  coreType==""){
        cout<<"Error: DynamicCore::weight CoreType has not specified"<<endl;
        exit(20);
    }else if(coreType=="Min" or coreType=="Max"){
        if(values.size()==0){
            return 0;
        }else{
            if(coreType=="Min"){
                return *values.begin();
            }else{
                return *values.rbegin();
            }
        }
    }
}

WitnessSetPointer DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
	cout << "ERROR: DynamicCore::intro_v";
	exit(20);
}

WitnessSetPointer DynamicCore::intro_e(unsigned i, unsigned j, Bag &b,
									   Witness &witness) {
	cout << "ERROR: DynamicCore::intro_e";
	exit(20);
}

WitnessSetPointer DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
	cout << "ERROR: DynamicCore::forget_v";
	exit(20);
}

WitnessSetPointer DynamicCore::join(Bag &b, Witness &witness1,
									Witness &witness2) {
	cout << "ERROR: DynamicCore::join";
	exit(20);
}

bool DynamicCore::is_final_witness(Witness &witness) {
	cout << "ERROR: DynamicCore::is_final_witness";
	exit(20);
}

int DynamicCore::weight(Witness &witness) {
	cout << "ERROR: DynamicCore::weight";
	exit(20);
}

void DynamicCore::createInitialWitnessSet() {
	cout << "ERROR: DynamicCore::createInitialWitnessSet";
	exit(20);
}

void DynamicCore::addAttribute(string x, string y) {
    attributes.insert(make_pair(x,y));
}

bool DynamicCore::isAttribute(string x, string y) {
    auto it = attributes.find(x);
    if(it!=attributes.end()){
        if(it->second==y){
            return true;
        }
    }
    return false;
}

string DynamicCore::getAttributeValue(string x) {
    auto it = attributes.find(x);
    if(it!=attributes.end()){
        return it->second;
    }else{
        return "NULL";
    }

}

map<string, string> DynamicCore::getAttributes() {
    return attributes;
}

void DynamicCore::setInitialWitnessSet(WitnessSetPointer witnessSetPointer) {
    this->initialWitnessSet = witnessSetPointer;
}

int DynamicCore::getWidth(){

    return this->width;
}

void DynamicCore::setWidth(int width){
    this->width = width;
}
