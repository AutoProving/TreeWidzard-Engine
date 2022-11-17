#include "InputController.h"
void InputController::check_available_cores() {
  for (const auto &entry : fs::directory_iterator(dynamicPluginPath)) {
    std::string s = entry.path();
    if (s.find(".so") != std::string::npos) {
      std::string fileName = entry.path().filename();
      std::cout << "File " << fileName;
      try {
        char *MyClassLibraryName = const_cast<char *>(s.c_str());
        DynamicCoreHandler factory(MyClassLibraryName);
        std::unique_ptr<DynamicCore> core = factory.create();
        std::map<std::string, std::string> attributes = core->getAttributes();
        coreNamesToFiles.insert(
            std::make_pair(core->getAttributeValue("CoreName"), fileName));
        coreList.insert(
            std::make_pair(core->getAttributeValue("CoreName"), attributes));
        std::cout << " Loaded. Core Name: "
                  << core->getAttributeValue("CoreName") << '\n';
      } catch (std::exception &e) {
        std::cerr << " failed to load." << e.what() << std::endl;
      }
    }
  }
}

void InputController::parse_input() {
  input_in = fopen(inputPath.c_str(), "r");
  if (!input_in) {
    std::perror("Input File opening failed, given path: ");
    std::cout << inputPath << std::endl;
    exit(20);
  }
  int result = 1;  // if parsing successful result will be 0 otherwise 1
  result = input_parse(conjecture, result, coreList, varToCoreName,varToProperty);  // Parser function from Parser.hpp
  // check for successful parsing
  if (result != 0) {
    std::cout << " Error: input file " << inputPath << " is not in valid format"
              << std::endl;
    exit(20);
  }
  std::cout << "CONJECTURE: ";
  conjecture.print();
  std::cout << std::endl;
  conjecture.setVariablesToCoreName(varToCoreName);
  if (!conjecture.checkConjectureStructure(conjecture.getRoot())) {
    exit(20);
  }
  width.print();
  dynamicKernel.set_width(width);
}

void InputController::construct_dynamicKernel() {
  DynamicCoreHandler *factory;
  int coreIndex = 1;
  for (auto core : varToProperty) {
      if (coreList.count(core.second->getName())) {
          std::cout << std::left << std::setw(2) << coreIndex << std::setw(5)
                    << "- Variable: " << std::setw(5) << core.first << std::setw(5)
                    << "NAME: " << std::setw(30) << core.second->getName();
          coreIndex++;
          // set coreType in core.second which is a PropertyAssignment.
          core.second->setType(coreList[core.second->getName()]["CoreType"]);
          // set parameterType in core.second which is a PropertyAssignment.
          core.second->setParameterType(coreList[core.second->getName()]["ParameterType"]);
          std::cout << std::setw(10) << "ParameterType: " << std::setw(10) << coreList[core.second->getName()]["ParameterType"];
          for(auto p: core.second->getParametersVec()){
              std::cout << " " << p;
          }
          std::cout << std::endl;
          std::string corePath = dynamicPluginPath + coreNamesToFiles[core.second->getName()];
          factory = new DynamicCoreHandler(corePath.c_str());
          std::unique_ptr<DynamicCore> handlerCore = factory->create_parameters(core.second->getParametersVec());
          DynamicCore *corePointer = handlerCore.release();
          corePointer->setWidth(dynamicKernel.get_width().get_value());
          if (dynamicKernel.isVarExists(core.first)) {
              std::cout << "ERROR: " << core.first << " exists in DynamicKernel"
                        << std::endl;
              exit(20);
          }
          dynamicKernel.addCore(*corePointer);
          auto varToNameAndIndex = dynamicKernel.getVarToNameAndIndex();
          varToNameAndIndex.insert(make_pair(
                  core.first,
                  make_pair(core.second->getName(), dynamicKernel.coreSize() - 1)));
          dynamicKernel.setVarToNameAndIndex(varToNameAndIndex);
//        if (coreList[core.second->getName()]["ParameterType"] ==
//                "ParameterLess" and
//            core.second->getParameterType() == "ParameterLess") {
//          std::string corePath =
//              dynamicPluginPath + coreNamesToFiles[core.second->getName()];
//          factory = new DynamicCoreHandler(corePath.c_str());
//          std::unique_ptr<DynamicCore> handlerCore = factory->create();
//          DynamicCore *corePointer = handlerCore.release();
//          corePointer->setWidth(dynamicKernel.get_width().get_value());
//          if (dynamicKernel.isVarExists(core.first)) {
//            std::cout << "ERROR: " << core.first << " exists in DynamicKernel"
//                      << std::endl;
//            exit(20);
//          }
//          dynamicKernel.addCore(*corePointer);
//          auto varToNameAndIndex = dynamicKernel.getVarToNameAndIndex();
//          varToNameAndIndex.insert(make_pair(
//              core.first,
//              make_pair(core.second->getName(), dynamicKernel.coreSize() - 1)));
//          dynamicKernel.setVarToNameAndIndex(varToNameAndIndex);
//        } else if (coreList[core.second->getName()]["ParameterType"] ==
//                       "UnsignedInt" and
//                   core.second->getParameterType() == "UnsignedInt") {
//          unsigned parameter = core.second->getParameter();
//          if (core.second->getOp() == ">") parameter++;
//          if (core.second->getOp() == "<") parameter--;
//          if (parameter < 0) {
//            std::cout << " The given parameter for the core "
//                      << core.second->getName() << " is " << parameter
//                      << ". Parameter could not be negative." << std::endl;
//            exit(20);
//          }
//          std::string corePath =
//              dynamicPluginPath + coreNamesToFiles[core.second->getName()];
//          factory = new DynamicCoreHandler(corePath.c_str());
//          std::unique_ptr<DynamicCore> handlerCore =
//              factory->create_int(parameter);
//          DynamicCore *corePointer = handlerCore.release();
//          corePointer->setWidth(dynamicKernel.get_width().get_value());
//          dynamicKernel.addCore(*corePointer);
//          auto varToNameAndIndex = dynamicKernel.getVarToNameAndIndex();
//          varToNameAndIndex.insert(make_pair(
//              core.first,
//              make_pair(core.second->getName(), dynamicKernel.coreSize() - 1)));
//          dynamicKernel.setVarToNameAndIndex(varToNameAndIndex);
//        } else {
//          std::cout << "ERROR: ParameterType of the core "
//                    << core.second->getName()
//                    << "  is not valid. The Given ParameterType is "
//                    << core.second->getParameterType() << std::endl;
//          exit(20);
//        }
//      } else {
//        std::cout << "ERROR: " << core.first
//                  << " hasn't ParameterType, check the properties of the core. "
//                  << std::endl;
//        exit(20);
//      }
      } else {
          std::cout << "Core " << core.first << " := " << core.second->getName() << " is not exist" << std::endl;
          exit(20);
      }
  }
    conjecture.setKernel(&dynamicKernel);
    std::cout << "core size: " << dynamicKernel.coreSize() << std::endl;
}

DynamicKernel &InputController::getDynamicKernel() { return dynamicKernel; }

Conjecture &InputController::getConjecture() { return conjecture; }

InputController::InputController(const std::string &inputPath,
                                 const std::string &dynamicPluginPath)
    : inputPath(inputPath), dynamicPluginPath(dynamicPluginPath) {
  check_available_cores();
  parse_input();
  construct_dynamicKernel();
}

const std::string &InputController::getInputPath() const { return inputPath; }

InputController::InputController(const std::string &inputPath,
                                 const std::string &dynamicPluginPath,
                                 const Width &width)
    : inputPath(inputPath), dynamicPluginPath(dynamicPluginPath), width(width) {
  check_available_cores();

  parse_input();
  construct_dynamicKernel();
}
