#ifndef BITSET_H
#define BITSET_H_H

#include <vector>

class Bitset {
  private:
	unsigned int bitsetSize;
	std::vector<unsigned int> array;
	unsigned int getBlock(unsigned int x) const;
	unsigned int getRem(unsigned int x) const;

  public:
	Bitset();
	Bitset(int initialSize);
	unsigned int size() const;
	void push_back(bool value);
	void pop_back();
	bool at(unsigned int i) const;
	void set(bool x, unsigned int i);
	void print() const;
};

#endif // END of BITSET.h