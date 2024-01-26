#include <filesystem>
#include <memory>
#include <stdexcept>
#include "InputController.h"

void InputController::check_available_cores() {
	for (std::string dynamicPluginPath : dynamicPluginPaths) {
		for (const auto &entry :
			 std::filesystem::directory_iterator(dynamicPluginPath)) {
			std::string s = entry.path();
			if (s.find(".so") != std::string::npos) {
				std::string fileName = entry.path().filename();
				std::cout << "File " << fileName;
				try {
					auto factory = DynamicCoreHandler(s);
					const std::string &name =
						factory.get_metadata().at("CoreName");

					auto [it, placed] =
						coreList.emplace(name, std::move(factory));
					if (!placed)
						throw std::runtime_error("Duplicate core name.");
					std::cout << " Loaded. Core Name: " << name << '\n';
				} catch (std::exception &e) {
					std::cerr << " failed to load." << e.what() << std::endl;
				}
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
	int result = 1; // if parsing successful result will be 0 otherwise 1

	input_parse(conjecture, result, coreList, varToCoreName,
				varToProperty); // Parser function from Parser.hpp

	// check for successful parsing
	if (result != 0) {
		std::cout << " Error: input file " << inputPath
				  << " is not in valid format" << std::endl;
		exit(20);
	}

	//  for (const auto &[name, props] : varToProperty) {
	//      std::cout << name << ": ";
	//      for (const auto &prop : props->getParametersVec()) {
	//          std::visit(overloaded{
	//              [&](char *p) {
	//                  std::cout << p << ' ';
	//              },
	//              [&](auto p) {
	//                  std::cout << "not a string" << ' ';
	//              }
	//              }, prop);
	//      }
	//      std::cout << '\n';
	//  }

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
	int coreIndex = 1;
	for (auto core : varToProperty) {
		if (coreList.count(core.second->getName())) {
			std::cout << std::left << std::setw(2) << coreIndex << std::setw(5)
					  << "- Variable: " << std::setw(5) << core.first
					  << std::setw(5) << "NAME: " << std::setw(30)
					  << core.second->getName();
			coreIndex++;
			// set coreType in core.second which is a PropertyAssignment.
			core.second->setType(coreList.at(core.second->getName())
									 .get_metadata()
									 .at("CoreType"));
			// set parameterType in core.second which is a PropertyAssignment.
			core.second->setParameterType(coreList.at(core.second->getName())
											  .get_metadata()
											  .at("ParameterType"));
			std::cout << std::setw(10) << "ParameterType: " << std::setw(10)
					  << coreList.at(core.second->getName())
							 .get_metadata()
							 .at("ParameterType");
			std::cout << std::endl;
			std::unique_ptr<DynamicCore> handlerCore =
				coreList.at(core.second->getName())
					.create(core.second->getParametersVec());
			DynamicCore *corePointer = handlerCore.release();
			corePointer->setWidth(dynamicKernel.get_width().get_value());
			if (dynamicKernel.isVarExists(core.first)) {
				std::cout << "ERROR: " << core.first
						  << " exists in DynamicKernel" << std::endl;
				exit(20);
			}
			dynamicKernel.addCore(*corePointer);
			auto varToNameAndIndex = dynamicKernel.getVarToNameAndIndex();
			varToNameAndIndex.insert(
				make_pair(core.first, make_pair(core.second->getName(),
												dynamicKernel.coreSize() - 1)));
			dynamicKernel.setVarToNameAndIndex(varToNameAndIndex);
		} else {
			std::cout << "Core " << core.first
					  << " := " << core.second->getName() << " is not exist"
					  << std::endl;
			exit(20);
		}
	}
	conjecture.setKernel(&dynamicKernel);
	std::cout << "core size: " << dynamicKernel.coreSize() << std::endl;
}

DynamicKernel &InputController::getDynamicKernel() { return dynamicKernel; }
Conjecture &InputController::getConjecture() { return conjecture; }
InputController::InputController(std::string inputPath,
								 std::vector<std::string> dynamicPluginPaths)
	: inputPath(std::move(inputPath)),
	  dynamicPluginPaths(std::move(dynamicPluginPaths)) {
	check_available_cores();
	parse_input();
	construct_dynamicKernel();
}

const std::string &InputController::getInputPath() const { return inputPath; }

InputController::InputController(std::string inputPath,
								 std::vector<std::string> dynamicPluginPaths,
								 Width width)
	: inputPath(inputPath),
	  dynamicPluginPaths(dynamicPluginPaths),
	  width(width) {
	check_available_cores();
	parse_input();
	construct_dynamicKernel();
}
