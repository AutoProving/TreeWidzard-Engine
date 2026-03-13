#include "Conjecture/Conjecture.h"
#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/CertificateUtils.h"
#include "Kernel/DynamicCoreHandler.h"
#include "Kernel/SearchStrategyHandler.h"
#include "tests/utils/kernel_test_doubles.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace {

using TreeWidzardTest::BoundedCountingCore;
using TreeWidzardTest::first_witness_value;

auto make_tree_width(unsigned value, const std::string &name = "tree_width")
    -> Width {
  Width width;
  width.set_name(name);
  width.set_value(value);
  return width;
}

class ConjectureHarness {
public:
  template <typename... Args> auto make(Args &&...args) -> ConjectureNode * {
    nodes_.push_back(
        std::make_unique<ConjectureNode>(std::forward<Args>(args)...));
    return nodes_.back().get();
  }

  static void attach(ConjectureNode *parent,
                     std::initializer_list<ConjectureNode *> children) {
    std::vector<ConjectureNode *> owned_children(children);
    parent->setChildren(owned_children);
    for (auto *child : owned_children) {
      child->setParent(parent);
    }
  }

private:
  std::vector<std::unique_ptr<ConjectureNode>> nodes_;
};

auto make_artifact_path(const fs::path &relative_path) -> fs::path {
  fs::path current = fs::current_path();
  for (int depth = 0; depth < 10; ++depth) {
    if (fs::exists(current / relative_path)) {
      return current / relative_path;
    }
    if (fs::exists(current / "build" / relative_path)) {
      return current / "build" / relative_path;
    }
    if (!current.has_parent_path()) {
      break;
    }
    current = current.parent_path();
  }

  throw std::runtime_error("could not locate build artifact: " +
                           relative_path.string());
}

void configure_kernel(DynamicKernel &kernel, BoundedCountingCore &core) {
  Width width = make_tree_width(2);
  kernel.set_width(width);
  kernel.addCore(core);
  kernel.setVarToNameAndIndex({{"x", {"CountingCore", 0}}});
}

auto make_always_true_conjecture(DynamicKernel &kernel, ConjectureHarness &h)
    -> Conjecture {
  Conjecture conjecture;
  conjecture.setKernel(&kernel);
  conjecture.setVariablesToCoreName({{"x", "CountingCore"}});
  conjecture.setRoot(h.make(NUMBER, 1.0));
  return conjecture;
}

auto make_inv_leq_conjecture(DynamicKernel &kernel, ConjectureHarness &h,
                             double limit) -> Conjecture {
  Conjecture conjecture;
  conjecture.setKernel(&kernel);
  conjecture.setVariablesToCoreName({{"x", "CountingCore"}});

  auto *variable = h.make(CORE_VARIABLE, "x");
  auto *inv = h.make(INV, "inv");
  auto *bound = h.make(NUMBER, limit);
  auto *root = h.make(OPERATOR, "<=");
  ConjectureHarness::attach(inv, {variable});
  ConjectureHarness::attach(root, {inv, bound});
  conjecture.setRoot(root);
  return conjecture;
}

auto make_premise_conjecture(DynamicKernel &kernel, ConjectureHarness &h)
    -> Conjecture {
  Conjecture conjecture;
  conjecture.setKernel(&kernel);
  conjecture.setVariablesToCoreName({{"x", "CountingCore"}});

  auto *variable = h.make(CORE_VARIABLE, "x");
  auto *inv = h.make(INV, "inv");
  auto *zero = h.make(NUMBER, 0.0);
  auto *premise = h.make(OPERATOR, "==");
  auto *conclusion = h.make(NUMBER, 1.0);
  auto *root = h.make(OPERATOR, "implies");

  ConjectureHarness::attach(inv, {variable});
  ConjectureHarness::attach(premise, {inv, zero});
  ConjectureHarness::attach(root, {premise, conclusion});
  conjecture.setRoot(root);
  return conjecture;
}

} // namespace

TEST(KernelTraversal, BreadthFirstTraversalCoversAllExpansionHooks) {
  DynamicKernel kernel;
  BoundedCountingCore core;
  configure_kernel(kernel, core);
  ConjectureHarness harness;
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  bool saw_initial = false;
  bool saw_intro_vertex = false;
  bool saw_forget_vertex = false;
  bool saw_intro_edge = false;
  bool saw_join = false;
  std::vector<std::string> callback_order;
  std::vector<size_t> iteration_sizes;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_initial_state = [&](const State::ptr &initial_state) {
    saw_initial = true;
    callback_order.push_back("initial");
    EXPECT_TRUE(initial_state->get_bag().get_elements().empty());
  };
  hooks.on_state_discovered = [&](const TreeWidzard::BreadthFirstExpansionEvent &event) {
    callback_order.push_back("discover");
    switch (event.kind) {
    case TreeWidzard::BreadthFirstExpansionKind::IntroVertex:
      saw_intro_vertex = true;
      break;
    case TreeWidzard::BreadthFirstExpansionKind::ForgetVertex:
      saw_forget_vertex = true;
      break;
    case TreeWidzard::BreadthFirstExpansionKind::IntroEdge:
      saw_intro_edge = true;
      break;
    case TreeWidzard::BreadthFirstExpansionKind::Join:
      saw_join = true;
      ASSERT_TRUE(event.second_parent.has_value());
      break;
    }
  };
  hooks.on_iteration_complete =
      [&](int iteration, size_t all_states_size, size_t new_states_size,
          const std::vector<unsigned> &max_witness_sizes) {
        callback_order.push_back("iteration");
        EXPECT_GE(iteration, 1);
        EXPECT_GE(all_states_size, new_states_size);
        ASSERT_EQ(max_witness_sizes.size(), 1U);
        iteration_sizes.push_back(all_states_size);
      };

  const auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conjecture,
                                                            flags, hooks);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_FALSE(result.counterexample_state.has_value());
  EXPECT_TRUE(saw_initial);
  EXPECT_TRUE(saw_intro_vertex);
  EXPECT_TRUE(saw_forget_vertex);
  EXPECT_TRUE(saw_intro_edge);
  EXPECT_TRUE(saw_join);
  EXPECT_GT(result.total_states, 1U);
  EXPECT_GT(result.iterations, 1);
  ASSERT_EQ(result.max_witness_sizes.size(), 1U);
  EXPECT_EQ(result.max_witness_sizes[0], 1U);
  ASSERT_FALSE(callback_order.empty());
  EXPECT_EQ(callback_order.front(), "initial");
  EXPECT_FALSE(iteration_sizes.empty());
}

TEST(KernelTraversal, BreadthFirstTraversalFiltersStatesByPremise) {
  DynamicKernel kernel;
  BoundedCountingCore core;
  configure_kernel(kernel, core);
  ConjectureHarness harness;
  Conjecture conjecture = make_premise_conjecture(kernel, harness);
  Flags flags;
  flags.add_flag("Premise", 1);

  size_t discovered = 0;
  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_state_discovered = [&](const TreeWidzard::BreadthFirstExpansionEvent &) {
    ++discovered;
  };

  const auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conjecture,
                                                            flags, hooks);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_FALSE(result.counterexample_state.has_value());
  EXPECT_EQ(result.total_states, 1U);
  EXPECT_EQ(result.iterations, 1);
  EXPECT_EQ(discovered, 0U);
}

TEST(KernelTraversal, BreadthFirstTraversalStopsAtCounterexample) {
  DynamicKernel kernel;
  BoundedCountingCore core;
  configure_kernel(kernel, core);
  ConjectureHarness harness;
  Conjecture conjecture = make_inv_leq_conjecture(kernel, harness, 0.0);
  Flags flags;

  const auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conjecture,
                                                            flags);

  EXPECT_FALSE(result.property_satisfied);
  ASSERT_TRUE(result.counterexample_state.has_value());
  EXPECT_EQ(result.iterations, 1);
  EXPECT_GE(result.total_states, 2U);
  EXPECT_EQ(result.counterexample_state.value()->get_bag().size(), 1U);
  EXPECT_EQ(first_witness_value(result.counterexample_state.value()->getWitnessSet(0)),
            1);
}

TEST(KernelTraversal, DynamicCoreHandlerLoadsCompiledCorePlugin) {
  const fs::path library_path = make_artifact_path(
      fs::path("DPCores") /
      ("VertexCount" TREEWIDZARD_DYNAMIC_LIB_EXTENSION));

  DynamicCoreHandler handler(library_path.string());
  const auto &metadata = handler.get_metadata();

  EXPECT_EQ(metadata.at("CoreName"), "VertexCount");
  EXPECT_EQ(metadata.at("ParameterType"), "None");

  auto core = handler.create({});
  ASSERT_NE(core, nullptr);
  EXPECT_EQ(core->getAttributeValue("CoreName"), "VertexCount");
  core->createInitialWitnessSet();
  ASSERT_NE(core->getInitialSet(), nullptr);
  EXPECT_EQ(core->getInitialSet()->size(), 1);
}

TEST(KernelTraversal, SearchStrategyHandlerLoadsCompiledSearchPlugin) {
  const fs::path library_path = make_artifact_path(
      fs::path("SearchStrategies") /
      ("BreadthFirstSearch" TREEWIDZARD_DYNAMIC_LIB_EXTENSION));

  DynamicKernel kernel;
  BoundedCountingCore core;
  Width width = make_tree_width(2);
  kernel.set_width(width);
  kernel.addCore(core);
  kernel.setVarToNameAndIndex({{"x", {"CountingCore", 0}}});

  ConjectureHarness harness;
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  SearchStrategyHandler handler(library_path.string());
  const auto &metadata = handler.get_metadata();

  EXPECT_EQ(metadata.at("SearchName"), "BreadthFirstSearch");

  auto strategy = handler.create(&kernel, &conjecture, &flags);
  ASSERT_NE(strategy, nullptr);
  EXPECT_EQ(strategy->getAttributeValue("SearchName"), "BreadthFirstSearch");
}
