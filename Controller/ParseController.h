#ifndef TREEWIDZARD_PARSECONTROLLER_H
#define TREEWIDZARD_PARSECONTROLLER_H
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include "../ConcreteTreeDecomposition/ctd_parser.hpp"
#include "../Kernel/Flags.h"
#include "../Multigraph/MultiGraph.h"
#include "../Translation/PACE/Parser/atd_parser.hpp"
#include "../Translation/PACE/Parser/gr_parser.hpp"
#include "../Translation/PACE/Parser/td_parser.hpp"
#include "../Translation/PACE/TreeDecompositionPACE.h"
#include "../Translation/PACE/WitnessTreePACE.h"
#include "../TreeAutomaton/InstructiveTreeDecomposition.h"
#include "../TreeAutomaton/TreeAutomaton.h"
#include "InputController.h"

extern std::FILE *ctd_in;
extern std::FILE *atd_in;

class ParseController {
  private:
	Flags flag;
	std::unique_ptr<InputController> inputController;

  public:
	// constructor
	ParseController(const Flags &flag, const std::string &inputPath);
	void parse_pace(std::string graphPath, std::string decompositionPath);
	void parse_itd(std::string itdPath);
	void test_term();
};

#endif // TREEWIDZARD_PARSECONTROLLER_H
