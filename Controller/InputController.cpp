#include "InputController.h"
#include <cstdio>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <unordered_map>


#ifdef _WIN32
    #define DYNAMIC_LIB_EXTENSION ".dll"
#elif __APPLE__
    #define DYNAMIC_LIB_EXTENSION ".dylib"
#elif __linux__
    #define DYNAMIC_LIB_EXTENSION ".so"
#else
    #error "Unsupported operating system"
#endif

extern std::map<std::string, ConjectureNode *> sub_formula_variables;

[[noreturn]] void InputController::fail(const std::string &message) const {
	if (errorMode == InputControllerErrorMode::Throw) {
		throw std::runtime_error(message);
	}
	std::cerr << message << std::endl;
	exit(20);
}

auto InputController::discover_core_handlers(
	const std::vector<std::string> &dynamicPluginPaths, bool quiet)
	-> std::map<std::string, DynamicCoreHandler> {
	std::map<std::string, DynamicCoreHandler> discovered_cores;
	std::unordered_map<std::string, std::string> core_locations;

	for (const std::string &dynamicPluginPath : dynamicPluginPaths) {
		std::error_code ec;
		if (!std::filesystem::exists(dynamicPluginPath, ec) ||
			!std::filesystem::is_directory(dynamicPluginPath, ec)) {
			continue;
		}
		for (const auto &entry :
			 std::filesystem::directory_iterator(dynamicPluginPath, ec)) {
			if (ec) {
				break;
			}
			const std::string library_path = entry.path().string();
			if (library_path.find(DYNAMIC_LIB_EXTENSION) == std::string::npos) {
				continue;
			}
			try {
				auto factory = DynamicCoreHandler(library_path);
				const std::string &name = factory.get_metadata().at("CoreName");
				auto [it, placed] =
					discovered_cores.emplace(name, std::move(factory));
				if (!placed) {
					if (!quiet) {
						if (auto loc = core_locations.find(name);
							loc != core_locations.end() &&
							loc->second != library_path) {
							std::cerr << "Duplicate core name '" << name
									  << "'. Keeping " << loc->second
									  << " and skipping " << library_path
									  << std::endl;
						}
					}
					continue;
				}
				core_locations[name] = library_path;
			} catch (std::exception &e) {
				if (!quiet) {
					std::cerr << " failed to load." << e.what() << std::endl;
				}
			}
		}
	}

	return discovered_cores;
}

void InputController::check_available_cores() {
	if (!quiet) {
		std::cout << "--------------- TreeWidzard Dynamic Cores LOADING "
					 "INFORAMATION --------------- "
				  << std::endl;
	}
	coreList = discover_core_handlers(dynamicPluginPaths, quiet);
}

void InputController::parse_input() {
	struct FileCloser {
		void operator()(std::FILE *file) const noexcept {
			if (file) {
				std::fclose(file);
			}
		}
	};

	std::unique_ptr<std::FILE, FileCloser> input_file(nullptr);
	if (inputIsInlineSource) {
		input_file.reset(tmpfile());
		if (!input_file) {
			fail("Error: failed to create a temporary input file.");
		}
		if (std::fputs(inputSource.c_str(), input_file.get()) < 0) {
			fail("Error: failed to write inline conjecture source.");
		}
		std::rewind(input_file.get());
	} else {
		input_file.reset(fopen(inputPath.c_str(), "r"));
		if (!input_file) {
			fail("Error: input file opening failed, given path: " + inputPath);
		}
	}

	int result = 1; // if parsing successful result will be 0 otherwise 1
	if (!quiet) {
		std::cout << "Loaded DP-Cores to TreeWidzard:";
		for (const auto &pair : coreList) {
			std::cout << pair.first << " - "; // pair.first is the key
		}
		std::endl(std::cout);
	}

	input_in = input_file.get();
	int parse_status = 0;
	sub_formula_variables.clear();
	try {
		parse_status =
			input_parse(conjecture, result, coreList, varToCoreName,
						varToProperty); // Parser function from Parser.hpp
	} catch (const std::exception &e) {
		input_in = nullptr;
		fail(std::string("Error while parsing input: ") + e.what());
	} catch (...) {
		input_in = nullptr;
		fail("Error while parsing input.");
	}
	input_in = nullptr;

	// check for successful parsing
	if (parse_status != 0 || result != 0) {
		fail("Error: input file " + inputPath + " is not in valid format");
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
		fail("Error: input file " + inputPath + " has an invalid conjecture structure");
	}

	dynamicKernel.set_width(width);
	if (!quiet) {
		std::cout << "----------------------------------------------------------"
				  << std::endl;
	}
}

void InputController::construct_dynamicKernel() {
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
			handlerCore->setWidth(dynamicKernel.get_width().get_value());
			if (dynamicKernel.isVarExists(core.first)) {
				fail("ERROR: " + core.first + " exists in DynamicKernel");
			}
			DynamicCore *corePointer = handlerCore.get();
			ownedCores.push_back(std::move(handlerCore));
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
		} else {
			fail("Core " + core.first + " := " + core.second->getName() +
				 " is not exist");
		}
	}
	conjecture.setKernel(&dynamicKernel);
	// std::cout << "core size: " << dynamicKernel.coreSize() << std::endl;
}

DynamicKernel &InputController::getDynamicKernel() { return dynamicKernel; }
Conjecture &InputController::getConjecture() { return conjecture; }
InputController::InputController(std::string inputIdentifier,
								 std::string inputSource,
								 bool inputIsInlineSource,
								 std::vector<std::string> dynamicPluginPaths,
								 Width width,
								 InputControllerErrorMode errorMode,
								 bool quiet)
	: inputPath(std::move(inputIdentifier)),
	  inputSource(std::move(inputSource)),
	  dynamicPluginPaths(std::move(dynamicPluginPaths)),
	  width(std::move(width)),
	  inputIsInlineSource(inputIsInlineSource),
	  quiet(quiet),
	  errorMode(errorMode) {
	check_available_cores();
	parse_input();
	construct_dynamicKernel();
}

InputController::InputController(std::string inputPath,
								 std::vector<std::string> dynamicPluginPaths,
								 InputControllerErrorMode errorMode)
	: InputController(std::move(inputPath), "", false,
					  std::move(dynamicPluginPaths), Width(), errorMode, false) {}

const std::string &InputController::getInputPath() const { return inputPath; }

InputController::InputController(std::string inputPath,
								 std::vector<std::string> dynamicPluginPaths,
								 Width width,
								 InputControllerErrorMode errorMode)
	: InputController(std::move(inputPath), "", false,
					  std::move(dynamicPluginPaths), std::move(width),
					  errorMode, false) {}

InputController::~InputController() {
	// Destroy dynamically loaded core instances before unloading the handler
	// libraries stored in coreList.
	ownedCores.clear();
}

auto InputController::default_paths() -> std::vector<std::string> {
	return TreeWidzard::default_plugin_paths(
		"TREEWIDZARD_DPCORES", TREEWIDZARD_DPCORES_DEFAULT, "DPCores");
}

auto InputController::fromSourceText(
	std::string inputSource, Width width,
	std::vector<std::string> dynamicPluginPaths,
	InputControllerErrorMode errorMode) -> std::unique_ptr<InputController> {
	return std::unique_ptr<InputController>(
		new InputController("<memory>", std::move(inputSource), true,
							 std::move(dynamicPluginPaths), std::move(width),
							 errorMode, true));
}
