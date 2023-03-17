#ifndef TREEWIDZARD_SEARCHCONTROLLER_H
#define TREEWIDZARD_SEARCHCONTROLLER_H
#include <iostream>
#include "../Kernel/Flags.h"
#include "../Kernel/SearchStrategyHandler.h"
#include "InputController.h"

extern std::FILE *input_in;

class SearchController {
  private:
	std::string searchStrategyName;
	std::vector<std::string> searchPluginPaths;
	Flags flags;
	std::map<std::string, std::map<std::string, std::string>>
		searchList; // Each entry is a pair of core name and core attributes.
	std::map<std::string, std::string> searchNamesToFiles;
	InputController *inputController;
	Width width;

  public:
	SearchController(std::string inputPath, std::string searchStrategyName,
					 Flags flags, Width width);
	void check_search();
	void action();
};

#endif // TREEWIDZARD_SEARCHCONTROLLER_H
