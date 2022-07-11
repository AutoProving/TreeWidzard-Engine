#ifndef WITNESS_WRAPPER_H
#define WITNESS_WRAPPER_H

#include <memory>

#include "WitnessBase.h"

template <class Witness>
class WitnessWrapper : public WitnessBase {
  friend Witness;

 private:
  WitnessWrapper() {}

  Witness &as_witness() { return *(Witness *)this; }

  const Witness &as_witness() const { return *(Witness *)this; }

 protected:
  bool is_equal(const WitnessBase &rhs) const override {
    return is_equal_implementation(this->as_witness(), as_witness(rhs));
  }

  bool is_less(const WitnessBase &rhs) const override {
    return is_less_implementation(this->as_witness(), as_witness(rhs));
  }

 public:
  static Witness &as_witness(WitnessBase &witnessBase) {
#ifdef ENABLE_DEBUG_INFO
    if (Witness *witness = dynamic_cast<Witness *>(&witnessBase)) {
      return *witness;
    } else {
      std::cerr << "ERROR: in WitnessWrapper cast error\n";
      exit(20);
    }
#else
    return *(Witness *)&witnessBase;
#endif
  }

  static const Witness &as_witness(const WitnessBase &witnessBase) {
#ifdef ENABLE_DEBUG_INFO
    if (Witness const *witness = dynamic_cast<Witness const *>(&witnessBase)) {
      return *witness;
    } else {
      std::cerr << "ERROR: in WitnessWrapper cast error\n";
      exit(20);
    }
#else
    return *(Witness *)&witnessBase;
#endif
  }

  std::shared_ptr<Witness> clone() const {
    return std::make_shared<Witness>(this->as_witness());
  }

  std::shared_ptr<WitnessBase> relabel(
      const std::map<unsigned, unsigned> &relabelingMap) const override {
    return std::make_shared<Witness>(
        ((Witness *)this)->relabel_implementation(relabelingMap));
  }
};

#endif
