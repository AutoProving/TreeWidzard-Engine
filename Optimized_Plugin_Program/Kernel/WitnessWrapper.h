#ifndef WITNESS_WRAPPER_H
#define WITNESS_WRAPPER_H

#include <memory>

#include "Witness.h"

template <class WitnessType>
class WitnessWrapper : public Witness {
 private:
  using WitnessAlias = WitnessType;

  friend WitnessAlias;

  WitnessWrapper() {}

  WitnessAlias &as_witness() { return *(WitnessAlias *)this; }

  const WitnessAlias &as_witness() const { return *(WitnessAlias *)this; }

 protected:
  bool is_equal(const Witness &rhs) const override {
    return is_equal_implementation(this->as_witness(), as_witness(rhs));
  }

  bool is_less(const Witness &rhs) const override {
    return is_less_implementation(this->as_witness(), as_witness(rhs));
  }

 public:
  static WitnessAlias &as_witness(Witness &witnessBase) {
#ifdef ENABLE_DEBUG_INFO
    if (WitnessAlias *witness = dynamic_cast<WitnessAlias *>(&witnessBase)) {
      return *witness;
    } else {
      std::cerr << "ERROR: in WitnessWrapper cast error\n";
      exit(20);
    }
#else
    return *(WitnessAlias *)&witnessBase;
#endif
  }

  static const WitnessAlias &as_witness(const Witness &witnessBase) {
#ifdef ENABLE_DEBUG_INFO
    if (WitnessAlias const *witness =
            dynamic_cast<WitnessAlias const *>(&witnessBase)) {
      return *witness;
    } else {
      std::cerr << "ERROR: in WitnessWrapper cast error\n";
      exit(20);
    }
#else
    return *(WitnessAlias *)&witnessBase;
#endif
  }

  std::shared_ptr<WitnessAlias> clone() const {
    return std::make_shared<WitnessAlias>(this->as_witness());
  }

  std::shared_ptr<Witness> relabel(
      const std::map<unsigned, unsigned> &relabelingMap) const override {
    return std::make_shared<WitnessAlias>(
        ((WitnessAlias *)this)->relabel_implementation(relabelingMap));
  }
};

#endif
