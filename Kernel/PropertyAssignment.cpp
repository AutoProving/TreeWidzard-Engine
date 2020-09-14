// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "PropertyAssignment.h"

void PropertyAssignment::print() {

    cout<<name << " "<< operatorSign << " " << value<<endl;
}

const string &PropertyAssignment::get_name() const {
    return name;
}

void PropertyAssignment::set_name(const string &name) {
    PropertyAssignment::name = name;
}

const string &PropertyAssignment::get_operatorSign() const {
    return operatorSign;
}

void PropertyAssignment::set_operatorSign(const string &op) {

    operatorSign = op;
}

int PropertyAssignment::get_value() const {
    return value;
}

void PropertyAssignment::set_value(int v) {
    value = v;
}

const string &PropertyAssignment::get_type() const {
    return type;
}

void PropertyAssignment::set_type(const string &type) {
    PropertyAssignment::type = type;
}

const string &PropertyAssignment::get_filePath() const {
    return filePath;
}

void PropertyAssignment::set_filePath(const string &filePath) {
    PropertyAssignment::filePath = filePath;
}

const string &PropertyAssignment::get_label() const {
    return label;
}

void PropertyAssignment::set_label(const string &label) {
    PropertyAssignment::label = label;
}
