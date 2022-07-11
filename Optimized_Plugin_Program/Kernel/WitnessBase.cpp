// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "WitnessBase.h"

WitnessBase::~WitnessBase() {}

bool WitnessBase::is_equal(const WitnessBase &rhs) const { return true; }

bool WitnessBase::is_less(const WitnessBase &rhs) const { return true; }

WitnessBase &WitnessBase::set_equal(const WitnessBase &witness) {
  return *this;
}

bool operator==(const WitnessBase &lhs, const WitnessBase &rhs) {
  return lhs.is_equal(rhs);
}

bool WitnessBase::operator!=(const WitnessBase &rhs) const {
  return !((*this) == rhs);
}

bool operator<(const WitnessBase &lhs, const WitnessBase &rhs) {
  return lhs.is_less(rhs);
}

WitnessBase &WitnessBase::operator=(WitnessBase &rhs) {
  return this->set_equal(rhs);
}

void WitnessBase::print() const {
  std::cout
      << "It you are seeing this, it is because the print function was not "
         "yet implemented in the derived class."
      << std::endl;
}

std::shared_ptr<WitnessBase> WitnessBase::relabel(
    const std::map<unsigned int, unsigned int> &relabelingMap) const {
  std::cout
      << "It you are seeing this, it is because the relabel function was not "
         "yet implemented in the derived class."
      << std::endl;
  exit(20);
}

std::string WitnessBase::witnessInformation() const {
  std::cout << "It you are seeing this, it is because the witnessInformation "
               "function was not "
               "yet implemented in the derived class."
            << std::endl;
  exit(20);
}

void WitnessBase::hash(Hasher &h) const {
  std::cout
      << "It you are seeing this, it is because the hash function was not "
         "yet implemented in the derived class."
      << std::endl;
}
