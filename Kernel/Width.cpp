// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Width.h"

string Width::get_name() {
    return name;
}

void Width::set_name(string s) {

    name = s;
}

unsigned int Width::get_value() const {
    return value;
}

void Width::set_value(int value) {
    Width::value = value;
}

void Width::print() {
    cout<<name<<" " <<value<<endl;
}

Width &Width::operator=(Width &rhs) {
    name = rhs.get_name();
    value = rhs.get_value();
    return *this;
}
