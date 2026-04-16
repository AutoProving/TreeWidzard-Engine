#include <gtest/gtest.h>
#include <memory>
#include <set>
#include <map>

#include "DPCores/Source/ChromaticNumber_AtMost.h"
#include "DPCores/Source/MaxDegree_AtLeast.h"
#include "DPCores/Source/CliqueNumber_AtLeast.h"
#include "Kernel/Bag.h"

using namespace std;

// ============================================================================
// Helper Functions for Creating Test Data
// ============================================================================

namespace {

// Helper: Create an empty bag for testing
Bag create_empty_bag() {
  Bag bag;
  bag.set_elements(set<unsigned>{});
  return bag;
}

// Helper: Create a bag with specific vertices
Bag create_bag(initializer_list<unsigned> vertices) {
  Bag bag;
  bag.set_elements(set<unsigned>(vertices));
  return bag;
}

// Helper: Create a ChromaticNumber witness with a specific coloring
auto create_chromatic_witness(const vector<set<unsigned>>& coloring)
    -> shared_ptr<ChromaticNumber_AtMost_Witness> {
  auto witness = make_shared<ChromaticNumber_AtMost_Witness>();
  for (const auto& color_class : coloring) {
    witness->partialColoring.push_back(color_class);
  }
  // Sort to maintain canonical form
  std::sort(witness->partialColoring.begin(), witness->partialColoring.end());
  return witness;
}

// Helper: Create a MaxDegree witness with specific degree counters
auto create_max_degree_witness(const map<unsigned, unsigned>& degrees,
                                bool found = false)
    -> shared_ptr<MaxDegree_AtLeast_Witness> {
  auto witness = make_shared<MaxDegree_AtLeast_Witness>();
  witness->degreeCounter = degrees;
  witness->found = found;
  return witness;
}

// Helper: Create a Clique witness with specific partial clique
auto create_clique_witness(const map<unsigned, unsigned>& partial_clique,
                            unsigned size = 0, bool found = false)
    -> shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> {
  auto witness = make_shared<CliqueNumberSimpleGraphs_AtLeast_Witness>();
  witness->partialClique = partial_clique;
  witness->size = size;
  witness->found = found;
  return witness;
}

}  // namespace

// ============================================================================
// ChromaticNumber_AtMost Tests
// ============================================================================

class ChromaticNumberAtMostTest : public ::testing::Test {
 protected:
  ChromaticNumber_AtMost_Core core{parameterType{2}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
};

TEST_F(ChromaticNumberAtMostTest, InitializeLeafProducesEmptyColoring) {
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> witness_set;
  core.initialize_leaf(witness_set);

  EXPECT_EQ(witness_set.size(), 1);
  auto witness_ptr = *witness_set.begin();
  auto& witness = ChromaticNumber_AtMost_Witness::as_witness(*witness_ptr);
  EXPECT_TRUE(witness.partialColoring.empty());
}

TEST_F(ChromaticNumberAtMostTest, IntroVAddVertexToExistingColorClass) {
  auto input_witness = create_chromatic_witness({{1}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.intro_v_implementation(2, bag_v1_2, *input_witness, output_set);

  EXPECT_GE(output_set.size(), 1);

  bool found_existing_class = false;
  for (const auto& w_ptr : output_set) {
    auto& output_witness =
        ChromaticNumber_AtMost_Witness::as_witness(*w_ptr);
    if (output_witness.partialColoring.size() != 1) {
      continue;
    }
    const auto& color_class = *output_witness.partialColoring.begin();
    if (color_class.count(1) && color_class.count(2)) {
      found_existing_class = true;
      break;
    }
  }

  EXPECT_TRUE(found_existing_class);
}

TEST_F(ChromaticNumberAtMostTest, IntroVAddVertexAsNewColorClass) {
  auto input_witness = create_chromatic_witness({{1}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.intro_v_implementation(2, bag_v1_2, *input_witness, output_set);

  EXPECT_GE(output_set.size(), 1);

  bool found_new_class = false;
  for (const auto& w_ptr : output_set) {
    auto& w = ChromaticNumber_AtMost_Witness::as_witness(*w_ptr);
    if (w.partialColoring.size() == 2) {
      found_new_class = true;
      bool has_single_2 = false;
      for (const auto& cc : w.partialColoring) {
        if (cc.size() == 1 && cc.count(2)) {
          has_single_2 = true;
        }
      }
      EXPECT_TRUE(has_single_2);
    }
  }
  EXPECT_TRUE(found_new_class);
}

TEST_F(ChromaticNumberAtMostTest, IntroVNewColorClassRespectsBound) {
  ChromaticNumber_AtMost_Core k1_core{parameterType{1}};
  auto input_witness = create_chromatic_witness({{1}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  k1_core.intro_v_implementation(2, bag_v1_2, *input_witness, output_set);

  for (const auto& w_ptr : output_set) {
    auto& w = ChromaticNumber_AtMost_Witness::as_witness(*w_ptr);
    EXPECT_LE(w.partialColoring.size(), 1);
  }
}

TEST_F(ChromaticNumberAtMostTest, IntroESameColorClassRejectsEdge) {
  auto input_witness = create_chromatic_witness({{1, 2}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(ChromaticNumberAtMostTest, IntroDifferentColorClassesAcceptsEdge) {
  auto input_witness = create_chromatic_witness({{1}, {2}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      ChromaticNumber_AtMost_Witness::as_witness(*output_ptr);
  EXPECT_EQ(output_witness.partialColoring.size(), 2);
}

TEST_F(ChromaticNumberAtMostTest, ForgetVRemovesVertexFromColorClass) {
  auto input_witness = create_chromatic_witness({{1, 2}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      ChromaticNumber_AtMost_Witness::as_witness(*output_ptr);

  bool found_only_2 = false;
  for (const auto& cc : output_witness.partialColoring) {
    if (cc.size() == 1 && cc.count(2)) {
      found_only_2 = true;
    }
  }
  EXPECT_TRUE(found_only_2);
}

TEST_F(ChromaticNumberAtMostTest, ForgetVRemovesEmptyColorClass) {
  auto input_witness = create_chromatic_witness({{1}, {2}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      ChromaticNumber_AtMost_Witness::as_witness(*output_ptr);

  EXPECT_EQ(output_witness.partialColoring.size(), 1);
  auto remaining_class = *output_witness.partialColoring.begin();
  EXPECT_TRUE(remaining_class.count(2));
  EXPECT_FALSE(remaining_class.count(1));
}

TEST_F(ChromaticNumberAtMostTest, JoinEqualColoringsProducesOutput) {
  auto witness1 = create_chromatic_witness({{1, 2}});
  auto witness2 = create_chromatic_witness({{1, 2}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      ChromaticNumber_AtMost_Witness::as_witness(*output_ptr);
  EXPECT_EQ(output_witness.partialColoring, witness1->partialColoring);
}

TEST_F(ChromaticNumberAtMostTest, JoinDifferentColoringsProducesNothing) {
  auto witness1 = create_chromatic_witness({{1, 2}});
  auto witness2 = create_chromatic_witness({{1}, {2}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(ChromaticNumberAtMostTest, IsFinalWitnessAlwaysTrue) {
  auto witness = create_chromatic_witness({{1, 2}});
  EXPECT_TRUE(core.is_final_witness_implementation(bag_v1_2, *witness));

  auto empty_witness = create_chromatic_witness({});
  EXPECT_TRUE(core.is_final_witness_implementation(empty_bag, *empty_witness));
}

TEST_F(ChromaticNumberAtMostTest, CompletePathGraphP3Scenario) {
  // Test building a path graph P3 (3 vertices, 2 edges) with 2-coloring
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> current_set;

  // Start with empty coloring (leaf)
  core.initialize_leaf(current_set);
  EXPECT_EQ(current_set.size(), 1);

  // intro_v(1): add vertex 1 with new color class {1}
  auto temp_set = make_shared<WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness>>();
  for (const auto& w_ptr : current_set) {
    core.intro_v_implementation(1, create_bag({1}),
                                ChromaticNumber_AtMost_Witness::as_witness(*w_ptr),
                                *temp_set);
  }
  current_set = *temp_set;
  EXPECT_EQ(current_set.size(), 1);

  // intro_v(2): add vertex 2, can add to {1} or create new {2}
  temp_set = make_shared<WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness>>();
  for (const auto& w_ptr : current_set) {
    core.intro_v_implementation(2, create_bag({1, 2}),
                                ChromaticNumber_AtMost_Witness::as_witness(*w_ptr),
                                *temp_set);
  }
  current_set = *temp_set;
  EXPECT_GE(current_set.size(), 1);

  // intro_e(1, 2): add edge between 1 and 2
  // This should only keep witnesses where 1 and 2 are in different color classes
  temp_set = make_shared<WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness>>();
  for (const auto& w_ptr : current_set) {
    core.intro_e_implementation(1, 2, create_bag({1, 2}),
                                ChromaticNumber_AtMost_Witness::as_witness(*w_ptr),
                                *temp_set);
  }
  current_set = *temp_set;

  // Ensure we have valid 2-colorings with edge (1,2)
  bool found_valid = false;
  for (const auto& w_ptr : current_set) {
    auto& w = ChromaticNumber_AtMost_Witness::as_witness(*w_ptr);
    bool has_1 = false, has_2 = false;
    for (const auto& cc : w.partialColoring) {
      if (cc.count(1)) has_1 = true;
      if (cc.count(2)) has_2 = true;
    }
    if (has_1 && has_2) {
      found_valid = true;
    }
  }
  EXPECT_TRUE(found_valid);
}

TEST_F(ChromaticNumberAtMostTest, ThreeColorClassesWhenKEquals3) {
  ChromaticNumber_AtMost_Core k3_core{parameterType{3}};
  auto input_witness = create_chromatic_witness({{1}, {2}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  k3_core.intro_v_implementation(3, create_bag({1, 2, 3}), *input_witness,
                                 output_set);

  bool found_third_class = false;
  for (const auto& w_ptr : output_set) {
    auto& w = ChromaticNumber_AtMost_Witness::as_witness(*w_ptr);
    if (w.partialColoring.size() == 3) {
      found_third_class = true;
    }
  }
  EXPECT_TRUE(found_third_class);
}

// ============================================================================
// MaximumDegree_AtLeast tests (MaxDegree_AtLeast_DynamicCore implementation)
// ============================================================================

class MaxDegreeAtLeastTest : public ::testing::Test {
 protected:
  MaxDegree_AtLeast_DynamicCore core{parameterType{3}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
  Bag bag_v1_2_3 = create_bag({1, 2, 3});
};

TEST_F(MaxDegreeAtLeastTest, InitializeLeafCreatesEmptyWitness) {
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> witness_set;
  core.initialize_leaf(witness_set);

  EXPECT_EQ(witness_set.size(), 1);
  auto witness_ptr = *witness_set.begin();
  auto& witness = MaxDegree_AtLeast_Witness::as_witness(*witness_ptr);
  EXPECT_TRUE(witness.degreeCounter.empty());
  EXPECT_FALSE(witness.found);
}

TEST_F(MaxDegreeAtLeastTest, IntroVAddsVertexWithCounterZero) {
  auto input_witness = create_max_degree_witness({});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_EQ(output_witness.degreeCounter.size(), 1);
  EXPECT_EQ(output_witness.degreeCounter[1], 0);
  EXPECT_FALSE(output_witness.found);
}

TEST_F(MaxDegreeAtLeastTest, IntroVWithFoundFlagPreservesBit) {
  auto input_witness = create_max_degree_witness({}, true);
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(MaxDegreeAtLeastTest, IntroEIncrementsDegreeBothVertices) {
  auto input_witness = create_max_degree_witness({{1, 0}, {2, 0}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_EQ(output_witness.degreeCounter[1], 1);
  EXPECT_EQ(output_witness.degreeCounter[2], 1);
  EXPECT_FALSE(output_witness.found);
}

TEST_F(MaxDegreeAtLeastTest, IntroESetFoundWhenDegreeReachesThreshold) {
  MaxDegree_AtLeast_DynamicCore maxdeg2_core{
      parameterType{2}};
  auto input_witness = create_max_degree_witness({{1, 1}, {2, 0}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  maxdeg2_core.intro_e_implementation(1, 2, bag_v1_2, *input_witness,
                                      output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_TRUE(output_witness.found);
  EXPECT_TRUE(output_witness.degreeCounter.empty());
}

TEST_F(MaxDegreeAtLeastTest, IntroEWithFoundFlagPreservesBit) {
  auto input_witness = create_max_degree_witness({{1, 0}, {2, 0}}, true);
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(MaxDegreeAtLeastTest, ForgetVRemovesVertexFromCounter) {
  auto input_witness = create_max_degree_witness({{1, 2}, {2, 1}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_EQ(output_witness.degreeCounter.size(), 1);
  EXPECT_FALSE(output_witness.degreeCounter.count(1));
  EXPECT_EQ(output_witness.degreeCounter[2], 1);
}

TEST_F(MaxDegreeAtLeastTest, ForgetVWithFoundFlagPreservesBit) {
  auto input_witness = create_max_degree_witness({{1, 2}}, true);
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.forget_v_implementation(1, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(MaxDegreeAtLeastTest, JoinWithFoundInFirstWitness) {
  auto witness1 = create_max_degree_witness({}, true);
  auto witness2 = create_max_degree_witness({{1, 1}, {2, 1}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(MaxDegreeAtLeastTest, JoinWithFoundInSecondWitness) {
  auto witness1 = create_max_degree_witness({{1, 1}, {2, 1}});
  auto witness2 = create_max_degree_witness({}, true);
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(MaxDegreeAtLeastTest, JoinSameSizeVertexSetsAddsDegrees) {
  auto witness1 = create_max_degree_witness({{1, 1}, {2, 1}});
  auto witness2 = create_max_degree_witness({{1, 1}, {2, 1}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_EQ(output_witness.degreeCounter[1], 2);
  EXPECT_EQ(output_witness.degreeCounter[2], 2);
}

TEST_F(MaxDegreeAtLeastTest, JoinDifferentVertexSizesMismatchReturnsNothing) {
  auto witness1 = create_max_degree_witness({{1, 1}, {2, 1}});
  auto witness2 = create_max_degree_witness({{1, 1}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(MaxDegreeAtLeastTest, JoinDifferentVertexKeysReturnsNothing) {
  auto witness1 = create_max_degree_witness({{1, 1}, {2, 1}});
  auto witness2 = create_max_degree_witness({{1, 1}, {3, 1}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(MaxDegreeAtLeastTest, JoinCombinedDegreeReachesThreshold) {
  MaxDegree_AtLeast_DynamicCore maxdeg3_core{
      parameterType{3}};
  auto witness1 = create_max_degree_witness({{1, 2}, {2, 1}});
  auto witness2 = create_max_degree_witness({{1, 1}, {2, 1}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  maxdeg3_core.join_implementation(bag_v1_2, *witness1, *witness2,
                                   output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      MaxDegree_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_TRUE(output_witness.found);
  EXPECT_TRUE(output_witness.degreeCounter.empty());
}

TEST_F(MaxDegreeAtLeastTest, IsFinalWitnessOnlyWhenFound) {
  auto witness_not_found =
      create_max_degree_witness({{1, 1}, {2, 1}}, false);
  EXPECT_FALSE(
      core.is_final_witness_implementation(bag_v1_2, *witness_not_found));

  auto witness_found = create_max_degree_witness({}, true);
  EXPECT_TRUE(core.is_final_witness_implementation(empty_bag, *witness_found));
}

// ============================================================================
// CliqueNumber_AtLeast Tests
// ============================================================================

class CliqueNumberAtLeastTest : public ::testing::Test {
 protected:
  CliqueNumberSimpleGraphs_AtLeast_Core core{parameterType{3}};
  Bag empty_bag = create_empty_bag();
  Bag bag_v1 = create_bag({1});
  Bag bag_v1_2 = create_bag({1, 2});
  Bag bag_v1_2_3 = create_bag({1, 2, 3});
};

TEST_F(CliqueNumberAtLeastTest, InitializeLeafCreatesEmptyWitness) {
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> witness_set;
  core.initialize_leaf(witness_set);

  EXPECT_EQ(witness_set.size(), 1);
  auto witness_ptr = *witness_set.begin();
  auto& witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*witness_ptr);
  EXPECT_TRUE(witness.partialClique.empty());
  EXPECT_EQ(witness.size, 0);
  EXPECT_FALSE(witness.found);
}

TEST_F(CliqueNumberAtLeastTest, IntroVAddVertexToClique) {
  auto input_witness =
      create_clique_witness({}, 0, false);  // Empty partial clique, size 0
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  bool found_in_clique = false;
  for (const auto& w_ptr : output_set) {
    auto& w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*w_ptr);
    if (w.partialClique.count(1)) {
      found_in_clique = true;
      EXPECT_EQ(w.size, 1);
    }
  }
  EXPECT_TRUE(found_in_clique);
}

TEST_F(CliqueNumberAtLeastTest, IntroVDoesNotAddWhenCliqueIsFull) {
  CliqueNumberSimpleGraphs_AtLeast_Core clique2_core{
      parameterType{2}};
  auto input_witness = create_clique_witness({{1, 1}}, 2, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  clique2_core.intro_v_implementation(2, bag_v1_2, *input_witness, output_set);

  for (const auto& w_ptr : output_set) {
    auto& w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*w_ptr);
    EXPECT_FALSE(w.partialClique.count(2));
  }
}

TEST_F(CliqueNumberAtLeastTest, IntroVWithFoundFlagPreservesBit) {
  auto input_witness = create_clique_witness({}, 0, true);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.intro_v_implementation(1, bag_v1, *input_witness, output_set);

  for (const auto& w_ptr : output_set) {
    auto& w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*w_ptr);
    EXPECT_TRUE(w.found);
  }
}

TEST_F(CliqueNumberAtLeastTest, IntroEIncrementCountersWhenBothInClique) {
  CliqueNumberSimpleGraphs_AtLeast_Core clique3_core{
      parameterType{3}};
  auto input_witness = create_clique_witness({{1, 0}, {2, 0}}, 2, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  clique3_core.intro_e_implementation(1, 2, bag_v1_2, *input_witness,
                                      output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_EQ(output_witness.partialClique[1], 1);
  EXPECT_EQ(output_witness.partialClique[2], 1);
}

TEST_F(CliqueNumberAtLeastTest, IntroEDetectsCompleteClique) {
  CliqueNumberSimpleGraphs_AtLeast_Core clique2_core{
      parameterType{2}};
  auto input_witness = create_clique_witness({{1, 0}, {2, 0}}, 2, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  clique2_core.intro_e_implementation(1, 2, bag_v1_2, *input_witness,
                                      output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_TRUE(output_witness.found);
  EXPECT_TRUE(output_witness.partialClique.empty());
}

TEST_F(CliqueNumberAtLeastTest, IntroECounterAtLimitDropsWitness) {
  auto input_witness = create_clique_witness({{1, 2}, {2, 0}}, 3, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(CliqueNumberAtLeastTest, IntroEVertexNotInCliqueIgnoresEdge) {
  auto input_witness = create_clique_witness({{1, 0}}, 1, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.intro_e_implementation(1, 2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_FALSE(output_witness.partialClique.count(2));
}

TEST_F(CliqueNumberAtLeastTest, ForgetVRemovesVertexWhenRequiredConditionsMet) {
  CliqueNumberSimpleGraphs_AtLeast_Core clique3_core{
      parameterType{3}};
  auto input_witness = create_clique_witness({{1, 2}, {2, 2}, {3, 2}}, 3, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  clique3_core.forget_v_implementation(1, bag_v1_2_3, *input_witness,
                                       output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_FALSE(output_witness.partialClique.count(1));
  EXPECT_TRUE(output_witness.partialClique.count(2));
  EXPECT_TRUE(output_witness.partialClique.count(3));
}

TEST_F(CliqueNumberAtLeastTest, ForgetVRejectsWhenSizeNotFull) {
  auto input_witness = create_clique_witness({{1, 1}}, 2, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.forget_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(CliqueNumberAtLeastTest, ForgetVRejectsWhenCounterNotMaximum) {
  CliqueNumberSimpleGraphs_AtLeast_Core clique3_core{
      parameterType{3}};
  auto input_witness =
      create_clique_witness({{1, 1}, {2, 2}, {3, 2}}, 3, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  clique3_core.forget_v_implementation(1, bag_v1_2_3, *input_witness,
                                       output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(CliqueNumberAtLeastTest, ForgetVVertexNotInCliquePassesThrough) {
  auto input_witness = create_clique_witness({{1, 2}}, 3, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.forget_v_implementation(2, bag_v1_2, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_EQ(output_witness.partialClique.size(), 1);
  EXPECT_TRUE(output_witness.partialClique.count(1));
}

TEST_F(CliqueNumberAtLeastTest, ForgetVWithFoundFlagPreservesBit) {
  auto input_witness = create_clique_witness({}, 0, true);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.forget_v_implementation(1, bag_v1, *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(CliqueNumberAtLeastTest, JoinWithFoundInFirstWitness) {
  auto witness1 = create_clique_witness({}, 0, true);
  auto witness2 = create_clique_witness({{1, 1}}, 1, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(CliqueNumberAtLeastTest, JoinWithFoundInSecondWitness) {
  auto witness1 = create_clique_witness({{1, 1}}, 1, false);
  auto witness2 = create_clique_witness({}, 0, true);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_TRUE(output_witness.found);
}

TEST_F(CliqueNumberAtLeastTest, JoinCombinesPartialCliquesWithMatchingDomains) {
  CliqueNumberSimpleGraphs_AtLeast_Core clique4_core{
      parameterType{4}};
  auto witness1 = create_clique_witness({{1, 1}, {2, 1}}, 2, false);
  auto witness2 = create_clique_witness({{1, 1}, {2, 1}}, 2, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  clique4_core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);

  EXPECT_EQ(output_witness.partialClique[1], 2);
  EXPECT_EQ(output_witness.partialClique[2], 2);
  EXPECT_EQ(output_witness.size, 2);
}

TEST_F(CliqueNumberAtLeastTest, JoinWithDifferentDomainsFails) {
  auto witness1 = create_clique_witness({{1, 1}}, 1, false);
  auto witness2 = create_clique_witness({{2, 1}}, 1, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1_2, *witness1, *witness2, output_set);

  EXPECT_TRUE(output_set.size() == 0);
}

TEST_F(CliqueNumberAtLeastTest, JoinWithForgettenVerticesInOneWitness) {
  auto witness1 = create_clique_witness({{1, 1}}, 2, false);
  auto witness2 = create_clique_witness({{1, 1}}, 1, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  core.join_implementation(bag_v1, *witness1, *witness2, output_set);

  EXPECT_EQ(output_set.size(), 1);
  auto output_ptr = *output_set.begin();
  auto& output_witness =
      CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(*output_ptr);
  EXPECT_EQ(output_witness.size, 2);  // 2 + 1 - shared bag vertex
}

TEST_F(CliqueNumberAtLeastTest, IsFinalWitnessOnlyWhenFound) {
  auto witness_not_found = create_clique_witness({{1, 1}}, 1, false);
  EXPECT_FALSE(
      core.is_final_witness_implementation(bag_v1, *witness_not_found));

  auto witness_found = create_clique_witness({}, 0, true);
  EXPECT_TRUE(core.is_final_witness_implementation(empty_bag, *witness_found));
}

// ============================================================================
// Edge Cases and Complex Scenarios
// ============================================================================

TEST_F(ChromaticNumberAtMostTest, MultipleIndependentColorClasses) {
  auto input_witness = create_chromatic_witness({{1}, {2}, {3}});
  WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output_set;

  core.intro_e_implementation(1, 2, create_bag({1, 2, 3}), *input_witness,
                              output_set);

  EXPECT_EQ(output_set.size(), 1);
}

TEST_F(MaxDegreeAtLeastTest, HighDegreeVertexDetected) {
  MaxDegree_AtLeast_DynamicCore maxdeg_high{
      parameterType{5}};
  auto input_witness = create_max_degree_witness({{1, 4}, {2, 2}});
  WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> output_set;

  maxdeg_high.intro_e_implementation(1, 3, create_bag({1, 2, 3}),
                                     *input_witness, output_set);

  EXPECT_EQ(output_set.size(), 1);
}

TEST_F(CliqueNumberAtLeastTest, CompleteTriangleK3Scenario) {
  CliqueNumberSimpleGraphs_AtLeast_Core clique3_core{
      parameterType{3}};
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> current_set;

  clique3_core.initialize_leaf(current_set);
  EXPECT_EQ(current_set.size(), 1);

  auto input_witness = create_clique_witness({{1, 0}, {2, 0}, {3, 0}}, 3, false);
  WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> output_set;

  clique3_core.intro_e_implementation(1, 2, bag_v1_2_3, *input_witness,
                                      output_set);
  EXPECT_GE(output_set.size(), 1);
}
