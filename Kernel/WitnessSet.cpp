// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "WitnessSet.h"

void WitnessSet::insert(shared_ptr<Witness> ws) {
	container.insert(ws);
}

//void WitnessSet::set_container(const set<shared_ptr<Witness>,compare> &witnessSet) {
//    WitnessSet::container = witnessSet;
//}

void WitnessSet::union_set_witness(shared_ptr<WitnessSet> witnessSet) {

	for (auto element : *witnessSet)
		container.insert(element);
}

void WitnessSet::print() {

	for (auto element : *this)
        element->print();
}

bool WitnessSet::operator==(WitnessSet &rhs){
	int cnt = 0;
    if(size() != rhs.size()){
        return false;
    } else{
		WitnessSet::iterator it = rhs.begin();
		for (auto element : *this) {
            if(!(*element == **it)){
                return false;
            }
            it++;
			cnt++;
        }
    }
    return true;
}

bool WitnessSet::operator<(WitnessSet &rhs) {
    if(size() < rhs.size()){
        return true;
    }else if( rhs.size() < size() ){
        return false;
    } else{
		WitnessSet::iterator it = rhs.begin();
		for (auto element : *this) {
            if(**it < *element){
                return false;
            }else if(*element < **it){
                return true;
            }
            it++;
        }
        return false;
    }
}

bool WitnessSet::operator!=(WitnessSet &rhs) {
    return !( *this == rhs);
}

int WitnessSet::size() {
	return container.size();
}
