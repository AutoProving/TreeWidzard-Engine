// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "SearchController.h"
#include "../Kernel/PathList.h"
#include "../Kernel/RuntimePaths.h"
#include <concepts>
#include <filesystem>

#ifndef TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT
#define TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT ""
#endif


#ifdef _WIN32
    #define DYNAMIC_LIB_EXTENSION ".dll"
#elif __APPLE__
    #define DYNAMIC_LIB_EXTENSION ".dylib"
#elif __linux__
    #define DYNAMIC_LIB_EXTENSION ".so"
#else
    #error "Unsupported operating system"
#endif


SearchController::SearchController(std::string inputPath,
								   std::string searchStrategyName_,
								   Flags flags_, Width width_)
	: searchStrategyName(std::move(searchStrategyName_)),
	  flags(std::move(flags_)),
	  width(std::move(width_)) {
	if (const char *paths = std::getenv("TREEWIDZARD_SEARCHSTRATEGIES")) {
		searchPluginPaths = TreeWidzard::split_path_list(paths);
	} else {
		searchPluginPaths = TreeWidzard::default_plugin_paths(
			"TREEWIDZARD_SEARCHSTRATEGIES",
			TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT, "SearchStrategies");
	}

	inputController = std::make_unique<InputController>(
		std::move(inputPath), InputController::default_paths(), width);
}

void SearchController::check_search() {
	try {
		searchList.clear();
		searchNamesToFiles.clear();
		for (const auto &searchPluginPath : searchPluginPaths) {
			std::error_code ec;
			if (!std::filesystem::exists(searchPluginPath, ec) ||
				!std::filesystem::is_directory(searchPluginPath, ec)) {
				continue;
			}
			for (const auto &entry :
				 std::filesystem::directory_iterator(searchPluginPath, ec)) {
				if (ec) {
					break;
				}
				const std::string s = entry.path().string();
				// for linux .so, and windows .dll
				if (s.find(DYNAMIC_LIB_EXTENSION) != std::string::npos) {
					SearchStrategyHandler factory(s);
					const std::map<std::string, std::string> &attributes =
						factory.get_metadata();
					searchNamesToFiles.emplace(attributes.at("SearchName"),
											   s);
					searchList.emplace(attributes.at("SearchName"), attributes);
				}
			}
		}
	} catch (std::exception &e) {
		std::cerr << "There is an error during reading search plugins:"
				  << std::endl;
		std::cerr << e.what() << '\n';
		exit(20);
	}
}

void SearchController::action() {
	std::cout << "Property information:\n" << std::endl;
	std::cout << "Formula: ";
	inputController->getConjecture().print();
	std::cout << "\n" <<std::endl;
	for (auto core:inputController->getvarToProperty()) {
		std::cout <<core.first << " := " ;
		std::cout << core.second->getName() << "(";
		core.second->printParameters();
		std::cout << ")" << std::endl;
		std::cout << "Core type: "<< core.second->getType() << std::endl;
	}

	std::cout << "\nSearch information:\n" << std::endl;
	std::cout << "Width parameter: " << width.get_name() << " = " << width.get_value() << std::endl;
	std::cout << "Search method: " << searchStrategyName << std::endl;
	std::cout << "Premise flag: " << (flags.get("Premise") ? "ACTIVATED" : "NOT ACTIVATED") << std::endl;
	check_search();
	auto it = searchList.find(searchStrategyName);
	if (it != searchList.end()) {
		std::string libPath = searchNamesToFiles[searchStrategyName];
		SearchStrategyHandler searchStrategyHandler(libPath);

		// std::cerr << "found matching search strategy at path: " << libPath
		// 		  << '\n';

		std::unique_ptr<SearchStrategy> search = searchStrategyHandler.create(
			&inputController->getDynamicKernel(),
			&inputController->getConjecture(), &flags);
		search->setPropertyFilePath(inputController->getInputPath());
		std::string output_file_path =
			std::filesystem::path(inputController->getInputPath())
				.parent_path()
				.string();
		if (output_file_path == "") {
			output_file_path =
				std::filesystem::path(inputController->getInputPath())
					.stem()
					.string() +
				"_" + searchStrategyName + "_" + width.get_name() + "_" +
				std::to_string(width.get_value());

		} else {
			output_file_path =
				output_file_path + "/" +
				std::filesystem::path(inputController->getInputPath())
					.stem()
					.string() +
				"_" + searchStrategyName + "_" + width.get_name() + "_" +
				std::to_string(width.get_value());
		}
		search->setOutputsPath(output_file_path);
		std::cout << "\nSearch process:" << std::endl;
		search->search();
		std::cout << "-----------------------------------------" << std::endl;
	} else {
		std::cout << "Error: Search method " << searchStrategyName
				  << " was not found." << std::endl;
		exit(20);
	}

}
