#ifndef SEARCHSTRATEGYHANDLER_H
#define SEARCHSTRATEGYHANDLER_H

#include <dlfcn.h>
#include <iostream>
#include "SearchStrategy.h"

class SearchStrategyHandler {
private:
    void * handler = nullptr;
    SearchStrategy_creator_t creator = nullptr;
    SearchStrategy_creator_t_parameter creator_parameter = nullptr;

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
    SearchStrategyHandler(const char* MyClassLibraryName) {
        handler = dlopen(MyClassLibraryName, RTLD_NOW);
        if (!handler) {
            throw std::runtime_error(dlerror());
        }
        Reset_dlerror();
        creator = reinterpret_cast<SearchStrategy_creator_t>(dlsym(handler, "create"));
        creator_parameter = reinterpret_cast<SearchStrategy_creator_t_parameter>(dlsym(handler, "create_parameter"));
        //Check_dlerror();
    }

    std::unique_ptr<SearchStrategy> create() const {
        return std::unique_ptr<SearchStrategy>(creator());
    }
    std::unique_ptr<SearchStrategy> create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) const {
        return std::unique_ptr<SearchStrategy>(creator_parameter(dynamicKernel,conjecture,flags));
    }

    ~SearchStrategyHandler() {
        if (handler) {
            dlclose(handler);
        }
    }
};


#endif
