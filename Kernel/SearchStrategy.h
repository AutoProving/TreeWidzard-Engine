// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef SEARCHSTRATEGY_H
#define SEARCHSTRATEGY_H

#include "ConcreteTreeDecomposition.h"
#include "DynamicKernel.h"
#include "Flags.h"
#include "Conjecture.h"
#include <algorithm>
#include <chrono>
#include "State.h"

struct compare_less{
    bool operator() (const shared_ptr<State> lhs, const shared_ptr<State> rhs) const {
        return *lhs < *rhs;
    }
};
struct compare_equality{
    bool operator() (const shared_ptr<State> lhs, const shared_ptr<State> rhs) const {
        return *lhs == *rhs;
    }
};



class SearchStrategy{
protected:
    DynamicKernel* kernel;
    Flags* flags;
    Conjecture* conjecture;
public:
    SearchStrategy(DynamicKernel* dynamicKernel, Conjecture* conjecture, Flags* flags);
    virtual pair<bool,ConcreteTreeDecomposition> search();// Returns <0,emptytree> if no counter example is found. Returns <1,counterexampletree> if a counter example found.
};

#endif
