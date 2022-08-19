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
	float get(string);
	void add_flag(string, float);

  private:
	map<string, float> content;
};

#endif
