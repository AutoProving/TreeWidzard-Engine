// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef TREEWIDZARD_SEARCHCONTROLLER_H
#define TREEWIDZARD_SEARCHCONTROLLER_H
#include <iostream>
#include "../Kernel/Flags.h"
#include "../Kernel/SearchStrategyHandler.h"
#include "InputController.h"

namespace fs = std::experimental::filesystem;
using fs::current_path;

extern std::FILE *input_in;

class SearchController {
private:
    std::string searchStrategy;
    std::string searchPluginPath;
    Flags flags;
    std::map<std::string, std::map<std::string, std::string>> searchList; // Each entry is a pair of core name and core attributes.
    std::map<std::string, std::string> searchNamesToFiles;
    InputController* inputController;
    Width width;

public:
    SearchController(const std::string &searchStrategy, const std::string &searchPluginPath, const Flags &flags,
                      Width &width);
    void check_search();
    void action();

};


#endif //TREEWIDZARD_SEARCHCONTROLLER_H
