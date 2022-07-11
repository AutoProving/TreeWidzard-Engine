#ifndef CORE_WRAPPER_H
#define CORE_WRAPPER_H

#include "DynamicCore.h"
#include "WitnessWrapper.h"

template <class Core, class Witness, template <class> class WitnessSetType>
class CoreWrapper : public DynamicCore {
 private:
  Core &as_core() { return *(Core *)this; }

 protected:
  using WitnessSet = WitnessSetType<Witness>;

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

  WitnessSetBasePointer intro_v(unsigned i, Bag &b,
                                const WitnessBase &witness) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().intro_v_implementation(i, b, Witness::as_witness(witness),
                                           *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetBasePointer intro_e(unsigned i, unsigned j, Bag &b,
                                const WitnessBase &witness) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().intro_e_implementation(
        i, j, b, Witness::as_witness(witness), *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetBasePointer forget_v(unsigned i, Bag &b,
                                 const WitnessBase &witness) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().forget_v_implementation(i, b, Witness::as_witness(witness),
                                            *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetBasePointer join(Bag &b, const WitnessBase &w1,
                             const WitnessBase &w2) override {
    auto witnessSet = std::make_shared<WitnessSet>();
    this->as_core().join_implementation(b, Witness::as_witness(w1),
                                        Witness::as_witness(w2), *witnessSet);
    this->as_core().clean_implementation(*witnessSet);
    return witnessSet;
  }

  WitnessSetBasePointer clean(WitnessSetBasePointer witnessSetBase) override {
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

  bool is_final_witness(const WitnessBase &witness) override {
    return this->as_core().is_final_witness_implementation(
        Witness::as_witness(witness));
  }
};

#endif
