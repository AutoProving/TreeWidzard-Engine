// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "State.h"

Bag State::get_bag()const {
    return State::bag;
}

void State::set_bag(const Bag &bag) {
    State::bag = bag;
}

const std::vector<shared_ptr<WitnessSet>> State::get_witnessSetVector() const {
    return witnessSetVector;
}

void State::set_witnessSetVector(const std::vector<shared_ptr<WitnessSet>> &vector) {
    State::witnessSetVector = vector;
}

bool State::operator<(const State& rhs) const {

    if(this->get_bag() < rhs.get_bag()){
        return true;
    }else if(State::get_witnessSetVector().size() != rhs.get_witnessSetVector().size()){
        cout<< "ERROR: In State::operator< sizes are different! "<<endl;
        exit(20);
    }else if(this->get_bag() == rhs.get_bag()){
        for(unsigned j=0 ; j < get_witnessSetVector().size(); j++){
            shared_ptr<WitnessSet> temp1 = State::get_witnessSetVector()[j];
            shared_ptr<WitnessSet> temp2 = rhs.get_witnessSetVector()[j];
            if(*temp1 < *temp2){
                return true;
            }else if(*temp2 < *temp1){
                return false;
            }
        }
        return false;
    }else{
        return false;
    }

}

bool State::operator==(const State& rhs) const {
    if (this->get_bag() == rhs.get_bag()){
        if (this->get_witnessSetVector().size() != rhs.get_witnessSetVector().size()) {
            cout<< "ERROR: In State::operator== sizes are different! "<<endl;
            exit(20);
        } else{
            for(size_t j=0 ; j < State::get_witnessSetVector().size(); j++){
                if(!(*(rhs.get_witnessSetVector()[j]) == *(this->get_witnessSetVector()[j])) ){
                    return false;
                }
            }
            return true;
        }
    } else{
        return false;
    }
}

/// Implementation of this function should change regard to WitnessSet adn Witness
size_t State::operator()(const State &b) const {
    string s;
    set<unsigned> bagSet = b.get_bag().get_elements();
    for (set<unsigned>::iterator i = bagSet.begin(); i != bagSet.end() ; ++i) {
        s = s + (char)(*i);
    }
    s = s + 'a';
    if(b.get_bag().get_edge().first != 0 and b.get_bag().get_edge().second != 0 ){
        s = s + (char)b.get_bag().get_edge().first + (char)b.get_bag().get_edge().second  + 'a';
    }
    std::size_t str_hash = std::hash<std::string>{}(s);
    return str_hash;
}

void State::print(){
    bag.print();
    int i = 1;
    for (vector<shared_ptr<WitnessSet>>::iterator it = witnessSetVector.begin(); it != witnessSetVector.end() ; ++it) {
        cout<<"\nCore "<<i<< " WitnessSet \n";
        (*it)->print();
        i++;
    }
    cout<<"\n------------\n";
}
