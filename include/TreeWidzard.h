#ifndef TREEWIDZARD_H
#define TREEWIDZARD_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Forward declarations to avoid exposing internal headers
class DynamicKernel;
class Conjecture;

namespace TreeWidzard {

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
   * @param parameter Optional parameter (e.g., 3 for k=3)
   */
  void loadCore(const std::string &coreName, int parameter = 0);

  /**
   * @brief Load a conjecture from a string (Legacy format).
   * @param conjectureString The conjecture definition.
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
  std::unique_ptr<DynamicKernel> kernel;
  std::unique_ptr<Conjecture> conjecture;
  // Pimpl idiom or opaque pointers to hide implementation details
};

} // namespace TreeWidzard

#endif // TREEWIDZARD_H
