// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef WIDTH_H
#define WIDTH_H

#include <iostream>
#include <vector>
using namespace std;

class Width {
  private:
	string name; // Typical width names are "treewidth", "pathwidth", etc
	unsigned int value;
  public:
	string get_name();
	void set_name(string s);
	unsigned int get_value() const;
	void set_value(unsigned int value);
	void print();
	Width& operator=(Width& rhs);

    bool operator==(const Width &rhs) const;

    bool operator!=(const Width &rhs) const;

    bool operator<(const Width &rhs) const;

    bool operator>(const Width &rhs) const;

    bool operator<=(const Width &rhs) const;

    bool operator>=(const Width &rhs) const;
};

#endif
