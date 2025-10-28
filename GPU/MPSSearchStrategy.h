#ifndef TREEWIDZARD_MPS_SEARCH_STRATEGY_H
#define TREEWIDZARD_MPS_SEARCH_STRATEGY_H

#include "../Performance/EnhancedParallelBreadthFirstSearch.h"
#include "../Kernel/State.h"
#include "../Kernel/Bag.h"
#include "../Kernel/WitnessSet.h"
#include "../DPCores/Source/VertexCount.h"
#include "MPSAcceleration.h"
#include <memory>
#include <chrono>

namespace TreeWidzard {
namespace MPS {

/**
 * MPS-Accelerated Search Strategy for Apple Silicon
 * 
 * Provides GPU acceleration using Metal Performance Shaders on M-series Macs.
 * Falls back gracefully to CPU when MPS is not available.
 */

class MPSAcceleratedBreadthFirstSearch : public SimpleSearchStrategy {
private:
    std::shared_ptr<MPSManager> mps_manager;
    std::unique_ptr<EnhancedParallelBreadthFirstSearch> cpu_fallback;
    bool mps_enabled;
    
    // Performance tracking
    size_t mps_operations;
    size_t cpu_operations;
    double mps_time;
    double cpu_time;
    
    // Configuration
    size_t mps_batch_size;
    size_t mps_threshold;
    
public:
    MPSAcceleratedBreadthFirstSearch();
    virtual ~MPSAcceleratedBreadthFirstSearch();
    
    // SearchStrategy interface
    std::vector<State> expandState(const State& state) override;
    bool isGoalState(const State& state) override;
    void reset() override;
    
    // MPS-specific methods
    bool isMPSEnabled() const { return mps_enabled; }
    void setMPSBatchSize(size_t size) { mps_batch_size = size; }
    void setMPSThreshold(size_t threshold) { mps_threshold = threshold; }
    
    // Performance monitoring
    void printPerformanceStats() const;
    void resetPerformanceCounters();
    
    // Batch processing
    std::vector<State> expandStateBatch(const std::vector<State>& states);
    
protected:
    // Internal methods
    std::vector<State> expandStateWithMPS(const State& state);
    std::vector<State> expandStateWithCPU(const State& state);
    bool shouldUseMPS(const State& state) const;
};

/**
 * Adaptive MPS Strategy
 * 
 * Automatically chooses between MPS and CPU based on problem characteristics
 * and runtime performance feedback.
 */
class AdaptiveMPSStrategy : public SimpleSearchStrategy {
private:
    std::unique_ptr<MPSAcceleratedBreadthFirstSearch> mps_strategy;
    std::unique_ptr<EnhancedParallelBreadthFirstSearch> cpu_strategy;
    
    // Learning parameters
    bool currently_using_mps;
    double mps_performance_score;
    double cpu_performance_score;
    size_t decision_count;
    
    // Adaptive thresholds
    size_t state_complexity_threshold;
    double performance_ratio_threshold;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point last_measurement;
    size_t operations_since_measurement;
    
public:
    AdaptiveMPSStrategy();
    virtual ~AdaptiveMPSStrategy();
    
    // SearchStrategy interface
    std::vector<State> expandState(const State& state) override;
    bool isGoalState(const State& state) override;
    void reset() override;
    
    // Adaptive methods
    bool isCurrentlyUsingMPS() const { return currently_using_mps; }
    void printLearningStats() const;
    void resetLearning();
    
protected:
    // Internal methods
    void updatePerformanceScores(double operation_time, bool used_mps);
    bool shouldSwitchStrategy(const State& state);
    size_t estimateStateComplexity(const State& state) const;
};

/**
 * MPS Search Factory
 * 
 * Factory for creating optimal search strategies based on Apple Silicon capabilities.
 */
class MPSSearchFactory {
public:
    // Factory creation
    static std::unique_ptr<SimpleSearchStrategy> createMPSStrategy();
    static std::unique_ptr<SimpleSearchStrategy> createOptimalStrategy();
    static std::unique_ptr<AdaptiveMPSStrategy> createAdaptiveStrategy();
    
    // System information
    static bool isMPSAvailable();
    static void printSystemCapabilities();
    static std::vector<std::string> detectAppleGPUs();
    
    // Benchmarking
    static void runMPSBenchmark();
    static double benchmarkMPSPerformance();
    static double benchmarkCPUPerformance();
};

} // namespace MPS
} // namespace TreeWidzard

#endif // TREEWIDZARD_MPS_SEARCH_STRATEGY_H