// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include <gtest/gtest.h>
#include <memory>
#include <set>
#include <map>

#include "DPCores/Source/VertexCover_AtMost.h"
#include "DPCores/Source/IndependentSet_Max.h"
#include "DPCores/Source/VertexCount.h"
#include "DPCores/Source/HasMultipleEdges.h"
#include "DPCores/Source/FeedbackVertexSet_AtMost.h"
#include "Kernel/Bag.h"

using namespace std;

// ============================================================================
// Helper Functions for Creating Test Data
// ============================================================================

namespace {

Bag create_empty_bag() {
  Bag bag;
  bag.set_elements(set<unsigned>{});
  return bag;
}

Bag create_bag(initializer_list<unsigned> vertices) {
  Bag bag;
  bag.set_elements(set<unsigned>(vertices));
  return bag;
}

[[maybe_unused]] Bag create_bag_with_edge(initializer_list<unsigned> vertices,
                                          unsigned u, unsigned v) {
  Bag bag;
  bag.set_elements(set<unsigned>(vertices));
  bag.set_edge(u, v);
  return bag;
}

}  // namespace

// ============================================================================
// VertexCoverNumber_AtMost tests (VertexCover_AtMost_Core implementation)
// ============================================================================

class VertexCoverAtMostTest : public ::testing::Test {
 protected:
  VertexCover_AtMost_Core core_r2{parameterType{2}};
  VertexCover_AtMost_Core core_r1{parameterType{1}};
  VertexCover_AtMost_Core core_r0{parameterType{0}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
  Bag bag_v1_2_3 = create_bag({1, 2, 3});
};

TEST_F(VertexCoverAtMostTest, InitializeLeafCreatesEmptyWitness) {
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> ws;
  core_r2.initialize_leaf(ws);

  EXPECT_EQ(ws.size(), 1);
  auto witness_ptr = *ws.begin();
  auto& witness = VertexCover_AtMost_Witness::as_witness(*witness_ptr);
  EXPECT_TRUE(witness.S.empty());
  EXPECT_EQ(witness.s, 0);
}

TEST_F(VertexCoverAtMostTest, IntroVCreatesTwoBranches) {
  auto input_witness = make_shared<VertexCover_AtMost_Witness>();
  input_witness->S.clear();
  input_witness->s = 0;
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;

  core_r2.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 2);
  bool found_selected = false, found_unselected = false;
  for (const auto& w_ptr : output_set) {
    auto& w = VertexCover_AtMost_Witness::as_witness(*w_ptr);
    if (w.S.count(1)) {
      found_selected = true;
      EXPECT_EQ(w.s, 0);
    } else {
      found_unselected = true;
      EXPECT_EQ(w.S.size(), 0);
    }
  }
  EXPECT_TRUE(found_selected && found_unselected);
}

TEST_F(VertexCoverAtMostTest, IntroEBothEndpointsNotSelectedProducesEmpty) {
  auto input_witness = make_shared<VertexCover_AtMost_Witness>();
  input_witness->S.clear();
  input_witness->s = 0;
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;

  core_r2.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 0);
}

TEST_F(VertexCoverAtMostTest, IntroEOneEndpointSelectedPassesThrough) {
  auto input_witness = make_shared<VertexCover_AtMost_Witness>();
  input_witness->S.insert(1);
  input_witness->s = 0;
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;

  core_r2.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCover_AtMost_Witness::as_witness(**output_set.begin());
  EXPECT_TRUE(w.S.count(1));
  EXPECT_EQ(w.s, 0);
}

TEST_F(VertexCoverAtMostTest, IntroEBothEndpointsSelectedPassesThrough) {
  auto input_witness = make_shared<VertexCover_AtMost_Witness>();
  input_witness->S.insert(1);
  input_witness->S.insert(2);
  input_witness->s = 0;
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;

  core_r2.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCover_AtMost_Witness::as_witness(**output_set.begin());
  EXPECT_TRUE(w.S.count(1) && w.S.count(2));
}

TEST_F(VertexCoverAtMostTest, ForgetVWithVertexInSIncrementsS) {
  auto input_witness = make_shared<VertexCover_AtMost_Witness>();
  input_witness->S.insert(1);
  input_witness->S.insert(2);
  input_witness->s = 0;
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;

  core_r2.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCover_AtMost_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.s, 1);
  EXPECT_FALSE(w.S.count(1));
  EXPECT_TRUE(w.S.count(2));
}

TEST_F(VertexCoverAtMostTest, ForgetVWithVertexNotInSPassesThrough) {
  auto input_witness = make_shared<VertexCover_AtMost_Witness>();
  input_witness->S.insert(2);
  input_witness->s = 1;
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;

  core_r2.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCover_AtMost_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.s, 1);
  EXPECT_TRUE(w.S.count(2));
}

TEST_F(VertexCoverAtMostTest, JoinRequiresIdenticalS) {
  auto w1 = make_shared<VertexCover_AtMost_Witness>();
  w1->S.insert(1);
  w1->s = 0;

  auto w2 = make_shared<VertexCover_AtMost_Witness>();
  w2->S.insert(2);
  w2->s = 0;

  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;
  core_r2.join_implementation(empty_bag, *w1, *w2, output_set);

  EXPECT_EQ(output_set.size(), 0);
}

TEST_F(VertexCoverAtMostTest, JoinIdenticalSSumsS) {
  auto w1 = make_shared<VertexCover_AtMost_Witness>();
  w1->S.insert(1);
  w1->s = 1;

  auto w2 = make_shared<VertexCover_AtMost_Witness>();
  w2->S.insert(1);
  w2->s = 2;

  WitnessSetTypeTwo<VertexCover_AtMost_Witness> output_set;
  core_r2.join_implementation(empty_bag, *w1, *w2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCover_AtMost_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.s, 3);
  EXPECT_TRUE(w.S.count(1));
}

TEST_F(VertexCoverAtMostTest, IsFinalWitnessRespectsBudget) {
  auto w = make_shared<VertexCover_AtMost_Witness>();
  w->S.insert(1);
  w->s = 1;

  EXPECT_TRUE(core_r2.is_final_witness_implementation(empty_bag, *w));
  EXPECT_FALSE(core_r1.is_final_witness_implementation(empty_bag, *w));
}

TEST_F(VertexCoverAtMostTest, PathGraphP2VertexCoverSize1) {
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> current;
  core_r1.initialize_leaf(current);

  // Introduce vertex 1
  WitnessSetTypeTwo<VertexCover_AtMost_Witness> next;
  for (auto it = current.begin(); it != current.end(); ++it) {
    auto& w = VertexCover_AtMost_Witness::as_witness(**it);
    core_r1.intro_v_implementation(1, bag_v1, w, next);
  }
  EXPECT_GE(next.size(), 1);

  // Introduce edge 1-2
  current = next;
  next = WitnessSetTypeTwo<VertexCover_AtMost_Witness>();
  for (auto it = current.begin(); it != current.end(); ++it) {
    auto& w = VertexCover_AtMost_Witness::as_witness(**it);
    core_r1.intro_e_implementation(1, 2, bag_v1_2, w, next);
  }
  EXPECT_GE(next.size(), 1);

  // Check that at least one witness survives
  bool found_valid = false;
  for (auto it = next.begin(); it != next.end(); ++it) {
    auto& w = VertexCover_AtMost_Witness::as_witness(**it);
    if (core_r1.is_final_witness_implementation(bag_v1_2, w)) {
      found_valid = true;
    }
  }
  EXPECT_TRUE(found_valid);
}

// ============================================================================
// IndependenceNumber tests (IndependentSet_Max_Core implementation)
// ============================================================================

class IndependentSetMaxTest : public ::testing::Test {
 protected:
  IndependentSet_Max_Core core{parameterType{}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
  Bag bag_v1_2_3 = create_bag({1, 2, 3});
};

TEST_F(IndependentSetMaxTest, InitializeLeafCreatesZeroSize) {
  WitnessSetTypeTwo<IndependentSet_Max_Witness> ws;
  core.initialize_leaf(ws);

  EXPECT_EQ(ws.size(), 1);
  auto& witness = IndependentSet_Max_Witness::as_witness(**ws.begin());
  EXPECT_EQ(witness.size, 0);
  EXPECT_TRUE(witness.used.empty());
}

TEST_F(IndependentSetMaxTest, IntroVCreatesTwoBranches) {
  auto input_witness = make_shared<IndependentSet_Max_Witness>();
  input_witness->size = 0;
  input_witness->used.clear();
  WitnessSetTypeTwo<IndependentSet_Max_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 2);
  bool found_skip = false, found_add = false;
  for (const auto& w_ptr : output_set) {
    auto& w = IndependentSet_Max_Witness::as_witness(*w_ptr);
    if (w.used.count(1)) {
      found_add = true;
      EXPECT_EQ(w.size, 1);
    } else {
      found_skip = true;
      EXPECT_EQ(w.size, 0);
    }
  }
  EXPECT_TRUE(found_skip && found_add);
}

TEST_F(IndependentSetMaxTest, IntroEBothVerticesInSetProducesEmpty) {
  auto input_witness = make_shared<IndependentSet_Max_Witness>();
  input_witness->size = 2;
  input_witness->used.insert(1);
  input_witness->used.insert(2);
  WitnessSetTypeTwo<IndependentSet_Max_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 0);
}

TEST_F(IndependentSetMaxTest, IntroEOneVertexInSetPassesThrough) {
  auto input_witness = make_shared<IndependentSet_Max_Witness>();
  input_witness->size = 1;
  input_witness->used.insert(1);
  WitnessSetTypeTwo<IndependentSet_Max_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = IndependentSet_Max_Witness::as_witness(**output_set.begin());
  EXPECT_TRUE(w.used.count(1));
  EXPECT_EQ(w.size, 1);
}

TEST_F(IndependentSetMaxTest, IntroENeitherVertexInSetPassesThrough) {
  auto input_witness = make_shared<IndependentSet_Max_Witness>();
  input_witness->size = 0;
  input_witness->used.clear();
  WitnessSetTypeTwo<IndependentSet_Max_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = IndependentSet_Max_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.size, 0);
}

TEST_F(IndependentSetMaxTest, ForgetVRemovesFromUsed) {
  auto input_witness = make_shared<IndependentSet_Max_Witness>();
  input_witness->size = 2;
  input_witness->used.insert(1);
  input_witness->used.insert(2);
  WitnessSetTypeTwo<IndependentSet_Max_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = IndependentSet_Max_Witness::as_witness(**output_set.begin());
  EXPECT_FALSE(w.used.count(1));
  EXPECT_TRUE(w.used.count(2));
  EXPECT_EQ(w.size, 2);
}

TEST_F(IndependentSetMaxTest, JoinRequiresIdenticalUsedSet) {
  auto w1 = make_shared<IndependentSet_Max_Witness>();
  w1->size = 1;
  w1->used.insert(1);

  auto w2 = make_shared<IndependentSet_Max_Witness>();
  w2->size = 1;
  w2->used.insert(2);

  WitnessSetTypeTwo<IndependentSet_Max_Witness> output_set;
  core.join_implementation(empty_bag, *w1, *w2, output_set);

  EXPECT_EQ(output_set.size(), 0);
}

TEST_F(IndependentSetMaxTest, JoinIdenticalUsedComputesCorrectSize) {
  auto w1 = make_shared<IndependentSet_Max_Witness>();
  w1->size = 2;
  w1->used.insert(1);
  w1->used.insert(2);

  auto w2 = make_shared<IndependentSet_Max_Witness>();
  w2->size = 2;
  w2->used.insert(1);
  w2->used.insert(2);

  WitnessSetTypeTwo<IndependentSet_Max_Witness> output_set;
  core.join_implementation(bag_v1_2, *w1, *w2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = IndependentSet_Max_Witness::as_witness(**output_set.begin());
  // size1 + size2 - |used| = 2 + 2 - 2 = 2
  EXPECT_EQ(w.size, 2);
}

TEST_F(IndependentSetMaxTest, InvImplementationReturnsSize) {
  auto witness = make_shared<IndependentSet_Max_Witness>();
  witness->size = 5;
  witness->used.insert(1);

  int result = core.inv_implementation(empty_bag, *witness);

  EXPECT_EQ(result, 5);
}

TEST_F(IndependentSetMaxTest, CleanKeepsMoreFlexibleSubsetWitnesses) {
  WitnessSetTypeTwo<IndependentSet_Max_Witness> witness_set;

  auto subset = make_shared<IndependentSet_Max_Witness>();
  subset->size = 1;
  subset->used.insert(1);
  witness_set.insert(subset);

  auto superset = make_shared<IndependentSet_Max_Witness>();
  superset->size = 2;
  superset->used.insert(1);
  superset->used.insert(2);
  witness_set.insert(superset);

  core.clean_implementation(witness_set);

  bool saw_subset = false;
  bool saw_superset = false;
  for (const auto& witness_ptr : witness_set) {
    const auto& witness =
        IndependentSet_Max_Witness::as_witness(*witness_ptr);
    if (witness.size == 1 && witness.used == set<unsigned>({1})) {
      saw_subset = true;
    }
    if (witness.size == 2 && witness.used == set<unsigned>({1, 2})) {
      saw_superset = true;
    }
  }

  EXPECT_TRUE(saw_subset);
  EXPECT_TRUE(saw_superset);
}

TEST_F(IndependentSetMaxTest, CompleteGraphK3MaxIndependentSetSize1) {
  WitnessSetTypeTwo<IndependentSet_Max_Witness> current;
  core.initialize_leaf(current);

  // Introduce vertices 1, 2, 3
  for (unsigned v : {1, 2, 3}) {
    WitnessSetTypeTwo<IndependentSet_Max_Witness> next;
    for (auto it = current.begin(); it != current.end(); ++it) {
      auto& w = IndependentSet_Max_Witness::as_witness(**it);
      core.intro_v_implementation(v, bag_v1_2_3, w, next);
    }
    current = next;
  }

  // Introduce edges 1-2, 1-3, 2-3
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {1, 3}, {2, 3}};
  for (auto [u, v] : edges) {
    WitnessSetTypeTwo<IndependentSet_Max_Witness> next;
    for (auto it = current.begin(); it != current.end(); ++it) {
      auto& w = IndependentSet_Max_Witness::as_witness(**it);
      core.intro_e_implementation(u, v, bag_v1_2_3, w, next);
    }
    current = next;
  }

  int max_size = 0;
  for (auto it = current.begin(); it != current.end(); ++it) {
    auto& w = IndependentSet_Max_Witness::as_witness(**it);
    max_size = max(max_size, w.size);
  }

  EXPECT_EQ(max_size, 1);
}

// ============================================================================
// VertexCount_Core Tests
// ============================================================================

class VertexCountTest : public ::testing::Test {
 protected:
  VertexCount_Core core{parameterType{}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
  Bag bag_v1_2_3 = create_bag({1, 2, 3});
};

TEST_F(VertexCountTest, InitializeLeafCreatesZeroCount) {
  WitnessSetTypeTwo<VertexCount_Witness> ws;
  core.initialize_leaf(ws);

  EXPECT_EQ(ws.size(), 1);
  auto& witness = VertexCount_Witness::as_witness(**ws.begin());
  EXPECT_EQ(witness.vertexCount, 0);
}

TEST_F(VertexCountTest, IntroVIncrementsCount) {
  auto input_witness = make_shared<VertexCount_Witness>();
  input_witness->vertexCount = 0;
  WitnessSetTypeTwo<VertexCount_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCount_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.vertexCount, 1);
}

TEST_F(VertexCountTest, IntroVMultipleTimes) {
  auto input_witness = make_shared<VertexCount_Witness>();
  input_witness->vertexCount = 0;
  WitnessSetTypeTwo<VertexCount_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);
  auto w1 = *output_set.begin();

  output_set = WitnessSetTypeTwo<VertexCount_Witness>();
  core.intro_v_implementation(2, bag_v1_2, VertexCount_Witness::as_witness(*w1), output_set);

  auto& w2 = VertexCount_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w2.vertexCount, 2);
}

TEST_F(VertexCountTest, IntroEDoesNotChangeCount) {
  auto input_witness = make_shared<VertexCount_Witness>();
  input_witness->vertexCount = 2;
  WitnessSetTypeTwo<VertexCount_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCount_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.vertexCount, 2);
}

TEST_F(VertexCountTest, ForgetVDoesNotChangeCount) {
  auto input_witness = make_shared<VertexCount_Witness>();
  input_witness->vertexCount = 3;
  WitnessSetTypeTwo<VertexCount_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2_3, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCount_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.vertexCount, 3);
}

TEST_F(VertexCountTest, JoinCombinesCountsMinusBagSize) {
  auto w1 = make_shared<VertexCount_Witness>();
  w1->vertexCount = 3;

  auto w2 = make_shared<VertexCount_Witness>();
  w2->vertexCount = 3;

  WitnessSetTypeTwo<VertexCount_Witness> output_set;
  core.join_implementation(bag_v1_2, *w1, *w2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = VertexCount_Witness::as_witness(**output_set.begin());
  // 3 + 3 - 2 = 4
  EXPECT_EQ(w.vertexCount, 4);
}

TEST_F(VertexCountTest, InvImplementationReturnsVertexCount) {
  auto witness = make_shared<VertexCount_Witness>();
  witness->vertexCount = 7;

  int result = core.inv_implementation(empty_bag, *witness);

  EXPECT_EQ(result, 7);
}

// ============================================================================
// HasMultipleEdges_DynamicCore Tests
// ============================================================================

class HasMultipleEdgesTest : public ::testing::Test {
 protected:
  HasMultipleEdges_DynamicCore core{parameterType{}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
  Bag bag_v1_2_3 = create_bag({1, 2, 3});
};

TEST_F(HasMultipleEdgesTest, InitializeLeafCreatesEmptyEdgeSet) {
  WitnessSetTypeTwo<HasMultipleEdges_Witness> ws;
  core.initialize_leaf(ws);

  EXPECT_EQ(ws.size(), 1);
  auto& witness = HasMultipleEdges_Witness::as_witness(**ws.begin());
  EXPECT_TRUE(witness.edgeSet.empty());
  EXPECT_FALSE(witness.found);
}

TEST_F(HasMultipleEdgesTest, IntroVPassesThroughUnchanged) {
  auto input_witness = make_shared<HasMultipleEdges_Witness>();
  input_witness->edgeSet.clear();
  input_witness->found = false;
  WitnessSetTypeTwo<HasMultipleEdges_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = HasMultipleEdges_Witness::as_witness(**output_set.begin());
  EXPECT_TRUE(w.edgeSet.empty());
  EXPECT_FALSE(w.found);
}

TEST_F(HasMultipleEdgesTest, FirstIntroEAddsEdgeToSet) {
  auto input_witness = make_shared<HasMultipleEdges_Witness>();
  input_witness->edgeSet.clear();
  input_witness->found = false;
  WitnessSetTypeTwo<HasMultipleEdges_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = HasMultipleEdges_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.edgeSet.size(), 1);
  EXPECT_FALSE(w.found);
  EXPECT_TRUE(w.edgeSet.count({1, 2}));
}

TEST_F(HasMultipleEdgesTest, DuplicateIntroESetsFound) {
  auto input_witness = make_shared<HasMultipleEdges_Witness>();
  input_witness->edgeSet.insert({1, 2});
  input_witness->found = false;
  WitnessSetTypeTwo<HasMultipleEdges_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = HasMultipleEdges_Witness::as_witness(**output_set.begin());
  EXPECT_TRUE(w.found);
  EXPECT_TRUE(w.edgeSet.empty());
}

TEST_F(HasMultipleEdgesTest, DuplicateIntroEReversedEdgeSetsFound) {
  auto input_witness = make_shared<HasMultipleEdges_Witness>();
  input_witness->edgeSet.insert({1, 2});
  input_witness->found = false;
  WitnessSetTypeTwo<HasMultipleEdges_Witness> output_set;

  core.intro_e_implementation(2, 1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = HasMultipleEdges_Witness::as_witness(**output_set.begin());
  EXPECT_TRUE(w.found);
}

TEST_F(HasMultipleEdgesTest, ForgetVWithFoundPassesThrough) {
  auto input_witness = make_shared<HasMultipleEdges_Witness>();
  input_witness->found = true;
  input_witness->edgeSet.clear();
  WitnessSetTypeTwo<HasMultipleEdges_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = HasMultipleEdges_Witness::as_witness(**output_set.begin());
  EXPECT_TRUE(w.found);
}

TEST_F(HasMultipleEdgesTest, ForgetVRemovesIncidentEdges) {
  auto input_witness = make_shared<HasMultipleEdges_Witness>();
  input_witness->edgeSet.insert({1, 2});
  input_witness->edgeSet.insert({1, 3});
  input_witness->found = false;
  WitnessSetTypeTwo<HasMultipleEdges_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2_3, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = HasMultipleEdges_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.edgeSet.size(), 0);
  EXPECT_FALSE(w.found);
}

TEST_F(HasMultipleEdgesTest, IsFinalWitnessReturnFound) {
  auto w_found = make_shared<HasMultipleEdges_Witness>();
  w_found->found = true;

  auto w_not_found = make_shared<HasMultipleEdges_Witness>();
  w_not_found->found = false;

  EXPECT_TRUE(core.is_final_witness_implementation(empty_bag, *w_found));
  EXPECT_FALSE(core.is_final_witness_implementation(empty_bag, *w_not_found));
}

// ============================================================================
// FeedbackVertexSet_Core Tests
// ============================================================================

class FeedbackVertexSetTest : public ::testing::Test {
 protected:
  FeedbackVertexSet_Core core_k2{parameterType{2}};
  FeedbackVertexSet_Core core_k1{parameterType{1}};
  FeedbackVertexSet_Core core_k0{parameterType{0}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
  Bag bag_v1_2_3 = create_bag({1, 2, 3});
};

TEST_F(FeedbackVertexSetTest, InitializeLeafCreatesEmptyCompAndRemovedZero) {
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> ws;
  core_k2.initialize_leaf(ws);

  EXPECT_EQ(ws.size(), 1);
  auto& witness = FeedbackVertexSet_Witness::as_witness(**ws.begin());
  EXPECT_TRUE(witness.comp.empty());
  EXPECT_EQ(witness.removed, 0);
}

TEST_F(FeedbackVertexSetTest, IntroVCreatesTwoBranches) {
  auto input_witness = make_shared<FeedbackVertexSet_Witness>();
  input_witness->comp.clear();
  input_witness->removed = 0;
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;

  core_k2.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 2);
  bool found_keep = false, found_remove = false;
  for (const auto& w_ptr : output_set) {
    auto& w = FeedbackVertexSet_Witness::as_witness(*w_ptr);
    if (w.comp.count(1)) {
      found_keep = true;
      EXPECT_EQ(w.comp[1], 1);
      EXPECT_EQ(w.removed, 0);
    } else {
      found_remove = true;
      EXPECT_EQ(w.removed, 1);
    }
  }
  EXPECT_TRUE(found_keep && found_remove);
}

TEST_F(FeedbackVertexSetTest, IntroVRemoveBranchRespectsKBudget) {
  auto input_witness = make_shared<FeedbackVertexSet_Witness>();
  input_witness->comp.clear();
  input_witness->removed = 1;
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;

  core_k1.intro_v_implementation(2, bag_v1_2, *input_witness, output_set);

  // Only keep branch should survive (remove would make removed=2 > k=1)
  int remove_count = 0;
  for (const auto& w_ptr : output_set) {
    auto& w = FeedbackVertexSet_Witness::as_witness(*w_ptr);
    if (w.removed > 1) remove_count++;
  }
  EXPECT_EQ(remove_count, 0);
}

TEST_F(FeedbackVertexSetTest, IntroEConnectsDifferentComponents) {
  auto input_witness = make_shared<FeedbackVertexSet_Witness>();
  input_witness->comp[1] = 1;
  input_witness->comp[2] = 2;
  input_witness->removed = 0;
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;

  core_k2.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = FeedbackVertexSet_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.comp[1], w.comp[2]);
}

TEST_F(FeedbackVertexSetTest, IntroESameComponentProducesEmpty) {
  auto input_witness = make_shared<FeedbackVertexSet_Witness>();
  input_witness->comp[1] = 1;
  input_witness->comp[2] = 1;
  input_witness->removed = 0;
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;

  core_k2.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 0);
}

TEST_F(FeedbackVertexSetTest, IntroEVertexNotInCompPassesThrough) {
  auto input_witness = make_shared<FeedbackVertexSet_Witness>();
  input_witness->comp[1] = 1;
  input_witness->removed = 0;
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;

  core_k2.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = FeedbackVertexSet_Witness::as_witness(**output_set.begin());
  EXPECT_EQ(w.comp[1], 1);
}

TEST_F(FeedbackVertexSetTest, ForgetVRemovesVertexFromComp) {
  auto input_witness = make_shared<FeedbackVertexSet_Witness>();
  input_witness->comp[1] = 1;
  input_witness->comp[2] = 1;
  input_witness->removed = 0;
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;

  core_k2.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto& w = FeedbackVertexSet_Witness::as_witness(**output_set.begin());
  EXPECT_FALSE(w.comp.count(1));
  EXPECT_TRUE(w.comp.count(2));
}

TEST_F(FeedbackVertexSetTest, IsFinalWitnessRespectsBudget) {
  auto w_valid = make_shared<FeedbackVertexSet_Witness>();
  w_valid->removed = 1;
  w_valid->comp.clear();

  auto w_invalid = make_shared<FeedbackVertexSet_Witness>();
  w_invalid->removed = 3;
  w_invalid->comp.clear();

  EXPECT_TRUE(core_k2.is_final_witness_implementation(empty_bag, *w_valid));
  EXPECT_FALSE(core_k2.is_final_witness_implementation(empty_bag, *w_invalid));
}

TEST_F(FeedbackVertexSetTest, SimpleTriangleFVS) {
  WitnessSetTypeTwo<FeedbackVertexSet_Witness> current;
  core_k1.initialize_leaf(current);

  // Introduce vertices 1, 2, 3
  for (unsigned v : {1, 2, 3}) {
    WitnessSetTypeTwo<FeedbackVertexSet_Witness> next;
    for (auto it = current.begin(); it != current.end(); ++it) {
      auto& w = FeedbackVertexSet_Witness::as_witness(**it);
      core_k1.intro_v_implementation(v, bag_v1_2_3, w, next);
    }
    current = next;
  }

  // Introduce edges 1-2, 2-3, 3-1
  vector<pair<unsigned, unsigned>> edges = {{1, 2}, {2, 3}, {1, 3}};
  for (auto [u, v] : edges) {
    WitnessSetTypeTwo<FeedbackVertexSet_Witness> next;
    for (auto it = current.begin(); it != current.end(); ++it) {
      auto& w = FeedbackVertexSet_Witness::as_witness(**it);
      core_k1.intro_e_implementation(u, v, bag_v1_2_3, w, next);
    }
    current = next;
  }

  // Check that at least one valid FVS of size <= 1 exists
  bool found_valid = false;
  for (auto it = current.begin(); it != current.end(); ++it) {
    auto& w = FeedbackVertexSet_Witness::as_witness(**it);
    if (core_k1.is_final_witness_implementation(bag_v1_2_3, w)) {
      found_valid = true;
    }
  }
  EXPECT_TRUE(found_valid);
}

TEST_F(FeedbackVertexSetTest, JoinRequiresSameKeptVertices) {
  auto w1 = make_shared<FeedbackVertexSet_Witness>();
  w1->comp[1] = 1;
  w1->comp[2] = 1;
  w1->removed = 0;

  auto w2 = make_shared<FeedbackVertexSet_Witness>();
  w2->comp[1] = 1;
  w2->comp[3] = 3;
  w2->removed = 0;

  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;
  core_k2.join_implementation(bag_v1_2, *w1, *w2, output_set);

  EXPECT_EQ(output_set.size(), 0);
}

TEST_F(FeedbackVertexSetTest, JoinIdenticalKeptVertices) {
  auto w1 = make_shared<FeedbackVertexSet_Witness>();
  w1->comp[1] = 1;
  w1->comp[2] = 2;
  w1->removed = 1;

  auto w2 = make_shared<FeedbackVertexSet_Witness>();
  w2->comp[1] = 1;
  w2->comp[2] = 2;
  w2->removed = 0;

  WitnessSetTypeTwo<FeedbackVertexSet_Witness> output_set;
  core_k2.join_implementation(bag_v1_2, *w1, *w2, output_set);

  EXPECT_GE(output_set.size(), 1);
}
