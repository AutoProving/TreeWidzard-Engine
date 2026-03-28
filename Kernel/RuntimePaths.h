#ifndef TREEWIDZARD_RUNTIME_PATHS_H
#define TREEWIDZARD_RUNTIME_PATHS_H

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace TreeWidzard {

auto runtime_module_dir() -> std::optional<std::filesystem::path>;

auto default_plugin_paths(const char *env_name,
                          const char *compiled_defaults,
                          const std::string &subdirectory)
    -> std::vector<std::string>;

} // namespace TreeWidzard

#endif
