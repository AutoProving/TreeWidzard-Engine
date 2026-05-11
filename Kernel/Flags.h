// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef FLAG_H
#define FLAG_H

#include <iostream>
#include <map>
#include <string>

class Flags {
  public:
	Flags();
	float get(std::string);
	void add_flag(std::string, float);

  private:
	std::map<std::string, float> content;
};

#endif
