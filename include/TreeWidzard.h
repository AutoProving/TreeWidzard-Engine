#ifndef TREEWIDZARD_H
#define TREEWIDZARD_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace TreeWidzard {

class EngineImpl;

/**
 * @brief Main interface for the TreeWidzard library.
 */
class Engine {
public:
  Engine();
  ~Engine();

  /**
   * @brief Load a specific Dynamic Programming Core (plugin).
   * @param coreName Name of the core (e.g., "ChromaticNumber_AtMost")
   * @param parameter Optional integer parameter (e.g., 3 for k=3). Use 0 for
   * parameterless cores.
   *
   * Repeated calls define the shorthand variables `x`, `y`, `z`, ... used by
   * formula-only conjecture strings passed to `setConjecture`.
   */
  void loadCore(const std::string &coreName, int parameter = 0);

  /**
   * @brief Load a conjecture from a string.
   * @param conjectureString Either a full conjecture file body or a shorthand
   * formula that references `x`, `y`, `z`, ... in `loadCore` order.
   */
  void setConjecture(const std::string &conjectureString);

  /**
   * @brief Set the bag/width size for the decomposition.
   * @param width The treewidth/pathwidth size.
   */
  void setWidth(int width);

  /**
   * @brief Run the algorithm.
   * @return True if a model/witness is found, False otherwise.
   */
  bool solve();

private:
  std::unique_ptr<EngineImpl> impl;
};

} // namespace TreeWidzard

#endif // TREEWIDZARD_H
