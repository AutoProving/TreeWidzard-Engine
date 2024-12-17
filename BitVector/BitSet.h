#ifndef BITSET_H
#define BITSET_H

#include <cstdint>
#include <vector>

#define BITSET_SIZE_LOG 3
#define BITSET_TYPE uint8_t

class BitSet {
  private:
	unsigned int bitSetSize;
	std::vector<BITSET_TYPE> array;
	unsigned int getBlock(unsigned int x) const;
	unsigned int getRem(unsigned int x) const;

  public:
	BitSet();
	BitSet(int initialSize);
	unsigned int size() const;
	void push_back(bool value);
	void pop_back();
	bool at(unsigned int i) const;
	void set(bool x, unsigned int i);
	void print() const;
};

#endif // END of BITSET.h
