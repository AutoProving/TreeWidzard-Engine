#ifndef DYNAMICCOREHANDLER_H
#define DYNAMICCOREHANDLER_H

#include "DynamicCore.h"
#include <dlfcn.h>
#include <iostream>

using namespace std;

class DynamicCoreHandler {
private:
    void * handler = nullptr;
    DynamicCore_creator_t creator = nullptr;
    DynamicCore_creator_t_int creator_int = nullptr;
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
        //Check_dlerror();
    }

    std::unique_ptr<DynamicCore> create() const {
        return std::unique_ptr<DynamicCore>(creator());
    }
    std::unique_ptr<DynamicCore> create_int(unsigned param) const {
        return std::unique_ptr<DynamicCore>(creator_int(param));
    }

    ~DynamicCoreHandler() {
        if (handler) {
            dlclose(handler);
        }
    }


};
#endif
