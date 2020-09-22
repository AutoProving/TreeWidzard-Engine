#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include <utility>
#include <vector>
#include "BitSet.h"

template <class Type>
class LargeBitVector {
  private:
	Bitset array;

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

	Bitset toBits(int value);
	int fromBits(Bitset &v, int dummy) const;

	Bitset toBits(std::pair<int, int> value);
	std::pair<int, int> fromBits(Bitset &v, std::pair<int, int> dummy) const;

  public:
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
