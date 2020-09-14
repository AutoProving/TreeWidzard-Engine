// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef WITNESSSET_H
#define WITNESSSET_H

#include "Witness.h"
#include <set>
#include <memory>
struct compare{
    bool operator()(const shared_ptr<Witness> lhs, const shared_ptr<Witness> rhs) const{
        return *lhs<*rhs;
    }
};
class WitnessSet {
    public:
    std::set<shared_ptr<Witness>,compare> container;
    const set<shared_ptr<Witness>,compare> &get_container() const;
    void set_container(const set<shared_ptr<Witness>,compare> &witnessSet);
    void union_set_witness(shared_ptr<WitnessSet> witnessSet);
    void print();
    bool operator==(WitnessSet &rhs);
    bool operator!=(WitnessSet &rhs);
    bool operator<(WitnessSet &rhs);

};

#endif
