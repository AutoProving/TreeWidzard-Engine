#include "TreeWidzard.h"
#include "Controller/InputController.h"
#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/Width.h"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <sys/wait.h>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace {

auto find_repo_root() -> fs::path {
  fs::path current = fs::current_path();
  for (int depth = 0; depth < 10; ++depth) {
    if (fs::exists(current / "treewidzard.sh") &&
        fs::exists(current / "CMakeLists.txt")) {
      return current;
    }
    if (!current.has_parent_path()) {
      break;
    }
    current = current.parent_path();
  }
  throw std::runtime_error("could not locate repo root");
}

auto shell_quote(const std::string &value) -> std::string {
  std::string quoted;
  quoted.reserve(value.size() + 2);
  quoted.push_back('\'');
  for (const char c : value) {
    if (c == '\'') {
      quoted += "'\\''";
    } else {
      quoted.push_back(c);
    }
  }
  quoted.push_back('\'');
  return quoted;
}

auto run_cmd(const std::string &command) -> int {
  const int rc = std::system(command.c_str());
  if (rc == -1) {
    return 127;
  }
  if (WIFEXITED(rc)) {
    return WEXITSTATUS(rc);
  }
  if (WIFSIGNALED(rc)) {
    return 128 + WTERMSIG(rc);
  }
  return rc;
}

} // namespace

auto make_tree_width(unsigned value) -> Width {
  Width width;
  width.set_name("tree_width");
  width.set_value(value);
  return width;
}

TEST(PublicApi, EngineFormulaShorthandReturnsTrue) {
  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber", 3);
  engine.setWidth(2);
  engine.setConjecture("x");

  EXPECT_TRUE(engine.solve());
}

TEST(PublicApi, EngineFormulaShorthandReturnsFalse) {
  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber", 3);
  engine.setWidth(3);
  engine.setConjecture("x");

  EXPECT_FALSE(engine.solve());
}

TEST(PublicApi, EngineAcceptsFullConjectureDefinition) {
  TreeWidzard::Engine engine;
  engine.setWidth(2);
  engine.setConjecture("x := VertexCount()\nFormula\nx == 0");

  EXPECT_FALSE(engine.solve());
}

TEST(PublicApi, EngineShorthandEqualityMatchesFullDefinition) {
  TreeWidzard::Engine shorthand_engine;
  shorthand_engine.loadCore("VertexCount");
  shorthand_engine.setWidth(2);
  shorthand_engine.setConjecture("x = 0");

  TreeWidzard::Engine full_engine;
  full_engine.setWidth(2);
  full_engine.setConjecture("x := VertexCount()\nFormula\nx == 0");

  const bool shorthand_result = shorthand_engine.solve();
  const bool full_result = full_engine.solve();

  EXPECT_FALSE(shorthand_result);
  EXPECT_EQ(shorthand_result, full_result);
}

TEST(PublicApi, EngineMatchesSharedInputControllerSetup) {
  auto input_controller = InputController::fromSourceText(
      "x := ChromaticNumber(3)\nFormula\nx", make_tree_width(3),
      InputController::default_paths(), InputControllerErrorMode::Throw);
  Flags flags;
  const bool expected =
      TreeWidzard::runBreadthFirstTraversal(input_controller->getDynamicKernel(),
                                            input_controller->getConjecture(),
                                            flags)
          .property_satisfied;

  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber", 3);
  engine.setWidth(3);
  engine.setConjecture("x");

  EXPECT_FALSE(expected);
  EXPECT_EQ(engine.solve(), expected);
}

TEST(PublicApi, InputControllerBuildsExpectedKernelAndMappings) {
  auto input_controller = InputController::fromSourceText(
      "x := ChromaticNumber(3)\ny := VertexCount()\nFormula\nx",
      make_tree_width(2), InputController::default_paths(),
      InputControllerErrorMode::Throw);

  DynamicKernel &kernel = input_controller->getDynamicKernel();
  Conjecture &conjecture = input_controller->getConjecture();
  const auto variables_to_core = conjecture.getVariablesToCoreName();
  const auto properties = input_controller->getvarToProperty();

  EXPECT_EQ(kernel.get_width().get_name(), "tree_width");
  EXPECT_EQ(kernel.get_width().get_value(), 2U);
  EXPECT_EQ(kernel.coreSize(), 2U);
  EXPECT_TRUE(kernel.isVarExists("x"));
  EXPECT_TRUE(kernel.isVarExists("y"));
  EXPECT_EQ(variables_to_core.at("x"), "ChromaticNumber");
  EXPECT_EQ(variables_to_core.at("y"), "VertexCount");
  ASSERT_EQ(properties.size(), 2U);
  EXPECT_EQ(properties.at("x")->getName(), "ChromaticNumber");
  EXPECT_EQ(properties.at("x")->getParameterType(), "UnsignedInt");
  EXPECT_EQ(properties.at("y")->getName(), "VertexCount");
  EXPECT_EQ(properties.at("y")->getParameterType(), "None");
}

TEST(PublicApi, InputControllerFromSourceThrowsOnInvalidCore) {
  EXPECT_THROW(
      InputController::fromSourceText("x := TotallyMissingCore()\nFormula\nx",
                                      make_tree_width(2),
                                      InputController::default_paths(),
                                      InputControllerErrorMode::Throw),
      std::runtime_error);
}

TEST(PublicApi, EngineRejectsUnsupportedShorthandInequality) {
  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber", 3);
  engine.setWidth(2);
  engine.setConjecture("x != x");

  EXPECT_THROW(engine.solve(), std::invalid_argument);
}

TEST(PublicApi, EngineRejectsUnexpectedParameterForParameterlessCore) {
  TreeWidzard::Engine engine;
  engine.loadCore("VertexCount", 1);
  engine.setWidth(2);
  engine.setConjecture("x");

  EXPECT_THROW(engine.solve(), std::invalid_argument);
}

TEST(PublicApi, InvalidCliInvocationReturnsNonZero) {
  const fs::path root = find_repo_root();
  const std::string command =
      shell_quote((root / "treewidzard.sh").string());

  EXPECT_NE(run_cmd(command), 0);
}
