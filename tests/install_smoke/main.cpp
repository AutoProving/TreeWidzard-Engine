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
