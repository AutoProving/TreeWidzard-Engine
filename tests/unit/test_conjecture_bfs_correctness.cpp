#include <gtest/gtest.h>
#include <memory>
#include <set>
#include <map>
#include <vector>

#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/DynamicKernel.h"
#include "Kernel/Width.h"
#include "Kernel/Flags.h"
#include "Kernel/State.h"
#include "Conjecture/Conjecture.h"
#include "DPCores/Source/ChromaticNumber_AtMost.h"
#include "DPCores/Source/MaxDegree_AtLeast.h"
#include "DPCores/Source/CliqueNumber_AtLeast.h"
#include "DPCores/Source/VertexCover_AtMost.h"
#include "DPCores/Source/VertexCount.h"
#include "DPCores/Source/IndependentSet_Max.h"

using namespace std;

// ============================================================================
// Helper Functions
// ============================================================================

namespace {

/// Create an empty bag for testing
[[maybe_unused]] Bag create_empty_bag() {
  Bag bag;
  bag.set_elements(set<unsigned>{});
  return bag;
}

/// Create a bag with specific vertices
[[maybe_unused]] Bag create_bag(initializer_list<unsigned> vertices) {
  Bag bag;
  bag.set_elements(set<unsigned>(vertices));
  return bag;
}

/// Build a simple NUMBER node
ConjectureNode* build_number_node(double value) {
  return new ConjectureNode(NUMBER, "", value);
}

/// Build a simple CORE_VARIABLE node
ConjectureNode* build_core_var_node(const string& var_name) {
  return new ConjectureNode(CORE_VARIABLE, var_name);
}

/// Build an INV node wrapping a CORE_VARIABLE
ConjectureNode* build_inv_node(ConjectureNode* child) {
  auto* inv_node = new ConjectureNode(INV, "inv");
  inv_node->setChildren({child});
  child->setParent(inv_node);
  return inv_node;
}

/// Build a binary operator node
ConjectureNode* build_binary_op_node(const string& op, ConjectureNode* left,
                                      ConjectureNode* right) {
  auto* op_node = new ConjectureNode(OPERATOR, op);
  op_node->setChildren({left, right});
  left->setParent(op_node);
  right->setParent(op_node);
  return op_node;
}

/// Build a unary operator node (e.g., "not")
[[maybe_unused]] ConjectureNode* build_unary_op_node(const string& op,
                                                     ConjectureNode* child) {
  auto* op_node = new ConjectureNode(OPERATOR, op);
  op_node->setChildren({child});
  child->setParent(op_node);
  return op_node;
}

}  // namespace

// ============================================================================
// Section A: Conjecture Evaluation Tests (Direct State Evaluation)
// ============================================================================

class ConjectureEvaluationTest : public ::testing::Test {
protected:
  void SetUp() override {
    kernel = make_unique<DynamicKernel>();
    conjecture = make_unique<Conjecture>();
    conjecture->setKernel(kernel.get());
  }

  unique_ptr<DynamicKernel> kernel;
  unique_ptr<Conjecture> conjecture;
};

/// Test A.1: NUMBER node returns its value
TEST_F(ConjectureEvaluationTest, NumberNodeEvaluation) {
  auto* num_node = build_number_node(42.0);
  conjecture->setRoot(num_node);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);
  EXPECT_DOUBLE_EQ(result, 42.0);
}

/// Test A.2: OPERATOR "+" on two NUMBER children
TEST_F(ConjectureEvaluationTest, AdditionOfTwoNumbers) {
  auto* left = build_number_node(5.0);
  auto* right = build_number_node(3.0);
  auto* root = build_binary_op_node("+", left, right);
  conjecture->setRoot(root);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);
  EXPECT_DOUBLE_EQ(result, 8.0);
}

/// Test A.3: OPERATOR "<=" comparison
TEST_F(ConjectureEvaluationTest, LessEqualComparison) {
  auto* left = build_number_node(3.0);
  auto* right = build_number_node(5.0);
  auto* root = build_binary_op_node("<=", left, right);
  conjecture->setRoot(root);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);
  EXPECT_DOUBLE_EQ(result, 1.0); // true
}

/// Test A.4: OPERATOR "and" logical operation
TEST_F(ConjectureEvaluationTest, LogicalAnd) {
  auto* left = build_number_node(1.0);  // true
  auto* right = build_number_node(1.0); // true
  auto* root = build_binary_op_node("and", left, right);
  conjecture->setRoot(root);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);
  EXPECT_DOUBLE_EQ(result, 1.0); // true
}

/// Test A.5: INV node calls core's inv method
TEST_F(ConjectureEvaluationTest, InvNodeWithCore) {
  // Setup: Add a simple core
  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Create INV(x)
  auto* var_node = build_core_var_node("x");
  auto* inv_node = build_inv_node(var_node);
  conjecture->setRoot(inv_node);

  auto state = kernel->initialState();
  double result = conjecture->evaluateConjectureOnState(*state);
  EXPECT_GE(result, 0.0);
}

/// Test A.6: CORE_VARIABLE node calls is_final_witness_set
TEST_F(ConjectureEvaluationTest, CoreVariableNode) {
  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);

  auto state = kernel->initialState();
  double result = conjecture->evaluateConjectureOnState(*state);
  // Should return 0 or 1
  EXPECT_GE(result, 0.0);
  EXPECT_LE(result, 1.0);
}

// ============================================================================
// Section B: BFS Correctness with ChromaticNumber
// ============================================================================

class BFSChromaticTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test B.1: Every graph of treewidth 1 is 2-colorable
/// Expected: property_satisfied = true
TEST_F(BFSChromaticTest, Treewidth1Is2Colorable) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  // Set width to 1 (pathwidth)
  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Core: Chromatic number <= 2
  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Conjecture: x (is_final for chromatic)
  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  // Run BFS
  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_FALSE(result.counterexample_state.has_value());
}

/// Test B.2: Every graph of treewidth 1 is 1-colorable should fail
/// Expected: property_satisfied = false (find counterexample)
TEST_F(BFSChromaticTest, Treewidth1Is1ColorableFails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Core: Chromatic number <= 1
  ChromaticNumber_AtMost_Core chroma_core{parameterType{1}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  // Should find a counterexample (any edge graph)
  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

// ============================================================================
// Section C: BFS Correctness with VertexCover
// ============================================================================

class BFSVertexCoverTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test C.1: Treewidth 1 has vertex cover <= 1 should fail
/// Expected: property_satisfied = false
TEST_F(BFSVertexCoverTest, Treewidth1VertexCover1Fails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Core: VertexCover <= 1
  VertexCover_AtMost_Core vc_core{parameterType{1}};
  kernel->addCore(vc_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"VertexCover_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "VertexCover_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  // P3 (path of length 2) needs vertex cover of size 2, so should fail
  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

/// Test C.2: Pathwidth 1 does not imply any constant-size vertex cover bound.
TEST_F(BFSVertexCoverTest, PathwidthVertexCoverAtMost2Fails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Core: VertexCover <= 2
  VertexCover_AtMost_Core vc_core{parameterType{2}};
  kernel->addCore(vc_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"VertexCover_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "VertexCover_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

// ============================================================================
// Section D: BFS Correctness with MaxDegree
// ============================================================================

class BFSMaxDegreeTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test D.1: The empty graph shows that max degree >= 0 is not witnessed by
/// this core until an edge is seen.
TEST_F(BFSMaxDegreeTest, Pathwidth1MaxDegree0Fails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Core: MaxDegree >= 0
  MaxDegree_AtLeast_DynamicCore maxdeg_core{parameterType{0}};
  kernel->addCore(maxdeg_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"MaxDegree_AtLeast", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "MaxDegree_AtLeast"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

/// Test D.2: Treewidth 2 has max degree >= 5 should fail
/// Expected: property_satisfied = false
TEST_F(BFSMaxDegreeTest, Treewidth2MaxDegree5Fails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("tree_width");
  w.set_value(2);
  kernel->set_width(w);

  // Core: MaxDegree >= 5
  MaxDegree_AtLeast_DynamicCore maxdeg_core{parameterType{5}};
  kernel->addCore(maxdeg_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"MaxDegree_AtLeast", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "MaxDegree_AtLeast"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  // Should find a counterexample (many graphs at tw<=2 don't have max deg >= 5)
  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

// ============================================================================
// Section E: BFS with VertexCount
// ============================================================================

class BFSVertexCountTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test E.1: Width 1, conjecture "inv(x) <= 2" should fail quickly.
TEST_F(BFSVertexCountTest, Width1VertexCountUpTo2Fails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Core: VertexCount (no parameters)
  VertexCount_Core vc_core{parameterType{}};
  kernel->addCore(vc_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"VertexCount", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "VertexCount"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Conjecture: inv(x) <= 2
  auto* var_node = build_core_var_node("x");
  auto* inv_node = build_inv_node(var_node);
  auto* num_node = build_number_node(2.0);
  auto* root = build_binary_op_node("<=", inv_node, num_node);
  conjecture->setRoot(root);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

/// Test E.2: Width 2, conjecture "inv(x) <= 2" should fail
/// Expected: property_satisfied = false
TEST_F(BFSVertexCountTest, Width2VertexCountUpTo2Fails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("tree_width");
  w.set_value(2);
  kernel->set_width(w);

  // Core: VertexCount
  VertexCount_Core vc_core{parameterType{}};
  kernel->addCore(vc_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"VertexCount", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "VertexCount"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Conjecture: inv(x) <= 2
  auto* var_node = build_core_var_node("x");
  auto* inv_node = build_inv_node(var_node);
  auto* num_node = build_number_node(2.0);
  auto* root = build_binary_op_node("<=", inv_node, num_node);
  conjecture->setRoot(root);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  // Width 2 can have 3 vertices, so should fail
  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

// ============================================================================
// Section F: BFS with IndependentSet_Max
// ============================================================================

class BFSIndependentSetTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test F.1: The empty graph is a counterexample to "inv(x) >= 1".
TEST_F(BFSIndependentSetTest, Width1IndependentSetAtLeast1Fails) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Core: IndependentSet_Max
  IndependentSet_Max_Core is_core{parameterType{}};
  kernel->addCore(is_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"IndependentSet_Max", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "IndependentSet_Max"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Conjecture: inv(x) >= 1
  auto* var_node = build_core_var_node("x");
  auto* inv_node = build_inv_node(var_node);
  auto* num_node = build_number_node(1.0);
  auto* root = build_binary_op_node(">=", inv_node, num_node);
  conjecture->setRoot(root);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

// ============================================================================
// Section G: State Manipulation Tests
// ============================================================================

class StateManipulationTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
    kernel = make_unique<DynamicKernel>();
    chroma_core = make_unique<ChromaticNumber_AtMost_Core>(parameterType{2});
    kernel->addCore(*chroma_core);
  }

  void TearDown() override {
    State::clearPool();
  }

  unique_ptr<DynamicKernel> kernel;
  unique_ptr<ChromaticNumber_AtMost_Core> chroma_core;
};

/// Test G.1: DynamicKernel.initialState() returns state with empty bag
TEST_F(StateManipulationTest, InitialStateHasEmptyBag) {
  auto state_ptr = kernel->initialState();
  const auto& bag = state_ptr->get_bag();
  EXPECT_EQ(bag.get_elements().size(), 0);
}

/// Test G.2: intro_v produces state with correct bag
TEST_F(StateManipulationTest, IntroVProducesCorrectBag) {
  auto initial = kernel->initialState();
  auto after_intro_v = kernel->intro_v(initial, 0);

  const auto& bag = after_intro_v->get_bag();
  EXPECT_EQ(bag.get_elements().size(), 1);
  EXPECT_TRUE(bag.get_elements().count(0));
}

/// Test G.3: intro_e doesn't change bag
TEST_F(StateManipulationTest, IntroEDoesNotChangeBag) {
  auto initial = kernel->initialState();
  auto after_v0 = kernel->intro_v(initial, 0);
  auto after_v1 = kernel->intro_v(after_v0, 1);

  auto bag_before = after_v1->get_bag();
  auto after_edge = kernel->intro_e(after_v1, 0, 1);
  auto bag_after = after_edge->get_bag();

  EXPECT_EQ(bag_before.get_elements(), bag_after.get_elements());
}

/// Test G.4: forget_v removes vertex from bag
TEST_F(StateManipulationTest, ForgetVRemovesVertexFromBag) {
  auto initial = kernel->initialState();
  auto after_v0 = kernel->intro_v(initial, 0);

  auto bag_before = after_v0->get_bag();
  EXPECT_EQ(bag_before.get_elements().size(), 1);

  auto after_forget = kernel->forget_v(after_v0, 0);
  auto bag_after = after_forget->get_bag();
  EXPECT_EQ(bag_after.get_elements().size(), 0);
}

/// Test G.5: join of two states with same bag works
TEST_F(StateManipulationTest, JoinTwoStatesWithSameBag) {
  auto initial = kernel->initialState();
  auto v0 = kernel->intro_v(initial, 0);

  // Create two copies
  auto left = v0;
  auto right = v0;

  // Join them
  auto joined = kernel->join(left, right);

  const auto& joined_bag = joined->get_bag();
  const auto& original_bag = v0->get_bag();
  EXPECT_EQ(joined_bag.get_elements(), original_bag.get_elements());
}

// ============================================================================
// Section H: Hook Callback Tests
// ============================================================================

class HookCallbackTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test H.1: on_initial_state gets empty bag state
TEST_F(HookCallbackTest, OnInitialStateHook) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  bool hook_called = false;
  State::ptr initial_state_from_hook;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_initial_state = [&](const State::ptr& state) {
    hook_called = true;
    initial_state_from_hook = state;
  };

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags, hooks);

  EXPECT_TRUE(hook_called);
  EXPECT_EQ(initial_state_from_hook->get_bag().get_elements().size(), 0);
}

/// Test H.2: on_state_discovered fires for each new state
TEST_F(HookCallbackTest, OnStateDiscoveredHook) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  int discovery_count = 0;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_state_discovered = [&](const TreeWidzard::BreadthFirstExpansionEvent&) {
    discovery_count++;
  };

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags, hooks);

  EXPECT_GT(discovery_count, 0);
}

/// Test H.3: on_iteration_complete fires after each BFS level
TEST_F(HookCallbackTest, OnIterationCompleteHook) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  int iteration_count = 0;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_iteration_complete = [&](int, size_t, size_t,
                                    const vector<unsigned>&) {
    iteration_count++;
  };

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags, hooks);

  EXPECT_GT(iteration_count, 0);
}

// ============================================================================
// Section I: Multi-Core Conjectures
// ============================================================================

class MultiCoreConjectureTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test I.1: Two cores in the same kernel can participate in one formula.
TEST_F(MultiCoreConjectureTest, TwoCoresConjunctionEvaluation) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Add two cores
  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  VertexCount_Core vc_core{parameterType{}};
  kernel->addCore(chroma_core);
  kernel->addCore(vc_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}, {"y", {"VertexCount", 1}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{
      {"x", "ChromaticNumber_AtMost"}, {"y", "VertexCount"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Conjecture: x and inv(y) >= 0
  auto* var_x = build_core_var_node("x");
  auto* var_y = build_core_var_node("y");
  auto* inv_y = build_inv_node(var_y);
  auto* zero = build_number_node(0.0);
  auto* y_non_negative = build_binary_op_node(">=", inv_y, zero);
  auto* root = build_binary_op_node("and", var_x, y_non_negative);
  conjecture->setRoot(root);
  conjecture->setKernel(kernel.get());

  auto state = kernel->initialState();
  double result = conjecture->evaluateConjectureOnState(*state);
  // Should evaluate without error
  EXPECT_GE(result, 0.0);
}

/// Test I.2: "inv(x) + inv(y) <= 5" with VertexCount + IndependentSet
TEST_F(MultiCoreConjectureTest, TwoCoresComplexArithmetic) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Add two cores
  VertexCount_Core vc_core{parameterType{}};
  IndependentSet_Max_Core is_core{parameterType{}};
  kernel->addCore(vc_core);
  kernel->addCore(is_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"VertexCount", 0}}, {"y", {"IndependentSet_Max", 1}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{
      {"x", "VertexCount"}, {"y", "IndependentSet_Max"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Conjecture: inv(x) + inv(y) <= 5
  auto* var_x = build_core_var_node("x");
  auto* inv_x = build_inv_node(var_x);
  auto* var_y = build_core_var_node("y");
  auto* inv_y = build_inv_node(var_y);
  auto* sum = build_binary_op_node("+", inv_x, inv_y);
  auto* num_5 = build_number_node(5.0);
  auto* root = build_binary_op_node("<=", sum, num_5);
  conjecture->setRoot(root);
  conjecture->setKernel(kernel.get());

  auto state = kernel->initialState();
  double result = conjecture->evaluateConjectureOnState(*state);
  // Should evaluate without error
  EXPECT_GE(result, 0.0);
}

/// Test I.3: Multi-core BFS with ChomaticNumber and VertexCount
TEST_F(MultiCoreConjectureTest, MultiCoreGraphPropertyCheckBFS) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  // Add cores
  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  VertexCount_Core vc_core{parameterType{}};
  kernel->addCore(chroma_core);
  kernel->addCore(vc_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}, {"y", {"VertexCount", 1}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{
      {"x", "ChromaticNumber_AtMost"}, {"y", "VertexCount"}};
  conjecture->setVariablesToCoreName(var_map2);

  // Conjecture: x and inv(y) <= 2
  auto* var_x = build_core_var_node("x");
  auto* var_y = build_core_var_node("y");
  auto* inv_y = build_inv_node(var_y);
  auto* two = build_number_node(2.0);
  auto* y_small = build_binary_op_node("<=", inv_y, two);
  auto* root = build_binary_op_node("and", var_x, y_small);
  conjecture->setRoot(root);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  EXPECT_FALSE(result.property_satisfied);
  EXPECT_TRUE(result.counterexample_state.has_value());
}

// ============================================================================
// Section J: Additional Edge Cases and Robustness
// ============================================================================

class EdgeCaseTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(1000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test J.1: Nested arithmetic operations
TEST_F(EdgeCaseTest, NestedArithmeticEvaluation) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  conjecture->setKernel(kernel.get());

  // Build: ((2 + 3) * 4) - 1 = 19
  auto* two = build_number_node(2.0);
  auto* three = build_number_node(3.0);
  auto* add = build_binary_op_node("+", two, three);

  auto* four = build_number_node(4.0);
  auto* mul = build_binary_op_node("*", add, four);

  auto* one = build_number_node(1.0);
  auto* sub = build_binary_op_node("-", mul, one);

  conjecture->setRoot(sub);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);
  EXPECT_DOUBLE_EQ(result, 19.0);
}

/// Test J.2: Complex comparison chains
TEST_F(EdgeCaseTest, ComparisonChainEvaluation) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  conjecture->setKernel(kernel.get());

  // Build: (3 < 5) and (5 < 10) = true
  auto* three = build_number_node(3.0);
  auto* five_1 = build_number_node(5.0);
  auto* lt1 = build_binary_op_node("<", three, five_1);

  auto* five_2 = build_number_node(5.0);
  auto* ten = build_number_node(10.0);
  auto* lt2 = build_binary_op_node("<", five_2, ten);

  auto* root = build_binary_op_node("and", lt1, lt2);
  conjecture->setRoot(root);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);
  EXPECT_DOUBLE_EQ(result, 1.0);
}

/// Test J.3: Core size retrieval
TEST_F(EdgeCaseTest, CoreSizeRetrieval) {
  auto kernel = make_unique<DynamicKernel>();

  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  VertexCount_Core vc_core{parameterType{}};

  kernel->addCore(chroma_core);
  kernel->addCore(vc_core);

  EXPECT_EQ(kernel->coreSize(), 2);
}

/// Test J.4: Kernel width getter/setter
TEST_F(EdgeCaseTest, KernelWidthManagement) {
  auto kernel = make_unique<DynamicKernel>();

  Width w;
  w.set_name("tree_width");
  w.set_value(3);
  kernel->set_width(w);

  Width retrieved = kernel->get_width();
  EXPECT_EQ(retrieved.get_value(), 3);
  EXPECT_EQ(retrieved.get_name(), "tree_width");
}

/// Test J.5: Flags management
TEST_F(EdgeCaseTest, FlagsManagement) {
  Flags flags;

  flags.add_flag("verbose", 1.0);
  flags.add_flag("max_iterations", 1000.0);

  EXPECT_EQ(flags.get("verbose"), 1.0);
  EXPECT_EQ(flags.get("max_iterations"), 1000.0);
}

// ============================================================================
// Section K: Integration Tests
// ============================================================================

class IntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    State::initializePool(2000);
  }

  void TearDown() override {
    State::clearPool();
  }
};

/// Test K.1: Full BFS pipeline with graph property verification
TEST_F(IntegrationTest, FullBFSPipelineChromatic) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  ChromaticNumber_AtMost_Core chroma_core{parameterType{3}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  auto* var_node = build_core_var_node("x");
  conjecture->setRoot(var_node);
  conjecture->setKernel(kernel.get());

  int hook_calls = 0;
  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_iteration_complete = [&](int, size_t, size_t,
                                    const vector<unsigned>&) {
    hook_calls++;
  };

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags, hooks);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_GT(result.total_states, 0);
  EXPECT_GT(hook_calls, 0);
  EXPECT_GE(result.iterations, 0);
}

/// Test K.2: BFS with mixed property checks
TEST_F(IntegrationTest, MixedPropertyCheckBFS) {
  auto kernel = make_unique<DynamicKernel>();
  auto conjecture = make_unique<Conjecture>();
  auto flags = make_unique<Flags>();

  Width w;
  w.set_name("path_width");
  w.set_value(1);
  kernel->set_width(w);

  ChromaticNumber_AtMost_Core chroma_core{parameterType{2}};
  kernel->addCore(chroma_core);

  auto var_map = map<string, pair<string, int>>{
      {"x", {"ChromaticNumber_AtMost", 0}}};
  kernel->setVarToNameAndIndex(var_map);

  auto var_map2 = map<string, string>{{"x", "ChromaticNumber_AtMost"}};
  conjecture->setVariablesToCoreName(var_map2);

  // More complex conjecture with comparison
  auto* var_node = build_core_var_node("x");
  auto* one = build_number_node(1.0);
  auto* root = build_binary_op_node(">=", var_node, one);
  conjecture->setRoot(root);
  conjecture->setKernel(kernel.get());

  auto result = TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture,
                                                       *flags);

  EXPECT_TRUE(result.property_satisfied);
}
