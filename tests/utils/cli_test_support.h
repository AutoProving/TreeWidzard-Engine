#ifndef TREEWIDZARD_CLI_TEST_SUPPORT_H
#define TREEWIDZARD_CLI_TEST_SUPPORT_H

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace TreeWidzard::Testing {

inline auto launcher_filename() -> const char * {
#ifdef _WIN32
  return "treewidzard.bat";
#else
  return "treewidzard.sh";
#endif
}

inline auto find_repo_root() -> std::filesystem::path {
  std::filesystem::path current = std::filesystem::current_path();
  for (int depth = 0; depth < 10; ++depth) {
    if (std::filesystem::exists(current / "CMakeLists.txt") &&
        (std::filesystem::exists(current / "treewidzard.sh") ||
         std::filesystem::exists(current / "treewidzard.bat"))) {
      return current;
    }
    if (!current.has_parent_path()) {
      break;
    }
    current = current.parent_path();
  }
  throw std::runtime_error("could not locate repo root");
}

inline auto launcher_path(const std::filesystem::path &root)
    -> std::filesystem::path {
  return root / launcher_filename();
}

inline auto shell_quote(const std::string &value) -> std::string {
#ifdef _WIN32
  std::string quoted;
  quoted.reserve(value.size() + 2);
  quoted.push_back('"');

  size_t trailing_backslashes = 0;
  for (const char c : value) {
    if (c == '\\') {
      trailing_backslashes++;
      continue;
    }
    if (c == '"') {
      quoted.append(trailing_backslashes * 2 + 1, '\\');
      quoted.push_back('"');
      trailing_backslashes = 0;
      continue;
    }
    quoted.append(trailing_backslashes, '\\');
    trailing_backslashes = 0;
    quoted.push_back(c);
  }
  quoted.append(trailing_backslashes * 2, '\\');
  quoted.push_back('"');
  return quoted;
#else
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
#endif
}

inline auto launcher_command(const std::filesystem::path &root)
    -> std::string {
#ifdef _WIN32
  return "call " + shell_quote(launcher_path(root).string());
#else
  return shell_quote(launcher_path(root).string());
#endif
}

inline auto decode_system_result(int rc) -> int {
  if (rc == -1) {
    return 127;
  }

#ifdef _WIN32
  return rc;
#else
  if (WIFEXITED(rc)) {
    return WEXITSTATUS(rc);
  }
  if (WIFSIGNALED(rc)) {
    return 128 + WTERMSIG(rc);
  }
  return rc;
#endif
}

inline auto run_cmd(const std::string &command) -> int {
  return decode_system_result(std::system(command.c_str()));
}

} // namespace TreeWidzard::Testing

#endif
