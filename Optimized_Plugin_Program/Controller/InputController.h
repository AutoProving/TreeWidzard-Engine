#ifndef TREEWIDZARD_INPUTCONTROLLER_H
#define TREEWIDZARD_INPUTCONTROLLER_H
#include <iostream>
#include <experimental/filesystem>
#include "../Kernel/DynamicCoreHandler.h"
#include "../Kernel/DynamicKernel.h"
#include "../Conjecture/Conjecture.h"
#include "../Parser/PropertyParser/input_parser.hpp"
#include "../Conjecture/PropertyAssignment.h"
#include <iomanip>

namespace fs = std::experimental::filesystem;
extern std::FILE *input_in;

class InputController {
private:
    std::string inputPath; // Path of the input file
    std::string dynamicPluginPath; // Path of the dynamic plugins that exist.
    DynamicKernel dynamicKernel;
    Width width;
    Conjecture conjecture;
    //These attributes are auxiliary and are used for parsing and constructing the input.
    std::map<std::string, std::map<std::string,std::string>> coreList; // Each entry is a pair of core name and core attributes.
    std::map<std::string, std::string> coreNamesToFiles; // map from name of a core to path of the core.
    std::map<std::string, std::string> varToCoreName; // map from variables to name of cores
    std::map<std::string, PropertyAssignment*> varToProperty; // map from var to Property
public:
    InputController(const std::string &inputPath, const std::string &dynamicPluginPath);

    InputController(const std::string &inputPath, const std::string &dynamicPluginPath, const Width &width);

    const std::string &getInputPath() const;
    DynamicKernel &getDynamicKernel();
    Conjecture &getConjecture();
    void check_available_cores();
    void parse_input();
    void construct_dynamicKernel();
};


#endif //TREEWIDZARD_INPUTCONTROLLER_H
