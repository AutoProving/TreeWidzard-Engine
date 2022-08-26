// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "PropertyAssignment.h"

const std::string &PropertyAssignment::getName() const {
    return name;
}

void PropertyAssignment::setName(const std::string &name) {
    PropertyAssignment::name = name;
}

const std::string &PropertyAssignment::getOp() const {
    return op;
}

void PropertyAssignment::setOp(const std::string &op) {
    PropertyAssignment::op = op;
}


const std::vector<int> &PropertyAssignment::getParameters() const {
    return parameters;
}

void PropertyAssignment::setParameters(const std::vector<int> &parameters) {
    PropertyAssignment::parameters = parameters;
}

const std::string &PropertyAssignment::getParameterType() const {
    return parameterType;
}

void PropertyAssignment::setParameterType(const std::string &parameterType) {
    PropertyAssignment::parameterType = parameterType;
}

const std::string &PropertyAssignment::getType() const {
    return type;
}

void PropertyAssignment::setType(const std::string &type) {
    PropertyAssignment::type = type;
}

int PropertyAssignment::getParameter() const {
    return parameter;
}

void PropertyAssignment::setParameter(int parameter) {
    PropertyAssignment::parameter = parameter;
}
