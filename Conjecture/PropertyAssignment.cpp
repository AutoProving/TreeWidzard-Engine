
#include "PropertyAssignment.h"

#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <utility>

namespace {

auto clone_parameter(
	const std::variant<char *, int, bool> &parameter)
	-> std::variant<char *, int, bool> {
	return std::visit(
		[](auto value) -> std::variant<char *, int, bool> {
			using ValueType = std::decay_t<decltype(value)>;
			if constexpr (std::is_same_v<ValueType, char *>) {
				return value != nullptr ? strdup(value) : nullptr;
			} else {
				return value;
			}
		},
		parameter);
}

void free_parameters(std::vector<std::variant<char *, int, bool>> &parameters) {
	for (auto &parameter : parameters) {
		if (auto *value = std::get_if<char *>(&parameter); value != nullptr &&
			*value != nullptr) {
			std::free(*value);
			*value = nullptr;
		}
	}
	parameters.clear();
}

} // namespace

PropertyAssignment::~PropertyAssignment() {
	free_parameters(parametersVec);
}

PropertyAssignment::PropertyAssignment(const PropertyAssignment &other)
	: name(other.name),
	  op(other.op),
	  parameter(other.parameter),
	  parameters(other.parameters),
	  parameterType(other.parameterType),
	  type(other.type) {
	setParametersVec(other.parametersVec);
}

PropertyAssignment &PropertyAssignment::operator=(const PropertyAssignment &other) {
	if (this == &other) {
		return *this;
	}

	name = other.name;
	op = other.op;
	parameter = other.parameter;
	parameters = other.parameters;
	parameterType = other.parameterType;
	type = other.type;
	setParametersVec(other.parametersVec);
	return *this;
}

PropertyAssignment::PropertyAssignment(PropertyAssignment &&other) noexcept
	: name(std::move(other.name)),
	  op(std::move(other.op)),
	  parameter(other.parameter),
	  parameters(std::move(other.parameters)),
	  parameterType(std::move(other.parameterType)),
	  type(std::move(other.type)),
	  parametersVec(std::move(other.parametersVec)) {
	other.parametersVec.clear();
}

PropertyAssignment &PropertyAssignment::operator=(PropertyAssignment &&other) noexcept {
	if (this == &other) {
		return *this;
	}

	free_parameters(parametersVec);
	name = std::move(other.name);
	op = std::move(other.op);
	parameter = other.parameter;
	parameters = std::move(other.parameters);
	parameterType = std::move(other.parameterType);
	type = std::move(other.type);
	parametersVec = std::move(other.parametersVec);
	other.parametersVec.clear();
	return *this;
}

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
    free_parameters(PropertyAssignment::parametersVec);
    PropertyAssignment::parametersVec.reserve(parametersVec.size());
    for (const auto &parameter : parametersVec) {
        PropertyAssignment::parametersVec.push_back(clone_parameter(parameter));
    }
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
