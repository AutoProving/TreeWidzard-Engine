#ifndef TREEWIDZARD_BREADTH_FIRST_TRAVERSAL_H
#define TREEWIDZARD_BREADTH_FIRST_TRAVERSAL_H

#include "../Conjecture/Conjecture.h"
#include "DynamicKernel.h"
#include "Flags.h"

#include <functional>
#include <optional>
#include <vector>

namespace TreeWidzard {

enum class BreadthFirstExpansionKind {
  IntroVertex,
  ForgetVertex,
  IntroEdge,
  Join,
};

struct BreadthFirstExpansionEvent {
  BreadthFirstExpansionKind kind;
  State::ptr consequent_state;
  State::ptr first_parent;
  std::optional<State::ptr> second_parent;
  unsigned value = 0;
  unsigned secondary_value = 0;
};

struct BreadthFirstTraversalHooks {
  std::function<void(const State::ptr &)> on_initial_state;
  std::function<void(const BreadthFirstExpansionEvent &)> on_state_discovered;
  std::function<void(int iteration,
                     size_t all_states_size,
                     size_t new_states_size,
                     const std::vector<unsigned> &max_witness_sizes)>
      on_iteration_complete;
};

struct BreadthFirstTraversalResult {
  bool property_satisfied = true;
  std::optional<State::ptr> counterexample_state;
  std::vector<unsigned> max_witness_sizes;
  size_t total_states = 0;
  int iterations = 0;
};

auto runBreadthFirstTraversal(DynamicKernel &kernel,
                              Conjecture &conjecture,
                              Flags &flags,
                              const BreadthFirstTraversalHooks &hooks = {})
    -> BreadthFirstTraversalResult;

} // namespace TreeWidzard

#endif
