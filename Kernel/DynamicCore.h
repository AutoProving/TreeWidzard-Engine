// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_CORE_H
#define DYNAMIC_CORE_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "Bag.h"
#include "Witness.h"
#include "WitnessSet.h"
#include "../Multigraph/MultiGraph.h"

using namespace std;
class DynamicCore {
  private:
	WitnessSetPointer initialWitnessSet;
    map<string,string> attributes; // Characteristics of the core. This is initialized in the constructor of the derived class.
    int width;
  public:
	DynamicCore() { initialWitnessSet = WitnessSetPointer(new WitnessSet); }
    virtual ~DynamicCore()= default;
	WitnessSetPointer getInitialSet(); // returns initialSet
	void setInitialWitnessSet(WitnessSetPointer witnessSetPointer);
    void addAttribute(string x, string y);
    bool isAttribute(string x, string y);
    string getAttributeValue(string x);// Returns "y" if (x,y) belongs to attributes.
    map<string,string> getAttributes();
	void insertIntoInitialWitnessSet(WitnessPointer);
	int getWidth();
	void setWidth(int width);
	virtual void createInitialWitnessSet();
	virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b,
									  Witness &witness);
	virtual WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
	virtual WitnessSetPointer join(Bag &b, Witness &witness1,
								   Witness &witness2);
	virtual bool is_final_witness(Witness &witness);
	virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
	virtual int weight(Witness &witness);
	WitnessSetPointer intro_v(unsigned i, Bag &b, WitnessSetPointer witnessSet);
	WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b,WitnessSetPointer witnessSet);
	WitnessSetPointer forget_v(unsigned i, Bag &b, WitnessSetPointer witnessSet);
	WitnessSetPointer join(Bag &b, WitnessSetPointer witnessSet1,WitnessSetPointer witnessSet2);
	bool is_final_set_witness(Bag &b, WitnessSetPointer witnessSet);
	int weight(WitnessSetPointer witnessSet);
};
using DynamicCore_creator_t =  DynamicCore *(*)();
using DynamicCore_creator_t_int =  DynamicCore *(*)(unsigned param);
using DynamicCore_creator_t_multiGraph =  DynamicCore *(*)(MultiGraph multiGraph);
using DynamicCore_creator_t_parameters =  DynamicCore *(*)(std::vector<int> parameters);

#endif
