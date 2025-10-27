#pragma once

#include "../Performance/PerformanceIntegration.h"
#include "GPUAcceleration.h"
#include <memory>
#include <future>
#include <vector>

namespace TreeWidzard {

/**
 * GPU-Accelerated Search Strategy
 * 
 * This search strategy combines the existing CPU optimizations
 * (memory pools, witness caching) with GPU acceleration for
 * maximum performance on complex graph properties.
 */
class GPUAcceleratedBreadthFirstSearch : public EnhancedParallelBreadthFirstSearch {
private:
    std::unique_ptr<GPU::GPUManager> gpu_manager;
    bool use_hybrid_processing;
    size_t gpu_batch_threshold;
    
    // Performance tracking
    struct PerformanceStats {
        size_t cpu_computations = 0;
        size_t gpu_computations = 0;
        double cpu_time_ms = 0.0;
        double gpu_time_ms = 0.0;
        size_t cache_hits = 0;
        size_t cache_misses = 0;
    } stats;

public:
    explicit GPUAcceleratedBreadthFirstSearch(bool enable_gpu = true);
    ~GPUAcceleratedBreadthFirstSearch();

    // Override core search methods
    std::vector<State> expandState(const State& state) override;
    bool isGoalState(const State& state) override;
    
    // GPU-specific methods
    void setGPUBatchThreshold(size_t threshold) { gpu_batch_threshold = threshold; }
    void setHybridProcessing(bool enable) { use_hybrid_processing = enable; }
    
    // Performance monitoring
    void printPerformanceStats() const;
    void resetPerformanceStats();
    
    // Configuration
    bool isGPUEnabled() const;
    void enableGPU(bool enable);
    
protected:
    // Hybrid CPU-GPU processing
    std::vector<WitnessSet> computeWitnessSetsHybrid(
        const std::vector<Bag>& bags,
        const std::string& core_name
    );
    
    // GPU batch processing
    std::vector<WitnessSet> computeWitnessSetsGPU(
        const std::vector<Bag>& bags,
        const std::string& core_name
    );
    
    // CPU fallback processing
    std::vector<WitnessSet> computeWitnessSetsCPU(
        const std::vector<Bag>& bags,
        const std::string& core_name
    );
    
    // Decision making
    bool shouldUseGPUForBatch(
        const std::vector<Bag>& bags,
        const std::string& core_name
    ) const;
    
    // Batch optimization
    std::vector<std::vector<Bag>> optimizeBatching(
        const std::vector<Bag>& bags
    ) const;
    
    // Memory management
    void optimizeMemoryUsage();
    
private:
    // GPU initialization
    bool initializeGPU();
    
    // Performance heuristics
    double estimateGPUBenefit(
        const std::vector<Bag>& bags,
        const std::string& core_name
    ) const;
    
    // Adaptive processing
    void updateProcessingStrategy();
};

/**
 * GPU-Accelerated Search Factory
 * 
 * Factory class for creating optimized search strategies
 * with automatic GPU detection and configuration.
 */
class GPUSearchFactory {
public:
    // Create optimal search strategy for current hardware
    static std::unique_ptr<SearchStrategy> createOptimalStrategy();
    
    // Create GPU-enabled strategy with specific configuration
    static std::unique_ptr<GPUAcceleratedBreadthFirstSearch> createGPUStrategy(
        bool enable_caching = true,
        bool enable_memory_pools = true,
        bool enable_gpu = true
    );
    
    // Hardware detection
    static bool isGPUAvailable();
    static std::vector<GPU::GPUDevice> detectGPUs();
    static void printSystemCapabilities();
    
    // Benchmarking
    static void runPerformanceBenchmark(
        const std::string& test_file,
        int tree_width = 3
    );
};

/**
 * GPU Performance Monitor
 * 
 * Specialized performance monitoring for GPU-accelerated computations.
 */
class GPUPerformanceMonitor {
private:
    struct Measurement {
        std::string operation;
        std::string core_name;
        size_t batch_size;
        double cpu_time_ms;
        double gpu_time_ms;
        double speedup;
        bool gpu_used;
        std::chrono::timestamp timestamp;
    };
    
    std::vector<Measurement> measurements;
    mutable std::mutex measurements_mutex;

public:
    // Record performance measurement
    void recordMeasurement(
        const std::string& operation,
        const std::string& core_name,
        size_t batch_size,
        double cpu_time_ms,
        double gpu_time_ms,
        bool gpu_used
    );
    
    // Generate performance report
    void generateReport(const std::string& filename) const;
    
    // Real-time statistics
    double getAverageSpeedup() const;
    double getGPUUtilization() const;
    size_t getTotalComputations() const;
    
    // Export data
    void exportToCSV(const std::string& filename) const;
    void exportToJSON(const std::string& filename) const;
    
    // Analysis
    std::vector<std::string> getTopPerformingCores() const;
    std::map<std::string, double> getCoreSpeedups() const;
};

/**
 * Adaptive GPU Strategy
 * 
 * Self-tuning search strategy that learns optimal CPU/GPU
 * allocation based on runtime performance measurements.
 */
class AdaptiveGPUStrategy : public GPUAcceleratedBreadthFirstSearch {
private:
    struct AdaptiveConfig {
        size_t gpu_threshold = 100;
        double cpu_gpu_ratio = 0.5;
        bool learning_enabled = true;
        size_t learning_window = 1000;
    } config;
    
    std::unique_ptr<GPUPerformanceMonitor> performance_monitor;
    size_t computation_count = 0;

public:
    explicit AdaptiveGPUStrategy();
    
    // Override to include learning
    std::vector<State> expandState(const State& state) override;
    
    // Adaptive configuration
    void enableLearning(bool enable) { config.learning_enabled = enable; }
    void setLearningWindow(size_t window) { config.learning_window = window; }
    
    // Strategy optimization
    void optimizeStrategy();
    void resetLearning();
    
    // Configuration access
    const AdaptiveConfig& getConfig() const { return config; }
    void updateConfig(const AdaptiveConfig& new_config) { config = new_config; }

private:
    // Learning algorithms
    void updateThresholds();
    void learnFromPerformance();
    
    // Strategy adaptation
    bool shouldAdaptStrategy() const;
    void adaptBasedOnWorkload(const std::vector<Bag>& bags);
};

} // namespace TreeWidzard