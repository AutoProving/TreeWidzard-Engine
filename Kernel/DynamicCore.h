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

using namespace std;
class DynamicCore {
  private:
	WitnessSetPointer initialWitnessSet;

  public:
	DynamicCore() { initialWitnessSet = WitnessSetPointer(new WitnessSet); }

	WitnessSetPointer getInitialSet(); // returns initialSet

	void insertIntoInitialWitnessSet(WitnessPointer);

	virtual void createInitialWitnessSet();
	virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b,
									  Witness &witness);
	virtual WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
	virtual WitnessSetPointer join(Bag &b, Witness &witness1,
								   Witness &witness2);
	virtual bool is_final_witness(Witness &witness);
	virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
	WitnessSetPointer intro_v(unsigned i, Bag &b, WitnessSet &witnessSet);
	WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b,
							  WitnessSet &witnessSet);
	WitnessSetPointer forget_v(unsigned i, Bag &b, WitnessSet &witnessSet);
	WitnessSetPointer join(Bag &b, WitnessSet &witnessSet1,
						   WitnessSet &witnessSet2);
	bool is_final_set_witness(Bag &b, WitnessSet &witnessSet);
};

#endif
