// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Witness.h"
Witness::~Witness() {

}

bool Witness::is_equal(const Witness &rhs) const {
    return true;
}

bool Witness::is_less(const Witness &rhs) const{
    return true;
}

Witness &Witness::set_equal(Witness &witness) {
    return *this;
}

bool operator==(const Witness &lhs,const Witness &rhs){
    return lhs.is_equal(rhs);
}

bool Witness::operator!=(Witness &rhs) {
    return !((*this)==rhs);
}

bool operator<(const Witness &lhs,const Witness &rhs){
    return lhs.is_less(rhs);
}

Witness& Witness::operator=(Witness &rhs) {
    return this->set_equal(rhs);
}

void Witness::print() {
    cout<<"It you are seeing this, it is because the print function was not yet implemented in the derived class."<<endl;
}









