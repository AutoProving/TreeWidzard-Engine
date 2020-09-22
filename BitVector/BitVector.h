#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include <utility>
#include <vector>
#include "BitSet.h"

template <class Type>
class LargeBitVector {
  private:
	BitSet array;

	unsigned int writeOnHeader(unsigned int pointer, unsigned int value,
							   unsigned int b);
	unsigned int readFromHeader(unsigned int pointer) const;
	unsigned int countNumberOfBits(unsigned int value) const;
	unsigned int jumpSpace(unsigned int n) const;
	unsigned int getFirstBit() const;
	void incrementSize();
	void decrementSize();

	void doubleMaximumSize();

	void quickSort(int l, int r);

	BitSet toBits(int value);
	int fromBits(BitSet &v, int dummy) const;

	BitSet toBits(std::pair<int, int> value);
	std::pair<int, int> fromBits(BitSet &v, std::pair<int, int> dummy) const;

  public:
	class iterator {
	  private:
		int it;
		LargeBitVector *bitVector;

	  public:
		iterator(LargeBitVector *bitVector_, int it_)
			: bitVector(bitVector_), it(it_) {}
		iterator &operator++() {
			it++;
			return *this;
		}
		iterator operator++(int) {
			it++;
			return *this;
		}
		bool operator!=(iterator rhs) { return it != rhs.it; }
		Type operator*() { return bitVector->at(it); }
	};

	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, size()); }

	LargeBitVector(unsigned int wordSize, unsigned int maximumSize);
	unsigned int getMaxSize() const;
	unsigned int size() const;
	unsigned int getWordSize() const;
	void insertSorted(Type value);
	void push_back(Type value);
	Type at(int index) const;
	void set(Type value, int index);
	void swap(unsigned int i, unsigned int j);
	void sort(bool ascendingOrder);
	Type back();
	bool binarySearch(Type value);
	bool find(Type value);
	void remove(unsigned int i);
	void pop_back();
	void print();
	LargeBitVector<Type> unionSets(LargeBitVector<Type> &aux);
	LargeBitVector<Type> intersectionSets(LargeBitVector<Type> &aux);
	LargeBitVector<Type> differenceSets(LargeBitVector<Type> &aux);
	bool operator==(const LargeBitVector<Type> &aux) const;
	bool operator<(const LargeBitVector<Type> &aux) const;
	void normalize();
};

template class LargeBitVector<int>;
template class LargeBitVector<std::pair<int, int>>;

#endif // END of bitvector.h
