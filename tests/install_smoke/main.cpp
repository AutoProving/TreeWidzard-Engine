// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "TreeWidzard.h"
#include "Controller/InputController.h"

int main() {
  if (InputController::default_paths().empty()) {
    return 2;
  }

  TreeWidzard::Engine engine;
  engine.setWidth(2);
  engine.setConjecture("x := VertexCount()\nFormula\nx == 0");
  return engine.solve() ? 1 : 0;
}
