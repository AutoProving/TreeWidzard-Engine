#include "Performance/PerformanceIntegration.h"
#include "Kernel/SearchStrategy.h"
#include "Conjecture/Conjecture.h"

// Example of an optimized search strategy using all performance features
class OptimizedTreeWidzardSearchStrategy : public TreeWidzard::EnhancedParallelBreadthFirstSearch {
public:
    OptimizedTreeWidzardSearchStrategy(DynamicKernel* kernel, 
                                     Conjecture* conjecture, 
                                     Flags* flags,
                                     size_t threads = 0)
        : EnhancedParallelBreadthFirstSearch(kernel, conjecture, flags, threads) {
        
        // Initialize performance monitoring
        TreeWidzard::Performance::initialize(true, true, threads);
    }

    ~OptimizedTreeWidzardSearchStrategy() {
        // Generate final performance report
        TreeWidzard::Performance::finalize("search_performance_report.txt");
    }

protected:
    State getInitialState() override {
        PROFILE_FUNCTION();
        
        // Create initial state using memory pool
        auto pooled_state = TreeWidzard::CREATE_POOLED_STATE();
        
        // Initialize state based on tree decomposition root
        auto root_node = getKernel()->getTreeDecomposition()->getRoot();
        pooled_state->set_bag(root_node->getBag());
        
        return *pooled_state;
    }

    std::vector<State> expandState(const State& state) override {
        PROFILE_SCOPE("OptimizedExpansion");
        
        std::vector<State> expanded_states;
        auto bag = state.get_bag();
        
        // Use cached witness set computation
        auto witnesses = TreeWidzard::CachedWitnessSet::computeWithCache(
            bag, 
            [this, &state]() {
                return computeWitnessSetForState(state);
            },
            estimateComputationCost(bag)
        );

        // Generate successor states
        for (const auto& witness : witnesses.getWitnesses()) {
            auto next_state = createSuccessorState(state, witness);
            if (isValidState(next_state)) {
                expanded_states.push_back(std::move(next_state));
            }
        }

        return expanded_states;
    }

    bool isGoalState(const State& state) override {
        PROFILE_SCOPE("GoalStateCheck");
        
        // Check if we've reached a leaf node and conjecture is satisfied
        auto current_node = getCurrentNode(state);
        if (!current_node->isLeaf()) {
            return false;
        }

        // Evaluate conjecture on final state
        double conjecture_value = getConjecture()->evaluateConjectureOnState(state);
        return conjecture_value > 0.5; // Threshold for satisfaction
    }

    void handleSolution(const State& solution) override {
        PROFILE_SCOPE("SolutionGeneration");
        
        std::cout << "Solution found! Generating witness files..." << std::endl;
        
        // Generate various output formats
        generateWitnessFiles(solution, "CounterExample");
        generateRunTree(solution);
        generateConcreteDecomposition(solution);
        
        // Print performance statistics
        auto& profiler = TreeWidzard::PerformanceProfiler::getInstance();
        auto metrics = profiler.getSystemMetrics();
        
        std::cout << "Search completed successfully:" << std::endl;
        std::cout << "  Peak memory usage: " << metrics.peak_memory_mb << " MB" << std::endl;
        std::cout << "  Total function calls: " << metrics.total_function_calls << std::endl;
        std::cout << "  CPU utilization: " << metrics.cpu_usage_percent << "%" << std::endl;
        
        // Print cache statistics
        auto& cache_manager = TreeWidzard::CacheManager::getInstance();
        auto cache_stats = cache_manager.getGlobalStatistics();
        std::cout << "  Cache hit ratio: " << cache_stats.witness_stats.hit_ratio * 100 << "%" << std::endl;
    }

private:
    WitnessSet computeWitnessSetForState(const State& state) {
        PROFILE_SCOPE("WitnessSetComputation");
        
        // Get the appropriate DP core for computation
        auto bag = state.get_bag();
        auto core_name = getCurrentCoreName();
        auto* core = getKernel()->getCoreByName(core_name);
        
        if (!core) {
            throw std::runtime_error("DP Core not found: " + core_name);
        }

        // Compute witness set using the core
        return core->computeWitnessSet(bag, state);
    }

    State createSuccessorState(const State& current, const Witness& witness) {
        PROFILE_SCOPE("StateTransition");
        
        // Create new state using memory pool
        auto pooled_state = TreeWidzard::CREATE_POOLED_STATE(current);
        
        // Apply witness to create successor
        pooled_state->applyWitness(witness);
        
        return *pooled_state;
    }

    bool isValidState(const State& state) {
        // Quick validation check
        return state.isValid() && !state.isEmpty();
    }

    size_t estimateComputationCost(const Bag& bag) {
        // Estimate based on bag size and current tree width
        auto bag_size = bag.getVertices().size();
        auto tree_width = getKernel()->getTreeWidth();
        
        // Exponential cost in bag size
        return static_cast<size_t>(std::pow(2, bag_size)) * tree_width;
    }

    void generateWitnessFiles(const State& solution, const std::string& prefix) {
        PROFILE_SCOPE("WitnessFileGeneration");
        
        // Generate multiple output formats
        auto output_dir = getOutputDirectory();
        
        // GML format for graph visualization
        generateGMLOutput(solution, output_dir + "/" + prefix + "_Graph.gml");
        
        // PACE format for standard compatibility
        generatePACEOutput(solution, output_dir + "/" + prefix + "_Graph.gr");
        
        // DOT format for Graphviz
        generateDOTOutput(solution, output_dir + "/" + prefix + "_Graph.dot");
    }

    void generateRunTree(const State& solution) {
        PROFILE_SCOPE("RunTreeGeneration");
        
        // Implementation would generate detailed execution trace
        auto output_file = getOutputDirectory() + "/RunTree.txt";
        // Generate run tree output...
    }

    void generateConcreteDecomposition(const State& solution) {
        PROFILE_SCOPE("ConcreteDecompositionGeneration");
        
        // Generate tree decomposition used in the computation
        auto output_file = getOutputDirectory() + "/ConcreteDecomposition.txt";
        // Generate decomposition output...
    }

    std::string getCurrentCoreName() {
        // Determine which DP core is currently being used
        return "ChromaticNumber"; // Example
    }

    std::string getOutputDirectory() {
        return "./output"; // Could be configurable
    }

    // Placeholder implementations for missing methods
    void generateGMLOutput(const State& solution, const std::string& filename) {}
    void generatePACEOutput(const State& solution, const std::string& filename) {}
    void generateDOTOutput(const State& solution, const std::string& filename) {}
    ConcreteNode* getCurrentNode(const State& state) { return nullptr; }
};

// Factory function to create optimized search strategy
std::unique_ptr<SearchStrategy> createOptimizedSearchStrategy(
    const std::string& strategy_name,
    DynamicKernel* kernel,
    Conjecture* conjecture,
    Flags* flags,
    size_t num_threads = 0) {
    
    PROFILE_FUNCTION();
    
    if (strategy_name == "OptimizedParallelBreadthFirstSearch") {
        return std::make_unique<OptimizedTreeWidzardSearchStrategy>(
            kernel, conjecture, flags, num_threads);
    }
    
    // Fall back to original strategies if optimization not requested
    return nullptr;
}

// Performance benchmarking function
void benchmarkPerformanceOptimizations() {
    std::cout << "Benchmarking TreeWidzard performance optimizations..." << std::endl;
    
    // Enable detailed profiling
    TreeWidzard::PerformanceProfiler::getInstance().enableProfiling(true, true);
    
    // Test memory pool performance
    {
        PROFILE_SCOPE("MemoryPoolBenchmark");
        auto& manager = TreeWidzard::TreeWidzardMemoryManager::getInstance();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<TreeWidzard::PooledObject<State>> states;
        for (int i = 0; i < 100000; ++i) {
            states.push_back(manager.createState());
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Memory pool allocation of 100k objects: " 
                  << duration.count() << " microseconds" << std::endl;
    }
    
    // Test cache performance
    {
        PROFILE_SCOPE("CacheBenchmark");
        auto& cache = TreeWidzard::CacheManager::getInstance().getWitnessCache();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Insert test data
        for (int i = 0; i < 10000; ++i) {
            Bag bag;
            WitnessSet ws;
            cache.insert(bag, std::move(ws));
        }
        
        // Test retrieval
        for (int i = 0; i < 10000; ++i) {
            Bag bag;
            auto result = cache.get(bag);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Cache operations (10k insert + 10k retrieve): " 
                  << duration.count() << " microseconds" << std::endl;
        
        auto stats = cache.getStatistics();
        std::cout << "Cache hit ratio: " << stats.hit_ratio * 100 << "%" << std::endl;
    }
    
    // Print final performance report
    TreeWidzard::PerformanceProfiler::getInstance().printReport();
}