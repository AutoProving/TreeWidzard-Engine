// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "MinorContainment_K2.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(MinorContainment_K2_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new MinorContainment_K2_Core(parameters);
}
}

