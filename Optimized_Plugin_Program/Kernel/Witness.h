// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef WITNESS_H
#define WITNESS_H
#include <algorithm>
#include <iostream>
#include <memory>
#include <map>
#include "Hasher.hpp"

class Witness {
protected:
    virtual bool is_equal(const Witness &rhs) const;
    virtual bool is_less(const Witness &rhs) const;
    virtual Witness &set_equal(const Witness &witness);
public:
    virtual std::shared_ptr<Witness> relabel(std::map<unsigned,unsigned> relabelingMap) const;
    virtual void print() const;
    virtual std::string witnessInformation() const;
    virtual ~Witness();
    friend bool operator==(const Witness &lhs, const Witness &rhs);
    friend bool operator<(const Witness &lhs, const Witness &rhs);
    virtual Witness &operator=(Witness &rhs);
    bool operator!=(const Witness &rhs) const;
    virtual void hash(Hasher &h) const;
    struct IsLessSharedPtr{
        bool operator()(const std::shared_ptr<const Witness> &lhs,
                        const std::shared_ptr<const Witness> &rhs) const {
            return *lhs < *rhs;
        }
    };
};

typedef std::shared_ptr<Witness> WitnessPointer;
typedef std::shared_ptr<Witness const> WitnessPointerConst;

#endif
