// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "WitnessSet.h"

const set<shared_ptr<Witness>,compare> &WitnessSet::get_container() const {
    return container;
}

void WitnessSet::set_container(const set<shared_ptr<Witness>,compare> &witnessSet) {
    WitnessSet::container = witnessSet;
}

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
    if(this->container.size()!=rhs.container.size()){
        return false;
    } else{
        set<shared_ptr<Witness>>::iterator it = rhs.container.begin();
        for(set<shared_ptr<Witness>>::iterator itr = this->container.begin(); itr != this->container.end(); itr++){
            if(!(**itr == **it)){
                return false;
            }
            it++;
        }
    }
    return true;
}

bool WitnessSet::operator<(WitnessSet &rhs) {
    if(this->container.size() < rhs.container.size()){
        return true;
    }else if( rhs.container.size() < this->container.size() ){
        return false;
    } else{
        set<shared_ptr<Witness>>::iterator it = rhs.container.begin();
        for(set<shared_ptr<Witness>>::iterator itr = this->container.begin(); itr != this->container.end(); itr++){
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


