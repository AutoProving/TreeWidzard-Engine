#include "PerfectMatching.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(PerfectMatching_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new PerfectMatching_Core(parameters);
}
}
