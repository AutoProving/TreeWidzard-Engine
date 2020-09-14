// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef WITNESS_H
#define WITNESS_H

#include <iostream>
#include <algorithm>
using namespace std;
class Witness {
    protected:
        virtual bool is_equal(const Witness &rhs) const;
        virtual bool is_less(const Witness &rhs) const;
        virtual Witness& set_equal(Witness &witness);
    public:
        virtual void print();
        virtual ~Witness();
        friend bool operator==(const Witness &lhs,const Witness &rhs);
        friend bool operator<(const Witness &lhs, const Witness &rhs);
        virtual Witness& operator=(Witness &rhs);
        bool operator!=(Witness &rhs);
};

#endif
