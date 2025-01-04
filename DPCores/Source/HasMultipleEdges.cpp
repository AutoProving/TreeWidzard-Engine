#include "HasMultipleEdges.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(HasMultipleEdges_DynamicCore::metadata());
}
DynamicCore *create(const parameterType &parameters) {
	return new HasMultipleEdges_DynamicCore(parameters);
}
}
