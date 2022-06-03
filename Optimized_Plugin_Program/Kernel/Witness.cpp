// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Witness.h"

Witness::~Witness() {}

bool Witness::is_equal(const Witness &rhs) const { return true; }

bool Witness::is_less(const Witness &rhs) const { return true; }

Witness &Witness::set_equal(const Witness &witness) { return *this; }

bool operator==(const Witness &lhs, const Witness &rhs) {
    return lhs.is_equal(rhs);
}

bool Witness::operator!=(const Witness &rhs) const { return !((*this) == rhs); }

bool operator<(const Witness &lhs, const Witness &rhs) {
    return lhs.is_less(rhs);
}

Witness &Witness::operator=(Witness &rhs) { return this->set_equal(rhs); }

void Witness::print() const {
    std::cout << "It you are seeing this, it is because the print function was not "
            "yet implemented in the derived class."
         << std::endl;
}

std::shared_ptr<Witness> Witness::relabel(std::map<unsigned int, unsigned int> relabelingMap) const {
    std::cout << "It you are seeing this, it is because the relabel function was not "
            "yet implemented in the derived class."
         << std::endl;
    exit(20);
}

std::string Witness::witnessInformation() const {
    std::cout << "It you are seeing this, it is because the witnessInformation function was not "
            "yet implemented in the derived class."
         << std::endl;
    exit(20);
}
