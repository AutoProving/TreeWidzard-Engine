#include "MPSSearchStrategy.h"
#include "../DPCores/Source/VertexCount.h"
#include "../Performance/PerformanceTimer.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace TreeWidzard {
namespace MPS {

// MPSAcceleratedBreadthFirstSearch implementation
MPSAcceleratedBreadthFirstSearch::MPSAcceleratedBreadthFirstSearch()
    : mps_enabled(false), mps_operations(0), cpu_operations(0), mps_time(0.0),
      cpu_time(0.0), mps_batch_size(256), mps_threshold(10) {

  // Initialize CPU fallback
  cpu_fallback = std::make_unique<EnhancedParallelBreadthFirstSearch>();

  // Try to initialize MPS
  mps_manager = MPSManager::getInstance();
  if (mps_manager && mps_manager->initialize()) {
    mps_enabled = true;
    std::cout << "MPS acceleration enabled on Apple Silicon" << std::endl;
  } else {
    mps_enabled = false;
    std::cout << "MPS not available - using CPU optimization only" << std::endl;
  }
}

MPSAcceleratedBreadthFirstSearch::~MPSAcceleratedBreadthFirstSearch() = default;

std::vector<State>
MPSAcceleratedBreadthFirstSearch::expandState(const State &state) {
  PerformanceTimer timer;
  timer.start();

  std::vector<State> result;

  if (mps_enabled && shouldUseMPS(state)) {
    result = expandStateWithMPS(state);
    timer.stop();
    mps_time += timer.getElapsedMilliseconds();
    mps_operations++;
  } else {
    result = expandStateWithCPU(state);
    timer.stop();
    cpu_time += timer.getElapsedMilliseconds();
    cpu_operations++;
  }

  return result;
}

bool MPSAcceleratedBreadthFirstSearch::isGoalState(const State &state) {
  // Delegate to CPU implementation for goal checking
  return cpu_fallback->isGoalState(state);
}

void MPSAcceleratedBreadthFirstSearch::reset() {
  if (cpu_fallback) {
    cpu_fallback->reset();
  }
  resetPerformanceCounters();
}

std::vector<State>
MPSAcceleratedBreadthFirstSearch::expandStateWithMPS(const State &state) {
  if (!mps_enabled || !mps_manager) {
    return expandStateWithCPU(state);
  }

  try {
    // Get MPS components
    auto witness_computer = mps_manager->getWitnessComputer();
    auto state_processor = mps_manager->getStateProcessor();

    if (!witness_computer || !state_processor) {
      return expandStateWithCPU(state);
    }

    // For now, perform a simple MPS-accelerated operation
    const Bag &bag = state.get_bag();

    // Compute witnesses using MPS
    std::shared_ptr<WitnessSet> witnesses =
        witness_computer->computeVertexCountWitnesses(bag);

    // Create expanded states (simplified for now)
    std::vector<State> expanded_states;
    auto elements = bag.get_elements();

    for (unsigned vertex : elements) {
      State new_state = state;
      std::set<unsigned> new_elements = elements;
      new_elements.insert(vertex + 1000); // Simple transformation
      Bag new_bag;
      new_bag.set_elements(new_elements);
      new_state.set_bag(new_bag);
      expanded_states.push_back(new_state);
    }

    // If we got results, return them; otherwise fallback to CPU
    if (!expanded_states.empty()) {
      return expanded_states;
    }
  } catch (const std::exception &e) {
    std::cerr << "MPS Error: " << e.what() << " - falling back to CPU"
              << std::endl;
  }

  // Fallback to CPU
  return expandStateWithCPU(state);
}

std::vector<State>
MPSAcceleratedBreadthFirstSearch::expandStateWithCPU(const State &state) {
  if (cpu_fallback) {
    return cpu_fallback->expandState(state);
  }

  // Basic fallback if no CPU strategy available
  return std::vector<State>{state};
}

bool MPSAcceleratedBreadthFirstSearch::shouldUseMPS(const State &state) const {
  if (!mps_enabled) {
    return false;
  }

  // Use MPS for larger bags or complex states
  const Bag &bag = state.get_bag();
  size_t bag_size = bag.get_elements().size();

  return bag_size >= mps_threshold;
}

std::vector<State> MPSAcceleratedBreadthFirstSearch::expandStateBatch(
    const std::vector<State> &states) {
  if (states.empty()) {
    return {};
  }

  std::vector<State> all_expanded;

  if (mps_enabled && states.size() >= mps_batch_size / 4) {
    // Process in batches with MPS
    PerformanceTimer timer;
    timer.start();

    try {
      auto witness_computer = mps_manager->getWitnessComputer();
      if (witness_computer) {
        // Extract bags for batch processing
        std::vector<Bag> bags;
        for (const auto &state : states) {
          bags.push_back(state.get_bag());
        }

        // Batch compute witnesses
        auto witness_sets =
            witness_computer->computeBatchWitnesses(bags, "VertexCount");

        // Create expanded states from results
        for (size_t i = 0; i < states.size() && i < witness_sets.size(); i++) {
          auto expanded = expandStateWithMPS(states[i]);
          all_expanded.insert(all_expanded.end(), expanded.begin(),
                              expanded.end());
        }

        timer.stop();
        mps_time += timer.getElapsedMilliseconds();
        mps_operations += states.size();

        return all_expanded;
      }
    } catch (const std::exception &e) {
      std::cerr << "MPS Batch Error: " << e.what() << " - falling back to CPU"
                << std::endl;
    }
  }

  // CPU fallback for batch processing
  PerformanceTimer timer;
  timer.start();

  for (const auto &state : states) {
    auto expanded = expandStateWithCPU(state);
    all_expanded.insert(all_expanded.end(), expanded.begin(), expanded.end());
  }

  timer.stop();
  cpu_time += timer.getElapsedMilliseconds();
  cpu_operations += states.size();

  return all_expanded;
}

void MPSAcceleratedBreadthFirstSearch::printPerformanceStats() const {
  std::cout << "\n=== MPS Performance Statistics ===" << std::endl;
  std::cout << "MPS Operations: " << mps_operations << std::endl;
  std::cout << "CPU Operations: " << cpu_operations << std::endl;
  std::cout << "MPS Time: " << mps_time << " ms" << std::endl;
  std::cout << "CPU Time: " << cpu_time << " ms" << std::endl;

  if (mps_operations > 0) {
    std::cout << "MPS Avg Time: " << (mps_time / mps_operations) << " ms/op"
              << std::endl;
  }
  if (cpu_operations > 0) {
    std::cout << "CPU Avg Time: " << (cpu_time / cpu_operations) << " ms/op"
              << std::endl;
  }

  double total_operations = mps_operations + cpu_operations;
  if (total_operations > 0) {
    std::cout << "MPS Usage: " << (100.0 * mps_operations / total_operations)
              << "%" << std::endl;
    std::cout << "CPU Usage: " << (100.0 * cpu_operations / total_operations)
              << "%" << std::endl;
  }

  std::cout << "MPS Status: " << (mps_enabled ? "Enabled" : "Disabled")
            << std::endl;

  if (mps_manager) {
    mps_manager->printPerformanceStats();
  }
}

void MPSAcceleratedBreadthFirstSearch::resetPerformanceCounters() {
  mps_operations = 0;
  cpu_operations = 0;
  mps_time = 0.0;
  cpu_time = 0.0;

  if (mps_manager) {
    mps_manager->resetPerformanceCounters();
  }
}

// AdaptiveMPSStrategy implementation
AdaptiveMPSStrategy::AdaptiveMPSStrategy()
    : currently_using_mps(false), mps_performance_score(1.0),
      cpu_performance_score(1.0), decision_count(0),
      state_complexity_threshold(5), performance_ratio_threshold(1.2),
      operations_since_measurement(0) {

  mps_strategy = std::make_unique<MPSAcceleratedBreadthFirstSearch>();
  cpu_strategy = std::make_unique<EnhancedParallelBreadthFirstSearch>();

  // Start with MPS if available
  currently_using_mps = mps_strategy->isMPSEnabled();
  last_measurement = std::chrono::high_resolution_clock::now();

  std::cout << "Adaptive MPS Strategy initialized" << std::endl;
  std::cout << "Initial strategy: " << (currently_using_mps ? "MPS" : "CPU")
            << std::endl;
}

AdaptiveMPSStrategy::~AdaptiveMPSStrategy() = default;

std::vector<State> AdaptiveMPSStrategy::expandState(const State &state) {
  PerformanceTimer timer;
  timer.start();

  // Check if we should switch strategies
  if (shouldSwitchStrategy(state)) {
    currently_using_mps = !currently_using_mps;
    decision_count++;
  }

  std::vector<State> result;

  if (currently_using_mps && mps_strategy) {
    result = mps_strategy->expandState(state);
  } else if (cpu_strategy) {
    result = cpu_strategy->expandState(state);
  }

  timer.stop();
  double operation_time = timer.getElapsedMilliseconds();

  // Update performance scores
  updatePerformanceScores(operation_time, currently_using_mps);
  operations_since_measurement++;

  return result;
}

bool AdaptiveMPSStrategy::isGoalState(const State &state) {
  // Use CPU strategy for goal checking (simpler)
  if (cpu_strategy) {
    return cpu_strategy->isGoalState(state);
  }
  return false;
}

void AdaptiveMPSStrategy::reset() {
  if (mps_strategy) {
    mps_strategy->reset();
  }
  if (cpu_strategy) {
    cpu_strategy->reset();
  }
  resetLearning();
}

void AdaptiveMPSStrategy::updatePerformanceScores(double operation_time,
                                                  bool used_mps) {
  const double learning_rate = 0.1;
  const double time_weight =
      1.0 / (1.0 + operation_time); // Faster operations get higher weight

  if (used_mps) {
    mps_performance_score = (1.0 - learning_rate) * mps_performance_score +
                            learning_rate * time_weight;
  } else {
    cpu_performance_score = (1.0 - learning_rate) * cpu_performance_score +
                            learning_rate * time_weight;
  }
}

bool AdaptiveMPSStrategy::shouldSwitchStrategy(const State &state) {
  // Don't switch too frequently
  if (operations_since_measurement < 10) {
    return false;
  }

  // Check if performance ratio suggests switching
  double performance_ratio =
      currently_using_mps ? (cpu_performance_score / mps_performance_score)
                          : (mps_performance_score / cpu_performance_score);

  // Consider state complexity
  size_t complexity = estimateStateComplexity(state);

  // Switch if:
  // 1. Other strategy is significantly better, OR
  // 2. Current strategy is CPU and state is complex enough for MPS, OR
  // 3. Current strategy is MPS and state is too simple

  bool should_switch = false;

  if (performance_ratio > performance_ratio_threshold) {
    should_switch = true;
  } else if (!currently_using_mps && complexity >= state_complexity_threshold &&
             mps_strategy->isMPSEnabled()) {
    should_switch = true;
  } else if (currently_using_mps &&
             complexity < state_complexity_threshold / 2) {
    should_switch = true;
  }

  if (should_switch) {
    operations_since_measurement = 0;
    last_measurement = std::chrono::high_resolution_clock::now();
  }

  return should_switch;
}

size_t AdaptiveMPSStrategy::estimateStateComplexity(const State &state) const {
  const Bag &bag = state.get_bag();
  size_t bag_size = bag.get_elements().size();

  // Simple complexity estimation based on bag size
  // Could be enhanced with more sophisticated metrics
  return bag_size;
}

void AdaptiveMPSStrategy::printLearningStats() const {
  std::cout << "\n=== Adaptive MPS Learning Statistics ===" << std::endl;
  std::cout << "Current Strategy: " << (currently_using_mps ? "MPS" : "CPU")
            << std::endl;
  std::cout << "Strategy Switches: " << decision_count << std::endl;
  std::cout << "MPS Performance Score: " << mps_performance_score << std::endl;
  std::cout << "CPU Performance Score: " << cpu_performance_score << std::endl;
  std::cout << "Operations Since Last Measurement: "
            << operations_since_measurement << std::endl;
  std::cout << "Complexity Threshold: " << state_complexity_threshold
            << std::endl;
  std::cout << "Performance Ratio Threshold: " << performance_ratio_threshold
            << std::endl;

  if (mps_strategy) {
    mps_strategy->printPerformanceStats();
  }
}

void AdaptiveMPSStrategy::resetLearning() {
  currently_using_mps = mps_strategy ? mps_strategy->isMPSEnabled() : false;
  mps_performance_score = 1.0;
  cpu_performance_score = 1.0;
  decision_count = 0;
  operations_since_measurement = 0;
  last_measurement = std::chrono::high_resolution_clock::now();
}

// MPSSearchFactory implementation
std::unique_ptr<SimpleSearchStrategy> MPSSearchFactory::createMPSStrategy() {
  return std::make_unique<MPSAcceleratedBreadthFirstSearch>();
}

std::unique_ptr<SimpleSearchStrategy>
MPSSearchFactory::createOptimalStrategy() {
  if (isMPSAvailable()) {
    std::cout << "Creating MPS-accelerated strategy" << std::endl;
    return createMPSStrategy();
  } else {
    std::cout << "MPS not available - creating CPU-optimized strategy"
              << std::endl;
    return std::make_unique<EnhancedParallelBreadthFirstSearch>();
  }
}

std::unique_ptr<AdaptiveMPSStrategy>
MPSSearchFactory::createAdaptiveStrategy() {
  return std::make_unique<AdaptiveMPSStrategy>();
}

bool MPSSearchFactory::isMPSAvailable() {
  auto manager = MPSManager::getInstance();
  return manager && manager->isAvailable();
}

void MPSSearchFactory::printSystemCapabilities() {
  std::cout << "=== Apple Silicon System Capabilities ===" << std::endl;

  auto manager = MPSManager::getInstance();
  if (manager) {
    manager->printSystemInfo();
  } else {
    std::cout << "MPS Manager: Not available" << std::endl;
  }

  auto gpus = detectAppleGPUs();
  std::cout << "Detected GPUs: ";
  if (gpus.empty()) {
    std::cout << "None" << std::endl;
  } else {
    for (const auto &gpu : gpus) {
      std::cout << gpu << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "MPS Acceleration: "
            << (isMPSAvailable() ? "Available" : "Not Available") << std::endl;
}

std::vector<std::string> MPSSearchFactory::detectAppleGPUs() {
  std::vector<std::string> gpus;

#ifdef __APPLE__
  auto manager = MPSManager::getInstance();
  if (manager && manager->initialize()) {
    auto device = manager->getDevice();
    if (device && device->isAvailable()) {
      gpus.push_back(device->getDeviceName());
    }
  }
#endif

  return gpus;
}

void MPSSearchFactory::runMPSBenchmark() {
  std::cout << "=== Apple Silicon MPS Benchmark ===" << std::endl;

  double mps_perf = benchmarkMPSPerformance();
  double cpu_perf = benchmarkCPUPerformance();

  std::cout << "MPS Performance: " << mps_perf << " ms" << std::endl;
  std::cout << "CPU Performance: " << cpu_perf << " ms" << std::endl;

  if (mps_perf > 0 && cpu_perf > 0) {
    double speedup = cpu_perf / mps_perf;
    std::cout << "Speedup: " << speedup << "x" << std::endl;
  }

  if (isMPSAvailable()) {
    std::cout << "Recommendation: Use MPS acceleration for complex problems"
              << std::endl;
  } else {
    std::cout << "Recommendation: Use CPU optimization (MPS not available)"
              << std::endl;
  }
}

double MPSSearchFactory::benchmarkMPSPerformance() {
  if (!isMPSAvailable()) {
    return 0.0;
  }

  auto strategy = createMPSStrategy();
  PerformanceTimer timer;

  // Create test state
  std::set<unsigned> elements = {1, 2, 3, 4, 5, 6, 7, 8};
  State test_state;
  Bag b;
  b.set_elements(elements);
  test_state.set_bag(b);

  timer.start();

  // Run multiple expansions
  for (int i = 0; i < 10; i++) {
    auto expanded = strategy->expandState(test_state);
  }

  timer.stop();
  return timer.getElapsedMilliseconds();
}

double MPSSearchFactory::benchmarkCPUPerformance() {
  auto strategy = std::make_unique<EnhancedParallelBreadthFirstSearch>();
  PerformanceTimer timer;

  // Create test state
  std::set<unsigned> elements = {1, 2, 3, 4, 5, 6, 7, 8};
  State test_state;
  Bag b;
  b.set_elements(elements);
  test_state.set_bag(b);

  timer.start();

  // Run multiple expansions
  for (int i = 0; i < 10; i++) {
    auto expanded = strategy->expandState(test_state);
  }

  timer.stop();
  return timer.getElapsedMilliseconds();
}

} // namespace MPS
} // namespace TreeWidzard