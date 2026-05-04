// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include <gtest/gtest.h>
#include <memory>
#include <set>
#include <map>
#include "DPCores/Source/ChromaticNumber_AtMost.h"
#include "DPCores/Source/MaxDegree_AtLeast.h"
#include "DPCores/Source/CliqueNumber_AtLeast.h"
#include "DPCores/Source/VertexCover_AtMost.h"
#include "DPCores/Source/IndependentSet_Max.h"
#include "DPCores/Source/VertexCount.h"
#include "Kernel/Bag.h"

using namespace std;

// Helper function to create bags with elements and optional edge
namespace {

Bag make_bag(set<unsigned> elems, unsigned ei = 0, unsigned ej = 0) {
    Bag b;
    b.set_elements(elems);
    if (ei && ej) b.set_edge(ei, ej);
    return b;
}

}  // namespace

// ============================================================================
// ChromaticNumber_AtMost Tests
// ============================================================================

class ChromaticNumberTest : public ::testing::Test {
protected:
    parameterType make_params(int k) {
        parameterType params;
        params.push_back(k);
        return params;
    }
};

// Test 1: Triangle (K3) requires 3 colors
TEST_F(ChromaticNumberTest, Triangle_K3_With_k2_NotColorable) {
    ChromaticNumber_AtMost_Core core(make_params(2));
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> initial;
    core.initialize_leaf(initial);
    ASSERT_EQ(initial.size(), 1);

    // After intro_v(1)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }
    ASSERT_EQ(set1.size(), 1);

    // After intro_v(2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }
    ASSERT_GE(set2.size(), 1);

    // After intro_e(1,2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }
    ASSERT_GE(set3.size(), 1);

    // After intro_v(3)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }
    ASSERT_GE(set4.size(), 1);

    // After intro_e(1,3)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }
    ASSERT_GE(set5.size(), 1);

    // After intro_e(2,3) - this should eliminate witnesses with 2 colors
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set6);
    }
    // K3 is not 2-colorable, so set6 should be empty
    ASSERT_EQ(set6.size(), 0);
}

// Test 2: Triangle (K3) with k=3
TEST_F(ChromaticNumberTest, Triangle_K3_With_k3_Colorable) {
    ChromaticNumber_AtMost_Core core(make_params(3));
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    // intro_v(3)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    // intro_e(1,3)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    // intro_e(2,3)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set6);
    }
    // K3 is 3-colorable
    ASSERT_GT(set6.size(), 0);
}

// Test 3: Path graph P3 (1-2-3) is 2-colorable
TEST_F(ChromaticNumberTest, PathP3_With_k2_Colorable) {
    ChromaticNumber_AtMost_Core core(make_params(2));
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    // intro_v(3)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    // intro_e(2,3) - only edge in path graph
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set5);
    }
    // P3 is 2-colorable
    ASSERT_GT(set5.size(), 0);
}

// Test 4: Path P3 with k=1 (trivial single vertex coloring)
TEST_F(ChromaticNumberTest, PathP3_With_k1_NotColorable) {
    ChromaticNumber_AtMost_Core core(make_params(1));
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = ChromaticNumber_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }
    // Single color cannot color even an edge
    ASSERT_EQ(set3.size(), 0);
}

// ============================================================================
// VertexCoverNumber_AtMost tests (VertexCover_AtMost_Core implementation)
// ============================================================================

class VertexCoverTest : public ::testing::Test {
protected:
    parameterType make_params(int r) {
        parameterType params;
        params.push_back(r);
        return params;
    }
};

// Test 5: Single edge requires cover of size >= 1
TEST_F(VertexCoverTest, SingleEdge_With_r1) {
    VertexCover_AtMost_Core core(make_params(1));
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> initial;
    core.initialize_leaf(initial);
    ASSERT_EQ(initial.size(), 1);

    // intro_v(1)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }
    ASSERT_GE(set1.size(), 1);

    // intro_v(2)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }
    ASSERT_GE(set2.size(), 1);

    // intro_e(1,2)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }
    ASSERT_GT(set3.size(), 0);

    // Check that at least one witness has cover size <= 1
    bool found_valid = false;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        if (core.is_final_witness_implementation(make_bag({1, 2}), w)) {
            found_valid = true;
            break;
        }
    }
    ASSERT_TRUE(found_valid);
}

// Test 6: Single edge with r=0 (no cover possible)
TEST_F(VertexCoverTest, SingleEdge_With_r0_NoCover) {
    VertexCover_AtMost_Core core(make_params(0));
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2) must cover edge - cannot do with r=0
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }
    ASSERT_GT(set3.size(), 0);

    bool found_valid = false;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        if (core.is_final_witness_implementation(make_bag({1, 2}), w)) {
            found_valid = true;
            break;
        }
    }
    ASSERT_FALSE(found_valid);
}

// Test 7: Triangle requires cover of size >= 2
TEST_F(VertexCoverTest, Triangle_K3_With_r2) {
    VertexCover_AtMost_Core core(make_params(2));
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    // intro_v(3)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    // intro_e(1,3)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    // intro_e(2,3)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set6);
    }

    // Should have valid covers of size 2 for K3
    bool found_valid = false;
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        if (core.is_final_witness_implementation(make_bag({1, 2, 3}), w)) {
            found_valid = true;
            break;
        }
    }
    ASSERT_TRUE(found_valid);
}

// Test 8: Triangle with r=1 (impossible)
TEST_F(VertexCoverTest, Triangle_K3_With_r1_NoValidCover) {
    VertexCover_AtMost_Core core(make_params(1));
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    // intro_v(3)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    // intro_e(1,3)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    // intro_e(2,3)
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set6);
    }

    // No valid cover of size 1 for K3
    bool found_valid = false;
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        if (core.is_final_witness_implementation(make_bag({1, 2, 3}), w)) {
            found_valid = true;
            break;
        }
    }
    ASSERT_FALSE(found_valid);
}

// ============================================================================
// MaximumDegree_AtLeast tests (MaxDegree_AtLeast_DynamicCore implementation)
// ============================================================================

class MaxDegreeTest : public ::testing::Test {
protected:
    parameterType make_params(int maxDeg) {
        parameterType params;
        params.push_back(maxDeg);
        return params;
    }
};

// Test 9: Star graph K(1,3) has max degree 3
TEST_F(MaxDegreeTest, StarGraph_K13_With_maxDeg3) {
    MaxDegree_AtLeast_DynamicCore core(make_params(3));
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1) - center
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    // intro_v(3)
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    // intro_e(1,3)
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    // intro_v(4)
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(4, make_bag({1, 2, 3, 4}), w, set6);
    }

    // intro_e(1,4)
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set7;
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 4, make_bag({1, 2, 3, 4}, 1, 4), w, set7);
    }

    // After all edges, vertex 1 should have degree 3
    bool found = false;
    for (auto it = set7.begin(); it != set7.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        if (w.found) {
            found = true;
            break;
        }
    }
    ASSERT_TRUE(found);
}

// Test 10: Star graph with maxDeg=4 (not found)
TEST_F(MaxDegreeTest, StarGraph_K13_With_maxDeg4_NotFound) {
    MaxDegree_AtLeast_DynamicCore core(make_params(4));
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> initial;
    core.initialize_leaf(initial);

    // Build the star graph
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(4, make_bag({1, 2, 3, 4}), w, set6);
    }

    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set7;
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 4, make_bag({1, 2, 3, 4}, 1, 4), w, set7);
    }

    // With maxDeg=4, star with degree 3 should not trigger
    bool found = false;
    for (auto it = set7.begin(); it != set7.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        if (w.found) {
            found = true;
            break;
        }
    }
    ASSERT_FALSE(found);
}

// ============================================================================
// CliqueNumber_AtLeast Tests
// ============================================================================

class CliqueNumberTest : public ::testing::Test {
protected:
    parameterType make_params(int cliqueSize) {
        parameterType params;
        params.push_back(cliqueSize);
        return params;
    }
};

// Test 11: Triangle (K3) has clique number 3
TEST_F(CliqueNumberTest, Triangle_K3_Has_CliqueSize3) {
    CliqueNumberSimpleGraphs_AtLeast_Core core(make_params(3));
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    // intro_v(3)
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    // intro_e(1,3)
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    // intro_e(2,3)
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set6);
    }

    // After all edges, should have found clique of size 3
    bool found = false;
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        if (w.found) {
            found = true;
            break;
        }
    }
    ASSERT_TRUE(found);
}

// Test 12: Triangle with clique size 4 (not found)
TEST_F(CliqueNumberTest, Triangle_K3_NoCliqueSize4) {
    CliqueNumberSimpleGraphs_AtLeast_Core core(make_params(4));
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> initial;
    core.initialize_leaf(initial);

    // Build K3
    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set6);
    }

    // K3 has no clique of size 4
    bool found = false;
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = CliqueNumberSimpleGraphs_AtLeast_Witness::as_witness(**it);
        if (w.found) {
            found = true;
            break;
        }
    }
    ASSERT_FALSE(found);
}

// ============================================================================
// VertexCount Tests
// ============================================================================

class VertexCountTest : public ::testing::Test {};

// Test 13: VertexCount counts all vertices correctly
TEST_F(VertexCountTest, CountThreeVertices) {
    VertexCount_Core core(parameterType{});
    WitnessSetTypeTwo<VertexCount_Witness> initial;
    core.initialize_leaf(initial);
    ASSERT_EQ(initial.size(), 1);

    // intro_v(1)
    WitnessSetTypeTwo<VertexCount_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<VertexCount_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_v(3)
    WitnessSetTypeTwo<VertexCount_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set3);
    }

    // forget_v(1)
    WitnessSetTypeTwo<VertexCount_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.forget_v_implementation(1, make_bag({2, 3}), w, set4);
    }

    // forget_v(2)
    WitnessSetTypeTwo<VertexCount_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.forget_v_implementation(2, make_bag({3}), w, set5);
    }

    // forget_v(3)
    WitnessSetTypeTwo<VertexCount_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.forget_v_implementation(3, make_bag({}), w, set6);
    }

    // All witnesses should have count 3
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        ASSERT_EQ(w.vertexCount, 3);
    }
}

// Test 14: VertexCount with join
TEST_F(VertexCountTest, CountWithJoin) {
    VertexCount_Core core(parameterType{});
    WitnessSetTypeTwo<VertexCount_Witness> initial;
    core.initialize_leaf(initial);

    // Create left subtree: 2 vertices
    WitnessSetTypeTwo<VertexCount_Witness> left1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, left1);
    }

    WitnessSetTypeTwo<VertexCount_Witness> left2;
    for (auto it = left1.begin(); it != left1.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, left2);
    }

    // Create right subtree: 2 vertices
    WitnessSetTypeTwo<VertexCount_Witness> right1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, right1);
    }

    WitnessSetTypeTwo<VertexCount_Witness> right2;
    for (auto it = right1.begin(); it != right1.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, right2);
    }

    // Join at bag {1, 2}
    WitnessSetTypeTwo<VertexCount_Witness> joined;
    auto bag = make_bag({1, 2});
    for (auto it1 = left2.begin(); it1 != left2.end(); ++it1) {
        for (auto it2 = right2.begin(); it2 != right2.end(); ++it2) {
            auto &w1 = VertexCount_Witness::as_witness(**it1);
            auto &w2 = VertexCount_Witness::as_witness(**it2);
            core.join_implementation(bag, w1, w2, joined);
        }
    }

    // Should have 2 + 2 - 2 = 2 vertices (2 bag vertices counted once)
    // Actually, for join: total = left + right - bag_size = 2 + 2 - 2 = 2
    ASSERT_EQ(joined.size(), 1);
    for (auto it = joined.begin(); it != joined.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        ASSERT_EQ(w.vertexCount, 2);
    }
}

// ============================================================================
// IndependenceNumber tests (IndependentSet_Max_Core implementation)
// ============================================================================

class IndependentSetTest : public ::testing::Test {};

// Test 15: Path P3 has max independent set size 2
TEST_F(IndependentSetTest, PathP3_MaxIndSetSize2) {
    IndependentSet_Max_Core core(parameterType{});
    WitnessSetTypeTwo<IndependentSet_Max_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v(1)
    WitnessSetTypeTwo<IndependentSet_Max_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v(2)
    WitnessSetTypeTwo<IndependentSet_Max_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // intro_e(1,2)
    WitnessSetTypeTwo<IndependentSet_Max_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    // intro_v(3)
    WitnessSetTypeTwo<IndependentSet_Max_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    // intro_e(2,3)
    WitnessSetTypeTwo<IndependentSet_Max_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set5);
    }

    // inv should return the maximum independent set size found
    // We need to extract a witness from the set
    int max_size = 0;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        int inv_value = core.inv_implementation(make_bag({1, 2, 3}), w);
        max_size = std::max(max_size, inv_value);
    }
    ASSERT_GE(max_size, 2);
}

// Test 16: Triangle K3 has max independent set size 1
TEST_F(IndependentSetTest, TriangleK3_MaxIndSetSize1) {
    IndependentSet_Max_Core core(parameterType{});
    WitnessSetTypeTwo<IndependentSet_Max_Witness> initial;
    core.initialize_leaf(initial);

    // Build K3
    WitnessSetTypeTwo<IndependentSet_Max_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    WitnessSetTypeTwo<IndependentSet_Max_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    WitnessSetTypeTwo<IndependentSet_Max_Witness> set3;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, set3);
    }

    WitnessSetTypeTwo<IndependentSet_Max_Witness> set4;
    for (auto it = set3.begin(); it != set3.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_v_implementation(3, make_bag({1, 2, 3}), w, set4);
    }

    WitnessSetTypeTwo<IndependentSet_Max_Witness> set5;
    for (auto it = set4.begin(); it != set4.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_e_implementation(1, 3, make_bag({1, 2, 3}, 1, 3), w, set5);
    }

    WitnessSetTypeTwo<IndependentSet_Max_Witness> set6;
    for (auto it = set5.begin(); it != set5.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        core.intro_e_implementation(2, 3, make_bag({1, 2, 3}, 2, 3), w, set6);
    }

    // inv should return 1 for K3
    int max_size = 0;
    for (auto it = set6.begin(); it != set6.end(); ++it) {
        auto &w = IndependentSet_Max_Witness::as_witness(**it);
        int inv_value = core.inv_implementation(make_bag({1, 2, 3}), w);
        max_size = std::max(max_size, inv_value);
    }
    ASSERT_EQ(max_size, 1);
}

// ============================================================================
// Join Operation Tests
// ============================================================================

// Test 17: ChromaticNumber join operation
TEST_F(ChromaticNumberTest, JoinOperation) {
    ChromaticNumber_AtMost_Core core(make_params(3));

    // Create witness 1: partition {{1}, {2}}
    auto w1 = make_shared<ChromaticNumber_AtMost_Witness>();
    w1->partialColoring.push_back({1});
    w1->partialColoring.push_back({2});

    // Create witness 2: identical partition {{1}, {2}}
    auto w2 = make_shared<ChromaticNumber_AtMost_Witness>();
    w2->partialColoring.push_back({1});
    w2->partialColoring.push_back({2});

    // Join should succeed since they are compatible
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> joined;
    core.join_implementation(make_bag({1, 2}), *w1, *w2, joined);
    ASSERT_GT(joined.size(), 0);

    // Create incompatible witness 3: partition {{1, 2}}
    auto w3 = make_shared<ChromaticNumber_AtMost_Witness>();
    w3->partialColoring.push_back({1, 2});

    // Join should fail since partitions differ
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> joined2;
    core.join_implementation(make_bag({1, 2}), *w1, *w3, joined2);
    ASSERT_EQ(joined2.size(), 0);
}

// Test 18: VertexCover join operation
TEST_F(VertexCoverTest, JoinOperation) {
    VertexCover_AtMost_Core core(make_params(3));

    // Create witness 1: S = {1}, s = 0
    auto w1 = make_shared<VertexCover_AtMost_Witness>();
    w1->S.insert(1);
    w1->s = 0;

    // Create witness 2: S = {1}, s = 1
    auto w2 = make_shared<VertexCover_AtMost_Witness>();
    w2->S.insert(1);
    w2->s = 1;

    // Join should succeed and combine s values
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> joined;
    core.join_implementation(make_bag({1}), *w1, *w2, joined);
    ASSERT_EQ(joined.size(), 1);

    for (auto it = joined.begin(); it != joined.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        ASSERT_EQ(w.S.size(), 1);
        ASSERT_EQ(w.s, 1);  // 0 + 1 = 1
    }
}

// Test 19: MaxDegree join operation
TEST_F(MaxDegreeTest, JoinOperation) {
    MaxDegree_AtLeast_DynamicCore core(make_params(3));

    // Create witness 1: degree counter {1 -> 1, 2 -> 1}
    auto w1 = make_shared<MaxDegree_AtLeast_Witness>();
    w1->degreeCounter[1] = 1;
    w1->degreeCounter[2] = 1;
    w1->found = false;

    // Create witness 2: degree counter {1 -> 2, 2 -> 2}
    auto w2 = make_shared<MaxDegree_AtLeast_Witness>();
    w2->degreeCounter[1] = 2;
    w2->degreeCounter[2] = 2;
    w2->found = false;

    // Join should combine degrees and detect max degree >= 3
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> joined;
    core.join_implementation(make_bag({1, 2}), *w1, *w2, joined);
    ASSERT_GT(joined.size(), 0);

    // At least one witness should have found = true
    bool found = false;
    for (auto it = joined.begin(); it != joined.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        if (w.found) {
            found = true;
            break;
        }
    }
    ASSERT_TRUE(found);
}

// Test 20: VertexCount join operation with correct computation
TEST_F(VertexCountTest, JoinOperationComputation) {
    VertexCount_Core core(parameterType{});

    // Create witness 1: count = 3
    auto w1 = make_shared<VertexCount_Witness>();
    w1->vertexCount = 3;

    // Create witness 2: count = 2
    auto w2 = make_shared<VertexCount_Witness>();
    w2->vertexCount = 2;

    // Join at bag {1, 2} with size 2
    // Result should be 3 + 2 - 2 = 3
    WitnessSetTypeTwo<VertexCount_Witness> joined;
    auto bag = make_bag({1, 2});
    core.join_implementation(bag, *w1, *w2, joined);
    ASSERT_EQ(joined.size(), 1);

    for (auto it = joined.begin(); it != joined.end(); ++it) {
        auto &w = VertexCount_Witness::as_witness(**it);
        ASSERT_EQ(w.vertexCount, 3);  // 3 + 2 - 2 = 3
    }
}

// ============================================================================
// is_final_witness Tests
// ============================================================================

// Test 21: ChromaticNumber is_final_witness
TEST_F(ChromaticNumberTest, IsFinalWitness) {
    ChromaticNumber_AtMost_Core core(make_params(3));

    auto w = make_shared<ChromaticNumber_AtMost_Witness>();
    w->partialColoring.push_back({1});
    w->partialColoring.push_back({2});

    // is_final_witness always returns true for ChromaticNumber
    ASSERT_TRUE(core.is_final_witness_implementation(make_bag({1, 2}), *w));
}

// Test 22: VertexCover is_final_witness validation
TEST_F(VertexCoverTest, IsFinalWitness_Valid) {
    VertexCover_AtMost_Core core(make_params(2));

    auto w = make_shared<VertexCover_AtMost_Witness>();
    w->S.insert(1);
    w->s = 0;

    // Total cover size = 1 + 0 = 1, which is <= 2
    ASSERT_TRUE(core.is_final_witness_implementation(make_bag({1}), *w));
}

// Test 23: VertexCover is_final_witness invalid
TEST_F(VertexCoverTest, IsFinalWitness_Invalid) {
    VertexCover_AtMost_Core core(make_params(1));

    auto w = make_shared<VertexCover_AtMost_Witness>();
    w->S.insert(1);
    w->s = 1;

    // Total cover size = 1 + 1 = 2, which is > 1
    ASSERT_FALSE(core.is_final_witness_implementation(make_bag({1}), *w));
}

// Test 24: MaxDegree is_final_witness
TEST_F(MaxDegreeTest, IsFinalWitness_Found) {
    MaxDegree_AtLeast_DynamicCore core(make_params(3));

    auto w = make_shared<MaxDegree_AtLeast_Witness>();
    w->found = true;

    // Should be final when found = true
    ASSERT_TRUE(core.is_final_witness_implementation(make_bag({1}), *w));
}

// Test 25: MaxDegree is_final_witness not found
TEST_F(MaxDegreeTest, IsFinalWitness_NotFound) {
    MaxDegree_AtLeast_DynamicCore core(make_params(3));

    auto w = make_shared<MaxDegree_AtLeast_Witness>();
    w->found = false;
    w->degreeCounter[1] = 2;

    // Should not be final when found = false
    ASSERT_FALSE(core.is_final_witness_implementation(make_bag({1}), *w));
}

// Test 26: VertexCount is_final_witness always true
TEST_F(VertexCountTest, IsFinalWitness) {
    VertexCount_Core core(parameterType{});

    auto w = make_shared<VertexCount_Witness>();
    w->vertexCount = 5;

    // is_final_witness always returns true for VertexCount
    ASSERT_TRUE(core.is_final_witness_implementation(make_bag({1, 2, 3}), *w));
}

// ============================================================================
// Edge Cases and Additional Tests
// ============================================================================

// Test 27: Empty graph (single leaf bag)
TEST_F(ChromaticNumberTest, EmptyGraph_SingleLeaf) {
    ChromaticNumber_AtMost_Core core(make_params(1));
    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // Empty graph should have a valid coloring with any k >= 0
    ASSERT_GT(initial.size(), 0);
}

// Test 28: Single vertex graph
TEST_F(VertexCoverTest, SingleVertex_NoCover) {
    VertexCover_AtMost_Core core(make_params(0));
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // Single vertex with no edges needs no cover
    ASSERT_EQ(initial.size(), 1);

    // intro_v without edges
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // Both selections (include/exclude vertex) are valid for single vertex
    ASSERT_GE(set1.size(), 1);
}

// Test 29: Forget operation reduces witness set
TEST_F(ChromaticNumberTest, ForgetOperation) {
    ChromaticNumber_AtMost_Core core(make_params(3));

    auto w = make_shared<ChromaticNumber_AtMost_Witness>();
    w->partialColoring.push_back({1});
    w->partialColoring.push_back({2, 3});

    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output;
    core.forget_v_implementation(3, make_bag({1, 2}), *w, output);

    // After forgetting vertex 3, should have witness with {1} and {2}
    ASSERT_GT(output.size(), 0);
    for (auto it = output.begin(); it != output.end(); ++it) {
        auto &w_out = ChromaticNumber_AtMost_Witness::as_witness(**it);
        // Verify that 3 is no longer in the coloring
        bool contains_3 = false;
        for (const auto &cell : w_out.partialColoring) {
            if (cell.count(3)) {
                contains_3 = true;
                break;
            }
        }
        ASSERT_FALSE(contains_3);
    }
}

TEST_F(ChromaticNumberTest, ForgetOperationCanonicalizesCellOrder) {
    ChromaticNumber_AtMost_Core core(make_params(3));

    auto w = make_shared<ChromaticNumber_AtMost_Witness>();
    w->partialColoring.push_back({1, 3});
    w->partialColoring.push_back({2});

    WitnessSetTypeTwo<ChromaticNumber_AtMost_Witness> output;
    core.forget_v_implementation(1, make_bag({2, 3}), *w, output);

    ASSERT_EQ(output.size(), 1);
    auto &w_out = ChromaticNumber_AtMost_Witness::as_witness(**output.begin());
    ASSERT_EQ(w_out.partialColoring.size(), 2U);
    EXPECT_EQ(w_out.partialColoring[0], std::set<unsigned>({2}));
    EXPECT_EQ(w_out.partialColoring[1], std::set<unsigned>({3}));
}

// Test 30: Complex scenario with multiple operations
TEST_F(VertexCoverTest, ComplexScenario) {
    VertexCover_AtMost_Core core(make_params(2));
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> initial;
    core.initialize_leaf(initial);

    // Build a small graph step by step
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> current = initial;

    for (unsigned v = 1; v <= 2; ++v) {
        WitnessSetTypeTwo<VertexCover_AtMost_Witness> next;
        for (auto it = current.begin(); it != current.end(); ++it) {
            auto &w = VertexCover_AtMost_Witness::as_witness(**it);
            core.intro_v_implementation(v, make_bag({v}), w, next);
        }
        current = next;
    }

    // Add edge
    WitnessSetTypeTwo<VertexCover_AtMost_Witness> after_edge;
    for (auto it = current.begin(); it != current.end(); ++it) {
        auto &w = VertexCover_AtMost_Witness::as_witness(**it);
        core.intro_e_implementation(1, 2, make_bag({1, 2}, 1, 2), w, after_edge);
    }

    // Verify some witnesses remain
    ASSERT_GT(after_edge.size(), 0);
}

// Test 31: Independence of multiple witnesses in a set
TEST_F(MaxDegreeTest, MultipleWitnessesInSet) {
    MaxDegree_AtLeast_DynamicCore core(make_params(2));
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> initial;
    core.initialize_leaf(initial);

    // intro_v creates one witness
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set1;
    for (auto it = initial.begin(); it != initial.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(1, make_bag({1}), w, set1);
    }

    // intro_v can create multiple branches
    WitnessSetTypeTwo<MaxDegree_AtLeast_Witness> set2;
    for (auto it = set1.begin(); it != set1.end(); ++it) {
        auto &w = MaxDegree_AtLeast_Witness::as_witness(**it);
        core.intro_v_implementation(2, make_bag({1, 2}), w, set2);
    }

    // All witnesses in set2 are independent
    int count = 0;
    for (auto it = set2.begin(); it != set2.end(); ++it) {
        count++;
    }
    ASSERT_GE(count, 1);
}

// Test 32: Witness equality and comparison
TEST_F(VertexCountTest, WitnessEqualityAndComparison) {
    VertexCount_Core core(parameterType{});

    auto w1 = make_shared<VertexCount_Witness>();
    w1->vertexCount = 5;

    auto w2 = make_shared<VertexCount_Witness>();
    w2->vertexCount = 5;

    // Check equality
    ASSERT_TRUE(*w1 == *w2);

    // Check inequality
    w2->vertexCount = 6;
    ASSERT_FALSE(*w1 == *w2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
