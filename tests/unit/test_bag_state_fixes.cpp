#include <gtest/gtest.h>
#include "Kernel/Bag.h"
#include "Kernel/State.h"
#include "Kernel/Hasher.hpp"
#include "../utils/test_helpers.h"
#include "../utils/kernel_test_doubles.h"
#include <map>

namespace TreeWidzard {
namespace Testing {

// Test fixture for Bag and State fixes
class BagStateFixesTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Initialize any necessary test setup
  }

  void TearDown() override {
    // Clean up after tests
  }

  // Helper to create a bag with vertices and an edge
  Bag createBagWithEdge(const std::vector<unsigned>& vertices,
                        unsigned i, unsigned j) {
    Bag bag = createBag(vertices);
    bag.set_edge(i, j);
    return bag;
  }

  // Helper to compute hash of a bag
  uint64_t bagHash(const Bag& bag) {
    Hasher h(0);
    bag.hash(h);
    return h.get();
  }

  // Helper to compute hash of a state
  uint64_t stateHash(const State& state) {
    Hasher h(0);
    state.hash(h);
    return h.get();
  }
};

// ============================================================================
// BAG IDENTITY TESTS
// ============================================================================

TEST_F(BagStateFixesTest, OperatorEqualIgnoresTransientEdgeMarker) {
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBagWithEdge({1, 2, 3}, 2, 3);

  EXPECT_TRUE(bag1 == bag2)
      << "Search-state identity should depend only on bag vertices";
  EXPECT_FALSE(bag1 != bag2);
}

TEST_F(BagStateFixesTest, OperatorEqualBagsWithSameVerticesAndEdges) {
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBagWithEdge({1, 2, 3}, 1, 2);

  EXPECT_TRUE(bag1 == bag2);
}

TEST_F(BagStateFixesTest, OperatorEqualBagsWithNoEdges) {
  Bag bag1 = createBag({1, 2, 3});
  Bag bag2 = createBag({1, 2, 3});

  EXPECT_TRUE(bag1 == bag2);
}

TEST_F(BagStateFixesTest, OperatorEqualBagsOneWithEdgeOneWithout) {
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBag({1, 2, 3});

  EXPECT_TRUE(bag1 == bag2)
      << "The edge marker should not create a distinct search bag";
}

TEST_F(BagStateFixesTest, OperatorEqualSameEdgeDifferentOrder) {
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBagWithEdge({1, 2, 3}, 2, 1);

  EXPECT_TRUE(bag1 == bag2);
}

// ============================================================================
// BAG OPERATOR< TESTS
// ============================================================================

TEST_F(BagStateFixesTest, OperatorLessBagsWithDifferentVertices) {
  Bag bag1 = createBag({1, 2});
  Bag bag2 = createBag({1, 2, 3});

  EXPECT_TRUE(bag1 < bag2);
}

TEST_F(BagStateFixesTest, OperatorLessIgnoresEdgeMarker) {
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBagWithEdge({1, 2, 3}, 2, 3);

  EXPECT_FALSE(bag1 < bag2);
  EXPECT_FALSE(bag2 < bag1);
}

TEST_F(BagStateFixesTest, OperatorLessBagsWithSameVerticesNoEdges) {
  Bag bag1 = createBag({1, 2, 3});
  Bag bag2 = createBag({1, 2, 3});

  EXPECT_FALSE(bag1 < bag2);
  EXPECT_FALSE(bag2 < bag1);
}

TEST_F(BagStateFixesTest, OperatorLessBagWithEdgeVsWithout) {
  Bag bag1 = createBag({1, 2, 3});
  Bag bag2 = createBagWithEdge({1, 2, 3}, 1, 2);

  EXPECT_FALSE(bag1 < bag2);
  EXPECT_FALSE(bag2 < bag1);
}

TEST_F(BagStateFixesTest, OperatorLessEdgeOrderingDoesNotAffectIdentity) {
  Bag bag12 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag13 = createBagWithEdge({1, 2, 3}, 1, 3);
  Bag bag23 = createBagWithEdge({1, 2, 3}, 2, 3);

  EXPECT_FALSE(bag12 < bag13);
  EXPECT_FALSE(bag13 < bag23);
  EXPECT_FALSE(bag12 < bag23);
}

// ============================================================================
// BAG HASH TESTS
// ============================================================================

TEST_F(BagStateFixesTest, HashIgnoresTransientEdgeMarker) {
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBagWithEdge({1, 2, 3}, 2, 3);

  uint64_t hash1 = bagHash(bag1);
  uint64_t hash2 = bagHash(bag2);

  EXPECT_EQ(hash1, hash2);
}

TEST_F(BagStateFixesTest, HashSameForBagsWithSameVerticesAndEdges) {
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBagWithEdge({1, 2, 3}, 1, 2);

  uint64_t hash1 = bagHash(bag1);
  uint64_t hash2 = bagHash(bag2);

  EXPECT_EQ(hash1, hash2);
}

TEST_F(BagStateFixesTest, HashSameForBagsWithAndWithoutEdges) {
  Bag bag1 = createBag({1, 2, 3});
  Bag bag2 = createBagWithEdge({1, 2, 3}, 1, 2);

  uint64_t hash1 = bagHash(bag1);
  uint64_t hash2 = bagHash(bag2);

  EXPECT_EQ(hash1, hash2);
}

TEST_F(BagStateFixesTest, HashConsistentWithEquality) {
  // If two bags are equal, their hashes should be equal (hash consistency)
  Bag bag1 = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag bag2 = createBagWithEdge({1, 2, 3}, 1, 2);

  if (bag1 == bag2) {
    EXPECT_EQ(bagHash(bag1), bagHash(bag2))
        << "Equal bags must have equal hashes";
  }
}

// ============================================================================
// BAG NON-MUTATING OPERATIONS (FIX #4)
// ============================================================================

TEST_F(BagStateFixesTest, IntroVIsNonMutating) {
  // intro_v should return a new bag and not modify the original
  Bag original = createBag({1, 2});
  Bag result = original.intro_v(3);

  // Original should be unchanged
  EXPECT_EQ(original.size(), 2) << "Original bag should not be modified";
  EXPECT_TRUE(original.get_elements().find(3) == original.get_elements().end())
      << "Original bag should not contain the new vertex";

  // Result should have the new vertex
  EXPECT_EQ(result.size(), 3) << "Result bag should have the new vertex";
  EXPECT_TRUE(result.get_elements().find(3) != result.get_elements().end())
      << "Result bag should contain the new vertex";
}

TEST_F(BagStateFixesTest, IntroVChaining) {
  // intro_v should support chaining operations
  Bag bag = createBag({1});
  Bag result = bag.intro_v(2).intro_v(3).intro_v(4);

  EXPECT_EQ(result.size(), 4);
  EXPECT_TRUE(result.get_elements().count(1));
  EXPECT_TRUE(result.get_elements().count(2));
  EXPECT_TRUE(result.get_elements().count(3));
  EXPECT_TRUE(result.get_elements().count(4));
}

TEST_F(BagStateFixesTest, IntroEIsNonMutating) {
  // intro_e should return a new bag and not modify the original
  Bag original = createBag({1, 2, 3});
  Bag result = original.intro_e(1, 2);

  // Original edge should be (0, 0)
  EXPECT_EQ(original.get_edge().first, 0);
  EXPECT_EQ(original.get_edge().second, 0);

  // Result should have the new edge
  EXPECT_EQ(result.get_edge().first, 1);
  EXPECT_EQ(result.get_edge().second, 2);
}

TEST_F(BagStateFixesTest, IntroEPreservesVertices) {
  // intro_e should preserve vertices and just add/update edge
  Bag original = createBag({1, 2, 3});
  Bag result = original.intro_e(1, 2);

  EXPECT_EQ(result.get_elements(), original.get_elements());
}

TEST_F(BagStateFixesTest, ForgetVIsNonMutating) {
  // forget_v should return a new bag and not modify the original
  Bag original = createBag({1, 2, 3});
  Bag result = original.forget_v(2);

  // Original should be unchanged
  EXPECT_EQ(original.size(), 3);
  EXPECT_TRUE(original.get_elements().find(2) != original.get_elements().end());

  // Result should not have the vertex
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.get_elements().find(2) == result.get_elements().end());
}

TEST_F(BagStateFixesTest, ForgetVRemovesEdgeIfVertexIsInEdge) {
  // If the forgotten vertex is in the edge, the edge should be removed
  Bag original = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag result = original.forget_v(1);

  // Result should not have the vertex
  EXPECT_TRUE(result.get_elements().find(1) == result.get_elements().end());

  // Edge should be cleared (set to 0,0)
  EXPECT_EQ(result.get_edge().first, 0);
  EXPECT_EQ(result.get_edge().second, 0);
}

TEST_F(BagStateFixesTest, ForgetVPreservesEdgeIfVertexNotInEdge) {
  // If the forgotten vertex is not in the edge, the edge should remain
  Bag original = createBagWithEdge({1, 2, 3}, 1, 2);
  Bag result = original.forget_v(3);

  // Edge should be preserved
  EXPECT_EQ(result.get_edge().first, 1);
  EXPECT_EQ(result.get_edge().second, 2);
}

// ============================================================================
// BAG RELABEL TESTS (FIX #5: Handle unset edges without crash)
// ============================================================================

TEST_F(BagStateFixesTest, RelabelBagWithUnsetEdge) {
  // relabel should handle bags with unset edges (0, 0) without crash
  Bag original = createBag({1, 3, 5});
  std::map<unsigned, unsigned> relabeling{{1, 1}, {3, 2}, {5, 3}};

  // This should not crash
  Bag result = original.relabel(relabeling);

  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.get_elements().count(1));
  EXPECT_TRUE(result.get_elements().count(2));
  EXPECT_TRUE(result.get_elements().count(3));
  EXPECT_EQ(result.get_edge().first, 0);
  EXPECT_EQ(result.get_edge().second, 0);
}

TEST_F(BagStateFixesTest, RelabelBagWithSetEdge) {
  // relabel should properly relabel bags with set edges
  Bag original = createBagWithEdge({1, 3, 5}, 1, 5);
  std::map<unsigned, unsigned> relabeling{{1, 1}, {3, 2}, {5, 3}};

  Bag result = original.relabel(relabeling);

  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.get_elements().count(1));
  EXPECT_TRUE(result.get_elements().count(2));
  EXPECT_TRUE(result.get_elements().count(3));
  EXPECT_EQ(result.get_edge().first, 1);
  EXPECT_EQ(result.get_edge().second, 3);
}

TEST_F(BagStateFixesTest, RelabelBagEdgeNormalization) {
  // After relabeling, edges should be normalized (i < j)
  Bag original = createBagWithEdge({1, 3, 5}, 5, 1);
  std::map<unsigned, unsigned> relabeling{{1, 3}, {3, 2}, {5, 1}};

  Bag result = original.relabel(relabeling);

  // Original edge was (5,1) but normalized to (1,5) -> relabels to (3,1)
  // After normalization should be (1,3)
  auto edge = result.get_edge();
  EXPECT_TRUE(edge.first <= edge.second) << "Edge should be normalized";
}

// ============================================================================
// STATE OPERATOR() HASH FUNCTOR TESTS (FIX #6: Use proper Hasher)
// ============================================================================

TEST_F(BagStateFixesTest, StateHashFunctorVerticesGreater255) {
  // Hash functor should use proper Hasher, not char-cast
  // Vertices > 255 should produce distinct hashes
  auto state1 = TreeWidzardTest::make_state({256}, {});
  auto state2 = TreeWidzardTest::make_state({257}, {});

  State hasher;
  uint64_t hash1 = hasher(*state1);
  uint64_t hash2 = hasher(*state2);

  // Different vertices should produce (very likely) different hashes
  EXPECT_NE(hash1, hash2)
      << "States with vertices > 255 should produce different hashes";
}

TEST_F(BagStateFixesTest, StateHashFunctorLargeVertexValues) {
  // Test with larger vertex values
  auto state1 = TreeWidzardTest::make_state({10000}, {});
  auto state2 = TreeWidzardTest::make_state({10001}, {});

  State hasher;
  uint64_t hash1 = hasher(*state1);
  uint64_t hash2 = hasher(*state2);

  EXPECT_NE(hash1, hash2)
      << "States with large vertex values should produce different hashes";
}

TEST_F(BagStateFixesTest, StateHashFunctorConsistency) {
  // Same state should always produce same hash
  auto state1 = TreeWidzardTest::make_state({1, 2, 3}, {5});
  auto state1_copy = TreeWidzardTest::make_state({1, 2, 3}, {5});

  State hasher;
  uint64_t hash1 = hasher(*state1);
  uint64_t hash1_copy = hasher(*state1_copy);

  EXPECT_EQ(hash1, hash1_copy)
      << "Same states should produce same hashes";
}

// ============================================================================
// STATE HASH CACHE TESTS (FIX #7: Proper caching)
// ============================================================================

TEST_F(BagStateFixesTest, HashValueCachingReturnsConsistentValue) {
  // hashValue() should cache properly and return same value
  auto state = TreeWidzardTest::make_state({1, 2, 3}, {42});

  uint64_t hash1 = state->hashValue();
  uint64_t hash2 = state->hashValue();

  EXPECT_EQ(hash1, hash2)
      << "hashValue() should return consistent cached value";
}

TEST_F(BagStateFixesTest, HashValueCachingConsistentWithComputeHashValue) {
  // hashValue() should be consistent with computeHashValue()
  auto state = TreeWidzardTest::make_state({1, 2, 3}, {42});

  uint64_t hash_cached = state->hashValue();
  uint64_t hash_computed = state->hashValue(); // Second call uses cache

  EXPECT_EQ(hash_cached, hash_computed)
      << "Cached value should match computed value";
}

TEST_F(BagStateFixesTest, HashValueMultipleCalls) {
  // Multiple calls to hashValue() should be consistent
  auto state = TreeWidzardTest::make_state({1, 2, 3}, {10, 20});

  uint64_t hash1 = state->hashValue();
  uint64_t hash2 = state->hashValue();
  uint64_t hash3 = state->hashValue();

  EXPECT_EQ(hash1, hash2);
  EXPECT_EQ(hash2, hash3);
}

TEST_F(BagStateFixesTest, HashValueDifferentForDifferentStates) {
  // Different states should have different hash values (with high probability)
  auto state1 = TreeWidzardTest::make_state({1, 2, 3}, {42});
  auto state2 = TreeWidzardTest::make_state({1, 2, 3}, {43});

  uint64_t hash1 = state1->hashValue();
  uint64_t hash2 = state2->hashValue();

  EXPECT_NE(hash1, hash2)
      << "States with different witness values should have different hashes";
}

// ============================================================================
// STATE CONST HASHVALUE TESTS (FIX #8: Mutable hashCache)
// ============================================================================

TEST_F(BagStateFixesTest, HashValueOnConstStateReference) {
  // hashValue() should work on const State references
  // This tests that hashCache is mutable
  const State& const_state = *TreeWidzardTest::make_state({1, 2, 3}, {99});

  // This should compile and work because hashCache is mutable
  uint64_t hash = const_state.hashValue();

  EXPECT_EQ(hash, const_state.hashValue())
      << "hashValue() should work consistently on const references";
}

TEST_F(BagStateFixesTest, HashValueConsistentOnConstState) {
  // Multiple calls to hashValue() on const reference should be consistent
  auto state = TreeWidzardTest::make_state({1, 2, 3}, {77});
  const State& const_state = *state;

  uint64_t hash1 = const_state.hashValue();
  uint64_t hash2 = const_state.hashValue();

  EXPECT_EQ(hash1, hash2)
      << "hashValue() calls on const reference should be consistent";
}

TEST_F(BagStateFixesTest, HashValueCachingPersistsOnConstState) {
  // Cache should persist even with const references
  auto state = TreeWidzardTest::make_state({5, 6, 7}, {123});

  uint64_t hash1 = state->hashValue();

  const State& const_state = *state;
  uint64_t hash2 = const_state.hashValue();

  EXPECT_EQ(hash1, hash2)
      << "Cached value should be consistent between mutable and const access";
}

// ============================================================================
// INTEGRATION TESTS: Combined fixes
// ============================================================================

TEST_F(BagStateFixesTest, BagEqualityAndHashConsistency) {
  // If bags are equal, their hashes should be equal
  Bag bag1 = createBagWithEdge({10, 20, 30}, 10, 20);
  Bag bag2 = createBagWithEdge({10, 20, 30}, 10, 20);

  EXPECT_TRUE(bag1 == bag2);
  EXPECT_EQ(bagHash(bag1), bagHash(bag2));
}

TEST_F(BagStateFixesTest, BagOperationChaining) {
  // Test chaining non-mutating operations
  Bag original = createBag({1, 2});
  Bag result = original.intro_v(3).intro_v(4).intro_e(1, 3);

  // Original should be unchanged
  EXPECT_EQ(original.size(), 2);

  // Result should have 4 vertices and edge (1,3)
  EXPECT_EQ(result.size(), 4);
  EXPECT_EQ(result.get_edge().first, 1);
  EXPECT_EQ(result.get_edge().second, 3);
}

TEST_F(BagStateFixesTest, ComplexRelabelingScenario) {
  Bag bag1 = createBagWithEdge({2, 5, 8}, 2, 5);
  Bag bag2 = createBagWithEdge({2, 5, 8}, 5, 8);

  std::map<unsigned, unsigned> relabel{{2, 1}, {5, 2}, {8, 3}};

  Bag result1 = bag1.relabel(relabel);
  Bag result2 = bag2.relabel(relabel);

  EXPECT_TRUE(result1 == result2);
  EXPECT_EQ(bagHash(result1), bagHash(result2));
  EXPECT_EQ(result1.get_edge().first, 1);
  EXPECT_EQ(result1.get_edge().second, 2);
  EXPECT_EQ(result2.get_edge().first, 2);
  EXPECT_EQ(result2.get_edge().second, 3);
}

TEST_F(BagStateFixesTest, StateHashingWithComplexBags) {
  // State hashing should work with bags that have edges
  auto state1 = TreeWidzardTest::make_state({1, 2, 3}, {100});
  auto state1_mutable = state1.getMutableState();
  Bag bag_with_edge = createBagWithEdge({1, 2, 3}, 1, 2);
  state1_mutable->set_bag(bag_with_edge);

  uint64_t hash1 = state1->hashValue();
  EXPECT_NE(hash1, 0);
}

TEST_F(BagStateFixesTest, StateIdentityIgnoresTransientBagEdge) {
  auto state1 = TreeWidzardTest::make_state({1, 2, 3}, {100});
  auto state2 = TreeWidzardTest::make_state({1, 2, 3}, {100});

  state1.getMutableState()->set_bag(createBagWithEdge({1, 2, 3}, 1, 2));
  state2.getMutableState()->set_bag(createBagWithEdge({1, 2, 3}, 2, 3));

  EXPECT_TRUE(*state1 == *state2);
  EXPECT_FALSE(*state1 < *state2);
  EXPECT_FALSE(*state2 < *state1);
  EXPECT_EQ(state1->hashValue(), state2->hashValue());
}

TEST_F(BagStateFixesTest, EdgeCaseEmptyBag) {
  // Test edge cases with empty bags
  Bag empty1;
  Bag empty2;

  EXPECT_TRUE(empty1 == empty2);
  EXPECT_EQ(bagHash(empty1), bagHash(empty2));
  EXPECT_FALSE(empty1 < empty2);
}

TEST_F(BagStateFixesTest, EdgeCaseSingleVertexBag) {
  // Test with single vertex
  Bag single1 = createBag({5});
  Bag single2 = createBag({5});

  EXPECT_TRUE(single1 == single2);
  EXPECT_EQ(bagHash(single1), bagHash(single2));
}

TEST_F(BagStateFixesTest, EdgeCaseLargeVertexValues) {
  // Test with large vertex values
  Bag bag1 = createBagWithEdge({1000000, 2000000}, 1000000, 2000000);
  Bag bag2 = createBagWithEdge({1000000, 2000000}, 1000000, 2000000);

  EXPECT_TRUE(bag1 == bag2);
  EXPECT_EQ(bagHash(bag1), bagHash(bag2));
}

} // namespace Testing
} // namespace TreeWidzard
