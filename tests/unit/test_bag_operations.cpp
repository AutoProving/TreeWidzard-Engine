// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "../utils/test_helpers.h"
#include "Kernel/Bag.h"
#include <gtest/gtest.h>
#include <set>
#include <string>
#include <vector>

using namespace TreeWidzard::Testing;

class BagTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Common setup for all tests
  }
};

// Test: Bag creation and element access
TEST_F(BagTest, CreateAndAccessElements) {
  std::vector<unsigned> vertices = {1, 2, 3, 4};
  Bag bag = createBag(vertices);

  std::set<unsigned> elements = bag.get_elements();
  EXPECT_EQ(elements.size(), 4);
  EXPECT_TRUE(elements.count(1) > 0);
  EXPECT_TRUE(elements.count(2) > 0);
  EXPECT_TRUE(elements.count(3) > 0);
  EXPECT_TRUE(elements.count(4) > 0);
}

// Test: Bag size
TEST_F(BagTest, BagSize) {
  Bag empty_bag = createBag({});
  EXPECT_EQ(empty_bag.size(), 0);

  Bag small_bag = createBag({1, 2, 3});
  EXPECT_EQ(small_bag.size(), 3);

  Bag large_bag = createBag({1, 2, 3, 4, 5, 6, 7, 8});
  EXPECT_EQ(large_bag.size(), 8);
}

// Test: Bag equality
TEST_F(BagTest, BagEquality) {
  Bag bag1 = createBag({1, 2, 3});
  Bag bag2 = createBag({1, 2, 3});
  Bag bag3 = createBag({1, 2, 4});

  EXPECT_TRUE(bag1 == bag2);
  EXPECT_FALSE(bag1 == bag3);
}

// Test: Bag ordering (for use in sets/maps)
TEST_F(BagTest, BagOrdering) {
  Bag bag1 = createBag({1, 2});
  Bag bag2 = createBag({1, 2, 3});
  Bag bag3 = createBag({2, 3, 4});

  // Smaller bags should come first
  EXPECT_TRUE(bag1 < bag2);

  // Lexicographic ordering for same size
  EXPECT_TRUE(bag2 < bag3);
}

// Test: Set elements
TEST_F(BagTest, SetElements) {
  Bag bag;
  std::set<unsigned> elements = {5, 6, 7};
  bag.set_elements(elements);

  EXPECT_EQ(bag.size(), 3);
  EXPECT_EQ(bag.get_elements(), elements);
}

// Test: Edge operations
TEST_F(BagTest, EdgeOperations) {
  Bag bag;
  bag.set_edge(2, 1);

  auto edge = bag.get_edge();
  EXPECT_EQ(edge.first, 1);
  EXPECT_EQ(edge.second, 2);
}

TEST_F(BagTest, BagPrintingAndInformation) {
  Bag bag = createBag({1, 3, 5});

  testing::internal::CaptureStdout();
  bag.print();
  const std::string printed = testing::internal::GetCapturedStdout();

  EXPECT_EQ(printed, "{1,3,5}");
  EXPECT_EQ(bag.bagInformation(), "{1,3,5}");
}

TEST_F(BagTest, IntroduceForgetAndRelabelOperations) {
  Bag bag = createBag({1, 2});

  EXPECT_TRUE(bag.vertex_introducible(3));
  EXPECT_FALSE(bag.vertex_introducible(2));
  EXPECT_TRUE(bag.edge_introducible(1, 2));
  EXPECT_FALSE(bag.edge_introducible(1, 4));
  EXPECT_TRUE(bag.vertex_forgettable(1));
  EXPECT_FALSE(bag.vertex_forgettable(4));

  Bag with_vertex = bag.intro_v(3);
  EXPECT_EQ(with_vertex.get_elements(), std::set<unsigned>({1, 2, 3}));

  Bag with_edge = with_vertex.intro_e(3, 1);
  EXPECT_EQ(with_edge.get_edge(), std::make_pair(1U, 3U));

  Bag forgot_vertex = with_edge.forget_v(3);
  EXPECT_EQ(forgot_vertex.get_elements(), std::set<unsigned>({1, 2}));
  EXPECT_EQ(forgot_vertex.get_edge(), std::make_pair(0U, 0U));

  Bag other = createBag({1, 2});
  Bag different = createBag({1, 4});
  EXPECT_TRUE(forgot_vertex.joinable(other));
  EXPECT_FALSE(forgot_vertex.joinable(different));

  Bag relabel_source = createBag({1, 3});
  relabel_source.set_edge(1, 3);
  Bag relabeled = relabel_source.relabel({{1, 4}, {3, 7}});
  EXPECT_EQ(relabeled.get_elements(), std::set<unsigned>({4, 7}));
  EXPECT_EQ(relabeled.get_edge(), std::make_pair(4U, 7U));
}

// Test: Duplicate elements (sets should remove duplicates)
TEST_F(BagTest, DuplicateElements) {
  std::set<unsigned> elements_with_duplicates = {1, 2, 2, 3, 3, 3};
  Bag bag;
  bag.set_elements(elements_with_duplicates);

  EXPECT_EQ(bag.size(), 3); // Should only have 1, 2, 3
}

// Test: Large bag
TEST_F(BagTest, LargeBag) {
  std::vector<unsigned> large_vertices;
  for (unsigned i = 0; i < 100; i++) {
    large_vertices.push_back(i);
  }

  Bag large_bag = createBag(large_vertices);
  EXPECT_EQ(large_bag.size(), 100);
}

// Test: Bag hashing (for use in hash tables)
TEST_F(BagTest, BagHashing) {
  Bag bag1 = createBag({1, 2, 3});
  Bag bag2 = createBag({1, 2, 3});
  Bag bag3 = createBag({1, 2, 4});

  Hasher h1(0), h2(0), h3(0);
  bag1.hash(h1);
  bag2.hash(h2);
  bag3.hash(h3);

  // Same bags should have same hash
  EXPECT_EQ(h1.get(), h2.get());

  // Different bags should (likely) have different hashes
  EXPECT_NE(h1.get(), h3.get());
}
