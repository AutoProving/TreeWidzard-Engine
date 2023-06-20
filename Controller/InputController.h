#ifndef TREEWIDZARD_INPUTCONTROLLER_H
#define TREEWIDZARD_INPUTCONTROLLER_H
#include <experimental/filesystem>
#include <iomanip>
#include <iostream>
#include "../Conjecture/Conjecture.h"
#include "../Conjecture/PropertyAssignment.h"
#include "../Kernel/DynamicCoreHandler.h"
#include "../Kernel/DynamicKernel.h"
#include "../Parser/PropertyParser/input_parser.hpp"

#ifndef TREEWIDZARD_DPCORES_DEFAULT
#define TREEWIDZARD_DPCORES_DEFAULT ""
#endif

extern std::FILE *input_in;

class InputController {
  private:
	std::string inputPath; // Path of the input file
	std::vector<std::string>
		dynamicPluginPaths; // Paths to look dynamic plugins in
	DynamicKernel dynamicKernel;
	Width width;
	Conjecture conjecture;

	// These attributes are auxiliary and are used for parsing and constructing
	// the input.
	std::map<std::string, DynamicCoreHandler>
		coreList; // Each entry is a pair of core name and core attributes.
	std::map<std::string, std::string>
		varToCoreName; // map from variables to name of cores
	std::map<std::string, PropertyAssignment *>
		varToProperty; // map from var to Property
  public:
	InputController(
		std::string inputPath,
		std::vector<std::string> dynamicPluginPath = default_paths());
	InputController(std::string inputPath,
					std::vector<std::string> dynamicPluginPath, Width width);

	const std::string &getInputPath() const;
	DynamicKernel &getDynamicKernel();
	Conjecture &getConjecture();
	void check_available_cores();
	void parse_input();
	void construct_dynamicKernel();

	static auto default_paths() -> std::vector<std::string> {
		std::vector<std::string> paths;
		if (const char *epaths = std::getenv("TREEWIDZARD_DPCORES")) {
			std::istringstream is(epaths);
			std::string path;
			while (getline(is, path, ':'), is) paths.push_back(path);
		} else {
			std::istringstream is(TREEWIDZARD_DPCORES_DEFAULT);
			std::string path;
			while (getline(is, path, ':'), is) paths.push_back(path);
		}
		return paths;
	}
};

#endif // TREEWIDZARD_INPUTCONTROLLER_H
