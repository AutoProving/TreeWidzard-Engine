// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "PropertyAssignment.h"

void PropertyAssignment::print() {
	std::cout << name << " " << operatorSign << " " << value << std::endl;
}

const std::string &PropertyAssignment::get_name() const { return name; }

void PropertyAssignment::set_name(const std::string &name) {
	PropertyAssignment::name = name;
}

const std::string &PropertyAssignment::get_operatorSign() const {
	return operatorSign;
}

void PropertyAssignment::set_operatorSign(const std::string &op) {
	operatorSign = op;
}

int PropertyAssignment::get_value() const { return value; }

void PropertyAssignment::set_value(int v) { value = v; }

const std::string &PropertyAssignment::get_type() const { return type; }

void PropertyAssignment::set_type(const std::string &type) {
	PropertyAssignment::type = type;
}

const std::string &PropertyAssignment::get_filePath() const { return filePath; }

void PropertyAssignment::set_filePath(const std::string &filePath) {
	PropertyAssignment::filePath = filePath;
}

const std::string &PropertyAssignment::get_label() const { return label; }

void PropertyAssignment::set_label(const std::string &label) {
	PropertyAssignment::label = label;
}

std::vector<int> &PropertyAssignment::get_parameters()  {
    return parameters;
}

void PropertyAssignment::set_parameters(const std::vector<int> &parameters) {
    PropertyAssignment::parameters = parameters;
}
