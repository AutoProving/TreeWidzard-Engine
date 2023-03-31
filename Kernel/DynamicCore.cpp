#include "DynamicCore.h"
#include <cassert>
#include <limits>
#include <stdexcept>

WitnessSetPointer DynamicCore::getInitialSet() { return initialWitnessSet; }

// TODO: Is it better to pass the set down
// instead of taking union of many small?

WitnessSetPointer DynamicCore::intro_v(unsigned i, const Bag &b,
									   WitnessSetPointer witnessSet) {
	WitnessSetPointer aux =
		witnessSet->createEmptyWitnessSet(); // aux initializes with empty set
	for (WitnessPointerConst temp : *witnessSet) {
		aux->union_set_witness(intro_v(i, b, *temp));
	}
	return clean(aux);
}

WitnessSetPointer DynamicCore::intro_e(unsigned i, unsigned j, const Bag &b,
									   WitnessSetPointer witnessSet) {
	assert(witnessSet);
	WitnessSetPointer aux =
		witnessSet->createEmptyWitnessSet(); // aux initializes with empty set
	for (WitnessPointerConst temp : *witnessSet) {
		aux->union_set_witness(intro_e(i, j, b, *temp));
	}
	return clean(aux);
}

WitnessSetPointer DynamicCore::forget_v(unsigned i, const Bag &b,
										WitnessSetPointer witnessSet) {
	assert(witnessSet);
	WitnessSetPointer aux =
		witnessSet->createEmptyWitnessSet(); // aux initializes with empty set
	for (WitnessPointerConst temp : *witnessSet) {
		aux->union_set_witness(forget_v(i, b, *temp));
	}
	return clean(aux);
}

WitnessSetPointer DynamicCore::join(const Bag &b, WitnessSetPointer witnessSet1,
									WitnessSetPointer witnessSet2) {
	assert(witnessSet1);
	assert(witnessSet2);
	WitnessSetPointer aux = witnessSet1->createEmptyWitnessSet();
	for (WitnessPointerConst temp1 : *witnessSet1) {
		for (WitnessPointerConst temp2 : *witnessSet2) {
			aux->union_set_witness(join(b, *temp1, *temp2));
		}
	}
	return clean(aux);
}

bool DynamicCore::is_final_witness_set(const Bag &bag,
									   WitnessSetPointer witnessSet) {
	assert(witnessSet);
	for (WitnessPointerConst temp : *witnessSet)
		if (is_final_witness(bag, *temp)) return true;
	return false;
}

WitnessSetPointer DynamicCore::clean(WitnessSetPointer witnessSet) {
	// By default, if this function is not implemented by a derived class
	// this function simply returns a pointer to the input witness set.
	// std::cout << "generic clean is called" << std::endl;
	return witnessSet;
}

int DynamicCore::inv(const Bag &bag, const WitnessSetPointer witnessSet) {
	// If CoreType is set to Max (or missing), return maximum weight in the set.
	// If CoreType is set to Min, return minimum weight in the set.
	//
	// Since the default behavior of weight is the same as is_final_witness_set,
	// omitting CoreType and keeping the default weight and inv implementations
	// results in inv behaving the same as is_final_witness_set.
	// (max of 0/1 is the same as or).

	auto it = attributes.find("CoreType");
	if (it == attributes.end() || it->second == "Max") {
		int max = 0;
		for (WitnessPointerConst temp : *witnessSet)
			max = std::max(max, weight(bag, *temp));
		return max;
	}
	if (it->second == "Min") {
		int min = std::numeric_limits<int>::max();
		for (WitnessPointerConst temp : *witnessSet)
			min = std::min(min, weight(bag, *temp));
		return min;
	}

	throw std::runtime_error("Unknown CoreType: " + it->second);
}

int DynamicCore::weight(const Bag &bag, const Witness &w) {
	return is_final_witness(bag, w);
}

void DynamicCore::addAttribute(std::string x, std::string y) {
	attributes.insert(make_pair(x, y));
}

bool DynamicCore::isAttribute(std::string x, std::string y) {
	auto it = attributes.find(x);
	if (it != attributes.end()) {
		if (it->second == y) {
			return true;
		}
	}
	return false;
}

std::string DynamicCore::getAttributeValue(std::string x) {
	auto it = attributes.find(x);
	if (it != attributes.end()) {
		return it->second;
	} else {
		return "NULL";
	}
}

std::map<std::string, std::string> DynamicCore::getAttributes() {
	return attributes;
}

void DynamicCore::setInitialWitnessSet(WitnessSetPointer witnessSetPointer) {
	this->initialWitnessSet = std::move(witnessSetPointer);
}

int DynamicCore::getWidth() { return this->width; }

void DynamicCore::setWidth(int new_width) { this->width = new_width; }
