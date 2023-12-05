#include <cassert>
#include <limits>
#include <stdexcept>
#include "DynamicCore.h"

WitnessSetPointer DynamicCore::getInitialSet() { return initialWitnessSet; }

WitnessSetPointer DynamicCore::clean(WitnessSetPointer witnessSet) {
	// By default, if this function is not implemented by a derived class
	// this function simply returns a pointer to the input witness set.
	// std::cout << "generic clean is called" << std::endl;
	return witnessSet;
}

/*
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
*/

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
