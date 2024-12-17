#include "BitVector.h"
#include <iostream>

template <class Type>
LargeBitVector<Type>::LargeBitVector(unsigned int wordSize,
									 unsigned int maximumSize) {
	unsigned int wsBits = this->countNumberOfBits(wordSize);
	unsigned int mxBits = this->countNumberOfBits(maximumSize);

	unsigned int sz = (wsBits << 1) + (mxBits << 2) + 6;
	for (unsigned int i = 0; i < sz; i++) this->array.push_back(0);

	unsigned int pointer = 0;
	pointer = writeOnHeader(pointer, wordSize, wsBits);
	pointer = writeOnHeader(pointer, maximumSize, mxBits);
	pointer = writeOnHeader(pointer, 0, mxBits);
}

template <class Type>
void LargeBitVector<Type>::doubleMaximumSize() {
	int p = this->jumpSpace(1);

	this->array.push_back(0);
	this->array.push_back(0);

	for (int i = (int)this->array.size() - 1; i > p + 1; i--)
		this->array.set(this->array.at(i - 2), i);

	this->array.set(0, p);
	this->array.set(1, p + 1);

	p = this->jumpSpace(3);

	this->array.push_back(0);
	this->array.push_back(0);

	for (int i = (int)this->array.size() - 1; i >= p; i--)
		this->array.set(this->array.at(i - 2), i);

	this->array.set(0, p - 2);
	this->array.set(1, p - 1);
}

template <class Type>
unsigned int LargeBitVector<Type>::countNumberOfBits(unsigned int value) const {
	if (value == 0) return 1;
	unsigned int cnt = 0;
	while (value > 0) {
		value >>= 1;
		cnt++;
	}
	return cnt;
}

template <class Type>
unsigned int LargeBitVector<Type>::writeOnHeader(unsigned int pointer,
												 unsigned int value,
												 unsigned int b) {
	for (unsigned int i = 0; i < b; i++) {
		if (value & (1 << i)) {
			this->array.set(1, pointer);
			this->array.set(0, pointer + 1);
		} else {
			this->array.set(0, pointer);
			this->array.set(1, pointer + 1);
		}
		pointer += 2;
	}
	this->array.set(0, pointer);
	this->array.set(0, pointer + 1);
	pointer += 2;
	return pointer;
}

template <class Type>
unsigned int LargeBitVector<Type>::readFromHeader(unsigned int pointer) const {
	unsigned int value = 0, k = 0;
	while (this->array.at(pointer) or this->array.at(pointer + 1)) {
		if (this->array.at(pointer) == 1 and this->array.at(pointer + 1) == 0)
			value += (1 << k);
		pointer += 2;
		k++;
	}
	return value;
}

template <class Type>
void LargeBitVector<Type>::print() {
	for (size_t i = 0; i < this->array.size(); i++)
		std::cout << this->array.at(i);
	std::cout << std::endl;
}

template <class Type>
unsigned int LargeBitVector<Type>::jumpSpace(unsigned int n) const {
	unsigned int pointer = 0;
	unsigned int cnt = 0;
	while (cnt < n) {
		if (this->array.at(pointer) == 0 and this->array.at(pointer + 1) == 0)
			cnt++;
		pointer += 2;
	}
	return pointer;
}

template <class Type>
unsigned int LargeBitVector<Type>::getWordSize() const {
	return this->readFromHeader(this->jumpSpace(0));
}

template <class Type>
unsigned int LargeBitVector<Type>::getMaxSize() const {
	return this->readFromHeader(this->jumpSpace(1));
}

template <class Type>
unsigned int LargeBitVector<Type>::size() const {
	return this->readFromHeader(this->jumpSpace(2));
}

template <class Type>
unsigned int LargeBitVector<Type>::getFirstBit() const {
	return this->jumpSpace(3);
}

template <class Type>
void LargeBitVector<Type>::incrementSize() {
	unsigned int newSize = this->size() + 1;
	if (newSize == this->getMaxSize()) this->doubleMaximumSize();
	this->writeOnHeader(this->jumpSpace(2), newSize,
						this->countNumberOfBits(this->getMaxSize()));
}

template <class Type>
void LargeBitVector<Type>::decrementSize() {
	unsigned int newSize = this->size() - 1;
	this->writeOnHeader(this->jumpSpace(2), newSize,
						this->countNumberOfBits(this->getMaxSize()));
}

template <class Type>
BitSet LargeBitVector<Type>::toBits(int value) {
	unsigned int sz = this->getWordSize();
	BitSet v(sz);

	for (size_t i = 0; i < sz; i++) {
		if (value & (1 << i))
			v.set(1, i);
		else
			v.set(0, i);
	}
	return v;
}

template <class Type>
int LargeBitVector<Type>::fromBits(BitSet &v, int /*dummy*/) const {
	unsigned int sz = this->getWordSize();
	int value = 0;
	for (size_t i = 0; i < sz; i++)
		if (v.at(i) == 1) value += (1 << i);
	return value;
}

template <class Type>
BitSet LargeBitVector<Type>::toBits(std::pair<int, int> value) {
	unsigned int sz = this->getWordSize() >> 1;
	BitSet v(sz << 1);

	for (size_t i = 0; i < sz; i++) {
		if (value.first & (1 << i))
			v.set(1, i);
		else
			v.set(0, i);
	}
	for (size_t i = 0; i < sz; i++) {
		if (value.second & (1 << i))
			v.set(1, i + sz);
		else
			v.set(0, i + sz);
	}
	return v;
}

template <class Type>
std::pair<int, int> LargeBitVector<Type>::fromBits(
	BitSet &v, std::pair<int, int> /*dummy*/) const {
	std::pair<int, int> value = std::make_pair(0, 0);
	unsigned int sz = this->getWordSize() >> 1;
	for (size_t i = 0; i < sz; i++)
		if (v.at(i) == 1) value.first += (1 << i);
	for (size_t i = 0; i < sz; i++)
		if (v.at(i + sz) == 1) value.second += (1 << i);
	return value;
}

template <class Type>
void LargeBitVector<Type>::push_back(Type element) {
	BitSet v = toBits(element);
	for (size_t i = 0; i < v.size(); i++) this->array.push_back(v.at(i));
	this->incrementSize();
}

template <class Type>
Type LargeBitVector<Type>::at(int index) const {
	unsigned int b = this->getFirstBit() + index * this->getWordSize();

	BitSet v(this->getWordSize());
	unsigned int sz = b + this->getWordSize();
	for (size_t i = b, j = 0; i < sz; i++, j++) v.set(this->array.at(i), j);
	Type dummy;
	return this->fromBits(v, dummy);
}

template <class Type>
void LargeBitVector<Type>::set(Type element, int index) {
	unsigned int b = this->getFirstBit() + index * this->getWordSize();
	BitSet v = toBits(element);
	unsigned int sz = b + this->getWordSize();
	for (size_t i = b, j = 0; i < sz; i++, j++) this->array.set(v.at(j), i);
}

template <class Type>
void LargeBitVector<Type>::swap(unsigned int i, unsigned int j) {
	Type temp = this->at(i);
	this->set(this->at(j), i);
	this->set(temp, j);
}

template <class Type>
void LargeBitVector<Type>::quickSort(int l, int r) {
	int j = l - 1;

	int offset = rand() % (r - l + 1);

	this->swap(l + offset, r);

	for (int i = l; i < r; i++)
		if (this->at(i) <= this->at(r)) this->swap(++j, i);

	this->swap(j + 1, r);

	if (l < j) this->quickSort(l, j);
	if (j + 2 < r) this->quickSort(j + 2, r);
}

template <class Type>
void LargeBitVector<Type>::sort(bool ascendingOrder) {
	int sz = this->size();
	if (sz <= 1) return;
	this->quickSort(0, sz - 1);
	if (ascendingOrder == false) {
		int sz_aux = sz >> 1;
		for (int i = 0; i < sz_aux; i++) this->swap(i, sz - i - 1);
	}
}

template <class Type>
Type LargeBitVector<Type>::back() {
	int sz = this->size();
	return this->at(sz - 1);
}

template <class Type>
void LargeBitVector<Type>::remove(unsigned int index) {
	int b = this->getFirstBit() + index * this->getWordSize();
	int e = this->getFirstBit() + (this->size() - 1) * this->getWordSize();
	for (int i = b; i < e; i++)
		this->array.set(this->array.at(i + this->getWordSize()), i);
	this->decrementSize();
	for (unsigned int i = 0; i < this->getWordSize(); i++)
		this->array.pop_back();
}

template <class Type>
void LargeBitVector<Type>::pop_back() {
	this->remove(this->size() - 1);
}

template <class Type>
LargeBitVector<Type> LargeBitVector<Type>::unionSets(
	LargeBitVector<Type> &aux) {
	// this->sort(true);
	// aux.sort(true);

	unsigned int ws = std::max(this->getWordSize(), aux.getWordSize());

	LargeBitVector<Type> uniSets(ws, this->size() + aux.size());

	int l = 0, r = 0;
	int sz_l = this->size();
	int sz_r = aux.size();

	while (l < sz_l or r < sz_r) {
		if (l == sz_l) {
			if (uniSets.back() != aux.at(r)) uniSets.push_back(aux.at(r));
			r++;
		} else if (r == sz_r) {
			if (uniSets.back() != this->at(l)) uniSets.push_back(this->at(l));
			l++;
		} else if (this->at(l) < aux.at(r)) {
			if (uniSets.back() != this->at(l)) uniSets.push_back(this->at(l));
			l++;
		} else {
			if (uniSets.back() != aux.at(r)) uniSets.push_back(aux.at(r));
			r++;
		}
	}
	return uniSets;
}

template <class Type>
bool LargeBitVector<Type>::binarySearch(Type value) {
	int lef = 0, rig = (int)this->size() - 1;
	while (lef <= rig) {
		int middle = (lef + rig) >> 1;
		if (this->at(middle) == value)
			return true;
		else if (this->at(middle) < value)
			lef = middle + 1;
		else
			rig = middle - 1;
	}
	return false;
}

template <class Type>
bool LargeBitVector<Type>::find(Type value) {
	for (unsigned int i = 0; i < this->size(); i++)
		if (this->at(i) == value) return true;
	return false;
}

template <class Type>
LargeBitVector<Type> LargeBitVector<Type>::intersectionSets(
	LargeBitVector<Type> &aux) {
	// this->sort(true);
	// aux.sort(true);

	unsigned int ws = std::max(this->getWordSize(), aux.getWordSize());
	LargeBitVector<Type> interSets(ws, this->size() + aux.size());

	int sz = aux.size();
	for (int i = 0; i < sz; i++) {
		if (this->binarySearch(aux.at(i)) == false) continue;
		if (interSets.size() == 0 or interSets.back() != aux.at(i))
			interSets.push_back(aux.at(i));
	}
	return interSets;
}

template <class Type>
void LargeBitVector<Type>::insertSorted(Type value) {
	unsigned int k = this->size();
	for (unsigned int i = 0; i < this->size(); i++)
		if (value <= this->at(i)) {
			k = i;
			break;
		}
	for (unsigned int i = 0; i < this->getWordSize(); i++)
		this->array.push_back(0);

	this->incrementSize();

	int b = this->getFirstBit() + k * this->getWordSize();
	int e = this->getFirstBit() + (this->size() - 1) * this->getWordSize();

	for (int i = e - 1; i >= b; i--)
		this->array.set(this->array.at(i), i + this->getWordSize());

	BitSet v = this->toBits(value);

	unsigned int sz = b + this->getWordSize();
	for (unsigned int i = b, j = 0; i < sz; i++, j++)
		this->array.set(v.at(j), i);
}

template <class Type>
bool LargeBitVector<Type>::operator==(const LargeBitVector<Type> &aux) const {
	if (this->size() != aux.size()) return false;
	// this->sort(true);
	// aux.sort(true);
	for (unsigned int i = 0; i < this->size(); i++)
		if (this->at(i) != aux.at(i)) return false;
	return true;
}

template <class Type>
bool LargeBitVector<Type>::operator<(const LargeBitVector<Type> &aux) const {
	int sz = std::min(this->size(), aux.size());
	// this->sort(true);
	// aux.sort(true);

	for (int i = 0; i < sz; i++)
		if (this->at(i) < aux.at(i))
			return true;
		else if (this->at(i) > aux.at(i))
			return false;
	return this->size() < aux.size();
}

template <class Type>
void LargeBitVector<Type>::normalize() {
	LargeBitVector<Type> aux(this->getWordSize(), 1);
	this->sort(true);
	for (unsigned int i = 0; i < this->size(); i++)
		if (aux.size() == 0 or aux.back() != this->at(i))
			aux.push_back(this->at(i));
	this->array = aux.array;
}

template <class Type>
LargeBitVector<Type> LargeBitVector<Type>::differenceSets(
	LargeBitVector<Type> &aux) {
	this->sort(true);
	aux.sort(true);
	LargeBitVector<Type> diffSets(this->getWordSize(), 1);
	for (unsigned int i = 0; i < this->size(); i++)
		if (aux.binarySearch(this->at(i)) == false)
			diffSets.push_back(this->at(i));
	return diffSets;
}
