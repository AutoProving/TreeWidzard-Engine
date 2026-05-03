// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "../include/TreeWidzard.h"

#include "../Controller/InputController.h"
#include "../Kernel/BreadthFirstTraversal.h"
#include "../Kernel/DynamicCoreHandler.h"
#include "../Kernel/Width.h"

#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace TreeWidzard {

class EngineImpl {
public:
  std::vector<std::pair<std::string, int>> loaded_cores;
  std::string conjecture_text;
  int width = -1;
};

namespace {

auto default_variable_name(size_t index) -> std::string {
  static const std::vector<std::string> kNames = {
      "x", "y", "z", "u", "v", "w", "a", "b", "c", "d"};
  if (index < kNames.size()) {
    return kNames[index];
  }
  return "x" + std::to_string(index);
}

auto normalize_formula_syntax(const std::string &formula) -> std::string {
  std::string normalized;
  normalized.reserve(formula.size() * 2);

  for (size_t i = 0; i < formula.size(); ++i) {
    const char current = formula[i];
    if (current != '=') {
      normalized.push_back(current);
      continue;
    }

    const char previous = i > 0 ? formula[i - 1] : '\0';
    const char next = i + 1 < formula.size() ? formula[i + 1] : '\0';
    if (previous == '!') {
      throw std::invalid_argument(
          "Engine shorthand formulas do not support '!='. Use "
          "'NOT (a == b)' or pass a full conjecture definition.");
    }
    const bool is_assignment_or_comparator =
        previous == ':' || previous == '<' || previous == '>' ||
        previous == '=' || next == '=';

    if (is_assignment_or_comparator) {
      normalized.push_back('=');
    } else {
      normalized += "==";
    }
  }

  return normalized;
}

auto looks_like_full_conjecture(const std::string &conjecture_text) -> bool {
  return conjecture_text.find(":=") != std::string::npos ||
         conjecture_text.find("Formula") != std::string::npos ||
         conjecture_text.find("FORMULA") != std::string::npos ||
         conjecture_text.find("EXP") != std::string::npos;
}

auto require_metadata_value(const std::map<std::string, std::string> &metadata,
                            const std::string &key) -> const std::string & {
  const auto it = metadata.find(key);
  if (it == metadata.end()) {
    throw std::runtime_error("Missing core metadata key: " + key);
  }
  return it->second;
}

auto build_conjecture_source(
    const EngineImpl &impl,
    const std::map<std::string, DynamicCoreHandler> &core_handlers)
    -> std::string {
  if (looks_like_full_conjecture(impl.conjecture_text)) {
    return impl.conjecture_text;
  }

  if (impl.loaded_cores.empty()) {
    throw std::invalid_argument(
        "Engine::setConjecture requires a full conjecture definition or at "
        "least one loaded core.");
  }

  std::ostringstream conjecture_source;
  for (size_t index = 0; index < impl.loaded_cores.size(); ++index) {
    const auto &[core_name, parameter] = impl.loaded_cores[index];
    const auto handler_it = core_handlers.find(core_name);
    if (handler_it == core_handlers.end()) {
      throw std::invalid_argument("Unknown DP core: " + core_name);
    }

    const auto &metadata = handler_it->second.get_metadata();
    const std::string parameter_type =
        require_metadata_value(metadata, "ParameterType");

    conjecture_source << default_variable_name(index) << " := " << core_name
                      << "(";
    if (parameter_type == "UnsignedInt") {
      conjecture_source << parameter;
    } else if (parameter_type == "None" ||
               parameter_type == "ParameterLess") {
      if (parameter != 0) {
        throw std::invalid_argument("Core '" + core_name +
                                    "' does not accept an integer parameter.");
      }
    } else if (parameter_type == "InputFile") {
      throw std::invalid_argument(
          "Engine::loadCore does not support file parameters.");
    } else if (parameter_type == "MultiParameter") {
      throw std::invalid_argument(
          "Engine::loadCore only supports single-integer parameters.");
    } else {
      throw std::invalid_argument("Unsupported core parameter type '" +
                                  parameter_type + "' for core '" + core_name +
                                  "'.");
    }
    conjecture_source << ")\n";
  }

  conjecture_source << "Formula\n"
                    << normalize_formula_syntax(impl.conjecture_text);
  return conjecture_source.str();
}

} // namespace

Engine::Engine() : impl(std::make_unique<EngineImpl>()) {}

Engine::~Engine() = default;

void Engine::loadCore(const std::string &coreName, int parameter) {
  if (coreName.empty()) {
    throw std::invalid_argument("Engine::loadCore requires a non-empty core name.");
  }
  if (parameter < 0) {
    throw std::invalid_argument(
        "Engine::loadCore requires a non-negative integer parameter.");
  }
  impl->loaded_cores.emplace_back(coreName, parameter);
}

void Engine::setConjecture(const std::string &conjectureString) {
  if (conjectureString.empty()) {
    throw std::invalid_argument(
        "Engine::setConjecture requires a non-empty conjecture string.");
  }
  impl->conjecture_text = conjectureString;
}

void Engine::setWidth(int width) {
  if (width < 0) {
    throw std::invalid_argument("Engine::setWidth requires a non-negative width.");
  }
  impl->width = width;
}

bool Engine::solve() {
  if (impl->width < 0) {
    throw std::logic_error("Engine::setWidth must be called before solve().");
  }
  if (impl->conjecture_text.empty()) {
    throw std::logic_error(
        "Engine::setConjecture must be called before solve().");
  }

  Width width;
  width.set_name("tree_width");
  width.set_value(impl->width);

  const auto core_handlers =
      InputController::discover_core_handlers(InputController::default_paths(),
                                              true);
  const auto conjecture_source = build_conjecture_source(*impl, core_handlers);

  auto input_controller = InputController::fromSourceText(
      conjecture_source, width, InputController::default_paths(),
      InputControllerErrorMode::Throw);
  Flags flags;
  return runBreadthFirstTraversal(input_controller->getDynamicKernel(),
                                  input_controller->getConjecture(), flags)
      .property_satisfied;
}

} // namespace TreeWidzard
