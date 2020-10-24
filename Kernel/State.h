// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef STATE_H
#define STATE_H
#include <cassert>
#include <vector>
#include "Bag.h"
#include "WitnessSet.h"

using namespace std;
class State : private std::enable_shared_from_this<State> {
  private:
	Bag bag;
	std::vector<shared_ptr<WitnessSet>> witnessSetVector;
  public:
	class ptr {
	  private:
		shared_ptr<State> pointer;
	  public:
		ptr() { pointer = make_shared<State>(); }
		ptr(shared_ptr<State> pointer_) : pointer(pointer_) {}
		State &operator*() const { return *pointer; }
		State *operator->() const { return &*pointer; }
		bool operator<(const ptr &rhs) const { return **this < *rhs; }
	};
	ptr get_ptr() { return ptr(this->shared_from_this()); }
	Bag get_bag() const;
	void set_bag(const Bag &bag);
	void addWitnessSet(shared_ptr<WitnessSet>);
	bool operator<(State &rhs);
	bool operator==(State &rhs);
	size_t operator()(const State &b) const;
	void print();
	shared_ptr<WitnessSet> getWitnessSet(int);
	int numberOfComponents() const;
};

#endif
