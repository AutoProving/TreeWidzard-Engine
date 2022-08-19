#ifndef TREEWIDZARD_INPUTCONTROLLER_H
#define TREEWIDZARD_INPUTCONTROLLER_H
#include <iostream>
#include <experimental/filesystem>
#include "../Kernel/DynamicCoreHandler.h"
#include "../Kernel/DynamicKernel.h"
#include "../Kernel/Conjecture.h"
#include "../Parser/input_parser.hpp"

using namespace std;
namespace fs = std::experimental::filesystem;
extern FILE *input_in;

class InputController {
private:
    string inputPath; // Path of the input file
    string dynamicPluginPath ; // Path of the dynamic plugins that exist.
    DynamicKernel dynamicKernel;
    Width width;
    Conjecture conjecture;
    // These attributes are auxiliary and are used for parsing and constructing the input.
    vector<PropertyAssignment> propertyList; // List of the properties
    map<string, map<string,string> > coreList; // Each entry is a pair of core name and core attributes.
    map<string, string> coreNamesToFiles; // Map from name of a core to path of the core.
public:
    InputController(const string &inputPath, const string &dynamicPluginPath);
    const string &getInputPath() const;
    DynamicKernel &getDynamicKernel();
    Conjecture &getConjecture();
    void check_available_cores();
    void parse_input();
    void construct_dynamicKernel();
};


#endif //TREEWIDZARD_INPUTCONTROLLER_H
