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

TEST(PublicApi, EngineMatchesSharedInputControllerSetup) {
  Width width;
  width.set_name("tree_width");
  width.set_value(3);

  auto input_controller = InputController::fromSourceText(
      "x := ChromaticNumber(3)\nFormula\nx", width,
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
