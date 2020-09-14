// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef STATE_H
#define STATE_H
#include "Bag.h"
#include <vector>
#include "WitnessSet.h"

using namespace std;
class State: public std::enable_shared_from_this<State>
{
    private:
        Bag bag;
        std::vector<shared_ptr<WitnessSet>> witnessSetVector;
    public:
        Bag get_bag() const;
        void set_bag(const Bag &bag);
        const std::vector<shared_ptr<WitnessSet>> get_witnessSetVector() const ;
        void set_witnessSetVector(const std::vector<shared_ptr<WitnessSet>> &vector) ;
        bool operator<(const State &rhs) const;
        bool operator==(const State &rhs) const;
        size_t operator()(const State &b)const;
        void print();
};

#endif
