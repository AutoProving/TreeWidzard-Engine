// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef TREEWIDZARD_PATH_LIST_H
#define TREEWIDZARD_PATH_LIST_H

#include <sstream>
#include <string>
#include <vector>

namespace TreeWidzard {

inline constexpr char path_list_separator() {
#ifdef _WIN32
  return ';';
#else
  return ':';
#endif
}

inline auto split_path_list(const std::string &paths)
    -> std::vector<std::string> {
  std::vector<std::string> out;
  std::istringstream is(paths);
  std::string path;

  while (std::getline(is, path, path_list_separator())) {
    if (!path.empty()) {
      out.push_back(path);
    }
  }

  return out;
}

} // namespace TreeWidzard

#endif
