#include "SearchController.h"
#include <concepts>
#include <filesystem>

#ifndef TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT
#define TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT ""
#endif

SearchController::SearchController(std::string inputPath,
								   std::string searchStrategyName_,
								   Flags flags_, Width width_)
	: searchStrategyName(std::move(searchStrategyName_)),
	  flags(std::move(flags_)),
	  width(std::move(width_)) {
	if (const char *paths = std::getenv("TREEWIDZARD_SEARCHSTRATEGIES")) {
		std::istringstream is(paths);
		std::string path;
		while (getline(is, path, ':'), is) searchPluginPaths.push_back(path);
	} else {
		std::istringstream is(TREEWIDZARD_SEARCHSTRATEGIES_DEFAULT);
		std::string path;
		while (getline(is, path, ':'), is) searchPluginPaths.push_back(path);
	}

	inputController = new InputController(
		std::move(inputPath), InputController::default_paths(), width);
}

void SearchController::check_search() {
	try {
		for (const auto &searchPluginPath : searchPluginPaths) {
			for (const auto &entry :
				 std::filesystem::directory_iterator(searchPluginPath)) {
				std::string s = entry.path();
				if (s.find(".so") != std::string::npos) {
					char *lib_path = const_cast<char *>(s.c_str());
					SearchStrategyHandler factory(lib_path);
					const std::map<std::string, std::string> &attributes =
						factory.get_metadata();
					searchNamesToFiles.emplace(attributes.at("SearchName"),
											   lib_path);
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
	std::cout << "Search Method: " << searchStrategyName << std::endl;
	check_search();
	auto it = searchList.find(searchStrategyName);
	SearchStrategyHandler *searchStrategyHandler;
	if (it != searchList.end()) {
		std::string libPath = searchNamesToFiles[searchStrategyName];
		searchStrategyHandler = new SearchStrategyHandler(libPath.c_str());

		std::cerr << "found matching search strategy at path: " << libPath
				  << '\n';

		std::unique_ptr<SearchStrategy> search = searchStrategyHandler->create(
			&inputController->getDynamicKernel(),
			&inputController->getConjecture(), &flags);
		SearchStrategy *searchStrategy = search.release();
		searchStrategy->setPropertyFilePath(inputController->getInputPath());
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
		searchStrategy->setOutputsPath(output_file_path);
		searchStrategy->search();
	} else {
		std::cout << "Error: Search method " << searchStrategyName
				  << " was not found." << std::endl;
		exit(20);
	}
}
