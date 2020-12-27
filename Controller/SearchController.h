// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef TREEWIDZARD_SEARCHCONTROLLER_H
#define TREEWIDZARD_SEARCHCONTROLLER_H
#include <iostream>
#include "../Kernel/Flags.h"
#include "../Kernel/SearchStrategyHandler.h"
#include "InputController.h"

using namespace std;
namespace fs = std::experimental::filesystem;
extern FILE *input_in;

class SearchController {
private:
    string searchStrategy;
    string searchPluginPath;
    Flags flags;
    map<string, map<string,string> > searchList; // Each entry is a pair of core name and core attributes.
    map<string, string> searchNamesToFiles;
    InputController* inputController;

public:
    SearchController(const string &inputPath, const string &searchStrategy, const Flags &flags);
    ///////////////////////////////////////////////////////////////////////////
    void check_search();
    void action();

};


#endif //TREEWIDZARD_SEARCHCONTROLLER_H
