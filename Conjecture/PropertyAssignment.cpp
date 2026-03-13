
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

const std::vector<std::variant<char *, int, bool>> &PropertyAssignment::getParametersVec() const {
    return parametersVec;
}

void PropertyAssignment::setParametersVec(const std::vector<std::variant<char *, int, bool>> &parametersVec) {
    PropertyAssignment::parametersVec = parametersVec;
}


// helper for overloaded lambdas
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void PropertyAssignment::printParameters() {
    for (const auto& param : this->parametersVec) {
        std::visit(overloaded{
            [](char* s)   { std::cout << '"' << s << '"'; },
            [](int   i)   { std::cout << i; },
            [](bool  b)   { std::cout << std::boolalpha << b; }
        }, param);
        if (param != this->parametersVec.back())
            std::cout << ", ";    // space between elements
    }
}
