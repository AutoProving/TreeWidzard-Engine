#include "HamiltonianCycle.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(HC_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new HC_Core();
}
}

