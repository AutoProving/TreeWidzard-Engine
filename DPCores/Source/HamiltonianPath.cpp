#include "HamiltonianPath.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(HP_Core::metadata());
}
DynamicCore *create(const parameterType &) {
    return new HP_Core();
}
}
