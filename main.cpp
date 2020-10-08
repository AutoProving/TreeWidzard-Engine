// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include <stdio.h>
#include <iostream>
#include "DynamicCores/CliqueNumber_AtLeast.h"
#include "DynamicCores/HasMultipleEdges.h"
#include "Kernel/ConcreteTreeDecomposition.h"
#include "Parser/parser.hpp"
#include "Search/LeveledSetSearch.h"

using namespace std;
extern FILE *yyin;

int main(int argc, char *arg[]) {
	string inputPath = "../input.txt";
	int printStates = 0;
	int loopTime = 1;
	int printVector = 0;
	string function = "search";
	string searchMethod = "leveledSetSearch";
	if (argc < 2) {
		cout << "Input Argument is empty" << endl;
		exit(20);
	}
	vector<string> parameters;
	for (int i = 1; i < argc; i++) {
		parameters.push_back(arg[i]);
	}
	sort(parameters.begin(), parameters.end());
	// Check for the search type
	vector<string> searchMethodVectorPotential;
	searchMethodVectorPotential.push_back("leveledSetSearch");
	sort(searchMethodVectorPotential.begin(),
		 searchMethodVectorPotential.end());
	vector<string> paramANDsearchMethod;
	set_intersection(
		parameters.begin(), parameters.end(),
		searchMethodVectorPotential.begin(), searchMethodVectorPotential.end(),
		inserter(paramANDsearchMethod, paramANDsearchMethod.begin()));
	if (paramANDsearchMethod.size() > 2) {
		cout << "ERROR: searchMethod was not inserted correctly" << endl;
		exit(20);
	} else if (paramANDsearchMethod.size() == 1) {
		searchMethod = paramANDsearchMethod[0];
	} else {
		cerr << "Search Method Error!" << endl;
		exit(20);
	}

	// Check for the flags
	auto parametersItr =
		find(parameters.begin(), parameters.end(), "printState");
	if (parametersItr != parameters.end()) {
		printStates = 1;
	}
	parametersItr = find(parameters.begin(), parameters.end(), "printVector");
	if (parametersItr != parameters.end()) {
		printVector = 1;
	}
	parametersItr = find(parameters.begin(), parameters.end(), "noLoopTime");

	if (parametersItr != parameters.end()) {
		loopTime = 0;
	}

	function = arg[1];

	char *path = arg[2];
	cout << "Configuration: input path: " << path
		 << " printState: " << printStates << " printVector: " << printVector
		 << " loopTime: " << loopTime << " search method: " << searchMethod
		 << endl;

	Width width;
	vector<PropertyAssignment *> pl;
	Conjecture conjecture;

	yyin = fopen(path, "r");
	if (!yyin) {
		std::perror("File opening failed");
		return EXIT_FAILURE;
	}
	int result = 1; // if parsing successful result will be 0 otherwise 1
	yyparse(pl, conjecture, width, result); // Parser function from Parser.hpp
	// check for successful parsing
	if (result != 0) {
		cout << " Error: input file " << path << " is not in valid format"
			 << endl;
		exit(20);
	}
	DynamicKernel dynamicKernel;
	dynamicKernel.get_width().set_name(width.get_name());
	dynamicKernel.get_width().set_value(width.get_value());
	vector<PropertyAssignment> propertyList;
	// change pl (vector<PropertyAssignment*>) to propertyList
	// (vector<PropertyAssignment>).
	for (size_t i = 0; i < pl.size(); ++i) {
		PropertyAssignment p;
		p.set_value(pl[i]->get_value());
		p.set_name(pl[i]->get_name());
		p.set_operatorSign(pl[i]->get_operatorSign());
		p.set_type(pl[i]->get_type());
		p.set_label(pl[i]->get_label());
		p.set_filePath(pl[i]->get_filePath());
		propertyList.push_back(p);
		dynamicKernel.addProperty(p);
	}
	cout << dynamicKernel.get_width().get_name() << " "
		 << dynamicKernel.get_width().get_value() << endl;
	// adding Cores to dynamicKernel
	for (size_t j = 0; j < propertyList.size(); ++j) {
		if (propertyList[j].get_name() == "HasMultipleEdges") {
			cout << "Core " << j << " " << propertyList[j].get_name() << " "
				 << propertyList[j].get_operatorSign() << " "
				 << propertyList[j].get_value() << endl;
			if (propertyList[j].get_operatorSign() == "=") {
				HasMultipleEdges_DynamicCore *hasMultipleEdgesDynamicCore =
					new HasMultipleEdges_DynamicCore();
				dynamicKernel.addCore(*hasMultipleEdgesDynamicCore);
			} else {
				cout << "Error of core sign!" << endl;
				exit(20);
			}
		}else 	if (propertyList[j].get_name() == "CliqueNumber") {
			cout << "Core " << j << " " << propertyList[j].get_name() << " "
				 << propertyList[j].get_operatorSign() << " "
				 << propertyList[j].get_value() << endl;
			if (propertyList[j].get_operatorSign() == ">") {
				CliqueNumber_AtLeast_DynamicCore
					*cliqueNumberGreaterThanOrEqualDynamicCore =
					new CliqueNumber_AtLeast_DynamicCore(
						propertyList[j].get_value() + 1);
				dynamicKernel.addCore(
					*cliqueNumberGreaterThanOrEqualDynamicCore);
			} else if (propertyList[j].get_operatorSign() == ">=") {
				CliqueNumber_AtLeast_DynamicCore
					*cliqueNumberGreaterThanOrEqualDynamicCore =
					new CliqueNumber_AtLeast_DynamicCore(
						propertyList[j].get_value());
				dynamicKernel.addCore(
					*cliqueNumberGreaterThanOrEqualDynamicCore);
			} else {
				cout << "Error of core sign!" << endl;
				exit(20);
			}
		} else {
			cout << "Core name isn't matched with defined cores" << endl;
			exit(20);
		}
	}
	Flags flags;
	flags.add_flag("PrintStates", printStates);
	flags.add_flag("LoopTime", loopTime);
	flags.add_flag("PrintVectors", printVector);
	conjecture.kernel = &dynamicKernel;
	conjecture.print();
	if (searchMethod == "leveledSetSearch") {
		LeveledSetSearch leveledSetSearch(&dynamicKernel, &conjecture, &flags);
		leveledSetSearch.search();
	} else {
		cout << "ERROR:  Search Method Error!" << endl;
		exit(20);
	}
	return 0;
}
