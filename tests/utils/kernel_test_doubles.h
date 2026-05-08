// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef TREEWIDZARD_TESTS_UTILS_KERNEL_TEST_DOUBLES_H
#define TREEWIDZARD_TESTS_UTILS_KERNEL_TEST_DOUBLES_H

#include "Kernel/CoreWrapper.h"
#include "Kernel/SearchStrategy.h"
#include "Kernel/WitnessWrapper.h"

#include <cassert>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace TreeWidzardTest {

struct IntWitness : WitnessWrapper<IntWitness> {
  int value = 0;

  friend bool is_equal_implementation(const IntWitness &lhs,
                                      const IntWitness &rhs) {
    return lhs.value == rhs.value;
  }

  friend bool is_less_implementation(const IntWitness &lhs,
                                     const IntWitness &rhs) {
    return lhs.value < rhs.value;
  }

  auto relabel_implementation(const std::map<unsigned, unsigned> &) const
      -> IntWitness {
    return *this;
  }

  void hash(Hasher &h) const override { h << value; }

  void witness_info(std::ostream &os) const { os << value << '\n'; }
};

inline auto make_int_witness(int value) -> std::shared_ptr<IntWitness> {
  auto witness = std::make_shared<IntWitness>();
  witness->value = value;
  return witness;
}

template <template <class> class WitnessSetTemplate>
struct CountingCoreImpl
    : CoreWrapper<CountingCoreImpl<WitnessSetTemplate>, IntWitness,
                  WitnessSetTemplate> {
  static auto metadata() -> std::map<std::string, std::string> {
    return {
        {"CoreName", "CountingCore"},
        {"CoreType", "Max"},
        {"ParameterType", "None"},
    };
  }

  explicit CountingCoreImpl(const parameterType &parameters = {}) {
    (void)parameters;
    assert(parameters.empty());
  }

  void initialize_leaf(WitnessSet &witness_set) {
    witness_set.insert(make_int_witness(0));
  }

  void intro_v_implementation(unsigned, const Bag &, const IntWitness &witness,
                              WitnessSet &witness_set) {
    witness_set.insert(make_int_witness(witness.value + 1));
  }

  void intro_e_implementation(unsigned, unsigned, const Bag &,
                              const IntWitness &witness,
                              WitnessSet &witness_set) {
    witness_set.insert(make_int_witness(witness.value + 10));
  }

  void forget_v_implementation(unsigned, const Bag &, const IntWitness &witness,
                               WitnessSet &witness_set) {
    witness_set.insert(make_int_witness(witness.value));
  }

  void join_implementation(const Bag &, const IntWitness &lhs,
                           const IntWitness &rhs, WitnessSet &witness_set) {
    witness_set.insert(make_int_witness(lhs.value + rhs.value));
  }

  void clean_implementation(WitnessSet &) {}

  auto is_final_witness_implementation(const Bag &, const IntWitness &) -> bool {
    return true;
  }

  auto inv_implementation(const Bag &, const IntWitness &witness) -> int {
    return witness.value;
  }
};

using CountingCore = CountingCoreImpl<WitnessSetTypeTwo>;
using CountingCoreTypeOne = CountingCoreImpl<WitnessSetTypeOne>;

template <template <class> class WitnessSetTemplate>
struct BoundedCountingCoreImpl
    : CoreWrapper<BoundedCountingCoreImpl<WitnessSetTemplate>, IntWitness,
                  WitnessSetTemplate> {
  static auto metadata() -> std::map<std::string, std::string> {
    return {
        {"CoreName", "CountingCore"},
        {"CoreType", "Max"},
        {"ParameterType", "None"},
    };
  }

  explicit BoundedCountingCoreImpl(const parameterType &parameters = {}) {
    (void)parameters;
    assert(parameters.empty());
  }

  void initialize_leaf(WitnessSet &witness_set) {
    witness_set.insert(make_int_witness(0));
  }

  void intro_v_implementation(unsigned, const Bag &, const IntWitness &witness,
                              WitnessSet &witness_set) {
    witness_set.insert(make_int_witness((witness.value + 1) % 5));
  }

  void intro_e_implementation(unsigned, unsigned, const Bag &,
                              const IntWitness &witness,
                              WitnessSet &witness_set) {
    witness_set.insert(make_int_witness((witness.value + 2) % 5));
  }

  void forget_v_implementation(unsigned, const Bag &, const IntWitness &witness,
                               WitnessSet &witness_set) {
    witness_set.insert(make_int_witness(witness.value));
  }

  void join_implementation(const Bag &, const IntWitness &lhs,
                           const IntWitness &rhs, WitnessSet &witness_set) {
    witness_set.insert(make_int_witness((lhs.value + rhs.value) % 5));
  }

  void clean_implementation(WitnessSet &) {}

  auto is_final_witness_implementation(const Bag &, const IntWitness &) -> bool {
    return true;
  }

  auto inv_implementation(const Bag &, const IntWitness &witness) -> int {
    return witness.value;
  }
};

using BoundedCountingCore = BoundedCountingCoreImpl<WitnessSetTypeTwo>;

class PassthroughCore final : public DynamicCore {
public:
  explicit PassthroughCore(const parameterType &parameters = {}) {
    (void)parameters;
    assert(parameters.empty());
  }

  void createInitialWitnessSet() override {
    auto witness_set = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    witness_set->insert(make_int_witness(0));
    setInitialWitnessSet(witness_set);
  }

  auto intro_v(unsigned, const Bag &, const Witness &witness)
      -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    result->insert(
        make_int_witness(IntWitness::as_witness(witness).value + 1));
    return result;
  }

  auto intro_e(unsigned, unsigned, const Bag &, const Witness &witness)
      -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    result->insert(
        make_int_witness(IntWitness::as_witness(witness).value + 10));
    return result;
  }

  auto forget_v(unsigned, const Bag &, const Witness &witness)
      -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    result->insert(make_int_witness(IntWitness::as_witness(witness).value));
    return result;
  }

  auto join(const Bag &, const Witness &lhs, const Witness &rhs)
      -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    result->insert(make_int_witness(IntWitness::as_witness(lhs).value +
                                    IntWitness::as_witness(rhs).value));
    return result;
  }

  auto is_final_witness(const Bag &, const Witness &) -> bool override {
    return true;
  }

  auto inv(const Bag &, const WitnessSetPointer witness_set) -> int override {
    for (const auto &witness : *witness_set) {
      return IntWitness::as_witness(*witness).value;
    }
    return 0;
  }

  auto intro_v(unsigned i, const Bag &bag, WitnessSetPointer witness_set)
      -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    for (const auto &witness : *witness_set) {
      result->union_set_witness(intro_v(i, bag, *witness));
    }
    return result;
  }

  auto intro_e(unsigned i, unsigned j, const Bag &bag,
               WitnessSetPointer witness_set) -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    for (const auto &witness : *witness_set) {
      result->union_set_witness(intro_e(i, j, bag, *witness));
    }
    return result;
  }

  auto forget_v(unsigned i, const Bag &bag, WitnessSetPointer witness_set)
      -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    for (const auto &witness : *witness_set) {
      result->union_set_witness(forget_v(i, bag, *witness));
    }
    return result;
  }

  auto join(const Bag &bag, WitnessSetPointer lhs, WitnessSetPointer rhs)
      -> WitnessSetPointer override {
    auto result = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    for (const auto &left_witness : *lhs) {
      for (const auto &right_witness : *rhs) {
        result->union_set_witness(join(bag, *left_witness, *right_witness));
      }
    }
    return result;
  }

  auto is_final_witness_set(const Bag &, WitnessSetPointer) -> bool override {
    return true;
  }
};

class StubSearchStrategy final : public SearchStrategy {
public:
  using SearchStrategy::SearchStrategy;

  bool search_called = false;

  void search() override { search_called = true; }

  auto kernel_ptr() const -> DynamicKernel * { return kernel; }
  auto conjecture_ptr() const -> Conjecture * { return conjecture; }
  auto flags_ptr() const -> Flags * { return flags; }
};

inline auto make_bag(std::initializer_list<unsigned> elements) -> Bag {
  Bag bag;
  bag.set_elements(std::set<unsigned>(elements));
  return bag;
}

inline auto make_state(std::initializer_list<unsigned> bag_elements,
                       std::initializer_list<int> witness_values)
    -> State::ptr {
  State::ptr state;
  auto mutable_state = state.getMutableState();
  mutable_state->set_bag(make_bag(bag_elements));
  for (const int witness_value : witness_values) {
    auto witness_set = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
    witness_set->insert(make_int_witness(witness_value));
    mutable_state->addWitnessSet(witness_set);
  }
  return state;
}

inline auto witness_values(const std::shared_ptr<WitnessSet> &witness_set)
    -> std::vector<int> {
  std::vector<int> values;
  for (const auto &witness : *witness_set) {
    values.push_back(IntWitness::as_witness(*witness).value);
  }
  return values;
}

inline auto first_witness_value(const std::shared_ptr<WitnessSet> &witness_set)
    -> int {
  const auto values = witness_values(witness_set);
  assert(!values.empty());
  return values.front();
}

} // namespace TreeWidzardTest

#endif
