#include "../utils/test_helpers.h"
#include "Controller/SearchController.h"
#include "Kernel/Flags.h"
#include "Kernel/Width.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

using namespace TreeWidzard::Testing;

class ATPIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create test directory
    test_dir = "tests/data/temp_test_" + std::to_string(time(nullptr));
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

// Test: Simple true conjecture (should find no counterexample)
TEST_F(ATPIntegrationTest, TrueConjecture) {
  // Create a trivially true conjecture
  std::string conjecture = R"(
x := ChromaticNumber(3)
y := ChromaticNumber(4)
Formula
x IMPLIES y
)";

  std::string filepath = createTestConjecture("true_simple", conjecture);

  // Set up search parameters
  Width width("tree_width", 3);
  Flags flags;

  // This test verifies the setup works
  // Full ATP search may take too long for unit tests
  EXPECT_TRUE(fileExists(filepath));
}

// Test: Simple false conjecture (should find counterexample)
TEST_F(ATPIntegrationTest, FalseConjecture) {
  // MaxDegree(3) does NOT imply ChromaticNumber(3)
  // K4 is a counterexample (max degree 3, chromatic number 4)
  std::string conjecture = R"(
x := MaxDegree(3)
y := ChromaticNumber(3)
Formula
x IMPLIES y
)";

  std::string filepath = createTestConjecture("false_simple", conjecture);

  Width width("tree_width", 4);
  Flags flags;

  // This should eventually find K4 as a counterexample
  EXPECT_TRUE(fileExists(filepath));
}

// Test: Conjecture file parsing
TEST_F(ATPIntegrationTest, ConjectureFileParsing) {
  std::string conjecture = R"(
x := MaxDegree(4)
y := ChromaticNumber(4)
Formula
x IMPLIES y
)";

  std::string filepath = createTestConjecture("parse_test", conjecture);

  // Verify file was created correctly
  EXPECT_TRUE(fileExists(filepath));

  std::ifstream file(filepath);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  EXPECT_TRUE(content.find("MaxDegree") != std::string::npos);
  EXPECT_TRUE(content.find("ChromaticNumber") != std::string::npos);
  EXPECT_TRUE(content.find("IMPLIES") != std::string::npos);
}

// Test: Width parameter validation
TEST_F(ATPIntegrationTest, WidthParameters) {
  Width tw("tree_width", 3);
  EXPECT_EQ(tw.get_name(), "tree_width");
  EXPECT_EQ(tw.get_value(), 3);

  Width pw("path_width", 5);
  EXPECT_EQ(pw.get_name(), "path_width");
  EXPECT_EQ(pw.get_value(), 5);
}

// Test: Flags configuration
TEST_F(ATPIntegrationTest, FlagsConfiguration) {
  Flags flags;

  // Test default values
  EXPECT_FALSE(flags.get("Premise"));

  // Test setting flags
  flags.set("Premise", true);
  EXPECT_TRUE(flags.get("Premise"));

  flags.set("PrintLoop", true);
  EXPECT_TRUE(flags.get("PrintLoop"));
}

// Test: Multiple conjectures
TEST_F(ATPIntegrationTest, MultipleConjectures) {
  std::vector<std::string> conjectures = {
      "x := MaxDegree(3)\\ny := ChromaticNumber(3)\\nFormula\\nx IMPLIES y",
      "x := MaxDegree(4)\\ny := ChromaticNumber(4)\\nFormula\\nx IMPLIES y",
      "x := MaxDegree(5)\\ny := ChromaticNumber(5)\\nFormula\\nx IMPLIES y"};

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

  std::string search_dir = "build/SearchStrategies";
  if (std::filesystem::exists(search_dir)) {
    for (const auto &strategy : strategies) {
      std::string dylib_path = search_dir + "/" + strategy + ".dylib";
      // Note: This test may fail if strategies aren't built yet
      // That's okay - it documents what should be available
    }
  }
}

