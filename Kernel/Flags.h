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
