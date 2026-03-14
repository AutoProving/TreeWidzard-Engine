#include "FeedbackVertexSet_AtMost.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(FeedbackVertexSet_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new FeedbackVertexSet_Core(parameters);
}
}

