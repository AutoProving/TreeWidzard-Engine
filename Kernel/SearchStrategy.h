// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef SEARCHSTRATEGY_H
#define SEARCHSTRATEGY_H

#include <algorithm>
#include <chrono>
#include "../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "../Conjecture/Conjecture.h"
#include "DynamicKernel.h"
#include "Flags.h"
#include "State.h"
#include <iomanip>

class SearchStrategy {
  protected:
	DynamicKernel* kernel;
	Flags* flags;
	Conjecture* conjecture;
    map<string,string> attributes; // Characteristics of the search. This is initialized in the constructor of the derived class.
    string propertyFilePath;
  public:
    SearchStrategy(DynamicKernel* dynamicKernel, Conjecture* conjecture, Flags* flags);
    SearchStrategy();
    const string &getPropertyFilePath() const;
    void setPropertyFilePath(const string &propertyFilePath);
    void addAttribute(string x, string y);
    bool isAttribute(string x, string y);
    string getAttributeValue(string x);// Returns "y" if (x,y) belongs to attributes.
    map<string,string> getAttributes();
    virtual void search();

};
using SearchStrategy_creator_t =  SearchStrategy *(*)();
using SearchStrategy_creator_t_parameter =  SearchStrategy *(*)(DynamicKernel*, Conjecture*,Flags*);


#endif
