#ifndef WIDTH_H
#define WIDTH_H

#include <iostream>
#include <vector>

class Width {
  private:
	std::string name; // Typical width names are "treewidth", "pathwidth", etc
	unsigned int value=0;
  public:
	std::string get_name();
	void set_name(std::string s);
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
