// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef WITNESS_H
#define WITNESS_H
#include <algorithm>
#include <iostream>
#include <memory>
#include <map>
#include "Hasher.hpp"

class WitnessBase {
protected:
    virtual bool is_equal(const WitnessBase &rhs) const;
    virtual bool is_less(const WitnessBase &rhs) const;
    virtual WitnessBase &set_equal(const WitnessBase &witness);
public:
    virtual std::shared_ptr<WitnessBase> relabel(const std::map<unsigned,unsigned> &relabelingMap) const;
    virtual void print() const;
    virtual std::string witnessInformation() const;
    virtual ~WitnessBase();
    friend bool operator==(const WitnessBase &lhs, const WitnessBase &rhs);
    friend bool operator<(const WitnessBase &lhs, const WitnessBase &rhs);
    virtual WitnessBase &operator=(WitnessBase &rhs);
    bool operator!=(const WitnessBase &rhs) const;
    virtual void hash(Hasher &h) const;
    struct IsLessSharedPtr{
        bool operator()(const std::shared_ptr<const WitnessBase> &lhs,
                        const std::shared_ptr<const WitnessBase> &rhs) const {
            return *lhs < *rhs;
        }
    };
};

typedef std::shared_ptr<WitnessBase> WitnessBasePointer;
typedef std::shared_ptr<WitnessBase const> WitnessBasePointerConst;

#endif
