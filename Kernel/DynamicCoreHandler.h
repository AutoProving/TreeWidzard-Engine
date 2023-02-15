#ifndef DYNAMICCOREHANDLER_H
#define DYNAMICCOREHANDLER_H

#include <dlfcn.h>
#include <iostream>
#include "../Multigraph/MultiGraph.h"
#include "DynamicCore.h"

class DynamicCoreHandler {
  private:
	void* handler = nullptr;
	DynamicCore_creator_t creator = nullptr;
	DynamicCore_creator_t_int creator_int = nullptr;
	DynamicCore_creator_t_multiGraph creator_multiGraph = nullptr;
	DynamicCore_creator_t_parameters creator_parameters = nullptr;
	static void Reset_dlerror() { dlerror(); }

	static void Check_dlerror() {
		const char* dlsym_error = dlerror();
		if (dlsym_error) {
			throw std::runtime_error(dlsym_error);
		}
	}

  public:
	DynamicCoreHandler(const char* MyClassLibraryName) {
		handler = dlopen(MyClassLibraryName, RTLD_NOW);
		if (!handler) {
			throw std::runtime_error(dlerror());
		}
		Reset_dlerror();
		creator =
			reinterpret_cast<DynamicCore_creator_t>(dlsym(handler, "create"));
		creator_int = reinterpret_cast<DynamicCore_creator_t_int>(
			dlsym(handler, "create_int"));
		creator_multiGraph = reinterpret_cast<DynamicCore_creator_t_multiGraph>(
			dlsym(handler, "create_multiGraph"));
		creator_parameters = reinterpret_cast<DynamicCore_creator_t_parameters>(
			dlsym(handler, "create_parameters"));
		// Check_dlerror();
	}

	std::unique_ptr<DynamicCore> create() const {
		if (!creator) throw std::runtime_error("Core is missing create.");
		return std::unique_ptr<DynamicCore>(creator());
	}
	std::unique_ptr<DynamicCore> create_int(unsigned param) const {
		if (!creator_int)
			throw std::runtime_error("Core is missing create_int.");
		return std::unique_ptr<DynamicCore>(creator_int(param));
	}

	std::unique_ptr<DynamicCore> create_multiGraph(
		MultiGraph multiGraph) const {
		if (!creator_multiGraph)
			throw std::runtime_error("Core is missing create_multiGraph.");
		return std::unique_ptr<DynamicCore>(creator_multiGraph(multiGraph));
	}

	std::unique_ptr<DynamicCore> create_parameters(
		const parameterType& parameters) const {
		if (!creator_parameters)
			throw std::runtime_error("Core is missing create_parameters.");
		return std::unique_ptr<DynamicCore>(creator_parameters(parameters));
	}

	~DynamicCoreHandler() {
		if (handler) {
			dlclose(handler);
		}
	}
};
#endif
