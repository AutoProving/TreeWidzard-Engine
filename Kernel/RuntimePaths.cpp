#include "RuntimePaths.h"

#include "PathList.h"

#include <cstdlib>
#include <system_error>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace fs = std::filesystem;

namespace TreeWidzard {

namespace {

void append_if_directory(std::vector<std::string> &out,
                         const fs::path &candidate) {
  std::error_code ec;
  if (!fs::exists(candidate, ec) || !fs::is_directory(candidate, ec)) {
    return;
  }

  const fs::path normalized = fs::weakly_canonical(candidate, ec);
  const std::string as_string = (ec ? candidate : normalized).string();
  for (const auto &existing : out) {
    if (existing == as_string) {
      return;
    }
  }
  out.push_back(as_string);
}

} // namespace

auto runtime_module_dir() -> std::optional<fs::path> {
#ifdef _WIN32
  HMODULE module = nullptr;
  if (!GetModuleHandleExW(
          GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
          reinterpret_cast<LPCWSTR>(&runtime_module_dir), &module)) {
    return std::nullopt;
  }

  std::wstring buffer(MAX_PATH, L'\0');
  DWORD length = GetModuleFileNameW(module, buffer.data(),
                                    static_cast<DWORD>(buffer.size()));
  while (length == buffer.size()) {
    buffer.resize(buffer.size() * 2, L'\0');
    length = GetModuleFileNameW(module, buffer.data(),
                                static_cast<DWORD>(buffer.size()));
  }
  if (length == 0) {
    return std::nullopt;
  }
  buffer.resize(length);
  return fs::path(buffer).parent_path();
#else
  Dl_info info{};
  if (dladdr(reinterpret_cast<const void *>(&runtime_module_dir), &info) == 0 ||
      info.dli_fname == nullptr) {
    return std::nullopt;
  }
  return fs::path(info.dli_fname).parent_path();
#endif
}

auto default_plugin_paths(const char *env_name,
                          const char *compiled_defaults,
                          const std::string &subdirectory)
    -> std::vector<std::string> {
  if (const char *configured = std::getenv(env_name);
      configured != nullptr && configured[0] != '\0') {
    return split_path_list(configured);
  }

  std::vector<std::string> paths;
  if (const auto module_dir = runtime_module_dir()) {
    append_if_directory(paths, *module_dir / subdirectory);
    append_if_directory(paths, module_dir->parent_path() / subdirectory);
    append_if_directory(paths, module_dir->parent_path() / "libexec" /
                                   "treewidzard" / subdirectory);
    append_if_directory(paths, module_dir->parent_path().parent_path() /
                                   "libexec" / "treewidzard" / subdirectory);
  }

  for (const auto &path : split_path_list(compiled_defaults)) {
    append_if_directory(paths, path);
  }

  return paths;
}

} // namespace TreeWidzard
