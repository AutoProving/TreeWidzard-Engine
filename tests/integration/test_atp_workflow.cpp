// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "../utils/test_helpers.h"
#include "Kernel/Flags.h"
#include "Kernel/Width.h"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>
#include <gtest/gtest.h>

using namespace TreeWidzard::Testing;

class ATPIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create test directory
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    const auto stamp =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    test_dir = "tests/data/temp_test_" + std::to_string(stamp);
    std::filesystem::create_directories(test_dir);
  }

  void TearDown() override {
    // Clean up test directory
    if (std::filesystem::exists(test_dir)) {
      std::filesystem::remove_all(test_dir);
    }
  }

  std::string createTestConjecture(const std::string &name,
                                   const std::string &content) {
    std::string filepath = test_dir + "/" + name + ".txt";
    writeConjectureFile(filepath, content);
    return filepath;
  }

  std::string test_dir;
};

namespace {

auto dynamic_library_extension() -> const char * {
#ifdef _WIN32
  return ".dll";
#elif __APPLE__
  return ".dylib";
#else
  return ".so";
#endif
}

} // namespace

// Test: Simple conjecture file creation
TEST_F(ATPIntegrationTest, ConjectureFileCreation) {
  std::string conjecture = R"(x := ChromaticNumber_AtMost(3)
y := ChromaticNumber_AtMost(4)
Formula
x IMPLIES y)";

  std::string filepath = createTestConjecture("true_simple", conjecture);
  EXPECT_TRUE(fileExists(filepath));
}

// Test: Conjecture file parsing
TEST_F(ATPIntegrationTest, ConjectureFileParsing) {
  std::string conjecture = R"(x := MaximumDegree_AtLeast(4)
y := ChromaticNumber_AtMost(4)
Formula
x IMPLIES y)";

  std::string filepath = createTestConjecture("parse_test", conjecture);

  // Verify file was created correctly
  EXPECT_TRUE(fileExists(filepath));

  std::ifstream file(filepath);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  EXPECT_TRUE(content.find("MaximumDegree_AtLeast") != std::string::npos);
  EXPECT_TRUE(content.find("ChromaticNumber_AtMost") != std::string::npos);
  EXPECT_TRUE(content.find("IMPLIES") != std::string::npos);
}

// Test: Width parameter API
TEST_F(ATPIntegrationTest, WidthParameters) {
  Width tw;
  tw.set_name("tree_width");
  tw.set_value(3);

  EXPECT_EQ(tw.get_name(), "tree_width");
  EXPECT_EQ(tw.get_value(), 3);

  Width pw;
  pw.set_name("path_width");
  pw.set_value(5);

  EXPECT_EQ(pw.get_name(), "path_width");
  EXPECT_EQ(pw.get_value(), 5);
}

// Test: Flags configuration
TEST_F(ATPIntegrationTest, FlagsConfiguration) {
  Flags flags;

  // Test setting flags
  flags.add_flag("Premise", 1.0f);
  EXPECT_EQ(flags.get("Premise"), 1.0f);

  flags.add_flag("PrintLoop", 1.0f);
  EXPECT_EQ(flags.get("PrintLoop"), 1.0f);

  // Test default value (-1.0 for non-existent flags)
  EXPECT_EQ(flags.get("NonExistent"), -1.0f);
}

// Test: Multiple conjectures
TEST_F(ATPIntegrationTest, MultipleConjectures) {
  std::vector<std::string> conjectures = {
      "x := MaximumDegree_AtLeast(3)\ny := ChromaticNumber_AtMost(3)\nFormula\nx IMPLIES y",
      "x := MaximumDegree_AtLeast(4)\ny := ChromaticNumber_AtMost(4)\nFormula\nx IMPLIES y",
      "x := MaximumDegree_AtLeast(5)\ny := ChromaticNumber_AtMost(5)\nFormula\nx IMPLIES y"};

  for (size_t i = 0; i < conjectures.size(); i++) {
    std::string filepath =
        createTestConjecture("conj_" + std::to_string(i), conjectures[i]);
    EXPECT_TRUE(fileExists(filepath));
  }
}

// Test: Search strategy availability
TEST_F(ATPIntegrationTest, SearchStrategyAvailability) {
  // Check if search strategies are built
  std::vector<std::string> strategies = {
      "BreadthFirstSearch", "ParallelBreadthFirstSearch",
      "IsomorphismBreadthFirstSearch", "ParallelIsomorphismBreadthFirstSearch"};

  std::vector<std::filesystem::path> search_dirs;
  if (const char *env_dir = std::getenv("TREEWIDZARD_SEARCHSTRATEGIES")) {
    search_dirs.emplace_back(env_dir);
  }
  search_dirs.emplace_back("out/build/default/SearchStrategies");
  search_dirs.emplace_back("out/build/release/SearchStrategies");
  search_dirs.emplace_back("out/build/debug/SearchStrategies");
  search_dirs.emplace_back("out/build/ci/SearchStrategies");
  search_dirs.emplace_back("build/SearchStrategies");

  for (const auto &search_dir : search_dirs) {
    if (!std::filesystem::exists(search_dir)) {
      continue;
    }

    int found_count = 0;
    for (const auto &strategy : strategies) {
      const auto library_path =
          search_dir / (strategy + dynamic_library_extension());
      if (std::filesystem::exists(library_path)) {
        found_count++;
      }
    }
    // At least some strategies should be built
    EXPECT_GT(found_count, 0);
    return;
  }
}

// Test: Width comparison operators
TEST_F(ATPIntegrationTest, WidthComparison) {
  Width w1, w2;
  w1.set_name("tree_width");
  w1.set_value(3);

  w2.set_name("tree_width");
  w2.set_value(5);

  EXPECT_TRUE(w1 < w2);
  EXPECT_TRUE(w2 > w1);
  EXPECT_TRUE(w1 != w2);

  Width w3;
  w3.set_name("tree_width");
  w3.set_value(3);

  EXPECT_TRUE(w1 == w3);
}
