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
#include "../TreeAutomaton/TreeAutomaton.h"
#include "../TreeAutomaton/AbstractTreeDecomposition.h"

using namespace std;
extern FILE *gr_in;
extern FILE *td_in;
extern FILE *ctd_in;
class ParseController {
private:
    Flags flag;
    InputController* inputController;
public:
    // constructor
    ParseController(const Flags &flag, const string &inputPath);
    void parse_pace(string graphPath, string decompositionPath);
    void parse_abstract(string abstractPath);
    void test_term();

};

class FarhadState{
public:
    string name;

};
#endif //TREEWIDZARD_PARSECONTROLLER_H
