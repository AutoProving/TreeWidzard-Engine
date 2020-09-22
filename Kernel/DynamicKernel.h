// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_KERNEL_H
#define DYNAMIC_KERNEL_H
#include <list>
#include <stack>
#include <unordered_map>
#include "DynamicCore.h"
#include "PropertyAssignment.h"
#include "State.h"
#include "Width.h"

using namespace std;

class DynamicKernel : public std::enable_shared_from_this<DynamicKernel> {
  private:
	Width width;
	vector<PropertyAssignment> properties;
	vector<DynamicCore *> cores;

  public:
	void addCore(DynamicCore &core);
	Width &get_width();
	void set_width(Width &width);
	void addProperty(PropertyAssignment &);
	const vector<PropertyAssignment> &get_properties() const;
	StatePointer initialState();
	StatePointer intro_v(StatePointer q, unsigned i);
	StatePointer intro_e(StatePointer q, const unsigned i, const unsigned j);
	StatePointer forget_v(StatePointer q, unsigned i);
	StatePointer join(StatePointer q1, StatePointer q2);
	DynamicCore *pointerToCoreNumber(unsigned i);
};

#endif
