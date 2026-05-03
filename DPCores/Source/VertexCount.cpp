// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "VertexCount.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(VertexCount_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
	return new VertexCount_Core(parameters);
}
}
