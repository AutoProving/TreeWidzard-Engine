// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "MaxDegree_AtLeast.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(MaxDegree_AtLeast_DynamicCore::metadata());
}
DynamicCore *create(const parameterType &parameters) {
	return new MaxDegree_AtLeast_DynamicCore(parameters);
}
}
