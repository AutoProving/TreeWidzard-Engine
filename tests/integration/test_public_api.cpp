#include "../utils/cli_test_support.h"

#include "TreeWidzard.h"
#include "Controller/InputController.h"
#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/Width.h"

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace fs = std::filesystem;
using TreeWidzard::Testing::find_repo_root;
using TreeWidzard::Testing::launcher_command;
using TreeWidzard::Testing::launcher_path;
using TreeWidzard::Testing::run_cmd;
using TreeWidzard::Testing::shell_quote;

namespace {

class ScopedEnvVar {
public:
  ScopedEnvVar(std::string name, std::string value) : name_(std::move(name)) {
    if (const char *existing = std::getenv(name_.c_str())) {
      original_ = existing;
    }
    set(name_, value);
  }

  ~ScopedEnvVar() {
    if (original_.has_value()) {
      set(name_, *original_);
    } else {
      clear(name_);
    }
  }

private:
  static void set(const std::string &name, const std::string &value) {
#ifdef _WIN32
    _putenv_s(name.c_str(), value.c_str());
#else
    setenv(name.c_str(), value.c_str(), 1);
#endif
  }

  static void clear(const std::string &name) {
#ifdef _WIN32
    _putenv_s(name.c_str(), "");
#else
    unsetenv(name.c_str());
#endif
  }

  std::string name_;
  std::optional<std::string> original_;
};

} // namespace

auto make_tree_width(unsigned value) -> Width {
  Width width;
  width.set_name("tree_width");
  width.set_value(value);
  return width;
}

auto make_path_width(unsigned value) -> Width {
  Width width;
  width.set_name("path_width");
  width.set_value(value);
  return width;
}

TEST(PublicApi, EngineLoadCoreRejectsEmptyName) {
  TreeWidzard::Engine engine;
  EXPECT_THROW(engine.loadCore("", 0), std::invalid_argument);
}

TEST(PublicApi, EngineLoadCoreRejectsNegativeParameter) {
  TreeWidzard::Engine engine;
  EXPECT_THROW(engine.loadCore("ChromaticNumber_AtMost", -1), std::invalid_argument);
}

TEST(PublicApi, EngineSetConjectureRejectsEmptyString) {
  TreeWidzard::Engine engine;
  EXPECT_THROW(engine.setConjecture(""), std::invalid_argument);
}

TEST(PublicApi, EngineSetWidthRejectsNegativeWidth) {
  TreeWidzard::Engine engine;
  EXPECT_THROW(engine.setWidth(-1), std::invalid_argument);
}

TEST(PublicApi, EngineSolveRequiresWidth) {
  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber_AtMost", 3);
  engine.setConjecture("x");

  EXPECT_THROW(engine.solve(), std::logic_error);
}

TEST(PublicApi, EngineSolveRequiresConjecture) {
  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber_AtMost", 3);
  engine.setWidth(2);

  EXPECT_THROW(engine.solve(), std::logic_error);
}

TEST(PublicApi, EngineFormulaShorthandReturnsTrue) {
  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber_AtMost", 3);
  engine.setWidth(2);
  engine.setConjecture("x");

  EXPECT_TRUE(engine.solve());
}

TEST(PublicApi, EngineFormulaShorthandReturnsFalse) {
  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber_AtMost", 3);
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

TEST(PublicApi, EngineSupportsExpAliasesInFullConjectures) {
  TreeWidzard::Engine engine;
  engine.setWidth(2);
  engine.setConjecture(
      "Formula\n"
      "EXP alias := TRUE\n"
      "alias");

  EXPECT_TRUE(engine.solve());
}

TEST(PublicApi, InputControllerClonesExpAliasesForRepeatedUse) {
  auto input_controller = InputController::fromSourceText(
      "Formula\nEXP alias := TRUE\nalias AND alias", make_tree_width(2),
      InputController::default_paths(), InputControllerErrorMode::Throw);

  ConjectureNode *root = input_controller->getConjecture().getRoot();
  ASSERT_NE(root, nullptr);
  ASSERT_EQ(root->getChildren().size(), 2U);
  EXPECT_EQ(root->getVal(), "and");
  EXPECT_NE(root->getChildren()[0], root->getChildren()[1]);
  EXPECT_EQ(root->getChildren()[0]->getParent(), root);
  EXPECT_EQ(root->getChildren()[1]->getParent(), root);
  EXPECT_EQ(root->getChildren()[0]->getType(), NUMBER);
  EXPECT_EQ(root->getChildren()[1]->getType(), NUMBER);
  EXPECT_DOUBLE_EQ(root->getChildren()[0]->getX(), 1.0);
  EXPECT_DOUBLE_EQ(root->getChildren()[1]->getX(), 1.0);
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
      "x := ChromaticNumber_AtMost(3)\nFormula\nx", make_tree_width(3),
      InputController::default_paths(), InputControllerErrorMode::Throw);
  Flags flags;
  const bool expected =
      TreeWidzard::runBreadthFirstTraversal(input_controller->getDynamicKernel(),
                                            input_controller->getConjecture(),
                                            flags)
          .property_satisfied;

  TreeWidzard::Engine engine;
  engine.loadCore("ChromaticNumber_AtMost", 3);
  engine.setWidth(3);
  engine.setConjecture("x");

  EXPECT_FALSE(expected);
  EXPECT_EQ(engine.solve(), expected);
}

TEST(PublicApi, ReedPathwidthTwoCountsMatchPublishedSmallCase) {
  const std::string logical_reed = R"(me   := HasMultipleEdges()
tri  := SimpleCliqueNumber_AtLeast(3)
deg3 := MaximumDegree_AtLeast(3)
col3 := ChromaticNumber_AtMost(3)
Formula
NOT me IMPLIES (NOT tri IMPLIES (NOT deg3 IMPLIES col3)))";

  const std::string premise_reed = R"(me   := HasMultipleEdges()
tri  := SimpleCliqueNumber_AtLeast(3)
deg3 := MaximumDegree_AtLeast(3)
col3 := ChromaticNumber_AtMost(3)
Formula
(NOT me AND NOT tri AND NOT deg3) IMPLIES col3)";

  auto logical_bfs = InputController::fromSourceText(
      logical_reed, make_path_width(2), InputController::default_paths(),
      InputControllerErrorMode::Throw);
  Flags plain_flags;
  const auto bfs_result = TreeWidzard::runBreadthFirstTraversal(
      logical_bfs->getDynamicKernel(), logical_bfs->getConjecture(), plain_flags);

  EXPECT_TRUE(bfs_result.property_satisfied);
  EXPECT_EQ(bfs_result.total_states, 2503U);

  auto logical_premise = InputController::fromSourceText(
      logical_reed, make_path_width(2), InputController::default_paths(),
      InputControllerErrorMode::Throw);
  Flags logical_premise_flags;
  logical_premise_flags.add_flag("Premise", 1);
  const auto logical_premise_result = TreeWidzard::runBreadthFirstTraversal(
      logical_premise->getDynamicKernel(), logical_premise->getConjecture(),
      logical_premise_flags);

  EXPECT_TRUE(logical_premise_result.property_satisfied);

  auto premise_encoded = InputController::fromSourceText(
      premise_reed, make_path_width(2), InputController::default_paths(),
      InputControllerErrorMode::Throw);
  Flags premise_flags;
  premise_flags.add_flag("Premise", 1);
  const auto premise_result = TreeWidzard::runBreadthFirstTraversal(
      premise_encoded->getDynamicKernel(), premise_encoded->getConjecture(),
      premise_flags);

  EXPECT_TRUE(premise_result.property_satisfied);
  EXPECT_EQ(premise_result.total_states, 149U);
  EXPECT_GT(logical_premise_result.total_states, premise_result.total_states);
}

TEST(PublicApi, InputControllerBuildsExpectedKernelAndMappings) {
  auto input_controller = InputController::fromSourceText(
      "x := ChromaticNumber_AtMost(3)\ny := VertexCount()\nFormula\nx",
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
  EXPECT_EQ(variables_to_core.at("x"), "ChromaticNumber_AtMost");
  EXPECT_EQ(variables_to_core.at("y"), "VertexCount");
  ASSERT_EQ(properties.size(), 2U);
  EXPECT_EQ(properties.at("x")->getName(), "ChromaticNumber_AtMost");
  EXPECT_EQ(properties.at("x")->getParameterType(), "UnsignedInt");
  EXPECT_EQ(properties.at("y")->getName(), "VertexCount");
  EXPECT_EQ(properties.at("y")->getParameterType(), "None");
}

TEST(PublicApi, InputControllerFromSourceUsesMemoryLabel) {
  auto input_controller = InputController::fromSourceText(
      "x := ChromaticNumber_AtMost(3)\nFormula\nx", make_tree_width(2),
      InputController::default_paths(), InputControllerErrorMode::Throw);

  EXPECT_EQ(input_controller->getInputPath(), "<memory>");
}

TEST(PublicApi, InputControllerDiscoversKnownCores) {
  const auto core_handlers =
      InputController::discover_core_handlers(InputController::default_paths(),
                                             true);

  EXPECT_TRUE(core_handlers.contains("ChromaticNumber_AtMost"));
  EXPECT_TRUE(core_handlers.contains("VertexCount"));
  EXPECT_TRUE(core_handlers.contains("MaximumDegree_AtLeast"));
  EXPECT_TRUE(core_handlers.contains("SimpleCliqueNumber_AtLeast"));
  EXPECT_TRUE(core_handlers.contains("VertexCoverNumber_AtMost"));
  EXPECT_TRUE(core_handlers.contains("FeedbackVertexSetNumber_AtMost"));
  EXPECT_TRUE(core_handlers.contains("IndependenceNumber"));
  EXPECT_TRUE(core_handlers.contains("HasHamiltonianCycle"));
  EXPECT_TRUE(core_handlers.contains("HasHamiltonianPath"));
  EXPECT_TRUE(core_handlers.contains("HasPerfectMatching"));
  EXPECT_TRUE(core_handlers.contains("ContainsK2Minor"));
  EXPECT_TRUE(core_handlers.contains("ContainsMinor"));
}

TEST(PublicApi, InputControllerDefaultPathsUsesPlatformSeparator) {
#ifdef _WIN32
  const std::string configured = "C:\\dpcores;D:\\alt";
  const std::vector<std::string> expected = {"C:\\dpcores", "D:\\alt"};
#else
  const std::string configured = "/tmp/dpcores:/tmp/alt";
  const std::vector<std::string> expected = {"/tmp/dpcores", "/tmp/alt"};
#endif

  ScopedEnvVar env("TREEWIDZARD_DPCORES", configured);
  EXPECT_EQ(InputController::default_paths(), expected);
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
  engine.loadCore("ChromaticNumber_AtMost", 3);
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
  const std::string command = launcher_command(root);

  EXPECT_NE(run_cmd(command), 0);
}
