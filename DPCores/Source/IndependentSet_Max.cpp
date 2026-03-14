#include "IndependentSet_Max.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(IndependentSet_Max_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
	return new IndependentSet_Max_Core(parameters);
}
}
