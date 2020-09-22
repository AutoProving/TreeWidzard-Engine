// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef SEARCHSTRATEGY_H
#define SEARCHSTRATEGY_H

#include <algorithm>
#include <chrono>
#include "ConcreteTreeDecomposition.h"
#include "Conjecture.h"
#include "DynamicKernel.h"
#include "Flags.h"
#include "State.h"

class SearchStrategy {
  protected:
	DynamicKernel* kernel;
	Flags* flags;
	Conjecture* conjecture;

  public:
	SearchStrategy(DynamicKernel* dynamicKernel, Conjecture* conjecture,
				   Flags* flags);
	virtual pair<bool, ConcreteTreeDecomposition>
	search(); // Returns <0,emptytree> if no counter example is found. Returns
			  // <1,counterexampletree> if a counter example found.
};

#endif
