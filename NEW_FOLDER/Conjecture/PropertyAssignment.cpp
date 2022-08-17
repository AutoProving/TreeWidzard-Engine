// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "PropertyAssignment.h"

const string &PropertyAssignment::getName() const {
    return name;
}

void PropertyAssignment::setName(const string &name) {
    PropertyAssignment::name = name;
}

const string &PropertyAssignment::getOp() const {
    return op;
}

void PropertyAssignment::setOp(const string &op) {
    PropertyAssignment::op = op;
}


const vector<int> &PropertyAssignment::getParameters() const {
    return parameters;
}

void PropertyAssignment::setParameters(const vector<int> &parameters) {
    PropertyAssignment::parameters = parameters;
}

const string &PropertyAssignment::getParameterType() const {
    return parameterType;
}

void PropertyAssignment::setParameterType(const string &parameterType) {
    PropertyAssignment::parameterType = parameterType;
}

const string &PropertyAssignment::getType() const {
    return type;
}

void PropertyAssignment::setType(const string &type) {
    PropertyAssignment::type = type;
}

int PropertyAssignment::getParameter() const {
    return parameter;
}

void PropertyAssignment::setParameter(int parameter) {
    PropertyAssignment::parameter = parameter;
}
