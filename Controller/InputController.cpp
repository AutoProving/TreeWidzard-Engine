#include "InputController.h"
#include <filesystem>
#include <memory>
#include <stdexcept>


#ifdef _WIN32
    #define DYNAMIC_LIB_EXTENSION ".dll"
#elif __APPLE__
    #define DYNAMIC_LIB_EXTENSION ".dylib"
#elif __linux__
    #define DYNAMIC_LIB_EXTENSION ".so"
#else
    #error "Unsupported operating system"
#endif

void InputController::check_available_cores() {
	std::cout<<"--------------- TreeWidzard Dynamic Cores LOADING INFORAMATION --------------- "<<std::endl;
	for (std::string dynamicPluginPath : dynamicPluginPaths) {
		for (const auto &entry :
			 std::filesystem::directory_iterator(dynamicPluginPath)) {
			std::string s = entry.path();
			// for linux .so, and windows .dll
			if (s.find(DYNAMIC_LIB_EXTENSION) != std::string::npos) {
				std::string fileName = entry.path().filename();
				// std::cout << "File " << fileName;
				try {
					auto factory = DynamicCoreHandler(s);
					const std::string &name =
						factory.get_metadata().at("CoreName");

					auto [it, placed] =
						coreList.emplace(name, std::move(factory));
					if (!placed)
						throw std::runtime_error(" Duplicate core name: " +
												 name + "\n File:"+	s);
					// std::cout << " Loaded. Core Name: " << name << '\n';
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
	std::cout << "Loaded DP-Cores to TreeWidzard:";
	for (const auto &pair : coreList) {
		std::cout << pair.first << " - "; // pair.first is the key
	}
	std::endl(std::cout);
	
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

	// std::cout << "------------- Property File Information ------------- " << std::endl;
	// std::cout << "\nFormula: ";
	// conjecture.print();
	// std::cout << std::endl;
	conjecture.setvarToProperty(varToProperty);
	conjecture.setVariablesToCoreName(varToCoreName);
	if (!conjecture.checkConjectureStructure(conjecture.getRoot())) {
		exit(20);
	}

	dynamicKernel.set_width(width);
	std::cout << "----------------------------------------------------------" << std::endl;
}

void InputController::construct_dynamicKernel() {
	int coreIndex = 1;
	for (auto core : varToProperty) {
		if (coreList.count(core.second->getName())) {
			// std::cout << std::left << std::setw(2) << coreIndex << std::setw(5)
			// 		  << "- Variable: " << std::setw(5) << core.first
			// 		  << std::setw(5) << "NAME: " << std::setw(30)
			// 		  << core.second->getName();
			
			// set coreType in core.second which is a PropertyAssignment.
			core.second->setType(coreList.at(core.second->getName())
									 .get_metadata()
									 .at("CoreType"));
			// set parameterType in core.second which is a PropertyAssignment.
			core.second->setParameterType(coreList.at(core.second->getName())
											  .get_metadata()
											  .at("ParameterType"));
			// std::cout << std::setw(10) << "ParameterType: " << std::setw(10)
			// 		  << coreList.at(core.second->getName())
			// 				 .get_metadata()
			// 				 .at("ParameterType");
			// std::cout << std::endl;
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

			// std::cout << coreIndex << "-  " << core.first << " := "
			// 		  << core.second->getName() << "( ";
			// core.second->printParameters();
			// std::cout<< ")"<< std::setw(5)<<" CoreType: '" << core.second->getType()<< "'" <<std::setw(5)<< " ParameterType: " << core.second->getParameterType() << std::endl;

			coreIndex++;
		} else {
			std::cout << "Core " << core.first
					  << " := " << core.second->getName() << " is not exist"
					  << std::endl;
			exit(20);
		}
	}
	conjecture.setKernel(&dynamicKernel);
	// std::cout << "core size: " << dynamicKernel.coreSize() << std::endl;
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
	std::cout << "Path" << dynamicPluginPaths[0] << std::endl;
	check_available_cores();
	parse_input();
	construct_dynamicKernel();
}
