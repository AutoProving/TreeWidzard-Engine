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

auto output_prefix(const fs::path &property_file, const std::string &strategy,
                   const std::string &width_name, int width_value) -> fs::path {
  return property_file.parent_path() /
         (property_file.stem().string() + "_" + strategy + "_" + width_name +
          "_" + std::to_string(width_value));
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

TEST(CliSearch, BreadthFirstSearchWritesCounterexampleFiles) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const auto result = run_capture(
      build_search_command(root, "tw", 3, {}, "BreadthFirstSearch", property),
      tmp.path / "bfs_unsat.txt");

  const fs::path prefix =
      output_prefix(property, "BreadthFirstSearch", "tree_width", 3) /
      "";
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
}

TEST(CliSearch, NoBfsDagAvoidsCounterexampleFilesButKeepsVerdict) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path property = copy_property(
      root / "examples/conjectures/simple_3_coloring.txt", tmp.path);

  const auto result = run_capture(build_search_command(
                                      root, "tw", 3, {"-no-bfs-dag"},
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
