// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef FLAG_H
#define FLAG_H

#include <iostream>
#include <map>
#include <string>
using namespace std;

class Flags {
  public:
	Flags();
	int get(string);
	void add_flag(string, int);

  private:
	map<string, int> content;
};

#endif
