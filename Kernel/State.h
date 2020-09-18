// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef STATE_H
#define STATE_H
#include "Bag.h"
#include <vector>
#include "WitnessSet.h"
#include <cassert>

using namespace std;
class State: public std::enable_shared_from_this<State>
{
    private:
        Bag bag;
        std::vector<shared_ptr<WitnessSet>> witnessSetVector;

    public:
        Bag get_bag() const;
        void set_bag(const Bag &bag);
		void addWitnessSet(shared_ptr<WitnessSet>);
        bool operator<(State &rhs);
        bool operator==(State &rhs);
        size_t operator()(const State &b)const;
        void print();

		shared_ptr<WitnessSet> getWitnessSet(int);
		int numberOfComponents() const;
		
};

typedef shared_ptr<State> StatePointer;

struct compareLessState {
    bool operator() (const StatePointer lhs, const StatePointer rhs) const {
        return *lhs < *rhs;
    }
};
struct compareEqualityState {
    bool operator() (const StatePointer lhs, const StatePointer rhs) const {
        return *lhs == *rhs;
    }
};

//class StatePointer {
//	private:
//		shared_ptr<State> pointer;
//
//	public:
//
//		StatePointer(State* pointer_) : pointer(pointer_) {}
//
//		State operator*() {
//			return *pointer;
//		}
//
//		bool operator<(StatePointer& rhs) {
//			return *pointer < *rhs
//		}
//		bool operator==(StatePointer& rhs) {
//			return *pointer < *rhs
//		}
//		void operator =(StatePointer rhs) {
//			pointer = rhs;
//		}
//};

#endif
