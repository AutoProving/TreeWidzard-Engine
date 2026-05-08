// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "HamiltonianPath.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(HP_Core::metadata());
}
DynamicCore *create(const parameterType &) {
    return new HP_Core();
}
}
