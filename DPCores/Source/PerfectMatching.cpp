// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "PerfectMatching.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
    return new std::map(PerfectMatching_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
    return new PerfectMatching_Core(parameters);
}
}
