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

class DynamicCore {
  private:
	WitnessSetPointer initialWitnessSet;
    std::map<std::string, std::string> attributes; // Characteristics of the core. This is initialized in the constructor of the derived class.
    int width;
  public:
	DynamicCore() { initialWitnessSet = WitnessSetPointer(new WitnessSet); }
    virtual ~DynamicCore()= default;
	WitnessSetPointer getInitialSet(); // returns initialSet
	void setInitialWitnessSet(WitnessSetPointer witnessSetPointer);
    void addAttribute(std::string x, std::string y);
    bool isAttribute(std::string x, std::string y);
    std::string getAttributeValue(std::string x);// Returns "y" if (x,y) belongs to attributes.
    std::map<std::string, std::string> getAttributes();
	void insertIntoInitialWitnessSet(WitnessPointer);
	int getWidth();
	void setWidth(int width);
	virtual void createInitialWitnessSet();
	virtual WitnessSetPointer intro_v(unsigned i, Bag &b, const Witness &witness);
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b,
									  const Witness &witness);
	virtual WitnessSetPointer forget_v(unsigned i, Bag &b, const Witness &witness);
	virtual WitnessSetPointer join(Bag &b, const Witness &witness1,
								   const Witness &witness2);
	virtual bool is_final_witness(const Witness &witness);
	virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
	virtual int weight(const Witness &witness);
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
