// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "Kernel/Bag.h"
#include "Kernel/State.h"
#include "Kernel/WitnessSet.h"
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

namespace TreeWidzard {
namespace Testing {

// Helper to create a bag from a vector of vertices
inline Bag createBag(const std::vector<unsigned> &vertices) {
  Bag bag;
  std::set<unsigned> elements(vertices.begin(), vertices.end());
  bag.set_elements(elements);
  return bag;
}

// Helper to create a state with a specific bag
inline State createState(const std::vector<unsigned> &vertices) {
  State state;
  state.set_bag(createBag(vertices));
  return state;
}

// Helper to compare two bags
inline bool bagsEqual(const Bag &b1, const Bag &b2) {
  return b1.get_elements() == b2.get_elements();
}

// Helper to create a simple graph in PACE format
inline std::string
createPACEGraph(int num_vertices,
                const std::vector<std::pair<int, int>> &edges) {
  std::string result = "p tw " + std::to_string(num_vertices) + " " +
                       std::to_string(edges.size()) + "\\n";
  for (const auto &edge : edges) {
    result +=
        std::to_string(edge.first) + " " + std::to_string(edge.second) + "\\n";
  }
  return result;
}

// Helper to write a test conjecture file
inline void writeConjectureFile(const std::string &filepath,
                                const std::string &content) {
  std::ofstream file(filepath);
  file << content;
  file.close();
}

// Helper to check if a file exists
inline bool fileExists(const std::string &filepath) {
  std::ifstream file(filepath);
  return file.good();
}

// Macro for testing approximate equality (for floating point)
#define EXPECT_NEAR_PERCENT(val1, val2, percent)                               \
  \ EXPECT_NEAR(val1, val2, std::abs(val1) * (percent / 100.0))

// Macro for timing test execution
#define TIME_TEST(test_name)                                                   \
  \ auto start_##test_name = std::chrono::high_resolution_clock::now();        \
  \ auto end_##test_name = std::chrono::high_resolution_clock::now();          \
  \ auto duration_##test_name =                                                \
      std::chrono::duration_cast<                                              \
          std::chrono::milliseconds>( \ end_##test_name - start_##test_name)   \
          .count()

} // namespace Testing
} // namespace TreeWidzard

#endif // TEST_HELPERS_H
