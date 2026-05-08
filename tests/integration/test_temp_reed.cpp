// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

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

struct ReedCase {
  const char *relative_path;
  const char *width_flag;
  int width_value;
  SolverVerdict expected;
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
      fs::temp_directory_path() / ("treewidzard_reed_" + std::to_string(stamp));
  fs::create_directories(dir);
  return TempDir{dir};
}

auto copy_case_file(const fs::path &source, const fs::path &destination_dir)
    -> fs::path {
  const fs::path destination = destination_dir / source.filename();
  fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
  return destination;
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

auto build_search_command(const fs::path &root, const char *width_flag,
                          int width_value, const fs::path &property_file)
    -> std::string {
  return launcher_command(root) + " -atp " + width_flag + "=" +
         std::to_string(width_value) + " BreadthFirstSearch " +
         shell_quote(property_file.string());
}

void expect_case_matches(const ReedCase &reed_case) {
  const fs::path root = find_repo_root();
  const auto tmp = make_temp_dir();
  const fs::path source = root / reed_case.relative_path;
  const fs::path property = copy_case_file(source, tmp.path);

  const auto result = run_capture(
      build_search_command(root, reed_case.width_flag, reed_case.width_value,
                           property),
      tmp.path / "reed_capture.txt");

  ASSERT_EQ(result.exit_code, 0) << result.output;
  ASSERT_EQ(parse_verdict(result.output), reed_case.expected) << result.output;
  EXPECT_NE(result.output.find("Search method: BreadthFirstSearch"),
            std::string::npos);

  const std::string expected_width_line =
      std::string("Width parameter: ") +
      (std::string(reed_case.width_flag) == "tw" ? "tree_width = "
                                                 : "path_width = ") +
      std::to_string(reed_case.width_value);
  EXPECT_NE(result.output.find(expected_width_line), std::string::npos)
      << result.output;
}

} // namespace

TEST(TempReedCli, TreeWidthSubsetMatchesThesisCategories) {
  const std::vector<ReedCase> cases = {
      {"temp-reed-test/true/tw1_degree_coloring_property.txt", "tw", 1,
       SolverVerdict::Satisfied},
      {"temp-reed-test/true/tw1_degree_coloring_reverse.txt", "tw", 1,
       SolverVerdict::Satisfied},
      {"temp-reed-test/true/tw1_degree_coloring_strong.txt", "tw", 1,
       SolverVerdict::Satisfied},
      {"temp-reed-test/false/tw3_degree_coloring_strong.txt", "tw", 3,
       SolverVerdict::NotSatisfied},
  };

  for (const auto &reed_case : cases) {
    SCOPED_TRACE(reed_case.relative_path);
    expect_case_matches(reed_case);
  }
}

TEST(TempReedCli, PathWidthSubsetMatchesThesisCategories) {
  const std::vector<ReedCase> cases = {
      {"temp-reed-test/true/pw1_degree_coloring_property.txt", "pw", 1,
       SolverVerdict::Satisfied},
      {"temp-reed-test/true/pw1_degree_coloring_strong.txt", "pw", 1,
       SolverVerdict::Satisfied},
      {"temp-reed-test/false/pw3_degree_coloring_strong.txt", "pw", 3,
       SolverVerdict::NotSatisfied},
  };

  for (const auto &reed_case : cases) {
    SCOPED_TRACE(reed_case.relative_path);
    expect_case_matches(reed_case);
  }
}
