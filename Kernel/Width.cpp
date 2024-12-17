#include "Width.h"

std::string Width::get_name() { return name; }

void Width::set_name(std::string s) { name = s; }

unsigned int Width::get_value() const { return value; }

void Width::set_value(unsigned int value) { Width::value = value; }

void Width::print() { std::cout << name << " " << value << std::endl; }

Width &Width::operator=(Width &rhs) {
	name = rhs.get_name();
	value = rhs.get_value();
	return *this;
}

bool Width::operator==(const Width &rhs) const {
    return name == rhs.name &&
           value == rhs.value;
}

bool Width::operator!=(const Width &rhs) const {
    return !(rhs == *this);
}

bool Width::operator<(const Width &rhs) const {
    if (name < rhs.name)
        return true;
    if (rhs.name < name)
        return false;
    return value < rhs.value;
}

bool Width::operator>(const Width &rhs) const {
    return rhs < *this;
}

bool Width::operator<=(const Width &rhs) const {
    return !(rhs < *this);
}

bool Width::operator>=(const Width &rhs) const {
    return !(*this < rhs);
}
