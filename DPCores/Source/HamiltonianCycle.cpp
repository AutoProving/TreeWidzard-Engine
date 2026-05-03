// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "HamiltonianCycle.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(HC_Core::metadata());
}
DynamicCore *create(const parameterType &) {
    return new HC_Core();
}
}
