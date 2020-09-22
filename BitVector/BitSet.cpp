#include "BitSet.h"
#include <iostream>

Bitset::Bitset() { this->bitsetSize = 0; }

Bitset::Bitset(int initialSize) {
	this->bitsetSize = 0;
	this->array.push_back(0);
	for (int i = 0; i < initialSize; i++) this->push_back(false);
}

unsigned int Bitset::getBlock(unsigned int x) const { return x >> 5; }

unsigned int Bitset::getRem(unsigned int x) const { return x & 31; }

bool Bitset::at(unsigned int i) const {
	unsigned int blk = this->getBlock(i);
	unsigned int pos = this->getRem(i);
	return this->array[blk] & (1 << pos);
}

void Bitset::set(bool x, unsigned int i) {
	unsigned int blk = this->getBlock(i);
	unsigned int pos = this->getRem(i);
	this->array[blk] |= (1 << pos);
	if (x == false) this->array[blk] ^= (1 << pos);
}

void Bitset::push_back(bool value) {
	if ((this->bitsetSize & 31) == 0) this->array.push_back(0);
	unsigned int blk = this->getBlock(this->bitsetSize);
	unsigned int pos = this->getRem(this->bitsetSize);
	this->set(value, this->bitsetSize);
	this->bitsetSize++;
}

void Bitset::pop_back() { this->bitsetSize--; }

unsigned int Bitset::size() const { return this->bitsetSize; }

void Bitset::print() const {
	for (int i = 0; i < this->size(); i++) std::cout << this->at(i) << ' ';
	std::cout << std::endl;
}
