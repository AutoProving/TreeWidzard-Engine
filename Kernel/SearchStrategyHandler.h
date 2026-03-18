#ifndef SEARCHSTRATEGYHANDLER_H
#define SEARCHSTRATEGYHANDLER_H

#include <algorithm>
#include <iostream>
#include <memory>
#include "SearchStrategy.h"
#include "../DynamicLibraryLoader/dynamic_library_loader.hpp"

class SearchStrategyHandler {
  public:
	using Creator = SearchStrategy *(DynamicKernel *, Conjecture *, Flags *);

  private:
	DynamicLibraryLoader<
		NamedFunctionSignature<"create", Creator>,
		NamedFunctionSignature<"metadata",
							   std::map<std::string, std::string> *()>>
		lib;
	std::unique_ptr<std::map<std::string, std::string>> metadata;

  public:
	SearchStrategyHandler(const std::string &so_path)
		: lib(so_path), metadata(lib.call<"metadata">()) {}

	const std::map<std::string, std::string> &get_metadata() const {
		return *metadata;
	}

	std::unique_ptr<SearchStrategy> create(DynamicKernel *dynamicKernel,
										   Conjecture *conjecture,
										   Flags *flags) const {
		return std::unique_ptr<SearchStrategy>(
			lib.call<"create">(dynamicKernel, conjecture, flags));
	}
};

#endif
