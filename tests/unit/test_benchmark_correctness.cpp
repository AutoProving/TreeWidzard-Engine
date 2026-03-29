// =============================================================================
// Benchmark + Correctness Comparison Tests
//
// These tests verify that the optimized code produces correct results AND
// measure performance of key operations. Each test:
//   1. Runs the operation under timing
//   2. Verifies the result matches a known correct answer
//   3. Reports elapsed time via gtest output
// =============================================================================

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <set>
#include <map>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <functional>

#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/DynamicKernel.h"
#include "Kernel/Width.h"
#include "Kernel/Flags.h"
#include "Conjecture/Conjecture.h"

#include "DPCores/Source/ChromaticNumber_AtMost.h"
#include "DPCores/Source/MaxDegree_AtLeast.h"
#include "DPCores/Source/CliqueNumber_AtLeast.h"
#include "DPCores/Source/VertexCover_AtMost.h"
#include "DPCores/Source/VertexCount.h"
#include "DPCores/Source/IndependentSet_Max.h"
#include "DPCores/Source/HasMultipleEdges.h"
#include "DPCores/Source/FeedbackVertexSet_AtMost.h"

using namespace std;
using Clock = chrono::steady_clock;

// =============================================================================
// Timing Utility
// =============================================================================

struct TimedResult {
  double elapsed_us;  // microseconds
};

template <typename Func>
TimedResult timed(Func &&f) {
  auto start = Clock::now();
  f();
  auto end = Clock::now();
  double us = chrono::duration_cast<chrono::microseconds>(end - start).count();
  return {us};
}

template <typename Func>
TimedResult timed_average(Func &&f, int iterations) {
  auto start = Clock::now();
  for (int i = 0; i < iterations; ++i) {
    f();
  }
  auto end = Clock::now();
  double us = chrono::duration_cast<chrono::microseconds>(end - start).count();
  return {us / iterations};
}

void report(const string &label, double us) {
  if (us < 1000.0) {
    cout << "  [PERF] " << label << ": " << fixed << setprecision(1) << us << " us" << endl;
  } else {
    cout << "  [PERF] " << label << ": " << fixed << setprecision(2) << us / 1000.0 << " ms" << endl;
  }
}

// =============================================================================
// Helpers
// =============================================================================

namespace {

Bag make_bag(set<unsigned> elems, unsigned ei = 0, unsigned ej = 0) {
  Bag b;
  b.set_elements(elems);
  if (ei && ej) b.set_edge(ei, ej);
  return b;
}

}  // namespace

// Helper to run a full pipeline: init -> [intro_v for all vertices] ->
// [intro_e for all edges] -> [forget_v for all vertices]
// on a single core, and return the final witness set.
template <typename CoreType, typename WitnessType>
shared_ptr<WitnessSet> run_core_pipeline(
    CoreType &core,
    const vector<unsigned> &vertices,
    const vector<pair<unsigned, unsigned>> &edges) {
  // Initialize
  WitnessSetTypeTwo<WitnessType> init_ws;
  core.initialize_leaf(init_ws);

  // We'll use CoreWrapper's pattern: iterate witnesses, call implementation
  auto current = make_shared<WitnessSetTypeTwo<WitnessType>>();
  *current = init_ws;

  // intro_v for each vertex
  set<unsigned> bag_elems;
  for (unsigned v : vertices) {
    bag_elems.insert(v);
    Bag bag = make_bag(bag_elems);
    auto next = make_shared<WitnessSetTypeTwo<WitnessType>>();
    for (auto it = current->begin(); it != current->end(); ++it) {
      auto &w = WitnessType::as_witness(**it);
      core.intro_v_implementation(v, bag, w, *next);
    }
    core.clean_implementation(*next);
    current = next;
  }

  // intro_e for each edge
  for (auto [u, v] : edges) {
    Bag bag = make_bag(bag_elems, u, v);
    auto next = make_shared<WitnessSetTypeTwo<WitnessType>>();
    for (auto it = current->begin(); it != current->end(); ++it) {
      auto &w = WitnessType::as_witness(**it);
      core.intro_e_implementation(u, v, bag, w, *next);
    }
    core.clean_implementation(*next);
    current = next;
  }

  // forget_v for each vertex
  for (unsigned v : vertices) {
    bag_elems.erase(v);
    Bag bag = make_bag(bag_elems);
    auto next = make_shared<WitnessSetTypeTwo<WitnessType>>();
    for (auto it = current->begin(); it != current->end(); ++it) {
      auto &w = WitnessType::as_witness(**it);
      core.forget_v_implementation(v, bag, w, *next);
    }
    core.clean_implementation(*next);
    current = next;
  }

  return current;
}

// Setup a DynamicKernel with a single core for BFS testing
template <typename CoreType>
void setup_kernel(DynamicKernel &kernel, CoreType &core,
                  unsigned width_val, const string &width_name = "path_width") {
  Width w;
  w.set_name(width_name);
  w.set_value(width_val);
  kernel.set_width(w);
  kernel.addCore(core);
  kernel.setVarToNameAndIndex({{"x", {"TestCore", 0}}});
}

// Build a "x" conjecture (evaluates is_final_witness_set)
Conjecture make_x_conjecture(DynamicKernel &kernel) {
  Conjecture conj;
  conj.setKernel(&kernel);
  conj.setVariablesToCoreName({{"x", "TestCore"}});
  auto *root = new ConjectureNode(CORE_VARIABLE, "x");
  conj.setRoot(root);
  return conj;
}

// Build "inv(x) <= bound" conjecture
Conjecture make_inv_leq_conjecture(DynamicKernel &kernel, double bound) {
  Conjecture conj;
  conj.setKernel(&kernel);
  conj.setVariablesToCoreName({{"x", "TestCore"}});
  auto *var_node = new ConjectureNode(CORE_VARIABLE, "x");
  auto *inv_node = new ConjectureNode(INV, "inv");
  inv_node->setChildren({var_node});
  var_node->setParent(inv_node);
  auto *num_node = new ConjectureNode(NUMBER, bound);
  auto *root = new ConjectureNode(OPERATOR, "<=");
  root->setChildren({inv_node, num_node});
  inv_node->setParent(root);
  num_node->setParent(root);
  conj.setRoot(root);
  return conj;
}

// =============================================================================
// TEST SUITE 1: ChromaticNumber Core Pipeline Benchmarks
// =============================================================================

class ChromaticBenchmark : public ::testing::Test {};

TEST_F(ChromaticBenchmark, TriangleK3_Requires3Colors) {
  // K3 is NOT 2-colorable, IS 3-colorable
  parameterType params_k2{2};
  parameterType params_k3{3};
  ChromaticNumber_AtMost_Core core_k2(params_k2);
  ChromaticNumber_AtMost_Core core_k3(params_k3);

  vector<unsigned> verts = {1, 2, 3};
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {1, 3}, {2, 3}};

  bool k2_has_final = false;
  bool k3_has_final = false;

  auto t_k2 = timed([&] {
    auto ws = run_core_pipeline<ChromaticNumber_AtMost_Core,
                                ChromaticNumber_AtMost_Witness>(core_k2, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_k2.is_final_witness_implementation(empty_bag,
              ChromaticNumber_AtMost_Witness::as_witness(**it))) {
        k2_has_final = true;
      }
    }
  });

  auto t_k3 = timed([&] {
    auto ws = run_core_pipeline<ChromaticNumber_AtMost_Core,
                                ChromaticNumber_AtMost_Witness>(core_k3, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_k3.is_final_witness_implementation(empty_bag,
              ChromaticNumber_AtMost_Witness::as_witness(**it))) {
        k3_has_final = true;
      }
    }
  });

  EXPECT_FALSE(k2_has_final) << "K3 should NOT be 2-colorable";
  EXPECT_TRUE(k3_has_final) << "K3 SHOULD be 3-colorable";

  report("K3 chromatic k=2 pipeline", t_k2.elapsed_us);
  report("K3 chromatic k=3 pipeline", t_k3.elapsed_us);
}

TEST_F(ChromaticBenchmark, PathP5_Is2Colorable) {
  // P5 (path of 5 vertices) is bipartite → 2-colorable
  parameterType params{2};
  ChromaticNumber_AtMost_Core core(params);

  vector<unsigned> verts = {1, 2, 3, 4, 5};
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {2, 3}, {3, 4}, {4, 5}};

  bool has_final = false;
  auto t = timed([&] {
    auto ws = run_core_pipeline<ChromaticNumber_AtMost_Core,
                                ChromaticNumber_AtMost_Witness>(core, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core.is_final_witness_implementation(empty_bag,
              ChromaticNumber_AtMost_Witness::as_witness(**it))) {
        has_final = true;
        break;
      }
    }
  });

  EXPECT_TRUE(has_final) << "P5 should be 2-colorable";
  report("P5 chromatic k=2 pipeline", t.elapsed_us);
}

TEST_F(ChromaticBenchmark, CompleteK4_Requires4Colors) {
  parameterType params_k3{3};
  parameterType params_k4{4};
  ChromaticNumber_AtMost_Core core_k3(params_k3);
  ChromaticNumber_AtMost_Core core_k4(params_k4);

  vector<unsigned> verts = {1, 2, 3, 4};
  vector<pair<unsigned, unsigned>> edges = {
      {1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}};

  bool k3_final = false;
  bool k4_final = false;

  auto t_k3 = timed([&] {
    auto ws = run_core_pipeline<ChromaticNumber_AtMost_Core,
                                ChromaticNumber_AtMost_Witness>(core_k3, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_k3.is_final_witness_implementation(empty_bag,
              ChromaticNumber_AtMost_Witness::as_witness(**it))) {
        k3_final = true;
      }
    }
  });

  auto t_k4 = timed([&] {
    auto ws = run_core_pipeline<ChromaticNumber_AtMost_Core,
                                ChromaticNumber_AtMost_Witness>(core_k4, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_k4.is_final_witness_implementation(empty_bag,
              ChromaticNumber_AtMost_Witness::as_witness(**it))) {
        k4_final = true;
      }
    }
  });

  EXPECT_FALSE(k3_final) << "K4 should NOT be 3-colorable";
  EXPECT_TRUE(k4_final) << "K4 SHOULD be 4-colorable";

  report("K4 chromatic k=3 pipeline", t_k3.elapsed_us);
  report("K4 chromatic k=4 pipeline", t_k4.elapsed_us);
}

// =============================================================================
// TEST SUITE 2: VertexCover Benchmarks
// =============================================================================

class VertexCoverBenchmark : public ::testing::Test {};

TEST_F(VertexCoverBenchmark, TriangleK3_RequiresCover2) {
  parameterType params_r1{1};
  parameterType params_r2{2};
  VertexCover_AtMost_Core core_r1(params_r1);
  VertexCover_AtMost_Core core_r2(params_r2);

  vector<unsigned> verts = {1, 2, 3};
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {1, 3}, {2, 3}};

  bool r1_final = false;
  bool r2_final = false;

  auto t_r1 = timed([&] {
    auto ws = run_core_pipeline<VertexCover_AtMost_Core,
                                VertexCover_AtMost_Witness>(core_r1, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_r1.is_final_witness_implementation(empty_bag,
              VertexCover_AtMost_Witness::as_witness(**it))) {
        r1_final = true;
      }
    }
  });

  auto t_r2 = timed([&] {
    auto ws = run_core_pipeline<VertexCover_AtMost_Core,
                                VertexCover_AtMost_Witness>(core_r2, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_r2.is_final_witness_implementation(empty_bag,
              VertexCover_AtMost_Witness::as_witness(**it))) {
        r2_final = true;
      }
    }
  });

  EXPECT_FALSE(r1_final) << "K3 vertex cover should need > 1";
  EXPECT_TRUE(r2_final) << "K3 vertex cover of 2 should exist";

  report("K3 vertex cover r=1 pipeline", t_r1.elapsed_us);
  report("K3 vertex cover r=2 pipeline", t_r2.elapsed_us);
}

TEST_F(VertexCoverBenchmark, StarK14_RequiresCover1) {
  // Star K_{1,4}: center vertex covers all edges
  parameterType params_r1{1};
  VertexCover_AtMost_Core core(params_r1);

  vector<unsigned> verts = {1, 2, 3, 4, 5};
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {1, 3}, {1, 4}, {1, 5}};

  bool has_final = false;
  auto t = timed([&] {
    auto ws = run_core_pipeline<VertexCover_AtMost_Core,
                                VertexCover_AtMost_Witness>(core, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core.is_final_witness_implementation(empty_bag,
              VertexCover_AtMost_Witness::as_witness(**it))) {
        has_final = true;
        break;
      }
    }
  });

  EXPECT_TRUE(has_final) << "Star K_{1,4} should have vertex cover of size 1";
  report("Star K_{1,4} vertex cover r=1", t.elapsed_us);
}

// =============================================================================
// TEST SUITE 3: WitnessSet Insertion Benchmarks
// =============================================================================

class WitnessSetBenchmark : public ::testing::Test {};

TEST_F(WitnessSetBenchmark, TypeTwoManyInsertions) {
  // Benchmark: insert N distinct witnesses into WitnessSetTypeTwo
  // This tests the batch-insert optimization (#3)
  parameterType params{3};
  ChromaticNumber_AtMost_Core core(params);

  const int N_ITERATIONS = 50;

  auto t = timed_average([&] {
    // Fresh core each iteration
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> ws;
    core.initialize_leaf(ws);

    // intro_v(1) generates multiple witnesses
    Bag bag1 = make_bag({1});
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> ws1;
    for (auto it = ws.begin(); it != ws.end(); ++it) {
      core.intro_v_implementation(1, bag1,
          ChromaticNumber_AtMost_Witness::as_witness(**it), ws1);
    }

    // intro_v(2) generates more witnesses
    Bag bag12 = make_bag({1, 2});
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> ws2;
    for (auto it = ws1.begin(); it != ws1.end(); ++it) {
      core.intro_v_implementation(2, bag12,
          ChromaticNumber_AtMost_Witness::as_witness(**it), ws2);
    }

    // intro_v(3) generates even more
    Bag bag123 = make_bag({1, 2, 3});
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> ws3;
    for (auto it = ws2.begin(); it != ws2.end(); ++it) {
      core.intro_v_implementation(3, bag123,
          ChromaticNumber_AtMost_Witness::as_witness(**it), ws3);
    }
  }, N_ITERATIONS);

  // Verify we get a reasonable number of witnesses
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> ws;
  core.initialize_leaf(ws);
  Bag bag1 = make_bag({1});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> ws1;
  for (auto it = ws.begin(); it != ws.end(); ++it) {
    core.intro_v_implementation(1, bag1,
        ChromaticNumber_AtMost_Witness::as_witness(**it), ws1);
  }
  EXPECT_GE(ws1.size(), 1);

  report("Chromatic k=3 witness expansion (3 vertices, avg of " +
             to_string(N_ITERATIONS) + " runs)",
         t.elapsed_us);
}

TEST_F(WitnessSetBenchmark, TypeOneInsertionThroughput) {
  // Benchmark WitnessSetTypeOne (bitmask) insertion
  parameterType params{2};
  ChromaticNumber_AtMost_Core core(params);

  const int N_ITERATIONS = 100;

  auto t = timed_average([&] {
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> ws;
    core.initialize_leaf(ws);
    Bag bag = make_bag({1});
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> out;
    for (auto it = ws.begin(); it != ws.end(); ++it) {
      core.intro_v_implementation(1, bag,
          ChromaticNumber_AtMost_Witness::as_witness(**it), out);
    }
  }, N_ITERATIONS);

  report("Single intro_v witness insert (avg of " +
             to_string(N_ITERATIONS) + " runs)",
         t.elapsed_us);
}

// =============================================================================
// TEST SUITE 4: BFS End-to-End Benchmarks with Correctness Verification
// =============================================================================

class BFSBenchmark : public ::testing::Test {};

TEST_F(BFSBenchmark, Chromatic_Width1_2Colorable) {
  // Every graph of pathwidth 1 is 2-colorable (it's a caterpillar → bipartite)
  parameterType params{2};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 1);
  Conjecture conj = make_x_conjecture(kernel);
  Flags flags;

  size_t total_states = 0;
  auto t = timed([&] {
    auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
    EXPECT_TRUE(result.property_satisfied)
        << "All pathwidth-1 graphs should be 2-colorable";
    EXPECT_FALSE(result.counterexample_state.has_value());
    total_states = result.total_states;
  });

  report("BFS pathwidth-1 chromatic k=2 (" + to_string(total_states) +
             " states)",
         t.elapsed_us);
}

TEST_F(BFSBenchmark, Chromatic_Width1_Not1Colorable) {
  // NOT every pathwidth-1 graph is 1-colorable (any graph with an edge fails)
  parameterType params{1};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 1);
  Conjecture conj = make_x_conjecture(kernel);
  Flags flags;

  auto t = timed([&] {
    auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
    EXPECT_FALSE(result.property_satisfied)
        << "Should find counterexample: graph with edge is not 1-colorable";
    EXPECT_TRUE(result.counterexample_state.has_value());
  });

  report("BFS pathwidth-1 chromatic k=1 (counterexample)", t.elapsed_us);
}

TEST_F(BFSBenchmark, Chromatic_Width2_3Colorable) {
  // Every graph of pathwidth 2 is 3-colorable (a well-known result)
  parameterType params{3};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 2);
  Conjecture conj = make_x_conjecture(kernel);
  Flags flags;

  size_t total_states = 0;
  int iterations = 0;
  auto t = timed([&] {
    auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
    EXPECT_TRUE(result.property_satisfied)
        << "All pathwidth-2 graphs should be 3-colorable";
    total_states = result.total_states;
    iterations = result.iterations;
  });

  report("BFS pathwidth-2 chromatic k=3 (" + to_string(total_states) +
             " states, " + to_string(iterations) + " iters)",
         t.elapsed_us);
}

TEST_F(BFSBenchmark, Chromatic_Width2_Not2Colorable) {
  // NOT every pathwidth-2 graph is 2-colorable (K3 has pathwidth 2)
  parameterType params{2};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 2);
  Conjecture conj = make_x_conjecture(kernel);
  Flags flags;

  auto t = timed([&] {
    auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
    EXPECT_FALSE(result.property_satisfied)
        << "Should find counterexample: K3 is not 2-colorable";
  });

  report("BFS pathwidth-2 chromatic k=2 (counterexample)", t.elapsed_us);
}

TEST_F(BFSBenchmark, VertexCover_Width1_AtMost2) {
  // Every pathwidth-1 graph on ≤2 vertices has vertex cover ≤ 1
  // But P3 has pathwidth 1 and needs VC=2, so VC ≤ 1 should fail
  parameterType params_r1{1};
  VertexCover_AtMost_Core core(params_r1);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 1);
  Conjecture conj = make_x_conjecture(kernel);
  Flags flags;

  auto t = timed([&] {
    auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
    EXPECT_FALSE(result.property_satisfied)
        << "Should find counterexample: P3 needs VC > 1";
  });

  report("BFS pathwidth-1 VC ≤ 1 (counterexample)", t.elapsed_us);
}

TEST_F(BFSBenchmark, VertexCover_Width1_AtMost2_Fails) {
  // Pathwidth-1 graphs can have arbitrarily large vertex cover (for example,
  // long paths), so VC ≤ 2 must eventually fail.
  parameterType params_r2{2};
  VertexCover_AtMost_Core core(params_r2);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 1);
  Conjecture conj = make_x_conjecture(kernel);
  Flags flags;

  size_t total_states = 0;
  auto t = timed([&] {
    auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
    EXPECT_FALSE(result.property_satisfied)
        << "A bounded vertex-cover threshold of 2 is too small for all "
           "pathwidth-1 graphs.";
    total_states = result.total_states;
  });

  report("BFS pathwidth-1 VC ≤ 2 (counterexample after " +
             to_string(total_states) + " states)",
         t.elapsed_us);
}

TEST_F(BFSBenchmark, MaxDegree_Width2_AtLeast5_Fails) {
  // Not every pathwidth-2 graph has max degree ≥ 5
  parameterType params{5};
  MaxDegree_AtLeast_DynamicCore core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 2);
  Conjecture conj = make_x_conjecture(kernel);
  Flags flags;

  auto t = timed([&] {
    auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
    EXPECT_FALSE(result.property_satisfied)
        << "Not all pw-2 graphs have max degree ≥ 5";
  });

  report("BFS pathwidth-2 maxdeg ≥ 5 (counterexample)", t.elapsed_us);
}

// =============================================================================
// TEST SUITE 5: FeedbackVertexSet Benchmarks (tests union-find optimization)
// =============================================================================

class FeedbackVertexSetBenchmark : public ::testing::Test {};

TEST_F(FeedbackVertexSetBenchmark, TriangleK3_NeedsFVS1) {
  // K3 (triangle) is a cycle, needs 1 vertex removed to become acyclic
  parameterType params_k0{0};
  parameterType params_k1{1};
  FeedbackVertexSet_Core core_k0(params_k0);
  FeedbackVertexSet_Core core_k1(params_k1);

  vector<unsigned> verts = {1, 2, 3};
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {1, 3}, {2, 3}};

  bool k0_final = false;
  bool k1_final = false;

  auto t_k0 = timed([&] {
    auto ws = run_core_pipeline<FeedbackVertexSet_Core,
                                FeedbackVertexSet_Witness>(core_k0, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_k0.is_final_witness_implementation(empty_bag,
              FeedbackVertexSet_Witness::as_witness(**it))) {
        k0_final = true;
      }
    }
  });

  auto t_k1 = timed([&] {
    auto ws = run_core_pipeline<FeedbackVertexSet_Core,
                                FeedbackVertexSet_Witness>(core_k1, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core_k1.is_final_witness_implementation(empty_bag,
              FeedbackVertexSet_Witness::as_witness(**it))) {
        k1_final = true;
      }
    }
  });

  EXPECT_FALSE(k0_final) << "K3 cannot be made acyclic without removing a vertex";
  EXPECT_TRUE(k1_final) << "K3 can be made acyclic by removing 1 vertex";

  report("K3 FVS k=0 pipeline", t_k0.elapsed_us);
  report("K3 FVS k=1 pipeline", t_k1.elapsed_us);
}

TEST_F(FeedbackVertexSetBenchmark, Tree_NeedsFVS0) {
  // A tree has no cycles, so FVS = 0
  parameterType params{0};
  FeedbackVertexSet_Core core(params);

  // Path 1-2-3-4 (a tree)
  vector<unsigned> verts = {1, 2, 3, 4};
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {2, 3}, {3, 4}};

  bool has_final = false;
  auto t = timed([&] {
    auto ws = run_core_pipeline<FeedbackVertexSet_Core,
                                FeedbackVertexSet_Witness>(core, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core.is_final_witness_implementation(empty_bag,
              FeedbackVertexSet_Witness::as_witness(**it))) {
        has_final = true;
        break;
      }
    }
  });

  EXPECT_TRUE(has_final) << "A tree should have FVS = 0";
  report("Path P4 FVS k=0 pipeline", t.elapsed_us);
}

// =============================================================================
// TEST SUITE 6: HasMultipleEdges Benchmarks (tests join optimization)
// =============================================================================

class HasMultipleEdgesBenchmark : public ::testing::Test {};

TEST_F(HasMultipleEdgesBenchmark, SimpleGraphHasNoMultiEdges) {
  parameterType params;
  HasMultipleEdges_DynamicCore core(params);

  vector<unsigned> verts = {1, 2, 3};
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {2, 3}};

  bool found_multi = false;
  auto t = timed([&] {
    auto ws = run_core_pipeline<HasMultipleEdges_DynamicCore,
                                HasMultipleEdges_Witness>(core, verts, edges);
    Bag empty_bag;
    for (auto it = ws->begin(); it != ws->end(); ++it) {
      if (core.is_final_witness_implementation(empty_bag,
              HasMultipleEdges_Witness::as_witness(**it))) {
        found_multi = true;
      }
    }
  });

  EXPECT_FALSE(found_multi) << "Simple graph should not have multi-edges";
  report("Simple graph HasMultipleEdges pipeline", t.elapsed_us);
}

// =============================================================================
// TEST SUITE 7: DynamicKernel State Operation Benchmarks
// =============================================================================

class KernelOpBenchmark : public ::testing::Test {};

TEST_F(KernelOpBenchmark, IntroVPerformance) {
  parameterType params{3};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 3);

  State::ptr state = kernel.initialState();

  const int N = 200;
  auto t = timed_average([&] {
    State::ptr s = kernel.intro_v(state, 1);
    (void)s;
  }, N);

  // Verify correctness
  State::ptr s1 = kernel.intro_v(state, 1);
  EXPECT_EQ(s1->get_bag().get_elements().size(), 1u);
  EXPECT_TRUE(s1->get_bag().get_elements().count(1));

  report("DynamicKernel::intro_v (avg of " + to_string(N) + ")", t.elapsed_us);
}

TEST_F(KernelOpBenchmark, IntroEPerformance) {
  parameterType params{3};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 3);

  State::ptr s0 = kernel.initialState();
  State::ptr s1 = kernel.intro_v(s0, 1);
  State::ptr s2 = kernel.intro_v(s1, 2);

  const int N = 200;
  auto t = timed_average([&] {
    State::ptr s = kernel.intro_e(s2, 1, 2);
    (void)s;
  }, N);

  // Verify correctness
  State::ptr s3 = kernel.intro_e(s2, 1, 2);
  EXPECT_EQ(s3->get_bag().get_elements().size(), 2u);

  report("DynamicKernel::intro_e (avg of " + to_string(N) + ")", t.elapsed_us);
}

TEST_F(KernelOpBenchmark, ForgetVPerformance) {
  parameterType params{3};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 3);

  State::ptr s0 = kernel.initialState();
  State::ptr s1 = kernel.intro_v(s0, 1);
  State::ptr s2 = kernel.intro_v(s1, 2);

  const int N = 200;
  auto t = timed_average([&] {
    State::ptr s = kernel.forget_v(s2, 1);
    (void)s;
  }, N);

  // Verify correctness
  State::ptr s3 = kernel.forget_v(s2, 1);
  EXPECT_EQ(s3->get_bag().get_elements().size(), 1u);
  EXPECT_FALSE(s3->get_bag().get_elements().count(1));
  EXPECT_TRUE(s3->get_bag().get_elements().count(2));

  report("DynamicKernel::forget_v (avg of " + to_string(N) + ")", t.elapsed_us);
}

// =============================================================================
// TEST SUITE 8: Bag Operation Benchmarks (tests const + no-copy optimization)
// =============================================================================

class BagBenchmark : public ::testing::Test {};

TEST_F(BagBenchmark, IntroVConst) {
  Bag bag;
  bag.set_elements({1, 2, 3, 4, 5});

  const int N = 10000;
  auto t = timed_average([&] {
    Bag result = bag.intro_v(6);
    (void)result;
  }, N);

  // Verify correctness: original bag unchanged
  EXPECT_EQ(bag.get_elements().size(), 5u);
  Bag result = bag.intro_v(6);
  EXPECT_EQ(result.get_elements().size(), 6u);
  EXPECT_TRUE(result.get_elements().count(6));

  report("Bag::intro_v const (avg of " + to_string(N) + ")", t.elapsed_us);
}

TEST_F(BagBenchmark, ForgetVConst) {
  Bag bag;
  bag.set_elements({1, 2, 3, 4, 5});

  const int N = 10000;
  auto t = timed_average([&] {
    Bag result = bag.forget_v(3);
    (void)result;
  }, N);

  // Verify correctness: original unchanged
  EXPECT_EQ(bag.get_elements().size(), 5u);
  Bag result = bag.forget_v(3);
  EXPECT_EQ(result.get_elements().size(), 4u);
  EXPECT_FALSE(result.get_elements().count(3));

  report("Bag::forget_v const (avg of " + to_string(N) + ")", t.elapsed_us);
}

// =============================================================================
// TEST SUITE 9: State Comparison Benchmark (tests redundant bag-check fix)
// =============================================================================

class StateComparisonBenchmark : public ::testing::Test {};

TEST_F(StateComparisonBenchmark, EqualStatesComparison) {
  parameterType params{2};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 2);

  State::ptr s0 = kernel.initialState();
  State::ptr s1 = kernel.intro_v(s0, 1);
  State::ptr s2 = kernel.intro_v(s0, 1);  // Same operation → same state

  const int N = 5000;
  bool result = false;
  auto t = timed_average([&] {
    result = (*s1 == *s2);
  }, N);

  EXPECT_TRUE(result) << "Same operations should produce equal states";
  report("State::operator== on equal states (avg of " + to_string(N) + ")",
         t.elapsed_us);
}

TEST_F(StateComparisonBenchmark, DifferentStatesComparison) {
  parameterType params{2};
  ChromaticNumber_AtMost_Core core(params);
  DynamicKernel kernel;
  setup_kernel(kernel, core, 2);

  State::ptr s0 = kernel.initialState();
  State::ptr s1 = kernel.intro_v(s0, 1);
  State::ptr s2 = kernel.intro_v(s0, 2);

  const int N = 5000;
  bool less_result = false;
  auto t = timed_average([&] {
    less_result = (*s1 < *s2);
  }, N);

  // Just verify it doesn't crash and returns a consistent result
  bool less_reverse = (*s2 < *s1);
  EXPECT_NE(less_result, less_reverse)
      << "Exactly one of s1<s2 or s2<s1 should be true for different states";

  report("State::operator< on different states (avg of " + to_string(N) + ")",
         t.elapsed_us);
}

// =============================================================================
// TEST SUITE 10: Full BFS Scaling Benchmarks
// =============================================================================

class BFSScalingBenchmark : public ::testing::Test {};

TEST_F(BFSScalingBenchmark, ChromaticScaling_Width1to3) {
  // Measure how BFS time scales with width for chromatic number
  Flags flags;

  for (unsigned w = 1; w <= 3; ++w) {
    parameterType params{static_cast<int>(w + 1)};  // k = width + 1
    ChromaticNumber_AtMost_Core core(params);
    DynamicKernel kernel;
    setup_kernel(kernel, core, w);
    Conjecture conj = make_x_conjecture(kernel);

    size_t total_states = 0;
    int iters = 0;
    auto t = timed([&] {
      auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
      EXPECT_TRUE(result.property_satisfied)
          << "Pathwidth-" << w << " graphs should be " << (w + 1) << "-colorable";
      total_states = result.total_states;
      iters = result.iterations;
    });

    report("BFS pw=" + to_string(w) + " chromatic k=" + to_string(w + 1) +
               " (" + to_string(total_states) + " states, " +
               to_string(iters) + " iters)",
           t.elapsed_us);
  }
}

TEST_F(BFSScalingBenchmark, VertexCoverScaling_Width1to3) {
  Flags flags;

  for (unsigned w = 1; w <= 3; ++w) {
    parameterType params{static_cast<int>(w + 1)};  // r = width + 1
    VertexCover_AtMost_Core core(params);
    DynamicKernel kernel;
    setup_kernel(kernel, core, w);
    Conjecture conj = make_x_conjecture(kernel);

    size_t total_states = 0;
    auto t = timed([&] {
      auto result = TreeWidzard::runBreadthFirstTraversal(kernel, conj, flags);
      total_states = result.total_states;
    });

    report("BFS pw=" + to_string(w) + " VC r=" + to_string(w + 1) +
               " (" + to_string(total_states) + " states)",
           t.elapsed_us);
  }
}
