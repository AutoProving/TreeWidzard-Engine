#include "BitSet.h"
#include <iostream>

BitSet::BitSet() { this->bitSetSize = 0; }

BitSet::BitSet(int initialSize) {
	this->bitSetSize = 0;
	this->array.push_back(0);
	for (int i = 0; i < initialSize; i++) this->push_back(false);
}

unsigned int BitSet::getBlock(unsigned int x) const { return x >> 5; }

unsigned int BitSet::getRem(unsigned int x) const { return x & 31; }

bool BitSet::at(unsigned int i) const {
	unsigned int blk = this->getBlock(i);
	unsigned int pos = this->getRem(i);
	return this->array[blk] & (1 << pos);
}

void BitSet::set(bool x, unsigned int i) {
	unsigned int blk = this->getBlock(i);
	unsigned int pos = this->getRem(i);
	this->array[blk] |= (1 << pos);
	if (x == false) this->array[blk] ^= (1 << pos);
}

void BitSet::push_back(bool value) {
	if ((this->bitSetSize & 31) == 0) this->array.push_back(0);
	unsigned int blk = this->getBlock(this->bitSetSize);
	unsigned int pos = this->getRem(this->bitSetSize);
	this->set(value, this->bitSetSize);
	this->bitSetSize++;
}

void BitSet::pop_back() { this->bitSetSize--; }

unsigned int BitSet::size() const { return this->bitSetSize; }

void BitSet::print() const {
	for (int i = 0; i < this->size(); i++) std::cout << this->at(i) << ' ';
	std::cout << std::endl;
}
