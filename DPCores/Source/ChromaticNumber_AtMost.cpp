// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "ChromaticNumber_AtMost.h"

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(ChromaticNumber_AtMost_Core::metadata());
}
DynamicCore *create(const parameterType &parameters) {
	return new ChromaticNumber_AtMost_Core(parameters);
}
}
