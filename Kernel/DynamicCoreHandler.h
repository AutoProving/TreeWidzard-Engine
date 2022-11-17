#ifndef DYNAMICCOREHANDLER_H
#define DYNAMICCOREHANDLER_H

#include "DynamicCore.h"
#include <dlfcn.h>
#include <iostream>
#include "../Multigraph/MultiGraph.h"

class DynamicCoreHandler {
private:
    void * handler = nullptr;
    DynamicCore_creator_t creator = nullptr;
    DynamicCore_creator_t_int creator_int = nullptr;
    DynamicCore_creator_t_multiGraph creator_multiGraph = nullptr;
    DynamicCore_creator_t_parameters creator_parameters = nullptr;
    static void Reset_dlerror() {
        dlerror();
    }

    static void Check_dlerror() {
        const char * dlsym_error = dlerror();
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
        creator = reinterpret_cast<DynamicCore_creator_t>(dlsym(handler, "create"));
        creator_int = reinterpret_cast<DynamicCore_creator_t_int>(dlsym(handler, "create_int"));
        creator_multiGraph = reinterpret_cast<DynamicCore_creator_t_multiGraph>(dlsym(handler, "create_multiGraph"));
        creator_parameters = reinterpret_cast<DynamicCore_creator_t_parameters>(dlsym(handler, "create_parameters"));
        //Check_dlerror();
    }

    std::unique_ptr<DynamicCore> create() const {
        return std::unique_ptr<DynamicCore>(creator());
    }

    std::unique_ptr<DynamicCore> create_int(unsigned param) const {
        return std::unique_ptr<DynamicCore>(creator_int(param));
    }

    std::unique_ptr<DynamicCore> create_multiGraph(MultiGraph multiGraph) const {
        return std::unique_ptr<DynamicCore>(creator_multiGraph(multiGraph));
    }

    std::unique_ptr<DynamicCore> create_parameters(const std::vector<char*> &parameters) const {
        return std::unique_ptr<DynamicCore>(creator_parameters(parameters));
    }

    ~DynamicCoreHandler() {
        if (handler) {
            dlclose(handler);
        }
    }


};
#endif
