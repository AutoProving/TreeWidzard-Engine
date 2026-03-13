#ifndef TREEWIDZARD_INPUTCONTROLLER_H
#define TREEWIDZARD_INPUTCONTROLLER_H
// #include <experimental/filesystem>
#include "../Conjecture/Conjecture.h"
#include "../Conjecture/PropertyAssignment.h"
#include "../Kernel/DynamicCoreHandler.h"
#include "../Kernel/DynamicKernel.h"
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <variant>
#include <vector>
#pragma push_macro("YYSTYPE")
#undef YYSTYPE
#pragma push_macro("YYLTYPE")
#undef YYLTYPE

#include "input_parser.hpp"

#pragma pop_macro("YYLTYPE")
#pragma pop_macro("YYSTYPE")

extern int
input_parse(Conjecture &conj, int &result,
            std::map<std::string, DynamicCoreHandler> &coreList,
            std::map<std::string, std::string> &varToCoreName,
            std::map<std::string, PropertyAssignment *> &varToProperty);

#ifndef TREEWIDZARD_DPCORES_DEFAULT
#define TREEWIDZARD_DPCORES_DEFAULT "DPCores"
#endif

extern std::FILE *input_in;

enum class InputControllerErrorMode {
  Exit,
  Throw,
};

class InputController {
private:
  std::string inputPath; // Path of the input file
  std::string inputSource;
  std::vector<std::string>
      dynamicPluginPaths; // Paths to look dynamic plugins in
  DynamicKernel dynamicKernel;
  Width width;
  Conjecture conjecture;
  bool inputIsInlineSource = false;
  bool quiet = false;
  InputControllerErrorMode errorMode = InputControllerErrorMode::Exit;

  // These attributes are auxiliary and are used for parsing and constructing
  // the input.
  std::map<std::string, DynamicCoreHandler>
      coreList; // Each entry is a pair of core name and core attributes.
  std::map<std::string, std::string>
      varToCoreName; // map from variables to name of cores
  std::map<std::string, PropertyAssignment *>
      varToProperty; // map from var to Property
  InputController(std::string inputIdentifier, std::string inputSource,
                  bool inputIsInlineSource,
                  std::vector<std::string> dynamicPluginPath, Width width,
                  InputControllerErrorMode errorMode, bool quiet);
  [[noreturn]] void fail(const std::string &message) const;
public:
  InputController(std::string inputPath,
                  std::vector<std::string> dynamicPluginPath = default_paths(),
                  InputControllerErrorMode errorMode =
                      InputControllerErrorMode::Exit);
  InputController(std::string inputPath,
                  std::vector<std::string> dynamicPluginPath, Width width,
                  InputControllerErrorMode errorMode =
                      InputControllerErrorMode::Exit);

  const std::string &getInputPath() const;
  DynamicKernel &getDynamicKernel();
  Conjecture &getConjecture();
  void check_available_cores();
  void parse_input();
  void construct_dynamicKernel();
  static auto fromSourceText(
      std::string inputSource, Width width,
      std::vector<std::string> dynamicPluginPath = default_paths(),
      InputControllerErrorMode errorMode = InputControllerErrorMode::Throw)
      -> std::unique_ptr<InputController>;
  static auto discover_core_handlers(
      const std::vector<std::string> &dynamicPluginPath = default_paths(),
      bool quiet = false) -> std::map<std::string, DynamicCoreHandler>;

  static auto default_paths() -> std::vector<std::string> {
    std::vector<std::string> paths;
    if (const char *epaths = std::getenv("TREEWIDZARD_DPCORES")) {
      std::istringstream is(epaths);
      std::string path;
      while (getline(is, path, ':'), is)
        paths.push_back(path);
    } else {
      std::istringstream is(TREEWIDZARD_DPCORES_DEFAULT);
      std::string path;
      while (getline(is, path, ':'), is)
        paths.push_back(path);
    }
    return paths;
  }
  std::map<std::string, PropertyAssignment *> getvarToProperty() {
    return varToProperty;
  }
};

#endif // TREEWIDZARD_INPUTCONTROLLER_H
