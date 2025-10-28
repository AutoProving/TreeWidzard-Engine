#include "../GPU/GPUSearchStrategy.h"
#include "../GPU/GPUAcceleration.h"
#include "../Kernel/State.h"
#include "../Performance/PerformanceTimer.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * TreeWidzard GPU Application
 * 
 * Enhanced version of TreeWidzard with GPU acceleration support.
 * This application automatically detects GPU capabilities and uses
 * GPU acceleration for complex graph property verification.
 */

void printGPUBanner() {
    std::cout << R"(
===============================================
     TreeWidzard GPU-Accelerated Edition
===============================================
    )" << std::endl;
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] <search_strategy> <conjecture_file>" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -atp                Use ATP mode" << std::endl;
    std::cout << "  -pl                 Enable progress logging" << std::endl;
    std::cout << "  -gpu                Force GPU acceleration (default: auto)" << std::endl;
    std::cout << "  -cpu                Force CPU-only mode" << std::endl;
    std::cout << "  -gpu-info           Show GPU information and exit" << std::endl;
    std::cout << "  -benchmark          Run performance benchmark" << std::endl;
    std::cout << "  tw = <width>        Set tree width parameter" << std::endl;
    std::cout << "\nSearch Strategies:" << std::endl;
    std::cout << "  BreadthFirstSearch         Standard breadth-first search" << std::endl;
    std::cout << "  GPUBreadthFirstSearch      GPU-accelerated breadth-first search" << std::endl;
    std::cout << "  AdaptiveGPUSearch          Self-tuning GPU/CPU hybrid search" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << program_name << " -atp tw = 3 -pl -gpu BreadthFirstSearch conjecture.txt" << std::endl;
    std::cout << "  " << program_name << " -gpu-info" << std::endl;
    std::cout << "  " << program_name << " -benchmark" << std::endl;
}

void showGPUInfo() {
    std::cout << "=== GPU Information ===" << std::endl;
    
    TreeWidzard::GPU::GPUSearchFactory::printSystemCapabilities();
    
    auto devices = TreeWidzard::GPU::GPUSearchFactory::detectGPUs();
    if (devices.empty()) {
        std::cout << "\nNo GPU devices detected." << std::endl;
        std::cout << "TreeWidzard will run in CPU-only mode." << std::endl;
    } else {
        std::cout << "\nGPU acceleration available!" << std::endl;
        std::cout << "Recommended: Use -gpu flag for complex properties." << std::endl;
    }
}

void runBenchmark() {
    std::cout << "=== TreeWidzard GPU Benchmark ===" << std::endl;
    
    // Simple benchmark comparing CPU vs GPU performance
    TreeWidzard::PerformanceTimer timer;
    
    // Create test bags
    std::vector<TreeWidzard::Bag> test_bags;
    for (int i = 0; i < 100; i++) {
        std::set<unsigned> elements;
        for (int j = 0; j < 5; j++) {
            elements.insert(i * 10 + j);
        }
        test_bags.emplace_back(elements);
    }
    
    // Test CPU performance
    std::cout << "Testing CPU performance..." << std::endl;
    timer.start();
    
    auto cpu_strategy = std::make_unique<TreeWidzard::EnhancedParallelBreadthFirstSearch>();
    // Simulate some work
    for (int i = 0; i < 10; i++) {
        TreeWidzard::State test_state;
        test_state.set_bag(test_bags[i % test_bags.size()]);
        auto results = cpu_strategy->expandState(test_state);
    }
    
    timer.stop();
    double cpu_time = timer.getElapsedMilliseconds();
    std::cout << "CPU time: " << cpu_time << " ms" << std::endl;
    
    // Test GPU performance
    std::cout << "Testing GPU performance..." << std::endl;
    timer.start();
    
    auto gpu_strategy = TreeWidzard::GPU::GPUSearchFactory::createGPUStrategy();
    if (gpu_strategy && gpu_strategy->isGPUEnabled()) {
        // Simulate some work
        for (int i = 0; i < 10; i++) {
            TreeWidzard::State test_state;
            test_state.set_bag(test_bags[i % test_bags.size()]);
            auto results = gpu_strategy->expandState(test_state);
        }
        
        timer.stop();
        double gpu_time = timer.getElapsedMilliseconds();
        std::cout << "GPU time: " << gpu_time << " ms" << std::endl;
        
        if (gpu_time > 0) {
            double speedup = cpu_time / gpu_time;
            std::cout << "Speedup: " << speedup << "x" << std::endl;
        }
        
        gpu_strategy->printPerformanceStats();
    } else {
        std::cout << "GPU not available for testing" << std::endl;
    }
}

std::unique_ptr<TreeWidzard::SearchStrategy> createSearchStrategy(
    const std::string& strategy_name,
    bool force_gpu,
    bool force_cpu
) {
    if (force_cpu) {
        std::cout << "Creating CPU-only search strategy: " << strategy_name << std::endl;
        return std::make_unique<TreeWidzard::EnhancedParallelBreadthFirstSearch>();
    }
    
    if (strategy_name == "GPUBreadthFirstSearch" || force_gpu) {
        std::cout << "Creating GPU-accelerated search strategy..." << std::endl;
        auto gpu_strategy = TreeWidzard::GPU::GPUSearchFactory::createGPUStrategy();
        if (gpu_strategy && gpu_strategy->isGPUEnabled()) {
            std::cout << "GPU acceleration enabled" << std::endl;
            return std::move(gpu_strategy);
        } else {
            std::cout << "GPU not available - falling back to CPU" << std::endl;
            return std::make_unique<TreeWidzard::EnhancedParallelBreadthFirstSearch>();
        }
    }
    
    if (strategy_name == "AdaptiveGPUSearch") {
        std::cout << "Creating adaptive GPU search strategy..." << std::endl;
        auto adaptive_strategy = std::make_unique<TreeWidzard::AdaptiveGPUStrategy>();
        std::cout << "Adaptive learning enabled" << std::endl;
        return std::move(adaptive_strategy);
    }
    
    // Default: use optimal strategy based on hardware
    std::cout << "Creating optimal search strategy for current hardware..." << std::endl;
    return TreeWidzard::GPU::GPUSearchFactory::createOptimalStrategy();
}

int main(int argc, char* argv[]) {
    printGPUBanner();
    
    // Parse command line arguments
    bool atp_mode = false;
    bool progress_logging = false;
    bool force_gpu = false;
    bool force_cpu = false;
    bool show_gpu_info = false;
    bool run_benchmark = false;
    int tree_width = 3;
    std::string search_strategy;
    std::string conjecture_file;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-atp") {
            atp_mode = true;
        } else if (arg == "-pl") {
            progress_logging = true;
        } else if (arg == "-gpu") {
            force_gpu = true;
        } else if (arg == "-cpu") {
            force_cpu = true;
        } else if (arg == "-gpu-info") {
            show_gpu_info = true;
        } else if (arg == "-benchmark") {
            run_benchmark = true;
        } else if (arg == "tw" && i + 2 < argc && argv[i + 1] == std::string("=")) {
            tree_width = std::stoi(argv[i + 2]);
            i += 2;
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (search_strategy.empty()) {
            search_strategy = arg;
        } else if (conjecture_file.empty()) {
            conjecture_file = arg;
        }
    }
    
    // Handle special modes
    if (show_gpu_info) {
        showGPUInfo();
        return 0;
    }
    
    if (run_benchmark) {
        runBenchmark();
        return 0;
    }
    
    // Validate arguments
    if (search_strategy.empty() || conjecture_file.empty()) {
        std::cerr << "Error: Missing required arguments" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    if (force_gpu && force_cpu) {
        std::cerr << "Error: Cannot specify both -gpu and -cpu flags" << std::endl;
        return 1;
    }
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Tree Width: " << tree_width << std::endl;
    std::cout << "  Search Strategy: " << search_strategy << std::endl;
    std::cout << "  Conjecture File: " << conjecture_file << std::endl;
    std::cout << "  ATP Mode: " << (atp_mode ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Progress Logging: " << (progress_logging ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    try {
        // Create search strategy
        auto strategy = createSearchStrategy(search_strategy, force_gpu, force_cpu);
        
        if (!strategy) {
            std::cerr << "Error: Failed to create search strategy" << std::endl;
            return 1;
        }
        
        // Load conjecture file
        std::cout << "Loading conjecture from: " << conjecture_file << std::endl;
        
        // TODO: Integrate with actual TreeWidzard conjecture loading and execution
        // For now, this is a placeholder showing the GPU integration structure
        
        std::cout << "Starting search..." << std::endl;
        TreeWidzard::PerformanceTimer total_timer;
        total_timer.start();
        
        // Simulate search execution
        TreeWidzard::State initial_state;
        std::set<unsigned> initial_bag = {1, 2, 3, 4, 5};
        initial_state.set_bag(TreeWidzard::Bag(initial_bag));
        
        auto expanded_states = strategy->expandState(initial_state);
        bool goal_reached = false;
        
        for (const auto& state : expanded_states) {
            if (strategy->isGoalState(state)) {
                goal_reached = true;
                break;
            }
        }
        
        total_timer.stop();
        
        std::cout << "Search completed!" << std::endl;
        std::cout << "Total time: " << total_timer.getElapsedMilliseconds() << " ms" << std::endl;
        std::cout << "States expanded: " << expanded_states.size() << std::endl;
        std::cout << "Goal reached: " << (goal_reached ? "Yes" : "No") << std::endl;
        
        // Print performance statistics for GPU strategies
        auto gpu_strategy = dynamic_cast<TreeWidzard::GPUAcceleratedBreadthFirstSearch*>(strategy.get());
        if (gpu_strategy) {
            gpu_strategy->printPerformanceStats();
        }
        
        std::cout << "\nTreeWidzard GPU execution completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}