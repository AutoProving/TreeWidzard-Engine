// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef WITNESS_H
#define WITNESS_H
#include <algorithm>
#include <iostream>
#include <memory>
#include <map>

class Witness {
protected:
    virtual bool is_equal(const Witness &rhs) const;
    virtual bool is_less(const Witness &rhs) const;
    virtual Witness &set_equal(Witness &witness);
public:
    virtual std::shared_ptr<Witness> relabel(std::map<unsigned,unsigned> relabelingMap);
    virtual void print();
    virtual std::string witnessInformation();
    virtual ~Witness();
    friend bool operator==(const Witness &lhs, const Witness &rhs);
    friend bool operator<(const Witness &lhs, const Witness &rhs);
    virtual Witness &operator=(Witness &rhs);
    bool operator!=(Witness &rhs);
    struct IsLessSharedPtr{
        bool operator()(const std::shared_ptr<Witness> lhs,
                        const std::shared_ptr<Witness> rhs) const {
            return *lhs < *rhs;
        }
    };
};

typedef std::shared_ptr<Witness> WitnessPointer;

#endif
