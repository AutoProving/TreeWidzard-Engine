// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef WITNESSSET_H
#define WITNESSSET_H

#include <memory>
#include <set>
#include "Witness.h"

class WitnessSet { // data structure to store 'shared_ptr<Witness>'
  private:
	struct compare {
		bool operator()(const shared_ptr<Witness> lhs,
						const shared_ptr<Witness> rhs) const {
			return *lhs < *rhs;
		}
	};
	std::set<shared_ptr<Witness>, compare> container;

  public:
	class iterator {
	  private:
		std::set<shared_ptr<Witness>>::iterator it;

	  public:
		iterator(std::set<shared_ptr<Witness>>::iterator it_) : it(it_) {}

		iterator &operator++() {
			it++;
			return *this;
		}
		iterator operator++(int) {
			it++;
			return *this;
		}
		bool operator!=(iterator rhs) { return it != rhs.it; }
		shared_ptr<Witness> operator*() { return *it; }
	};

	iterator begin() { return iterator(container.begin()); }
	iterator end() { return iterator(container.end()); }

	void insert(shared_ptr<Witness> w);
	void union_set_witness(shared_ptr<WitnessSet> witnessSet);
	void print();
	bool operator==(WitnessSet &rhs);
	bool operator!=(WitnessSet &rhs);
	bool operator<(WitnessSet &rhs);
	int size();
};

typedef shared_ptr<WitnessSet> WitnessSetPointer;

#endif
