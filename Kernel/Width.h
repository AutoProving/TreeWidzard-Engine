// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef WIDTH_H
#define WIDTH_H

#include <iostream>
#include <vector>

class Width {
  private:
	std::string name; // Typical width names are "treewidth", "pathwidth", etc
	unsigned int value = 0;

  public:
	Width() {}
	Width(const Width &other) : name(other.name), value(other.value) {}

	std::string get_name() const;
	void set_name(std::string s);
	unsigned int get_value() const;
	void set_value(unsigned int value);
	void print();
	Width &operator=(Width &rhs);

	bool operator==(const Width &rhs) const;

	bool operator!=(const Width &rhs) const;

	bool operator<(const Width &rhs) const;

	bool operator>(const Width &rhs) const;

	bool operator<=(const Width &rhs) const;

	bool operator>=(const Width &rhs) const;
};

#endif
