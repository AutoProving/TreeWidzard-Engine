// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_CORE_H
#define DYNAMIC_CORE_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "../Multigraph/MultiGraph.h"
#include "Bag.h"
#include "Witness.h"
#include "WitnessSet.h"

class DynamicCore {
 private:
  WitnessSetBasePointer initialWitnessSet;
  std::map<std::string, std::string>
      attributes;  // Characteristics of the core. This is initialized in the
                   // constructor of the derived class.
  int width;

 public:
  DynamicCore() { initialWitnessSet = std::make_shared<WitnessSetBase>(); }
  virtual ~DynamicCore() = default;
  WitnessSetBasePointer getInitialSet();  // returns initialSet
  void setInitialWitnessSet(WitnessSetBasePointer witnessSetPointer);
  void addAttribute(std::string x, std::string y);
  bool isAttribute(std::string x, std::string y);
  std::string getAttributeValue(
      std::string x);  // Returns "y" if (x,y) belongs to attributes.
  std::map<std::string, std::string> getAttributes();
  void insertIntoInitialWitnessSet(WitnessPointer);
  int getWidth();
  void setWidth(int width);
  virtual void createInitialWitnessSet();
  virtual WitnessSetBasePointer intro_v(unsigned i, Bag &b,
                                        const Witness &witness);
  virtual WitnessSetBasePointer intro_e(unsigned i, unsigned j, Bag &b,
                                        const Witness &witness);
  virtual WitnessSetBasePointer forget_v(unsigned i, Bag &b,
                                         const Witness &witness);
  virtual WitnessSetBasePointer join(Bag &b, const Witness &witness1,
                                     const Witness &witness2);
  virtual bool is_final_witness(const Witness &witness);
  virtual WitnessSetBasePointer clean(WitnessSetBasePointer witnessSet);
  virtual int weight(const Witness &witness);
  WitnessSetBasePointer intro_v(unsigned i, Bag &b,
                                WitnessSetBasePointer witnessSet);
  WitnessSetBasePointer intro_e(unsigned i, unsigned j, Bag &b,
                                WitnessSetBasePointer witnessSet);
  WitnessSetBasePointer forget_v(unsigned i, Bag &b,
                                 WitnessSetBasePointer witnessSet);
  WitnessSetBasePointer join(Bag &b, WitnessSetBasePointer witnessSet1,
                             WitnessSetBasePointer witnessSet2);
  bool is_final_set_witness(Bag &b, WitnessSetBasePointer witnessSet);
  int weight(WitnessSetBasePointer witnessSet);
};
using DynamicCore_creator_t = DynamicCore *(*)();
using DynamicCore_creator_t_int = DynamicCore *(*)(unsigned param);
using DynamicCore_creator_t_multiGraph =
    DynamicCore *(*)(MultiGraph multiGraph);
using DynamicCore_creator_t_parameters =
    DynamicCore *(*)(std::vector<int> parameters);

#endif
