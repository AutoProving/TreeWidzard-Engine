#include "../include/TreeWidzard.h"
#include "../Conjecture/Conjecture.h"
#include "../Kernel/DynamicKernel.h"

namespace TreeWidzard {

Engine::Engine() {
  kernel = std::make_unique<DynamicKernel>();
  conjecture = std::make_unique<Conjecture>();
  conjecture->setKernel(kernel.get());
}

Engine::~Engine() = default;

void Engine::loadCore(const std::string &coreName, int parameter) {
  // Logic to load core via DynamicCoreHandler would be needed here
  // For now, we assume the kernel can handle core creation or configuration
  // This part might need refactoring internal kernel logic to accept dynamic
  // loading requests explicitly Or we might need to expose DynamicCoreHandler
  // in the kernel. Given current public API limits, we might just set flags?

  // For specific implementation:
  // kernel->loadCore(coreName, parameter); // DOES NOT EXIST YET
}

void Engine::setConjecture(const std::string &conjectureString) {
  // Need to parse string and build conjecture tree
  // Ideally: conjecture->parse(conjectureString);
}

void Engine::setWidth(int width) {
  Width w;
  w.set_value(width);
  w.set_name("treewidth");
  kernel->set_width(w);
}

bool Engine::solve() {
  // Logic to run finding witness
  // kernel->modelCheck(...)
  return false; // Stub
}

} // namespace TreeWidzard
