// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_KERNEL_H
#define DYNAMIC_KERNEL_H
#include <list>
#include <stack>
#include <unordered_map>
#include "DynamicCore.h"
#include "../Conjecture/PropertyAssignment.h"
#include "State.h"
#include "Width.h"

using namespace std;

class DynamicKernel : public std::enable_shared_from_this<DynamicKernel> {
private:
  Width width;
  //vector<PropertyAssignment> properties;
  vector<DynamicCore *> cores;
  map<string ,pair<string,int> > varToNameAndIndex;

public:
  void addCore(DynamicCore& core);
  size_t coreSize();
  Width &get_width();
  void set_width(Width &width);

  const map<string, pair<string, int>> &getVarToNameAndIndex() const;

  void setVarToNameAndIndex(const map<string, pair<string, int>> &varToNameAndIndex);

  DynamicCore* getCoreByVar(string var);
  bool isVarExists(string var);
  int getIndexByVar(string var);
  //void addProperty(PropertyAssignment &);
  //const vector<PropertyAssignment> &get_properties() const;

  State::ptr initialState();
  State::ptr intro_v(State::ptr q, unsigned i);
  State::ptr intro_e(State::ptr q, const unsigned i, const unsigned j);
  State::ptr forget_v(State::ptr q, unsigned i);
  State::ptr join(State::ptr q1, State::ptr q2);
  DynamicCore *pointerToCoreNumber(unsigned i);
};

#endif
