// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "BreadthFirstTraversal.h"

#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace TreeWidzard {

namespace {

struct BagSignature {
  std::vector<unsigned> elements;

  bool operator==(const BagSignature &rhs) const {
    return elements == rhs.elements;
  }
};

struct BagSignatureHash {
  auto operator()(const BagSignature &signature) const -> std::size_t {
    Hasher h(0);
    h << static_cast<unsigned>(signature.elements.size());
    for (unsigned element : signature.elements) {
      h << element;
    }
    return h.get();
  }
};

auto make_bag_signature(const std::set<unsigned> &elements) -> BagSignature {
  BagSignature signature;
  signature.elements.assign(elements.begin(), elements.end());
  return signature;
}

auto make_bag_signature(const Bag &bag) -> BagSignature {
  return make_bag_signature(bag.get_elements());
}

void update_max_witness_sizes(const State::ptr &state,
                              std::vector<unsigned> &max_witness_sizes) {
  for (size_t component = 0; component < max_witness_sizes.size(); ++component) {
    max_witness_sizes[component] = std::max(
        max_witness_sizes[component],
        static_cast<unsigned>(state->getWitnessSet(static_cast<int>(component))
                                  ->size()));
  }
}

auto should_evaluate_state(const State::ptr &state,
                           Conjecture &conjecture,
                           bool premise_flag) -> bool {
  return !premise_flag || conjecture.evaluatePremiseOnState(*state);
}

void maybe_add_state(const State::ptr &state,
                     Conjecture &conjecture,
                     bool premise_flag,
                     std::unordered_set<State::ptr, State::ptr::Hash,
                                        State::ptr::Equal> &all_states,
                     std::unordered_set<State::ptr, State::ptr::Hash,
                                        State::ptr::Equal> &new_states_set,
                     std::vector<State::ptr> &new_states,
                     std::vector<unsigned> &max_witness_sizes,
                     const BreadthFirstTraversalHooks &hooks,
                     const BreadthFirstExpansionEvent &event) {
  bool satisfies_premise = false;
  if (premise_flag) {
    satisfies_premise = conjecture.evaluatePremiseOnState(*state);
  }

  if (premise_flag && !satisfies_premise) {
    return;
  }
  if (all_states.find(state) != all_states.end()) {
    return;
  }

  if (!new_states_set.insert(state).second) {
    return;
  }
  new_states.push_back(state);
  update_max_witness_sizes(state, max_witness_sizes);
  if (hooks.on_state_discovered) {
    hooks.on_state_discovered(event);
  }
}

} // namespace

auto runBreadthFirstTraversal(DynamicKernel &kernel,
                              Conjecture &conjecture,
                              Flags &flags,
                              const BreadthFirstTraversalHooks &hooks)
    -> BreadthFirstTraversalResult {
  BreadthFirstTraversalResult result;

  std::unordered_set<State::ptr, State::ptr::Hash, State::ptr::Equal>
      all_states;
  std::unordered_set<State::ptr, State::ptr::Hash, State::ptr::Equal>
      new_states_set;
  std::vector<State::ptr> new_states;
  State::ptr initial_state = kernel.initialState();
  const bool enable_joins = kernel.get_width().get_name() == "tree_width";
  std::unordered_map<BagSignature, std::vector<State::ptr>, BagSignatureHash>
      states_by_bag;
  // Track which (state, existing) pairs have already been joined to avoid redundant work
  std::set<std::pair<const State*, const State*>> joined_pairs;

  all_states.insert(initial_state);
  new_states_set.insert(initial_state);
  new_states.push_back(initial_state);
  if (enable_joins) {
    const auto initial_bag_signature =
        make_bag_signature(initial_state->get_bag());
    states_by_bag[initial_bag_signature].push_back(initial_state);
  }
  result.max_witness_sizes.resize(initial_state->numberOfComponents());
  update_max_witness_sizes(initial_state, result.max_witness_sizes);
  if (hooks.on_initial_state) {
    hooks.on_initial_state(initial_state);
  }

  const bool premise_flag = static_cast<bool>(flags.get("Premise"));
  const unsigned width = static_cast<unsigned>(kernel.get_width().get_value());

  if (should_evaluate_state(initial_state, conjecture, premise_flag) &&
      !conjecture.evaluateConjectureOnState(*initial_state)) {
    result.property_satisfied = false;
    result.counterexample_state = initial_state;
    result.total_states = 1;
    return result;
  }

  while (!new_states.empty()) {
    ++result.iterations;
    std::vector<State::ptr> frontier = std::move(new_states);
    new_states_set.clear();
    new_states.clear();

    for (const auto &state : frontier) {
      const Bag &bag = state->get_bag();
      const auto &elements = bag.get_elements();

      for (unsigned vertex = 1; vertex <= width + 1; ++vertex) {
        if (!bag.vertex_introducible(vertex)) {
          continue;
        }

        State::ptr new_state = kernel.intro_v(state, vertex);
        maybe_add_state(new_state, conjecture, premise_flag, all_states,
                        new_states_set, new_states, result.max_witness_sizes,
                        hooks,
                        {BreadthFirstExpansionKind::IntroVertex,
                         new_state,
                         state,
                         std::nullopt,
                         vertex,
                         0});
      }

      for (const unsigned vertex : elements) {
        State::ptr new_state = kernel.forget_v(state, vertex);
        maybe_add_state(new_state, conjecture, premise_flag, all_states,
                        new_states_set, new_states, result.max_witness_sizes,
                        hooks,
                        {BreadthFirstExpansionKind::ForgetVertex,
                         new_state,
                         state,
                         std::nullopt,
                         vertex,
                         0});
      }

      if (elements.size() > 1) {
        for (auto left = elements.begin(); left != elements.end(); ++left) {
          auto right = left;
          ++right;
          for (; right != elements.end(); ++right) {
            State::ptr new_state = kernel.intro_e(state, *left, *right);
            maybe_add_state(new_state, conjecture, premise_flag, all_states,
                            new_states_set, new_states, result.max_witness_sizes,
                            hooks,
                            {BreadthFirstExpansionKind::IntroEdge,
                             new_state,
                             state,
                             std::nullopt,
                             *left,
                             *right});
          }
        }
      }

      if (enable_joins) {
        const BagSignature state_signature = make_bag_signature(elements);
        auto bucket_it = states_by_bag.find(state_signature);
        if (bucket_it != states_by_bag.end()) {
          for (const auto &existing_state : bucket_it->second) {
            // Skip if this pair was already joined
            auto pair_key = std::make_pair(
                &*state < &*existing_state ? &*state : &*existing_state,
                &*state < &*existing_state ? &*existing_state : &*state);
            if (!joined_pairs.insert(pair_key).second) continue;

            State::ptr new_state = kernel.join(state, existing_state);
            maybe_add_state(new_state, conjecture, premise_flag, all_states,
                            new_states_set, new_states,
                            result.max_witness_sizes, hooks,
                            {BreadthFirstExpansionKind::Join,
                             new_state,
                             state,
                             existing_state,
                             0,
                             0});
          }
        }
      }
    }

    for (const auto &state : new_states) {
      if (!conjecture.evaluateConjectureOnState(*state)) {
        result.property_satisfied = false;
        result.counterexample_state = state;
        result.total_states = all_states.size() + new_states.size();
        return result;
      }
    }

    for (const auto &state : new_states) {
      if (all_states.insert(state).second) {
        if (enable_joins) {
          const auto bag_signature = make_bag_signature(state->get_bag());
          states_by_bag[bag_signature].push_back(state);
        }
      }
    }
    if (hooks.on_iteration_complete) {
      hooks.on_iteration_complete(result.iterations, all_states.size(),
                                  new_states.size(), result.max_witness_sizes);
    }
  }

  result.total_states = all_states.size();
  return result;
}

} // namespace TreeWidzard
