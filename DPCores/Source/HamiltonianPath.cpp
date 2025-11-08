#include "HamiltonianPath.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(HP_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new HP_Core();
}
}

