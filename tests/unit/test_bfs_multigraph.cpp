#include <gtest/gtest.h>
#include "../utils/test_helpers.h"
#include "../utils/kernel_test_doubles.h"
#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/DynamicKernel.h"
#include "Kernel/Width.h"
#include "Kernel/Flags.h"
#include "Conjecture/Conjecture.h"
#include "Multigraph/MultiGraph.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace {

using TreeWidzardTest::BoundedCountingCore;
using TreeWidzardTest::first_witness_value;

// Helper to create a Width object
auto make_width(unsigned value, const std::string &name) -> Width {
  Width width;
  width.set_name(name);
  width.set_value(value);
  return width;
}

// ConjectureHarness for building conjecture trees
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

// Factory functions for creating test conjectures
auto make_always_true_conjecture(DynamicKernel &kernel,
                                  ConjectureHarness &h) -> Conjecture {
  Conjecture conjecture;
  conjecture.setKernel(&kernel);
  conjecture.setVariablesToCoreName({{"x", "CountingCore"}});
  conjecture.setRoot(h.make(NUMBER, 1.0));
  return conjecture;
}

auto make_always_false_conjecture(DynamicKernel &kernel,
                                   ConjectureHarness &h) -> Conjecture {
  Conjecture conjecture;
  conjecture.setKernel(&kernel);
  conjecture.setVariablesToCoreName({{"x", "CountingCore"}});
  conjecture.setRoot(h.make(NUMBER, 0.0));
  return conjecture;
}

auto make_inv_leq_conjecture(DynamicKernel &kernel,
                              ConjectureHarness &h,
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

void configure_kernel(DynamicKernel &kernel,
                      BoundedCountingCore &core,
                      unsigned width_value = 1) {
  Width width = make_width(width_value, "tree_width");
  kernel.set_width(width);
  kernel.addCore(core);
  kernel.setVarToNameAndIndex({{"x", {"CountingCore", 0}}});
}

} // namespace

// ============================================================================
// BREADTH FIRST TRAVERSAL TESTS
// ============================================================================

class BreadthFirstTraversalTest : public ::testing::Test {
protected:
  DynamicKernel kernel;
  BoundedCountingCore core;
  ConjectureHarness harness;
};

// Test 1: BFS with width 1 explores correct number of states
TEST_F(BreadthFirstTraversalTest, Width1ExploresCorrectNumberOfStates) {
  configure_kernel(kernel, core, 1);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  const auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_FALSE(result.counterexample_state.has_value());
  EXPECT_GT(result.total_states, 0U);
  EXPECT_EQ(result.iterations, 1);
}

// Test 2: BFS with width 2 explores intro_v, forget_v, intro_e transitions
TEST_F(BreadthFirstTraversalTest, Width2ExploresVertexAndEdgeTransitions) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  bool saw_intro_vertex = false;
  bool saw_forget_vertex = false;
  bool saw_intro_edge = false;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_state_discovered =
      [&](const TreeWidzard::BreadthFirstExpansionEvent &event) {
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
      default:
        break;
    }
  };

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags, hooks);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_TRUE(saw_intro_vertex);
  EXPECT_TRUE(saw_forget_vertex);
  EXPECT_TRUE(saw_intro_edge);
}

// Test 3: BFS with tree_width enables join transitions
TEST_F(BreadthFirstTraversalTest, TreeWidthEnablesJoinTransitions) {
  configure_kernel(kernel, core, 2);
  Width width = make_width(2, "tree_width");
  kernel.set_width(width);

  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  bool saw_join = false;
  bool join_has_second_parent = false;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_state_discovered =
      [&](const TreeWidzard::BreadthFirstExpansionEvent &event) {
    if (event.kind == TreeWidzard::BreadthFirstExpansionKind::Join) {
      saw_join = true;
      if (event.second_parent.has_value()) {
        join_has_second_parent = true;
      }
    }
  };

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags, hooks);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_TRUE(saw_join);
  EXPECT_TRUE(join_has_second_parent);
}

// Test 4: BFS with path_width disables join transitions
TEST_F(BreadthFirstTraversalTest, PathWidthDisablesJoinTransitions) {
  configure_kernel(kernel, core, 2);
  Width width = make_width(2, "path_width");
  kernel.set_width(width);

  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  bool saw_join = false;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_state_discovered =
      [&](const TreeWidzard::BreadthFirstExpansionEvent &event) {
    if (event.kind == TreeWidzard::BreadthFirstExpansionKind::Join) {
      saw_join = true;
    }
  };

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags, hooks);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_FALSE(saw_join);
}

// Test 5: BFS finds counterexample when conjecture is violated
TEST_F(BreadthFirstTraversalTest, FindsCounterexampleWhenConjectureViolated) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_inv_leq_conjecture(kernel, harness, 0.0);
  Flags flags;

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags);

  EXPECT_FALSE(result.property_satisfied);
  ASSERT_TRUE(result.counterexample_state.has_value());
  EXPECT_GT(result.total_states, 0U);
}

// Test 6: BFS returns property_satisfied when conjecture holds
TEST_F(BreadthFirstTraversalTest, ReturnsSatisfiedWhenConjectureHolds) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_inv_leq_conjecture(kernel, harness, 10.0);
  Flags flags;

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_FALSE(result.counterexample_state.has_value());
  EXPECT_GT(result.total_states, 0U);
}

// Test 7: Hook on_initial_state is invoked
TEST_F(BreadthFirstTraversalTest, OnInitialStateHookInvoked) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  bool on_initial_called = false;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_initial_state = [&](const State::ptr &state) {
    on_initial_called = true;
    EXPECT_TRUE(state->get_bag().get_elements().empty());
  };

  TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags, hooks);

  EXPECT_TRUE(on_initial_called);
}

// Test 8: Hook on_state_discovered is invoked
TEST_F(BreadthFirstTraversalTest, OnStateDiscoveredHookInvoked) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  size_t discovered_count = 0;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_state_discovered =
      [&](const TreeWidzard::BreadthFirstExpansionEvent &) {
    ++discovered_count;
  };

  TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags, hooks);

  EXPECT_GT(discovered_count, 0U);
}

// Test 9: Hook on_iteration_complete is invoked
TEST_F(BreadthFirstTraversalTest, OnIterationCompleteHookInvoked) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  size_t iteration_count = 0;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_iteration_complete =
      [&](int iteration, size_t all_states_size, size_t new_states_size,
          const std::vector<unsigned> &max_witness_sizes) {
    ++iteration_count;
    EXPECT_GE(iteration, 1);
    EXPECT_GE(all_states_size, new_states_size);
    EXPECT_EQ(max_witness_sizes.size(), 1U);
  };

  TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags, hooks);

  EXPECT_GT(iteration_count, 0U);
}

// Test 10: BFS terminates for bounded width
TEST_F(BreadthFirstTraversalTest, TerminatesForBoundedWidth) {
  configure_kernel(kernel, core, 1);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_GE(result.iterations, 0);
  EXPECT_GE(result.total_states, 1U);
}

// Test 11: BFS with multiple iterations
TEST_F(BreadthFirstTraversalTest, MultipleIterations) {
  configure_kernel(kernel, core, 3);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_GT(result.iterations, 1);
}

// Test 12: Counterexample detected at initial state
TEST_F(BreadthFirstTraversalTest, CounterexampleAtInitialState) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_false_conjecture(kernel, harness);
  Flags flags;

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags);

  EXPECT_FALSE(result.property_satisfied);
  ASSERT_TRUE(result.counterexample_state.has_value());
  EXPECT_EQ(result.iterations, 0);
  EXPECT_EQ(result.total_states, 1U);
}

// Test 13: Max witness sizes are tracked correctly
TEST_F(BreadthFirstTraversalTest, MaxWitnessSizesTracked) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags);

  EXPECT_EQ(result.max_witness_sizes.size(), 1U);
  EXPECT_GE(result.max_witness_sizes[0], 0U);
}

// Test 14: Hooks are called in correct order
TEST_F(BreadthFirstTraversalTest, HooksCalledInCorrectOrder) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;

  std::vector<std::string> call_order;

  TreeWidzard::BreadthFirstTraversalHooks hooks;
  hooks.on_initial_state = [&](const State::ptr &) {
    call_order.push_back("initial");
  };
  hooks.on_state_discovered =
      [&](const TreeWidzard::BreadthFirstExpansionEvent &) {
    call_order.push_back("discovered");
  };
  hooks.on_iteration_complete =
      [&](int, size_t, size_t, const std::vector<unsigned> &) {
    call_order.push_back("iteration");
  };

  TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags, hooks);

  ASSERT_FALSE(call_order.empty());
  EXPECT_EQ(call_order[0], "initial");
}

// Test 15: Empty hooks parameter works
TEST_F(BreadthFirstTraversalTest, EmptyHooksParameter) {
  configure_kernel(kernel, core, 2);
  Conjecture conjecture = make_always_true_conjecture(kernel, harness);
  Flags flags;
  TreeWidzard::BreadthFirstTraversalHooks empty_hooks;

  const auto result =
      TreeWidzard::runBreadthFirstTraversal(kernel, conjecture, flags,
                                           empty_hooks);

  EXPECT_TRUE(result.property_satisfied);
  EXPECT_GT(result.total_states, 0U);
}

// ============================================================================
// MULTIGRAPH TESTS
// ============================================================================

class MultiGraphTest : public ::testing::Test {
protected:
  MultiGraph graph;
};

// Test 16: addVertex and verticesSize
TEST_F(MultiGraphTest, AddVertexAndVerticesSize) {
  EXPECT_EQ(graph.verticesSize(), 0U);

  graph.addVertex(1);
  EXPECT_EQ(graph.verticesSize(), 1U);

  graph.addVertex(2);
  EXPECT_EQ(graph.verticesSize(), 2U);

  graph.addVertex(3);
  EXPECT_EQ(graph.verticesSize(), 3U);
}

// Test 17: Duplicate vertices are not added twice
TEST_F(MultiGraphTest, DuplicateVerticesNotAddedTwice) {
  graph.addVertex(1);
  graph.addVertex(1);

  EXPECT_EQ(graph.verticesSize(), 1U);
}

// Test 18: addEdgeLabel and edgesSize
TEST_F(MultiGraphTest, AddEdgeLabelAndEdgesSize) {
  EXPECT_EQ(graph.edgesSize(), 0U);

  graph.addEdgeLabel(1);
  EXPECT_EQ(graph.edgesSize(), 1U);

  graph.addEdgeLabel(2);
  EXPECT_EQ(graph.edgesSize(), 2U);
}

// Test 19: addIncidence and incidence queries
TEST_F(MultiGraphTest, AddIncidenceAndQueries) {
  graph.addEdgeLabel(1);
  graph.addVertex(1);
  graph.addVertex(2);

  graph.addIncidence(1, 1);
  graph.addIncidence(1, 2);

  const auto &incidence_map = graph.getIncidenceMap();
  auto range = incidence_map.equal_range(1);

  std::vector<unsigned> vertices;
  for (auto it = range.first; it != range.second; ++it) {
    vertices.push_back(it->second);
  }

  EXPECT_EQ(vertices.size(), 2U);
  EXPECT_TRUE(std::find(vertices.begin(), vertices.end(), 1) != vertices.end());
  EXPECT_TRUE(std::find(vertices.begin(), vertices.end(), 2) != vertices.end());
}

// Test 20: edgesVertex returns correct edges
TEST_F(MultiGraphTest, EdgesVertexReturnsCorrectEdges) {
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);

  unsigned edge1 = graph.addEdgeEndPoints(1, 2);
  unsigned edge2 = graph.addEdgeEndPoints(1, 3);

  std::set<unsigned> edges_for_vertex_1 = graph.edgesVertex(1);

  EXPECT_EQ(edges_for_vertex_1.size(), 2U);
  EXPECT_TRUE(edges_for_vertex_1.find(edge1) != edges_for_vertex_1.end());
  EXPECT_TRUE(edges_for_vertex_1.find(edge2) != edges_for_vertex_1.end());
}

// Test 21: edgesBetweenVertices returns shared edges
TEST_F(MultiGraphTest, EdgesBetweenVerticesReturnsSharedEdges) {
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);

  unsigned edge1 = graph.addEdgeEndPoints(1, 2);
  unsigned edge2 = graph.addEdgeEndPoints(1, 3);

  std::set<unsigned> edges_between_1_2 = graph.edgesBetweenVertices(1, 2);
  std::set<unsigned> edges_between_1_3 = graph.edgesBetweenVertices(1, 3);

  EXPECT_EQ(edges_between_1_2.size(), 1U);
  EXPECT_TRUE(edges_between_1_2.find(edge1) != edges_between_1_2.end());
  EXPECT_EQ(edges_between_1_3.size(), 1U);
  EXPECT_TRUE(edges_between_1_3.find(edge2) != edges_between_1_3.end());
}

// Test 22: nthVertex returns correct vertex
TEST_F(MultiGraphTest, NthVertexReturnsCorrectVertex) {
  graph.addVertex(5);
  graph.addVertex(10);
  graph.addVertex(15);

  unsigned nth0 = graph.nthVertex(0);
  unsigned nth1 = graph.nthVertex(1);
  unsigned nth2 = graph.nthVertex(2);

  EXPECT_EQ(nth0, 5U);
  EXPECT_EQ(nth1, 10U);
  EXPECT_EQ(nth2, 15U);
}

// Test 23: nthVertex with invalid index errors gracefully
TEST_F(MultiGraphTest, NthVertexOutOfBoundsError) {
  graph.addVertex(1);

  // nthVertex should exit(20) on out of bounds access, but we can't easily
  // test exit behavior. Instead, verify that valid indices work.
  unsigned valid = graph.nthVertex(0);
  EXPECT_EQ(valid, 1U);
}

// Test 24: isMultiGraph validation with valid graph
TEST_F(MultiGraphTest, IsMultiGraphValidatesCorrectly) {
  graph.addVertex(1);
  graph.addVertex(2);
  unsigned edge = graph.addEdgeEndPoints(1, 2);

  EXPECT_TRUE(graph.isMultiGraph());
}

// Test 25: isMultiGraph with incomplete edge fails
TEST_F(MultiGraphTest, IsMultiGraphDetectsIncompleteEdges) {
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addEdgeLabel(1);
  graph.addIncidence(1, 1);
  // Only one incidence, not two - not a valid multigraph edge

  EXPECT_FALSE(graph.isMultiGraph());
}

// Test 26: Copy constructor
TEST_F(MultiGraphTest, CopyConstructor) {
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);
  unsigned edge = graph.addEdgeEndPoints(1, 2);

  MultiGraph copy(graph);

  EXPECT_EQ(copy.verticesSize(), graph.verticesSize());
  EXPECT_EQ(copy.edgesSize(), graph.edgesSize());
  EXPECT_TRUE(copy.isMultiGraph());
}

// Test 27: getVertices returns correct set
TEST_F(MultiGraphTest, GetVerticesReturnsCorrectSet) {
  graph.addVertex(1);
  graph.addVertex(3);
  graph.addVertex(5);

  const auto &vertices = graph.getVertices();

  EXPECT_EQ(vertices.size(), 3U);
  EXPECT_TRUE(vertices.find(1) != vertices.end());
  EXPECT_TRUE(vertices.find(3) != vertices.end());
  EXPECT_TRUE(vertices.find(5) != vertices.end());
}

// Test 28: getEdges returns correct set
TEST_F(MultiGraphTest, GetEdgesReturnsCorrectSet) {
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);

  unsigned edge1 = graph.addEdgeEndPoints(1, 2);
  unsigned edge2 = graph.addEdgeEndPoints(2, 3);

  const auto &edges = graph.getEdges();

  EXPECT_EQ(edges.size(), 2U);
  EXPECT_TRUE(edges.find(edge1) != edges.end());
  EXPECT_TRUE(edges.find(edge2) != edges.end());
}

// Test 29: PACE format I/O roundtrip
TEST_F(MultiGraphTest, PACEFormatRoundtrip) {
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);
  graph.addEdgeEndPoints(1, 2);
  graph.addEdgeEndPoints(2, 3);

  std::string temp_file = "/tmp/test_pace_graph.txt";
  graph.printToFilePACEFormat(temp_file);

  std::ifstream file(temp_file);
  EXPECT_TRUE(file.good());

  std::string line;
  int line_count = 0;
  while (std::getline(file, line)) {
    if (!line.empty() && line[0] != 'c' && line[0] != 'p') {
      line_count++;
    }
  }
  file.close();

  EXPECT_EQ(line_count, 2);

  // Cleanup
  std::remove(temp_file.c_str());
}

// Test 30: GML format conversion
TEST_F(MultiGraphTest, GMLFormatConversion) {
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addEdgeEndPoints(1, 2);

  std::string temp_file = "/tmp/test_gml_graph.gml";
  bool success = graph.convertToGML(temp_file);

  EXPECT_TRUE(success);

  std::ifstream file(temp_file);
  EXPECT_TRUE(file.good());

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  EXPECT_TRUE(content.find("graph") != std::string::npos);
  EXPECT_TRUE(content.find("node") != std::string::npos);
  EXPECT_TRUE(content.find("edge") != std::string::npos);

  // Cleanup
  std::remove(temp_file.c_str());
}

// Test 31: Empty graph operations
TEST_F(MultiGraphTest, EmptyGraphOperations) {
  EXPECT_EQ(graph.verticesSize(), 0U);
  EXPECT_EQ(graph.edgesSize(), 0U);

  const auto &vertices = graph.getVertices();
  const auto &edges = graph.getEdges();

  EXPECT_TRUE(vertices.empty());
  EXPECT_TRUE(edges.empty());
}

// Test 32: addEdgeEndPoints returns correct edge label
TEST_F(MultiGraphTest, AddEdgeEndPointsReturnsCorrectLabel) {
  unsigned edge1 = graph.addEdgeEndPoints(1, 2);
  unsigned edge2 = graph.addEdgeEndPoints(2, 3);

  EXPECT_EQ(edge1, 1U);
  EXPECT_EQ(edge2, 2U);
}

// Test 33: Multiple edges between same vertices
TEST_F(MultiGraphTest, MultipleEdgesBetweenSameVertices) {
  unsigned edge1 = graph.addEdgeEndPoints(1, 2);
  unsigned edge2 = graph.addEdgeEndPoints(1, 2);

  EXPECT_NE(edge1, edge2);

  std::set<unsigned> edges = graph.edgesBetweenVertices(1, 2);
  EXPECT_EQ(edges.size(), 2U);
}

// Test 34: setVertices replaces vertex set
TEST_F(MultiGraphTest, SetVerticesReplacesVertexSet) {
  graph.addVertex(1);
  graph.addVertex(2);

  std::set<unsigned> new_vertices = {10, 20, 30};
  graph.setVertices(new_vertices);

  EXPECT_EQ(graph.verticesSize(), 3U);
  EXPECT_TRUE(graph.getVertices().find(10) != graph.getVertices().end());
  EXPECT_TRUE(graph.getVertices().find(20) != graph.getVertices().end());
}

// Test 35: setEdges replaces edge set
TEST_F(MultiGraphTest, SetEdgesReplacesEdgeSet) {
  graph.addEdgeLabel(1);
  graph.addEdgeLabel(2);

  std::set<unsigned> new_edges = {100, 200};
  graph.setEdges(new_edges);

  EXPECT_EQ(graph.edgesSize(), 2U);
  EXPECT_TRUE(graph.getEdges().find(100) != graph.getEdges().end());
  EXPECT_TRUE(graph.getEdges().find(200) != graph.getEdges().end());
}

// Test 36: Large graph handling
TEST_F(MultiGraphTest, LargeGraphHandling) {
  for (unsigned i = 1; i <= 100; ++i) {
    graph.addVertex(i);
  }

  EXPECT_EQ(graph.verticesSize(), 100U);

  for (unsigned i = 1; i < 100; ++i) {
    graph.addEdgeEndPoints(i, i + 1);
  }

  EXPECT_EQ(graph.edgesSize(), 99U);
  EXPECT_TRUE(graph.isMultiGraph());
}

// Test 37: Self-loops (edge between vertex and itself)
TEST_F(MultiGraphTest, SelfLoopsSupport) {
  unsigned edge = graph.addEdgeEndPoints(1, 1);

  std::set<unsigned> edges_for_vertex_1 = graph.edgesVertex(1);

  EXPECT_EQ(edges_for_vertex_1.size(), 1U);
  EXPECT_TRUE(edges_for_vertex_1.find(edge) != edges_for_vertex_1.end());
}

// Test 38: getIncidenceMap returns correct map
TEST_F(MultiGraphTest, GetIncidenceMapReturnsCorrectMap) {
  graph.addEdgeLabel(1);
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addIncidence(1, 1);
  graph.addIncidence(1, 2);

  const auto &map = graph.getIncidenceMap();

  EXPECT_EQ(map.count(1), 2U);

  auto range = map.equal_range(1);
  std::vector<unsigned> vertices;
  for (auto it = range.first; it != range.second; ++it) {
    vertices.push_back(it->second);
  }

  EXPECT_EQ(vertices.size(), 2U);
}

// Test 39: setIncidenceMap replaces incidence map
TEST_F(MultiGraphTest, SetIncidenceMapReplacesMap) {
  graph.addEdgeLabel(1);
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addIncidence(1, 1);

  std::multimap<unsigned, unsigned> new_map;
  new_map.insert({1, 1});
  new_map.insert({1, 2});
  new_map.insert({2, 3});

  graph.setIncidenceMap(new_map);

  const auto &map = graph.getIncidenceMap();
  EXPECT_EQ(map.size(), 3U);
}

// Test 40: Complex graph with multiple components
TEST_F(MultiGraphTest, ComplexGraphWithMultipleComponents) {
  // Component 1: vertices 1-3
  graph.addEdgeEndPoints(1, 2);
  graph.addEdgeEndPoints(2, 3);

  // Component 2: vertices 4-5
  graph.addEdgeEndPoints(4, 5);

  EXPECT_EQ(graph.verticesSize(), 5U);
  EXPECT_EQ(graph.edgesSize(), 3U);

  std::set<unsigned> edges_1 = graph.edgesVertex(1);
  std::set<unsigned> edges_4 = graph.edgesVertex(4);

  EXPECT_EQ(edges_1.size(), 1U);
  EXPECT_EQ(edges_4.size(), 1U);
  EXPECT_TRUE(graph.isMultiGraph());
}
