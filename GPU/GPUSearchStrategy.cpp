#include "GPUSearchStrategy.h"
#include "../Performance/PerformanceTimer.h"
#include "../Kernel/CoreWrapper.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>

namespace TreeWidzard {

// GPUAcceleratedBreadthFirstSearch Implementation
GPUAcceleratedBreadthFirstSearch::GPUAcceleratedBreadthFirstSearch(bool enable_gpu)
    : EnhancedParallelBreadthFirstSearch()
    , gpu_manager(std::make_unique<GPU::GPUManager>())
    , use_hybrid_processing(true)
    , gpu_batch_threshold(100)
{
    if (enable_gpu) {
        initializeGPU();
    }
}

GPUAcceleratedBreadthFirstSearch::~GPUAcceleratedBreadthFirstSearch() = default;

bool GPUAcceleratedBreadthFirstSearch::initializeGPU() {
    if (!gpu_manager->initialize()) {
        std::cout << "GPU initialization failed - falling back to CPU-only mode" << std::endl;
        return false;
    }
    
    gpu_manager->printDeviceInfo();
    return true;
}

std::vector<State> GPUAcceleratedBreadthFirstSearch::expandState(const State& state) {
    PROFILE_SCOPE("GPUAcceleratedBFS::expandState");
    
    // Get the bag for this state
    auto bag = state.get_bag();
    std::vector<Bag> bags = {bag};
    
    // Determine core name (simplified - would be more sophisticated in practice)
    std::string core_name = getCurrentCoreName();
    
    // Compute witness sets using hybrid approach
    auto witness_sets = computeWitnessSetsHybrid(bags, core_name);
    
    // Generate states from witness sets
    std::vector<State> expanded_states;
    for (const auto& witness_set : witness_sets) {
        // Convert witness set to states (simplified implementation)
        State new_state = state; // Copy current state
        // Apply witness set transformations
        expanded_states.push_back(std::move(new_state));
    }
    
    return expanded_states;
}

bool GPUAcceleratedBreadthFirstSearch::isGoalState(const State& state) {
    PROFILE_SCOPE("GPUAcceleratedBFS::isGoalState");
    
    // Use parent implementation with potential GPU optimization for complex evaluations
    return EnhancedParallelBreadthFirstSearch::isGoalState(state);
}

std::vector<WitnessSet> GPUAcceleratedBreadthFirstSearch::computeWitnessSetsHybrid(
    const std::vector<Bag>& bags,
    const std::string& core_name
) {
    PROFILE_SCOPE("GPUAcceleratedBFS::computeWitnessSetsHybrid");
    
    // Decision: use GPU or CPU?
    if (shouldUseGPUForBatch(bags, core_name)) {
        return computeWitnessSetsGPU(bags, core_name);
    } else {
        return computeWitnessSetsCPU(bags, core_name);
    }
}

std::vector<WitnessSet> GPUAcceleratedBreadthFirstSearch::computeWitnessSetsGPU(
    const std::vector<Bag>& bags,
    const std::string& core_name
) {
    PROFILE_SCOPE("GPUAcceleratedBFS::computeWitnessSetsGPU");
    
    PerformanceTimer timer;
    timer.start();
    
    auto result = gpu_manager->processWithAutoSelection(bags, core_name, getCurrentTreeWidth());
    
    timer.stop();
    
    // Update statistics
    stats.gpu_computations++;
    stats.gpu_time_ms += timer.getElapsedMilliseconds();
    
    if (result.success) {
        return result.witness_sets;
    } else {
        // Fallback to CPU
        return computeWitnessSetsCPU(bags, core_name);
    }
}

std::vector<WitnessSet> GPUAcceleratedBreadthFirstSearch::computeWitnessSetsCPU(
    const std::vector<Bag>& bags,
    const std::string& core_name
) {
    PROFILE_SCOPE("GPUAcceleratedBFS::computeWitnessSetsCPU");
    
    PerformanceTimer timer;
    timer.start();
    
    std::vector<WitnessSet> witness_sets;
    witness_sets.reserve(bags.size());
    
    // Use cached witness set computation from parent class
    for (const auto& bag : bags) {
        auto witness_set = CachedWitnessSet::computeWithCache(bag, [&]() {
            // Compute witness set using appropriate core
            return computeWitnessSetForBag(bag, core_name);
        });
        witness_sets.push_back(std::move(witness_set));
    }
    
    timer.stop();
    
    // Update statistics
    stats.cpu_computations++;
    stats.cpu_time_ms += timer.getElapsedMilliseconds();
    
    return witness_sets;
}

bool GPUAcceleratedBreadthFirstSearch::shouldUseGPUForBatch(
    const std::vector<Bag>& bags,
    const std::string& core_name
) const {
    // Use GPU manager's decision making
    if (!isGPUEnabled() || bags.empty()) {
        return false;
    }
    
    // Check batch size threshold
    if (bags.size() < gpu_batch_threshold) {
        return false;
    }
    
    // Use utility function for core-specific decisions
    return GPU::Utils::isGPUBeneficial(core_name, bags.size());
}

std::vector<std::vector<Bag>> GPUAcceleratedBreadthFirstSearch::optimizeBatching(
    const std::vector<Bag>& bags
) const {
    std::vector<std::vector<Bag>> batches;
    
    // Simple batching strategy - group by bag size
    std::map<size_t, std::vector<Bag>> size_groups;
    
    for (const auto& bag : bags) {
        size_t bag_size = bag.get_elements().size();
        size_groups[bag_size].push_back(bag);
    }
    
    // Convert to batches
    for (const auto& [size, group_bags] : size_groups) {
        batches.push_back(group_bags);
    }
    
    return batches;
}

void GPUAcceleratedBreadthFirstSearch::printPerformanceStats() const {
    std::cout << "\n=== GPU Performance Statistics ===" << std::endl;
    std::cout << "CPU Computations: " << stats.cpu_computations << std::endl;
    std::cout << "GPU Computations: " << stats.gpu_computations << std::endl;
    std::cout << "CPU Time: " << std::fixed << std::setprecision(2) << stats.cpu_time_ms << " ms" << std::endl;
    std::cout << "GPU Time: " << std::fixed << std::setprecision(2) << stats.gpu_time_ms << " ms" << std::endl;
    
    if (stats.cpu_computations > 0 && stats.gpu_computations > 0) {
        double cpu_avg = stats.cpu_time_ms / stats.cpu_computations;
        double gpu_avg = stats.gpu_time_ms / stats.gpu_computations;
        double speedup = cpu_avg / gpu_avg;
        std::cout << "Average Speedup: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    }
    
    std::cout << "Cache Hits: " << stats.cache_hits << std::endl;
    std::cout << "Cache Misses: " << stats.cache_misses << std::endl;
    std::cout << "==================================" << std::endl;
}

void GPUAcceleratedBreadthFirstSearch::resetPerformanceStats() {
    stats = PerformanceStats{};
}

bool GPUAcceleratedBreadthFirstSearch::isGPUEnabled() const {
    return gpu_manager && gpu_manager->isGPUEnabled();
}

void GPUAcceleratedBreadthFirstSearch::enableGPU(bool enable) {
    if (gpu_manager) {
        gpu_manager->setGPUEnabled(enable);
    }
}

// Helper methods (simplified implementations)
std::string GPUAcceleratedBreadthFirstSearch::getCurrentCoreName() const {
    // This would be determined from the current search context
    return "ChromaticNumber"; // Default for demonstration
}

int GPUAcceleratedBreadthFirstSearch::getCurrentTreeWidth() const {
    // This would be determined from the current search context
    return 3; // Default for demonstration
}

WitnessSet GPUAcceleratedBreadthFirstSearch::computeWitnessSetForBag(
    const Bag& bag, 
    const std::string& core_name
) const {
    // Simplified implementation - would interface with actual DP cores
    WitnessSet witness_set;
    
    // For demonstration, create a simple witness set
    // In practice, this would use the appropriate CoreWrapper
    
    return witness_set;
}

// GPUSearchFactory Implementation
std::unique_ptr<SearchStrategy> GPUSearchFactory::createOptimalStrategy() {
    if (isGPUAvailable()) {
        return createGPUStrategy(true, true, true);
    } else {
        return std::make_unique<EnhancedParallelBreadthFirstSearch>();
    }
}

std::unique_ptr<GPUAcceleratedBreadthFirstSearch> GPUSearchFactory::createGPUStrategy(
    bool enable_caching,
    bool enable_memory_pools,
    bool enable_gpu
) {
    auto strategy = std::make_unique<GPUAcceleratedBreadthFirstSearch>(enable_gpu);
    
    // Configure based on parameters
    if (!enable_gpu) {
        strategy->enableGPU(false);
    }
    
    return strategy;
}

bool GPUSearchFactory::isGPUAvailable() {
    GPU::GPUManager manager;
    return manager.initialize();
}

std::vector<GPU::GPUDevice> GPUSearchFactory::detectGPUs() {
    GPU::GPUManager manager;
    if (manager.initialize()) {
        return manager.detectDevices();
    }
    return {};
}

void GPUSearchFactory::printSystemCapabilities() {
    std::cout << "=== TreeWidzard System Capabilities ===" << std::endl;
    
    auto devices = detectGPUs();
    if (devices.empty()) {
        std::cout << "No GPU devices detected" << std::endl;
    } else {
        std::cout << "GPU Devices: " << devices.size() << std::endl;
        for (const auto& device : devices) {
            std::cout << "  - " << device.name << " (" << device.memory_mb << " MB)" << std::endl;
        }
    }
    
    std::cout << "CPU Optimizations: Enabled" << std::endl;
    std::cout << "Memory Pools: Enabled" << std::endl;
    std::cout << "Witness Caching: Enabled" << std::endl;
    std::cout << "Parallel Processing: Enabled" << std::endl;
    std::cout << "=======================================" << std::endl;
}

// AdaptiveGPUStrategy Implementation
AdaptiveGPUStrategy::AdaptiveGPUStrategy()
    : GPUAcceleratedBreadthFirstSearch(true)
    , performance_monitor(std::make_unique<GPUPerformanceMonitor>())
{
    // Initialize with conservative settings
    config.gpu_threshold = 100;
    config.cpu_gpu_ratio = 0.5;
    config.learning_enabled = true;
    config.learning_window = 1000;
}

std::vector<State> AdaptiveGPUStrategy::expandState(const State& state) {
    auto result = GPUAcceleratedBreadthFirstSearch::expandState(state);
    
    computation_count++;
    
    // Trigger learning periodically
    if (config.learning_enabled && (computation_count % config.learning_window == 0)) {
        learnFromPerformance();
    }
    
    return result;
}

void AdaptiveGPUStrategy::optimizeStrategy() {
    if (!config.learning_enabled) {
        return;
    }
    
    updateThresholds();
    learnFromPerformance();
    
    std::cout << "Strategy optimized based on performance data" << std::endl;
}

void AdaptiveGPUStrategy::updateThresholds() {
    // Analyze performance data to adjust GPU threshold
    auto speedups = performance_monitor->getCoreSpeedups();
    
    double avg_speedup = 0.0;
    if (!speedups.empty()) {
        avg_speedup = std::accumulate(speedups.begin(), speedups.end(), 0.0,
            [](double sum, const auto& pair) { return sum + pair.second; }) / speedups.size();
    }
    
    // Adjust threshold based on average speedup
    if (avg_speedup > 2.0) {
        config.gpu_threshold = std::max(50, static_cast<int>(config.gpu_threshold * 0.8));
    } else if (avg_speedup < 1.2) {
        config.gpu_threshold = std::min(500, static_cast<int>(config.gpu_threshold * 1.2));
    }
}

void AdaptiveGPUStrategy::learnFromPerformance() {
    // Simple learning algorithm - adjust CPU/GPU ratio based on performance
    double gpu_utilization = performance_monitor->getGPUUtilization();
    
    if (gpu_utilization > 0.8) {
        // GPU is being used heavily - increase GPU preference
        config.cpu_gpu_ratio = std::max(0.1, config.cpu_gpu_ratio - 0.1);
    } else if (gpu_utilization < 0.2) {
        // GPU is underutilized - increase CPU preference
        config.cpu_gpu_ratio = std::min(0.9, config.cpu_gpu_ratio + 0.1);
    }
}

// GPUPerformanceMonitor Implementation (simplified)
void GPUPerformanceMonitor::recordMeasurement(
    const std::string& operation,
    const std::string& core_name,
    size_t batch_size,
    double cpu_time_ms,
    double gpu_time_ms,
    bool gpu_used
) {
    std::lock_guard<std::mutex> lock(measurements_mutex);
    
    Measurement measurement;
    measurement.operation = operation;
    measurement.core_name = core_name;
    measurement.batch_size = batch_size;
    measurement.cpu_time_ms = cpu_time_ms;
    measurement.gpu_time_ms = gpu_time_ms;
    measurement.speedup = gpu_used ? (cpu_time_ms / gpu_time_ms) : 1.0;
    measurement.gpu_used = gpu_used;
    measurement.timestamp = std::chrono::system_clock::now();
    
    measurements.push_back(measurement);
}

double GPUPerformanceMonitor::getAverageSpeedup() const {
    std::lock_guard<std::mutex> lock(measurements_mutex);
    
    if (measurements.empty()) return 1.0;
    
    double total_speedup = 0.0;
    size_t gpu_measurements = 0;
    
    for (const auto& m : measurements) {
        if (m.gpu_used) {
            total_speedup += m.speedup;
            gpu_measurements++;
        }
    }
    
    return gpu_measurements > 0 ? (total_speedup / gpu_measurements) : 1.0;
}

double GPUPerformanceMonitor::getGPUUtilization() const {
    std::lock_guard<std::mutex> lock(measurements_mutex);
    
    if (measurements.empty()) return 0.0;
    
    size_t gpu_count = 0;
    for (const auto& m : measurements) {
        if (m.gpu_used) gpu_count++;
    }
    
    return static_cast<double>(gpu_count) / measurements.size();
}

size_t GPUPerformanceMonitor::getTotalComputations() const {
    std::lock_guard<std::mutex> lock(measurements_mutex);
    return measurements.size();
}

std::map<std::string, double> GPUPerformanceMonitor::getCoreSpeedups() const {
    std::lock_guard<std::mutex> lock(measurements_mutex);
    
    std::map<std::string, std::vector<double>> core_speedups;
    
    for (const auto& m : measurements) {
        if (m.gpu_used) {
            core_speedups[m.core_name].push_back(m.speedup);
        }
    }
    
    std::map<std::string, double> averages;
    for (const auto& [core, speedups] : core_speedups) {
        if (!speedups.empty()) {
            double avg = std::accumulate(speedups.begin(), speedups.end(), 0.0) / speedups.size();
            averages[core] = avg;
        }
    }
    
    return averages;
}

} // namespace TreeWidzard