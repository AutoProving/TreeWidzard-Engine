#include "CliqueNumberSimpleGraphs_AtLeast.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(CliqueNumberSimpleGraphs_AtLeast_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
	return new CliqueNumberSimpleGraphs_AtLeast_Core(parameters);
}
}
