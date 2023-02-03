#ifndef CORE_WRAPPER_H
#define CORE_WRAPPER_H

#include "DynamicCore.h"
#include "WitnessWrapper.h"

template <class Core, class WitnessType, template <class> class WitnessSetType>
class CoreWrapper : public DynamicCore {
 private:
  using WitnessAlias = WitnessType;
  using WitnessSet = WitnessSetType<WitnessAlias>;

  friend Core;

  Core &as_core() { return *(Core *)this; }

 public:
  CoreWrapper() {
    addAttribute("CoreName", Core::CoreName);
    addAttribute("CoreType", Core::CoreType);
    addAttribute("ParameterType", Core::ParameterType);
    addAttribute("PrimaryOperator", Core::PrimaryOperator);
    createInitialWitnessSet();
  }

  void createInitialWitnessSet() override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().initialize_leaf(*witnessSet);
    setInitialWitnessSet(witnessSet);
  }

  WitnessSetPointer intro_v(unsigned i, Bag &b,
                            const Witness &witness) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().intro_v_implementation(
        i, b, WitnessAlias::as_witness(witness), *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b,
                            const Witness &witness) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().intro_e_implementation(
        i, j, b, WitnessAlias::as_witness(witness), *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetPointer forget_v(unsigned i, Bag &b,
                             const Witness &witness) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().forget_v_implementation(
        i, b, WitnessAlias::as_witness(witness), *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetPointer join(Bag &b, const Witness &w1,
                         const Witness &w2) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().join_implementation(b, WitnessAlias::as_witness(w1),
                                        WitnessAlias::as_witness(w2),
                                        *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetPointer clean(WitnessSetPointer witnessSetBase) override {
#ifdef ENABLE_DEBUG_INFO
    if (WitnessSet *witnessSet = dynamic_cast<WitnessSet *>(&*witnessSetBase)) {
      this->as_core().clean_implementation(*witnessSet);
      return witnessSetBase;
    } else {
      std::cerr << "ERROR: in CoreWrapper::clean cast error\n";
      exit(20);
    }
#else
    this->as_core().clean_implementation(*(WitnessSet *)&*witnessSetBase);
    return witnessSetBase;
#endif
  }

  bool is_final_witness(const Witness &witness) override {
    return this->as_core().is_final_witness_implementation(
        WitnessAlias::as_witness(witness));
  }

  int weight(const Witness &witness) override {
    return this->as_core().weight_implementation(WitnessAlias::as_witness(witness));
  }
};

#endif
