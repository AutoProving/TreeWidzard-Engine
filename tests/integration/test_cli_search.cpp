#include "../utils/cli_test_support.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace fs = std::filesystem;
using TreeWidzard::Testing::decode_system_result;
using TreeWidzard::Testing::find_repo_root;
using TreeWidzard::Testing::launcher_command;
using TreeWidzard::Testing::shell_quote;

namespace {

struct TempDir {
  fs::path path;

  ~TempDir() {
    std::error_code ec;
    fs::remove_all(path, ec);
  }
};

struct CommandResult {
  int exit_code = 127;
  std::string output;
};

struct CounterexampleArtifacts {
  std::vector<fs::path> standard_files;
  fs::path nauty_file;
};

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

enum class SolverVerdict {
  Satisfied,
  NotSatisfied,
};

auto run_capture(const std::string &command, const fs::path &capture_path)
    -> CommandResult {
  const std::string full_command =
      command + " > " + shell_quote(capture_path.string()) + " 2>&1";

  const int rc = std::system(full_command.c_str());
  const int exit_code = decode_system_result(rc);

  std::ifstream in(capture_path);
  if (!in) {
    throw std::runtime_error("failed to read captured output: " +
                             capture_path.string());
  }

  std::ostringstream output;
  output << in.rdbuf();
  return CommandResult{exit_code, output.str()};
}

auto make_temp_dir() -> TempDir {
  const auto stamp =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now().time_since_epoch())
          .count();
  const fs::path dir =
      fs::temp_directory_path() / ("treewidzard_cli_" + std::to_string(stamp));
  fs::create_directories(dir);
  return TempDir{dir};
}

auto copy_property(const fs::path &source, const fs::path &destination_dir)
    -> fs::path {
  const fs::path destination = destination_dir / source.filename();
  fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
  return destination;
}

auto write_text_file(const fs::path &path, const std::string &contents)
    -> fs::path {
  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("failed to open file for writing: " +
                             path.string());
  }
  out << contents;
  return path;
}

auto output_prefix(const fs::path &property_file, const std::string &strategy,
                   const std::string &width_name, int width_value) -> fs::path {
  return property_file.parent_path() /
         (property_file.stem().string() + "_" + strategy + "_" + width_name +
          "_" + std::to_string(width_value));
}

auto counterexample_artifacts(const fs::path &property_file,
                              const std::string &strategy,
                              const std::string &width_name, int width_value)
    -> CounterexampleArtifacts {
  const fs::path base =
      output_prefix(property_file, strategy, width_name, width_value)
          .string() +
      "_CounterExample";

  CounterexampleArtifacts artifacts;
  if (strategy == "IsomorphismBreadthFirstSearch") {
    artifacts.standard_files = {
        base.string() + "_RunTree.txt",
        base.string() + "_InstructiveDecomposition.txt",
        base.string() + "_ConcreteDecomposition.txt",
        base.string() + "_Graph.txt",
        base.string() + "_GMLGraph.gml",
        base.string() + "_GraphPaceFormat.gr",
    };
  } else {
    artifacts.standard_files = {
        base.string() + "_ITD.txt",
        base.string() + "_ConcreteDecomposition.txt",
        base.string() + "_RunTree.txt",
        base.string() + "_Graph.txt",
        base.string() + "_GMLGraph.gml",
        base.string() + "_GraphPaceFormat.gr",
    };
  }
  artifacts.nauty_file = base.string() + "_DirectedBipartiteGraphNAUTY.txt";
  return artifacts;
}

void expect_artifacts_state(const CounterexampleArtifacts &artifacts,
                            bool expect_standard_files,
                            bool expect_nauty_file) {
  for (const auto &path : artifacts.standard_files) {
    EXPECT_EQ(fs::exists(path), expect_standard_files) << path.string();
  }
  EXPECT_EQ(fs::exists(artifacts.nauty_file), expect_nauty_file)
      << artifacts.nauty_file.string();
}

auto counterexample_base(const fs::path &property_file,
                         const std::string &strategy,
                         const std::string &width_name, int width_value)
    -> std::string {
  return output_prefix(property_file, strategy, width_name, width_value)
             .string() +
         "_CounterExample";
}

auto standard_counterexample_suffixes(const std::string &strategy)
    -> std::vector<std::string> {
  std::vector<std::string> suffixes;
  if (strategy == "IsomorphismBreadthFirstSearch") {
    suffixes.push_back("_InstructiveDecomposition.txt");
  } else {
    suffixes.push_back("_ITD.txt");
  }
  suffixes.push_back("_ConcreteDecomposition.txt");
  suffixes.push_back("_RunTree.txt");
  suffixes.push_back("_Graph.txt");
  suffixes.push_back("_GMLGraph.gml");
  suffixes.push_back("_GraphPaceFormat.gr");
  return suffixes;
}

void expect_counterexample_artifacts(const std::string &base,
                                     const std::string &strategy,
                                     bool expect_standard_files,
                                     bool expect_nauty_file) {
  for (const auto &suffix : standard_counterexample_suffixes(strategy)) {
    EXPECT_EQ(fs::exists(base + suffix), expect_standard_files)
        << "artifact mismatch: " << (base + suffix);
  }

  EXPECT_EQ(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"),
            expect_nauty_file)
      << "artifact mismatch: "
      << (base + "_DirectedBipartiteGraphNAUTY.txt");
}

auto parse_verdict(const std::string &output) -> std::optional<SolverVerdict> {
  if (output.find("PROPERTY NOT SATISFIED") != std::string::npos) {
    return SolverVerdict::NotSatisfied;
  }
  if (output.find("PROPERTY SATISFIED") != std::string::npos) {
    return SolverVerdict::Satisfied;
  }
  return std::nullopt;
}

auto maybe_parallel_prefix(const std::string &strategy) -> std::string {
  if (strategy.rfind("Parallel", 0) == 0) {
    return "-nthreads 2 ";
  }
  return "";
}

auto build_search_command(const fs::path &root, const std::string &width_flag,
                          int width_value, const std::vector<std::string> &flags,
                          const std::string &strategy,
                          const fs::path &property_file,
                          const std::optional<fs::path> &cert_file = std::nullopt)
    -> std::string {
  std::string command =
      launcher_command(root) + " -atp " + width_flag +
      "=" + std::to_string(width_value) + " ";

  for (const auto &flag : flags) {
    command += flag + " ";
  }

  command += maybe_parallel_prefix(strategy);
  command += strategy + " " + shell_quote(property_file.string());

  if (cert_file.has_value()) {
    command += " -cert " + shell_quote(cert_file->string());
  }

  return command;
}

auto build_checkcert_command(const fs::path &root, const fs::path &property_file,
                             const fs::path &cert_file) -> std::string {
  return launcher_command(root) + " -checkcert " +
         shell_quote(property_file.string()) + " " +
         shell_quote(cert_file.string());
}

auto all_search_strategies() -> const std::vector<std::string> & {
  static const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };
  return strategies;
}

auto instructive_artifact_path(const std::string &base,
                               const std::string &strategy) -> fs::path {
  if (strategy == "IsomorphismBreadthFirstSearch") {
    return fs::path(base + "_InstructiveDecomposition.txt");
  }
  return fs::path(base + "_ITD.txt");
}

void expect_standard_counterexample_files_exist(const std::string &base,
                                                const std::string &strategy) {
  EXPECT_TRUE(fs::exists(instructive_artifact_path(base, strategy)));
  EXPECT_TRUE(fs::exists(base + "_ConcreteDecomposition.txt"));
  EXPECT_TRUE(fs::exists(base + "_RunTree.txt"));
  EXPECT_TRUE(fs::exists(base + "_Graph.txt"));
  EXPECT_TRUE(fs::exists(base + "_GMLGraph.gml"));
  EXPECT_TRUE(fs::exists(base + "_GraphPaceFormat.gr"));
}

void expect_no_standard_counterexample_files(const std::string &base,
                                             const std::string &strategy) {
  EXPECT_FALSE(fs::exists(instructive_artifact_path(base, strategy)));
  EXPECT_FALSE(fs::exists(base + "_ConcreteDecomposition.txt"));
  EXPECT_FALSE(fs::exists(base + "_RunTree.txt"));
  EXPECT_FALSE(fs::exists(base + "_Graph.txt"));
  EXPECT_FALSE(fs::exists(base + "_GMLGraph.gml"));
  EXPECT_FALSE(fs::exists(base + "_GraphPaceFormat.gr"));
}

auto supports_state_tree_output(const std::string &strategy) -> bool {
  return strategy == "BreadthFirstSearch" ||
         strategy == "ParallelBreadthFirstSearch";
}

} // namespace

TEST(CliSearch, TreeWidthHappyPathReportsSatisfied) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_implies.txt", tmp.path);

  const auto result = run_capture(
      build_search_command(root, "tw", 2, {}, "BreadthFirstSearch", property),
      tmp.path / "tw_happy.txt");

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_TRUE(parse_verdict(result.output).has_value());
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::Satisfied);
  EXPECT_NE(result.output.find("Width parameter: tree_width = 2"),
            std::string::npos);
  EXPECT_NE(result.output.find("Search method: BreadthFirstSearch"),
            std::string::npos);
}

TEST(CliSearch, PathWidthHappyPathReportsSatisfied) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_implies.txt", tmp.path);

  const auto result = run_capture(
      build_search_command(root, "pw", 2, {}, "BreadthFirstSearch", property),
      tmp.path / "pw_happy.txt");

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::Satisfied);
  EXPECT_NE(result.output.find("Width parameter: path_width = 2"),
            std::string::npos);
}

TEST(CliSearch, PremiseFlagHappyPathReportsActivated) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_implies.txt", tmp.path);

  const auto result = run_capture(build_search_command(
                                      root, "tw", 2, {"-premise"},
                                      "BreadthFirstSearch", property),
                                  tmp.path / "premise_happy.txt");

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::Satisfied);
  EXPECT_NE(result.output.find("Premise flag: ACTIVATED"), std::string::npos);
}

TEST(CliSearch, HelpReturnsSuccess) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();

  const auto result = run_capture(launcher_command(root) + " --help",
                                  tmp.path / "help.txt");

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_NE(result.output.find("treewidzard"), std::string::npos);
}

TEST(CliSearch, RejectsNonIntegralWidthValues) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_implies.txt", tmp.path);

  const auto result = run_capture(
      launcher_command(root) + " -atp tw=2.5 BreadthFirstSearch " +
          shell_quote(property.string()),
      tmp.path / "bad_width.txt");

  EXPECT_NE(result.exit_code, 0);
  EXPECT_NE(result.output.find("width value should be a non-negative integer"),
            std::string::npos);
}

TEST(CliSearch, MissingSearchPathEntryDoesNotBreakDiscovery) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_implies.txt", tmp.path);
  const char *build_dir_env = std::getenv("TREEWIDZARD_BUILD_DIR");
  ASSERT_NE(build_dir_env, nullptr);

#ifdef _WIN32
  const std::string configured_paths =
      std::string("Z:\\does-not-exist;") + build_dir_env +
      "\\SearchStrategies";
#else
  const std::string configured_paths =
      std::string("/definitely-missing:") + build_dir_env +
      "/SearchStrategies";
#endif
  ScopedEnvVar search_paths("TREEWIDZARD_SEARCHSTRATEGIES", configured_paths);

  const auto result = run_capture(
      build_search_command(root, "tw", 2, {}, "BreadthFirstSearch", property),
      tmp.path / "search_paths.txt");

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::Satisfied);
}

TEST(CliSearch, GenericTermParserAcceptsSerializedTerms) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path term_file = write_text_file(
      tmp.path / "sample.term",
      "1  Leaf\n"
      "2  IntroVertex_1(1)\n"
      "3  ForgetVertex_1(2)\n");

  const auto result = run_capture(
      launcher_command(root) + " -term " + shell_quote(term_file.string()),
      tmp.path / "term.txt");

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_NE(result.output.find("IntroVertex_1"), std::string::npos);
}

TEST(CliSearch, RejectsDisconnectedTreeDecompositions) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_implies.txt", tmp.path);
  const fs::path graph = write_text_file(tmp.path / "graph.gr",
                                         "p tw 3 1\n"
                                         "1 2\n");
  const fs::path td = write_text_file(tmp.path / "broken.td",
                                      "s td 3 2 3\n"
                                      "b 1 1\n"
                                      "b 2 2\n"
                                      "b 3 3\n"
                                      "1 2\n"
                                      "1 2\n");

  const auto result = run_capture(
      launcher_command(root) + " -modelcheck PACE " +
          shell_quote(property.string()) + " " + shell_quote(graph.string()) +
          " " + shell_quote(td.string()),
      tmp.path / "broken_td.txt");

  EXPECT_NE(result.exit_code, 0);
  EXPECT_NE(result.output.find("valid tree decomposition"), std::string::npos);
}

TEST(CliSearch, DefaultCounterexampleSearchDoesNotWriteFiles) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const auto result = run_capture(
      build_search_command(root, "tw", 3, {}, "BreadthFirstSearch", property),
      tmp.path / "bfs_unsat.txt");

  const std::string base =
      output_prefix(property, "BreadthFirstSearch", "tree_width", 3).string() +
      "_CounterExample";

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied);
  EXPECT_NE(result.output.find("Counterexample found:"), std::string::npos);
  EXPECT_FALSE(fs::exists(base + "_ITD.txt"));
  EXPECT_FALSE(fs::exists(base + "_ConcreteDecomposition.txt"));
  EXPECT_FALSE(fs::exists(base + "_RunTree.txt"));
  EXPECT_FALSE(fs::exists(base + "_Graph.txt"));
  EXPECT_FALSE(fs::exists(base + "_GMLGraph.gml"));
  EXPECT_FALSE(fs::exists(base + "_GraphPaceFormat.gr"));
  EXPECT_FALSE(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"));
}

TEST(CliSearch, FilesFlagWritesCounterexampleFiles) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const auto result = run_capture(
      build_search_command(root, "tw", 3, {"-files"}, "BreadthFirstSearch",
                           property),
      tmp.path / "bfs_files.txt");

  const std::string base =
      output_prefix(property, "BreadthFirstSearch", "tree_width", 3).string() +
      "_CounterExample";

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied);
  EXPECT_NE(result.output.find("Counterexample found:"), std::string::npos);
  EXPECT_TRUE(fs::exists(base + "_ITD.txt"));
  EXPECT_TRUE(fs::exists(base + "_ConcreteDecomposition.txt"));
  EXPECT_TRUE(fs::exists(base + "_RunTree.txt"));
  EXPECT_TRUE(fs::exists(base + "_Graph.txt"));
  EXPECT_TRUE(fs::exists(base + "_GMLGraph.gml"));
  EXPECT_TRUE(fs::exists(base + "_GraphPaceFormat.gr"));
  EXPECT_FALSE(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"));
}

TEST(CliSearch, StateTreeFlagPrintsCounterexampleStateTree) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const auto result = run_capture(
      build_search_command(root, "tw", 3, {"-st"}, "BreadthFirstSearch",
                           property),
      tmp.path / "bfs_state_tree.txt");

  const std::string base =
      output_prefix(property, "BreadthFirstSearch", "tree_width", 3).string() +
      "_CounterExample";

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied);
  EXPECT_NE(result.output.find("Counterexample state tree:"), std::string::npos);
  EXPECT_NE(result.output.find("Core 1 WitnessSet"), std::string::npos);
  EXPECT_FALSE(fs::exists(base + "_ITD.txt"));
  EXPECT_FALSE(fs::exists(base + "_RunTree.txt"));
}

TEST(CliSearch, DirectedBipartiteGraphFlagWritesOnlyNautyArtifact) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const auto result = run_capture(
      build_search_command(root, "tw", 3, {"-pdbgn"}, "BreadthFirstSearch",
                           property),
      tmp.path / "bfs_nauty.txt");

  const std::string base =
      output_prefix(property, "BreadthFirstSearch", "tree_width", 3).string() +
      "_CounterExample";

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied);
  EXPECT_TRUE(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"));
  EXPECT_FALSE(fs::exists(base + "_ITD.txt"));
  EXPECT_FALSE(fs::exists(base + "_ConcreteDecomposition.txt"));
  EXPECT_FALSE(fs::exists(base + "_RunTree.txt"));
}

TEST(CliSearch, NoBfsDagAvoidsCounterexampleFilesButKeepsVerdict) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const auto result = run_capture(build_search_command(
                                      root, "tw", 3, {"-files", "-no-bfs-dag"},
                                      "BreadthFirstSearch", property),
                                  tmp.path / "no_bfs_dag.txt");

  const std::string base =
      output_prefix(property, "BreadthFirstSearch", "tree_width", 3).string() +
      "_CounterExample";

  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied);
  EXPECT_NE(result.output.find("Rerun without -no-bfs-dag"), std::string::npos);
  EXPECT_FALSE(fs::exists(base + "_ITD.txt"));
  EXPECT_FALSE(fs::exists(base + "_ConcreteDecomposition.txt"));
  EXPECT_FALSE(fs::exists(base + "_RunTree.txt"));
  EXPECT_FALSE(fs::exists(base + "_Graph.txt"));
  EXPECT_FALSE(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"));
}

TEST(CliSearch, CounterexampleFileFlagsWorkAcrossStrategies) {
  const fs::path root = find_repo_root();
  const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };

  for (const auto &strategy : strategies) {
    {
      SCOPED_TRACE(strategy + " default");
      const auto tmp = make_temp_dir();
      const fs::path property = copy_property(
          root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
      const auto result =
          run_capture(build_search_command(root, "tw", 3, {}, strategy, property),
                      tmp.path / (strategy + "_default_flags.txt"));
      const std::string base =
          counterexample_base(property, strategy, "tree_width", 3);

      ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
      ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
          << strategy << "\n" << result.output;
      EXPECT_NE(result.output.find("Counterexample found:"), std::string::npos)
          << strategy;
      expect_counterexample_artifacts(base, strategy, false, false);
    }

    {
      SCOPED_TRACE(strategy + " -files");
      const auto tmp = make_temp_dir();
      const fs::path property = copy_property(
          root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
      const auto result = run_capture(
          build_search_command(root, "tw", 3, {"-files"}, strategy, property),
          tmp.path / (strategy + "_files_flag.txt"));
      const std::string base =
          counterexample_base(property, strategy, "tree_width", 3);

      ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
      ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
          << strategy << "\n" << result.output;
      expect_counterexample_artifacts(base, strategy, true, false);
    }

    {
      SCOPED_TRACE(strategy + " -pdbgn");
      const auto tmp = make_temp_dir();
      const fs::path property = copy_property(
          root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
      const auto result = run_capture(
          build_search_command(root, "tw", 3, {"-pdbgn"}, strategy, property),
          tmp.path / (strategy + "_pdbgn_flag.txt"));
      const std::string base =
          counterexample_base(property, strategy, "tree_width", 3);

      ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
      ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
          << strategy << "\n" << result.output;
      expect_counterexample_artifacts(base, strategy, false, true);
    }
  }
}

TEST(CliSearch, StateTreeFlagBehaviorAcrossStrategies) {
  const fs::path root = find_repo_root();

  struct SearchCase {
    std::string strategy;
    bool supports_state_tree;
  };

  const std::vector<SearchCase> strategies = {
      {"BreadthFirstSearch", true},
      {"ParallelBreadthFirstSearch", true},
      {"IsomorphismBreadthFirstSearch", false},
      {"ParallelIsomorphismBreadthFirstSearch", false},
  };

  for (const auto &search_case : strategies) {
    SCOPED_TRACE(search_case.strategy);
    const auto tmp = make_temp_dir();
    const fs::path property = copy_property(
        root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
    const auto result =
        run_capture(build_search_command(root, "tw", 3, {"-st"},
                                         search_case.strategy, property),
                    tmp.path / (search_case.strategy + "_state_tree.txt"));
    const std::string base =
        counterexample_base(property, search_case.strategy, "tree_width", 3);

    ASSERT_EQ(result.exit_code, 0) << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << result.output;

    if (search_case.supports_state_tree) {
      EXPECT_NE(result.output.find("Counterexample state tree:"),
                std::string::npos)
          << result.output;
      EXPECT_NE(result.output.find("Core 1 WitnessSet"), std::string::npos)
          << result.output;
    } else {
      EXPECT_NE(result.output.find("State tree output is not supported for " +
                                   search_case.strategy + "."),
                std::string::npos)
          << result.output;
    }

    expect_counterexample_artifacts(base, search_case.strategy, false, false);
  }
}

TEST(CliSearch, NoBfsDagSuppressesArtifactsAcrossStrategies) {
  const fs::path root = find_repo_root();
  const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };

  for (const auto &strategy : strategies) {
    SCOPED_TRACE(strategy);
    const auto tmp = make_temp_dir();
    const fs::path property = copy_property(
        root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
    const auto result = run_capture(
        build_search_command(root, "tw", 3, {"-files", "-no-bfs-dag"}, strategy,
                             property),
        tmp.path / (strategy + "_no_bfs_dag.txt"));
    const std::string base =
        counterexample_base(property, strategy, "tree_width", 3);

    ASSERT_EQ(result.exit_code, 0) << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << result.output;
    EXPECT_NE(result.output.find("Rerun without -no-bfs-dag"), std::string::npos)
        << result.output;
    expect_counterexample_artifacts(base, strategy, false, false);
  }
}

TEST(CliSearch, FilesFlagControlsCounterexampleArtifactsAcrossStrategies) {
  const fs::path root = find_repo_root();

  for (const auto &strategy : all_search_strategies()) {
    const auto tmp = make_temp_dir();
    const fs::path property = copy_property(
        root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

    const auto result = run_capture(
        build_search_command(root, "tw", 3, {"-files"}, strategy, property),
        tmp.path / (strategy + "_files.out"));

    const std::string base =
        output_prefix(property, strategy, "tree_width", 3).string() +
        "_CounterExample";

    ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << strategy << "\n" << result.output;
    EXPECT_NE(result.output.find("Counterexample found:"), std::string::npos)
        << strategy;
    expect_standard_counterexample_files_exist(base, strategy);
    EXPECT_FALSE(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"))
        << strategy;
  }
}

TEST(CliSearch, DirectedBipartiteGraphFlagOnlyWritesNautyAcrossStrategies) {
  const fs::path root = find_repo_root();

  for (const auto &strategy : all_search_strategies()) {
    const auto tmp = make_temp_dir();
    const fs::path property = copy_property(
        root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

    const auto result = run_capture(
        build_search_command(root, "tw", 3, {"-pdbgn"}, strategy, property),
        tmp.path / (strategy + "_nauty.out"));

    const std::string base =
        output_prefix(property, strategy, "tree_width", 3).string() +
        "_CounterExample";

    ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << strategy << "\n" << result.output;
    EXPECT_TRUE(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"))
        << strategy;
    expect_no_standard_counterexample_files(base, strategy);
  }
}

TEST(CliSearch, StateTreeFlagBehaviorMatchesStrategyCapabilities) {
  const fs::path root = find_repo_root();

  for (const auto &strategy : all_search_strategies()) {
    const auto tmp = make_temp_dir();
    const fs::path property = copy_property(
        root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

    const auto result = run_capture(
        build_search_command(root, "tw", 3, {"-st"}, strategy, property),
        tmp.path / (strategy + "_state_tree.out"));

    const std::string base =
        output_prefix(property, strategy, "tree_width", 3).string() +
        "_CounterExample";

    ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << strategy << "\n" << result.output;

    if (supports_state_tree_output(strategy)) {
      EXPECT_NE(result.output.find("Counterexample state tree:"),
                std::string::npos)
          << strategy;
      EXPECT_NE(result.output.find("Core 1 WitnessSet"), std::string::npos)
          << strategy;
    } else {
      EXPECT_NE(result.output.find("State tree output is not supported for " +
                                       strategy + "."),
                std::string::npos)
          << strategy;
    }

    expect_no_standard_counterexample_files(base, strategy);
    EXPECT_FALSE(fs::exists(base + "_DirectedBipartiteGraphNAUTY.txt"))
        << strategy;
  }
}

TEST(CliSearch, SearchStrategiesAgreeOnSatisfiedProperty) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_implies.txt", tmp.path);

  const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };

  for (const auto &strategy : strategies) {
    const fs::path capture = tmp.path / (strategy + "_sat.out");
    const auto result = run_capture(
        build_search_command(root, "tw", 2, {}, strategy, property), capture);

    ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
    ASSERT_TRUE(parse_verdict(result.output).has_value())
        << strategy << "\n" << result.output;
    EXPECT_EQ(parse_verdict(result.output), SolverVerdict::Satisfied)
        << strategy << "\n" << result.output;
  }
}

TEST(CliSearch, SearchStrategiesAgreeOnUnsatisfiedPropertyAndCertificates) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };

  for (const auto &strategy : strategies) {
    const fs::path capture = tmp.path / (strategy + "_unsat.out");
    const fs::path cert = tmp.path / (strategy + ".twzcert");

    const auto result =
        run_capture(build_search_command(root, "tw", 3, {}, strategy, property,
                                         cert),
                    capture);
    ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << strategy << "\n" << result.output;
    ASSERT_TRUE(fs::exists(cert)) << strategy;

    const auto check_result = run_capture(
        build_checkcert_command(root, property, cert),
        tmp.path / (strategy + "_check.out"));
    EXPECT_EQ(check_result.exit_code, 0)
        << strategy << "\n" << check_result.output;
  }
}

TEST(CliSearch, CounterexampleFileFlagsApplyAcrossSearchStrategies) {
  const fs::path root = find_repo_root();
  const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };

  for (const auto &strategy : strategies) {
    {
      const auto tmp = make_temp_dir();
      const fs::path property = copy_property(
          root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
      const auto result =
          run_capture(build_search_command(root, "tw", 3, {}, strategy, property),
                      tmp.path / (strategy + "_default.out"));
      const auto artifacts =
          counterexample_artifacts(property, strategy, "tree_width", 3);

      ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
      ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
          << strategy << "\n" << result.output;
      expect_artifacts_state(artifacts, false, false);
    }

    {
      const auto tmp = make_temp_dir();
      const fs::path property = copy_property(
          root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
      const auto result = run_capture(
          build_search_command(root, "tw", 3, {"-files"}, strategy, property),
          tmp.path / (strategy + "_files.out"));
      const auto artifacts =
          counterexample_artifacts(property, strategy, "tree_width", 3);

      ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
      ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
          << strategy << "\n" << result.output;
      expect_artifacts_state(artifacts, true, false);
    }
  }
}

TEST(CliSearch, StateTreeFlagMatchesStrategyCapabilities) {
  const fs::path root = find_repo_root();
  const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };

  for (const auto &strategy : strategies) {
    const auto tmp = make_temp_dir();
    const fs::path property = copy_property(
        root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
    const auto result = run_capture(
        build_search_command(root, "tw", 3, {"-st"}, strategy, property),
        tmp.path / (strategy + "_state_tree.out"));
    const auto artifacts =
        counterexample_artifacts(property, strategy, "tree_width", 3);

    ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << strategy << "\n" << result.output;

    if (strategy == "BreadthFirstSearch" ||
        strategy == "ParallelBreadthFirstSearch") {
      EXPECT_NE(result.output.find("Counterexample state tree:"),
                std::string::npos)
          << strategy << "\n" << result.output;
      EXPECT_NE(result.output.find("Core 1 WitnessSet"), std::string::npos)
          << strategy << "\n" << result.output;
    } else {
      EXPECT_NE(result.output.find("State tree output is not supported for " +
                                   strategy + "."),
                std::string::npos)
          << strategy << "\n" << result.output;
    }

    expect_artifacts_state(artifacts, false, false);
  }
}

TEST(CliSearch, NoBfsDagSuppressesCounterexampleArtifactsAcrossStrategies) {
  const fs::path root = find_repo_root();
  const std::vector<std::string> strategies = {
      "BreadthFirstSearch",
      "IsomorphismBreadthFirstSearch",
      "ParallelBreadthFirstSearch",
      "ParallelIsomorphismBreadthFirstSearch",
  };

  for (const auto &strategy : strategies) {
    const auto tmp = make_temp_dir();
    const fs::path property = copy_property(
        root / "examples/conjectures/simple_3_coloring.txt", tmp.path);
    const auto result = run_capture(
        build_search_command(root, "tw", 3, {"-files", "-pdbgn", "-no-bfs-dag"},
                             strategy, property),
        tmp.path / (strategy + "_no_bfs_dag.out"));
    const auto artifacts =
        counterexample_artifacts(property, strategy, "tree_width", 3);

    ASSERT_EQ(result.exit_code, 0) << strategy << "\n" << result.output;
    ASSERT_EQ(parse_verdict(result.output), SolverVerdict::NotSatisfied)
        << strategy << "\n" << result.output;
    EXPECT_NE(result.output.find("Rerun without -no-bfs-dag"),
              std::string::npos)
        << strategy << "\n" << result.output;
    expect_artifacts_state(artifacts, false, false);
  }
}
