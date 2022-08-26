#ifndef TREEWIDZARD_PARSECONTROLLER_H
#define TREEWIDZARD_PARSECONTROLLER_H
#include <iostream>
#include <memory>
#include <experimental/filesystem>
#include "../Kernel/Flags.h"
#include "../Multigraph/MultiGraph.h"
#include "../Translation/PACE/TreeDecompositionPACE.h"
#include "InputController.h"
#include "../Translation/PACE/WitnessTreePACE.h"
#include "../Translation/PACE/Parser/td_parser.hpp"
#include "../Translation/PACE/Parser/gr_parser.hpp"
#include "../ConcreteTreeDecomposition/ctd_parser.hpp"
#include "../Translation/PACE/Parser/atd_parser.hpp"
#include "../TreeAutomaton/TreeAutomaton.h"
#include "../TreeAutomaton/AbstractTreeDecomposition.h"

extern std::FILE *gr_in;
extern std::FILE *td_in;
extern std::FILE *ctd_in;
extern std::FILE *atd_in;

class ParseController {
private:
    Flags flag;
    InputController* inputController;
public:
    // constructor
    ParseController(const Flags &flag, const std::string &inputPath);
    void parse_pace(std::string graphPath, std::string decompositionPath);
    void parse_abstract(std::string abstractPath);
    void test_term();

};

#endif //TREEWIDZARD_PARSECONTROLLER_H
