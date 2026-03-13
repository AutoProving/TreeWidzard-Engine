#include "BreadthFirstTraversal.h"

#include <algorithm>
#include <set>
#include <vector>

namespace TreeWidzard {

namespace {

void update_max_witness_sizes(const State::ptr &state,
                              std::vector<unsigned> &max_witness_sizes) {
  for (size_t component = 0; component < max_witness_sizes.size(); ++component) {
    max_witness_sizes[component] = std::max(
        max_witness_sizes[component],
        static_cast<unsigned>(state->getWitnessSet(static_cast<int>(component))
                                  ->size()));
  }
}

void maybe_add_state(const State::ptr &state,
                     Conjecture &conjecture,
                     bool premise_flag,
                     std::set<State::ptr> &all_states,
                     std::set<State::ptr> &new_states,
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
  if (all_states.count(state) || new_states.count(state)) {
    return;
  }

  new_states.insert(state);
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

  std::set<State::ptr> all_states;
  std::set<State::ptr> new_states;
  State::ptr initial_state = kernel.initialState();

  all_states.insert(initial_state);
  new_states.insert(initial_state);
  result.max_witness_sizes.resize(initial_state->numberOfComponents());
  if (hooks.on_initial_state) {
    hooks.on_initial_state(initial_state);
  }

  const bool premise_flag = static_cast<bool>(flags.get("Premise"));
  const unsigned width = static_cast<unsigned>(kernel.get_width().get_value());

  while (!new_states.empty()) {
    ++result.iterations;
    const std::vector<State::ptr> frontier(new_states.begin(), new_states.end());
    new_states.clear();

    for (const auto &state : frontier) {
      const Bag bag = state->get_bag();
      const std::set<unsigned> elements = bag.get_elements();

      for (unsigned vertex = 1; vertex <= width + 1; ++vertex) {
        if (!bag.vertex_introducible(vertex)) {
          continue;
        }

        State::ptr new_state = kernel.intro_v(state, vertex);
        maybe_add_state(new_state, conjecture, premise_flag, all_states,
                        new_states, result.max_witness_sizes, hooks,
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
                        new_states, result.max_witness_sizes, hooks,
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
                            new_states, result.max_witness_sizes, hooks,
                            {BreadthFirstExpansionKind::IntroEdge,
                             new_state,
                             state,
                             std::nullopt,
                             *left,
                             *right});
          }
        }
      }

      if (kernel.get_width().get_name() == "tree_width") {
        for (const auto &existing_state : all_states) {
          if (!state->get_bag().joinable(existing_state->get_bag())) {
            continue;
          }

          State::ptr new_state = kernel.join(state, existing_state);
          maybe_add_state(new_state, conjecture, premise_flag, all_states,
                          new_states, result.max_witness_sizes, hooks,
                          {BreadthFirstExpansionKind::Join,
                           new_state,
                           state,
                           existing_state,
                           0,
                           0});
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

    all_states.insert(new_states.begin(), new_states.end());
    if (hooks.on_iteration_complete) {
      hooks.on_iteration_complete(result.iterations, all_states.size(),
                                  new_states.size(), result.max_witness_sizes);
    }
  }

  result.total_states = all_states.size();
  return result;
}

} // namespace TreeWidzard
