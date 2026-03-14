#include "VertexCover_AtMost.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(VertexCover_AtMost_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new VertexCover_AtMost_Core(parameters);
}
}

