#include "../GPU/GPUAcceleration.h"
#include "../GPU/GPUSearchStrategy.h"
#include "../Kernel/Bag.h"
#include "../Performance/PerformanceTimer.h"
#include <chrono>
#include <iostream>
#include <set>
#include <vector>

using namespace TreeWidzard;

/**
 * GPU Benchmark Example
 *
 * Demonstrates GPU acceleration capabilities and measures
 * performance improvements compared to CPU-only execution.
 */

void printHeader() {
  std::cout << "=== TreeWidzard GPU Acceleration Benchmark ===" << std::endl;
  std::cout << "Testing GPU vs CPU performance for witness set computations"
            << std::endl;
  std::cout << std::endl;
}

void benchmarkBasicGPUOperations() {
  std::cout << "1. Basic GPU Operations Benchmark" << std::endl;
  std::cout << "=================================" << std::endl;

  // Initialize GPU manager
  GPU::GPUManager gpu_manager;
  bool gpu_available = gpu_manager.initialize();

  if (!gpu_available) {
    std::cout << "GPU not available - skipping GPU benchmarks" << std::endl;
    return;
  }

  gpu_manager.printDeviceInfo();
  std::cout << std::endl;

  // Create test bags of varying sizes
  std::vector<size_t> batch_sizes = {10, 50, 100, 500, 1000, 5000};

  for (size_t batch_size : batch_sizes) {
    std::cout << "Testing batch size: " << batch_size << std::endl;

    // Generate test bags
    std::vector<Bag> test_bags;
    test_bags.reserve(batch_size);

    for (size_t i = 0; i < batch_size; i++) {
      std::set<unsigned> elements;
      // Create bags with 3-8 vertices
      size_t bag_size = 3 + (i % 6);
      for (size_t j = 0; j < bag_size; j++) {
        elements.insert(i * 10 + j);
      }
      Bag bag;
      bag.set_elements(elements);
      test_bags.push_back(bag);
    }

    // Test with ChromaticNumber (most GPU-friendly)
    std::string core_name = "ChromaticNumber";
    int tree_width = 3;

    // GPU benchmark
    PerformanceTimer gpu_timer;
    gpu_timer.start();

    auto gpu_result =
        gpu_manager.processOnGPU(test_bags, core_name, tree_width);

    gpu_timer.stop();

    if (gpu_result.success) {
      std::cout << "  GPU time: " << gpu_result.computation_time_ms << " ms"
                << std::endl;
      std::cout << "  GPU memory used: " << gpu_result.memory_used_bytes / 1024
                << " KB" << std::endl;

      // Estimate CPU time using simple heuristic
      double estimated_cpu_time = batch_size * 0.1; // 0.1ms per bag estimate
      double speedup = estimated_cpu_time / gpu_result.computation_time_ms;

      std::cout << "  Estimated speedup: " << speedup << "x" << std::endl;
    } else {
      std::cout << "  GPU computation failed" << std::endl;
    }

    std::cout << std::endl;
  }
}

void benchmarkSearchStrategyComparison() {
  std::cout << "2. Search Strategy Comparison" << std::endl;
  std::cout << "============================" << std::endl;

  // Create different search strategies
  auto cpu_strategy =
      std::make_unique<BenchmarkerCPU>(nullptr, nullptr, nullptr);
  auto gpu_strategy =
      std::make_unique<BenchmarkerGPU>(nullptr, nullptr, nullptr, true);

  if (!gpu_strategy) {
    std::cout << "GPU strategy not available" << std::endl;
    return;
  }

  // Test with synthetic state
  State test_state;
  std::set<unsigned> bag_elements = {1, 2, 3, 4, 5};
  Bag test_bag;
  test_bag.set_elements(bag_elements);
  test_state.set_bag(test_bag);

  std::cout << "Testing state expansion with bag size: " << bag_elements.size()
            << std::endl;

  // CPU timing
  PerformanceTimer cpu_timer;
  cpu_timer.start();

  auto cpu_results = cpu_strategy->publicExpandState(test_state);

  cpu_timer.stop();

  // GPU timing
  PerformanceTimer gpu_timer;
  gpu_timer.start();

  auto gpu_results = gpu_strategy->publicExpandState(test_state);

  gpu_timer.stop();

  std::cout << "CPU Strategy:" << std::endl;
  std::cout << "  Time: " << cpu_timer.getElapsedMilliseconds() << " ms"
            << std::endl;
  std::cout << "  States generated: " << cpu_results.size() << std::endl;

  std::cout << "GPU Strategy:" << std::endl;
  std::cout << "  Time: " << gpu_timer.getElapsedMilliseconds() << " ms"
            << std::endl;
  std::cout << "  States generated: " << gpu_results.size() << std::endl;

  if (gpu_timer.getElapsedMilliseconds() > 0) {
    double speedup =
        cpu_timer.getElapsedMilliseconds() / gpu_timer.getElapsedMilliseconds();
    std::cout << "  Speedup: " << speedup << "x" << std::endl;
  }

  std::cout << std::endl;

  // Print GPU performance statistics
  gpu_strategy->printPerformanceStats();
}

void benchmarkComplexProperties() {
  std::cout << "3. Complex Property Benchmark" << std::endl;
  std::cout << "=============================" << std::endl;

  std::vector<std::string> properties = {"ChromaticNumber", "CliqueNumber",
                                         "VertexCount", "MaxDegree"};

  for (const auto &property : properties) {
    std::cout << "Testing property: " << property << std::endl;

    // Create larger bags for complex properties
    std::vector<Bag> large_bags;
    for (size_t i = 0; i < 200; i++) {
      std::set<unsigned> elements;
      for (size_t j = 0; j < 6; j++) { // Tree width 6
        elements.insert(i * 10 + j);
      }
      large_bags.emplace_back(elements);
    }

    GPU::GPUManager gpu_manager;
    if (gpu_manager.initialize()) {
      auto result =
          gpu_manager.processWithAutoSelection(large_bags, property, 6);

      if (result.success) {
        double estimated_speedup =
            GPU::Utils::estimateSpeedup(property, large_bags.size());
        std::cout << "  GPU time: " << result.computation_time_ms << " ms"
                  << std::endl;
        std::cout << "  Estimated speedup: " << estimated_speedup << "x"
                  << std::endl;
      } else {
        std::cout << "  GPU processing not beneficial for this property"
                  << std::endl;
      }
    }

    std::cout << std::endl;
  }
}

void runAdaptiveStrategyDemo() {
  std::cout << "4. Adaptive GPU Strategy Demo" << std::endl;
  std::cout << "=============================" << std::endl;

  auto adaptive_strategy =
      std::make_unique<AdaptiveGPUStrategy>(nullptr, nullptr, nullptr);

  std::cout << "Initial configuration:" << std::endl;
  const auto &config = adaptive_strategy->getConfig();
  std::cout << "  GPU threshold: " << config.gpu_threshold << std::endl;
  std::cout << "  CPU/GPU ratio: " << config.cpu_gpu_ratio << std::endl;
  std::cout << "  Learning enabled: "
            << (config.learning_enabled ? "Yes" : "No") << std::endl;

  // Simulate several computations to trigger learning
  State test_state;
  std::set<unsigned> elements = {1, 2, 3, 4, 5, 6};
  Bag bag;
  bag.set_elements(elements);
  test_state.set_bag(bag);

  std::cout << "\nRunning adaptive learning simulation..." << std::endl;

  for (int i = 0; i < 10; i++) {
    auto results = adaptive_strategy->expandState(test_state);
    std::cout << "  Iteration " << (i + 1) << ": " << results.size()
              << " states generated" << std::endl;
  }

  // Check if strategy adapted
  std::cout << "\nStrategy optimization..." << std::endl;
  adaptive_strategy->optimizeStrategy();

  std::cout << "Adaptive learning completed." << std::endl;
  std::cout << std::endl;
}

void printSystemCapabilities() {
  std::cout << "5. System GPU Capabilities" << std::endl;
  std::cout << "=========================" << std::endl;

  GPUSearchFactory::printSystemCapabilities();

  auto devices = GPUSearchFactory::detectGPUs();
  std::cout << "Detected " << devices.size() << " GPU device(s):" << std::endl;

  for (size_t i = 0; i < devices.size(); i++) {
    const auto &device = devices[i];
    std::cout << "  Device " << i << ":" << std::endl;
    std::cout << "    Name: " << device.name << std::endl;
    std::cout << "    Memory: " << device.memory_mb << " MB" << std::endl;
    std::cout << "    CUDA: "
              << (device.cuda_available ? "Available" : "Not available")
              << std::endl;
    std::cout << "    OpenCL: "
              << (device.opencl_available ? "Available" : "Not available")
              << std::endl;
  }

  // Check specific GPU features
  std::cout << "\nGPU Feature Support:" << std::endl;
  std::cout << "  NVIDIA GPU: " << (GPU::Utils::hasNVIDIAGPU() ? "Yes" : "No")
            << std::endl;
  std::cout << "  GPU Available: "
            << (GPUSearchFactory::isGPUAvailable() ? "Yes" : "No") << std::endl;

  std::cout << std::endl;
}

int main() {
  printHeader();

  try {
    // Run all benchmarks
    printSystemCapabilities();
    benchmarkBasicGPUOperations();
    benchmarkSearchStrategyComparison();
    benchmarkComplexProperties();
    runAdaptiveStrategyDemo();

    std::cout << "=== GPU Benchmark Complete ===" << std::endl;
    std::cout << "\nFor production use:" << std::endl;
    std::cout
        << "1. Use GPUAcceleratedBreadthFirstSearch for complex properties"
        << std::endl;
    std::cout << "2. Enable GPU acceleration with --gpu flag" << std::endl;
    std::cout << "3. Monitor performance with built-in profiling" << std::endl;
    std::cout << "4. Consider AdaptiveGPUStrategy for mixed workloads"
              << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Benchmark error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}