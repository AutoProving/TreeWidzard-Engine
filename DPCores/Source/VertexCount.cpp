#include "VertexCount.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(VertexCount_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
	return new VertexCount_Core(parameters);
}
}
