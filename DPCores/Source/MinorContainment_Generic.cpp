// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "MinorContainment_Generic.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(MinorContainment_Generic_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new MinorContainment_Generic_Core(parameters);
}
}

