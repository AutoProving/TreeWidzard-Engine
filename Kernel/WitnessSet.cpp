// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "WitnessSet.h"

void WitnessSet::insert(shared_ptr<Witness> ws) {
	container.insert(ws);
}

//void WitnessSet::set_container(const set<shared_ptr<Witness>,compare> &witnessSet) {
//    WitnessSet::container = witnessSet;
//}

void WitnessSet::union_set_witness(shared_ptr<WitnessSet> witnessSet) {

    for (set<shared_ptr<Witness>,compare>::iterator it = witnessSet->container.begin(); it != witnessSet->container.end() ; ++it) {
                this->container.insert(*it);
    }
}

void WitnessSet::print() {

    for (set<shared_ptr<Witness>,compare>::iterator it = container.begin();  it!=container.end() ; it++) {
        (*it)->print();
        if( (! container.empty()) and it != --container.end()  ){
            cout<<endl;
        }
    }
}

bool WitnessSet::operator==(WitnessSet &rhs){
	int cnt = 0;
    if(size() != rhs.size()){
        return false;
    } else{
		WitnessSet::iterator it = rhs.begin();
		for (WitnessSet::iterator itr = begin(); itr != end(); itr++) {
            if(!(**itr == **it)){
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
		for (WitnessSet::iterator itr = begin(); itr != end(); itr++) {
            if(**it < **itr){
                return false;
            }else if(**itr < **it){
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
