#include "Witness.h"

Witness::~Witness() {}

bool operator==(const Witness &lhs, const Witness &rhs) {
	return lhs.is_equal(rhs);
}

bool Witness::operator!=(const Witness &rhs) const { return !((*this) == rhs); }

bool operator<(const Witness &lhs, const Witness &rhs) {
	return lhs.is_less(rhs);
}

void Witness::print() const {
	std::cout
		<< "It you are seeing this, it is because the print function was not "
		   "yet implemented in the derived class."
		<< std::endl;
}

std::shared_ptr<Witness> Witness::relabel(
	const std::map<unsigned int, unsigned int> &) const {
	std::cout
		<< "It you are seeing this, it is because the relabel function was not "
		   "yet implemented in the derived class."
		<< std::endl;
	exit(20);
}

std::string Witness::witnessInformation() const {
	std::cout << "It you are seeing this, it is because the witnessInformation "
				 "function was not "
				 "yet implemented in the derived class."
			  << std::endl;
	exit(20);
}

void Witness::hash(Hasher &) const {
	std::cout
		<< "It you are seeing this, it is because the hash function was not "
		   "yet implemented in the derived class."
		<< std::endl;
}
