#ifndef DYNAMIC_CORE_H
#define DYNAMIC_CORE_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "../Multigraph/MultiGraph.h"
#include "Bag.h"
#include "Witness.h"
#include "WitnessSet.h"

template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
using parameterType = std::vector<std::variant<char *, int, bool>>;

class DynamicCore {
  private:
	WitnessSetPointer initialWitnessSet;
	std::map<std::string, std::string>
		attributes; // Characteristics of the core. This is initialized in the
					// constructor of the derived class.
	int width;

  public:
	DynamicCore() {}
	virtual ~DynamicCore() = default;
	WitnessSetPointer getInitialSet(); // returns initialSet
	void setInitialWitnessSet(WitnessSetPointer witnessSetPointer);
	void addAttribute(std::string x, std::string y);
	bool isAttribute(std::string x, std::string y);
	std::string getAttributeValue(
		std::string x); // Returns "y" if (x,y) belongs to attributes.
	std::map<std::string, std::string> getAttributes();
	int getWidth();
	void setWidth(int width);

	// Making the methods pure virtual,
	// meaning one can not construct a DynamicCore
	// or a descendant of DynamicCore without overriding
	// these methods
	virtual void createInitialWitnessSet() = 0;
  /*
	virtual WitnessSetPointer intro_v(unsigned i, const Bag &b,
									  const Witness &witness) = 0;
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, const Bag &b,
									  const Witness &witness) = 0;
	virtual WitnessSetPointer forget_v(unsigned i, const Bag &b,
									   const Witness &witness) = 0;
	virtual WitnessSetPointer join(const Bag &b, const Witness &witness1,
								   const Witness &witness2) = 0;
	virtual bool is_final_witness(const Bag &b, const Witness &witness) = 0;
  */
	virtual WitnessSetPointer clean(WitnessSetPointer witnessSet);
	virtual int inv(const Bag &b, const WitnessSetPointer witnessSet) = 0;

	virtual WitnessSetPointer intro_v(unsigned i, const Bag &b,
							  WitnessSetPointer witnessSet) = 0;
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, const Bag &b,
							  WitnessSetPointer witnessSet) = 0;
	virtual WitnessSetPointer forget_v(unsigned i, const Bag &b,
							   WitnessSetPointer witnessSet) = 0;
	virtual WitnessSetPointer join(const Bag &b, WitnessSetPointer witnessSet1,
						   WitnessSetPointer witnessSet2) = 0;
	virtual bool is_final_witness_set(const Bag &b, WitnessSetPointer witnessSet) = 0;
};

#endif
