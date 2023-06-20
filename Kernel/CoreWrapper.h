#ifndef CORE_WRAPPER_H
#define CORE_WRAPPER_H

#include "DynamicCore.h"
#include "WitnessWrapper.h"
#include <cassert>

inline int as_core_counter = 1;

template <class Core, class WitnessType, template <class> class WitnessSetType>
class CoreWrapper : public DynamicCore {
  private:
	using WitnessAlias = WitnessType;
	using WitnessSet = WitnessSetType<WitnessAlias>;

	friend Core;

	Core &as_core() { return *static_cast<Core *>(this); }
  static WitnessSetType<WitnessAlias> &as_witness_set(::WitnessSet &witnessSetBase) {
#ifdef ENABLE_DEBUG_INFO
		if (WitnessAlias *witness =
				dynamic_cast<WitnessSetType<WitnessAlias> *>(&witnessSetBase)) {
			return *witness;
		} else {
			std::cerr << "ERROR: in CoreWrapper cast error\n";
			exit(20);
		}
#else
		return static_cast<WitnessSetType<WitnessAlias> &>(witnessSetBase);
#endif
	}

  public:
	CoreWrapper() {
		for (const auto &[k, v] : Core::metadata()) addAttribute(k, v);
	}

	void createInitialWitnessSet() override {
		auto witnessSet = std::make_shared<WitnessSet>();
		this->as_core().initialize_leaf(*witnessSet);
		setInitialWitnessSet(std::move(witnessSet));
	}

	WitnessSetPointer intro_v(unsigned i, const Bag &b,
							  WitnessSetPointer ws_ptr) override {
    const auto &witnessSet = as_witness_set(*ws_ptr);
		auto newWitnessSet = std::make_shared<WitnessSet>();

    for (const auto &w_ptr : witnessSet)
      this->as_core().intro_v_implementation(
        i, b, WitnessAlias::as_witness(*w_ptr), *newWitnessSet);

    this->as_core().clean_implementation(*newWitnessSet);

    return newWitnessSet;
	}

	WitnessSetPointer intro_e(unsigned i, unsigned j, const Bag &b,
							  WitnessSetPointer ws_ptr) override {
    const auto &witnessSet = as_witness_set(*ws_ptr);
    auto newWitnessSet = std::make_shared<WitnessSet>();

    for (const auto &w_ptr : witnessSet)
      this->as_core().intro_e_implementation(
        i, j, b, WitnessAlias::as_witness(*w_ptr), *newWitnessSet);

    this->as_core().clean_implementation(*newWitnessSet);
    return newWitnessSet;
	}

	WitnessSetPointer forget_v(unsigned i, const Bag &b,
							   WitnessSetPointer ws_ptr) override {
    const auto &witnessSet = as_witness_set(*ws_ptr);
    auto newWitnessSet = std::make_shared<WitnessSet>();

    for (const auto &w_ptr : witnessSet)
      this->as_core().forget_v_implementation(
        i, b, WitnessAlias::as_witness(*w_ptr), *newWitnessSet);

    this->as_core().clean_implementation(*newWitnessSet);
    return newWitnessSet;
	}

	WitnessSetPointer join(const Bag &b, const WitnessSetPointer ws_ptr1,
						   WitnessSetPointer ws_ptr2) override {
    const auto &ws1 = as_witness_set(*ws_ptr1);
    const auto &ws2 = as_witness_set(*ws_ptr2);
    auto newWitnessSet = std::make_shared<WitnessSet>();

    for (const auto &w_ptr1 : ws1)
      for (const auto &w_ptr2 : ws2)
        this->as_core().join_implementation(
          b, WitnessAlias::as_witness(*w_ptr1), WitnessAlias::as_witness(*w_ptr2),
          *newWitnessSet);

    this->as_core().clean_implementation(*newWitnessSet);
    return newWitnessSet;
	}

	WitnessSetPointer clean(WitnessSetPointer witnessSetBase) override {
#ifdef ENABLE_DEBUG_INFO
		if (WitnessSet *witnessSet =
				dynamic_cast<WitnessSet *>(&*witnessSetBase)) {
			this->as_core().clean_implementation(*witnessSet);
			return witnessSetBase;
		} else {
			std::cerr << "ERROR: in CoreWrapper::clean cast error\n";
			exit(20);
		}
#else
		this->as_core().clean_implementation(
			static_cast<WitnessSet &>(*witnessSetBase));
		return witnessSetBase;
#endif
	}

	bool is_final_witness_set(const Bag &bag, WitnessSetPointer ws_ptr) override {
    const auto &witnessSet = as_witness_set(*ws_ptr);

    for (const auto &w_ptr : witnessSet)
      if (this->as_core().is_final_witness_implementation(
            bag, WitnessAlias::as_witness(*w_ptr)))
        return true;

    return false;
	}

	int inv(const Bag &bag, WitnessSetPointer witnessSet) override {
    auto coreType = getAttributeValue("CoreType");

    if (coreType == "NULL" || coreType == "Max") {
      int max = 0;
      for (WitnessPointerConst temp : *witnessSet)
        max = std::max(max, this->as_core().weight_implementation(bag, WitnessAlias::as_witness(*temp)));
      return max;
    }

    if (coreType == "Min") {
      int min = std::numeric_limits<int>::max();
      for (WitnessPointerConst temp : *witnessSet)
        min = std::min(min, this->as_core().weight_implementation(bag, WitnessAlias::as_witness(*temp)));
      return min;
    }

    throw std::runtime_error("Unknown CoreType: " + coreType);
  }

	// This method will never be called through a reference to this base class.
	// It is only here to allow the derived class to use "override" to make
	// sure they got the signature right.
	virtual int weight_implementation(const Bag &bag,
									  const WitnessAlias &witness) {
		return this->as_core().is_final_witness_implementation(bag, witness);
	}
};

#endif
