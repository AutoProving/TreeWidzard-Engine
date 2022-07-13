// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "DynamicCore.h"

WitnessSetBasePointer DynamicCore::getInitialSet() { return initialWitnessSet; }

void DynamicCore::insertIntoInitialWitnessSet(WitnessPointer w) {
  initialWitnessSet->insert(w);
}

// TODO: Is it better to pass the set down
// instead of taking union of many small?

WitnessSetBasePointer DynamicCore::intro_v(unsigned i, Bag &b,
                                           WitnessSetBasePointer witnessSet) {
  WitnessSetBasePointer aux =
      witnessSet->createEmptyWitnessSet();  // aux initializes with empty set
  for (WitnessPointerConst temp : *witnessSet) {
    aux->union_set_witness(intro_v(i, b, *temp));
  }
  return clean(aux);
}

WitnessSetBasePointer DynamicCore::intro_e(unsigned i, unsigned j, Bag &b,
                                           WitnessSetBasePointer witnessSet) {
  WitnessSetBasePointer aux =
      witnessSet->createEmptyWitnessSet();  // aux initializes with empty set
  WitnessSetBase t = *aux;
  for (WitnessPointerConst temp : *witnessSet) {
    aux->union_set_witness(intro_e(i, j, b, *temp));
  }

  return clean(aux);
}

WitnessSetBasePointer DynamicCore::forget_v(unsigned i, Bag &b,
                                            WitnessSetBasePointer witnessSet) {
  WitnessSetBasePointer aux =
      witnessSet->createEmptyWitnessSet();  // aux initializes with empty set
  for (WitnessPointerConst temp : *witnessSet) {
    aux->union_set_witness(forget_v(i, b, *temp));
  }
  return clean(aux);
}

WitnessSetBasePointer DynamicCore::join(Bag &b,
                                        WitnessSetBasePointer witnessSet1,
                                        WitnessSetBasePointer witnessSet2) {
  WitnessSetBasePointer aux = witnessSet1->createEmptyWitnessSet();
  for (WitnessPointerConst temp1 : *witnessSet1) {
    for (WitnessPointerConst temp2 : *witnessSet2) {
      aux->union_set_witness(join(b, *temp1, *temp2));
    }
  }
  return clean(aux);
}

bool DynamicCore::is_final_set_witness(Bag &b,
                                       WitnessSetBasePointer witnessSet) {
  bool flag = false;
  for (WitnessPointerConst temp : *witnessSet) {
    if (is_final_witness(*temp)) {
      return true;
    }
  }
  return flag;
}

WitnessSetBasePointer DynamicCore::clean(WitnessSetBasePointer witnessSet) {
  // By default, if this function is not implemented by a derived class
  // this function simply returns a pointer to the input witness set.
  std::cout << "generic clean is called" << std::endl;
  return witnessSet;
}

int DynamicCore::weight(WitnessSetBasePointer witnessSet) {
  std::set<int> values;
  for (auto temp : *witnessSet) {
    values.insert(weight(*temp));
  }
  std::string coreType = getAttributeValue("CoreType");
  if (coreType == "NULL" or coreType == "") {
    std::cout << "Error: DynamicCore::weight CoreType has not specified"
              << std::endl;
    exit(20);
  } else if (coreType == "Min" or coreType == "Max") {
    if (values.size() == 0) {
      return 0;
    } else {
      if (coreType == "Min") {
        return *values.begin();
      } else {
        return *values.rbegin();
      }
    }
  }
}

WitnessSetBasePointer DynamicCore::intro_v(unsigned i, Bag &b,
                                           const Witness &witness) {
  std::cout << "ERROR: DynamicCore::intro_v";
  exit(20);
}

WitnessSetBasePointer DynamicCore::intro_e(unsigned i, unsigned j, Bag &b,
                                           const Witness &witness) {
  std::cout << "ERROR: DynamicCore::intro_e";
  exit(20);
}

WitnessSetBasePointer DynamicCore::forget_v(unsigned i, Bag &b,
                                            const Witness &witness) {
  std::cout << "ERROR: DynamicCore::forget_v";
  exit(20);
}

WitnessSetBasePointer DynamicCore::join(Bag &b, const Witness &witness1,
                                        const Witness &witness2) {
  std::cout << "ERROR: DynamicCore::join";
  exit(20);
}

bool DynamicCore::is_final_witness(const Witness &witness) {
  std::cout << "ERROR: DynamicCore::is_final_witness";
  exit(20);
}

int DynamicCore::weight(const Witness &witness) {
  std::cout << "ERROR: DynamicCore::weight";
  exit(20);
}

void DynamicCore::createInitialWitnessSet() {
  std::cout << "ERROR: DynamicCore::createInitialWitnessSet";
  exit(20);
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

void DynamicCore::setInitialWitnessSet(
    WitnessSetBasePointer witnessSetPointer) {
  this->initialWitnessSet = witnessSetPointer;
}

int DynamicCore::getWidth() { return this->width; }

void DynamicCore::setWidth(int width) { this->width = width; }
