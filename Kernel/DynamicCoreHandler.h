// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef DYNAMICCOREHANDLER_H
#define DYNAMICCOREHANDLER_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "../Multigraph/MultiGraph.h"
#include "DynamicCore.h"
#include "../DynamicLibraryLoader/dynamic_library_loader.hpp"

class DynamicCoreHandler {
	DynamicLibraryLoader<
		NamedFunctionSignature<"create",
							   DynamicCore *(const parameterType &parameters)>,
		NamedFunctionSignature<"metadata",
							   std::map<std::string, std::string> *()>>
		lib;
	std::unique_ptr<std::map<std::string, std::string>> metadata;

  public:
	DynamicCoreHandler(const std::string &so_path)
		: lib(so_path), metadata(lib.call<"metadata">()) {}

	const std::map<std::string, std::string> &get_metadata() const {
		return *metadata;
	}

	std::unique_ptr<DynamicCore> create(const parameterType &parameters) const {
		return std::unique_ptr<DynamicCore>(lib.call<"create">(parameters));
	}
};

#endif
