#ifndef PERFORMANCE_INTEGRATION_H
#define PERFORMANCE_INTEGRATION_H

#include "MemoryPool.h"
#include "WitnessCache.h"
#include "ParallelOptimization.h"
#include "PerformanceProfiler.h"
#include "../Kernel/State.h"
#include "../Kernel/WitnessSet.h"
#include "../Kernel/SearchStrategy.h"

namespace TreeWidzard {

// Performance-optimized State class
class OptimizedState : public State {
public:
    // Use memory pool for allocation
    static void* operator new(size_t size) {
        return TreeWidzardMemoryManager::getInstance().getStatePool().acquire();
    }

    static void operator delete(void* ptr) {
        TreeWidzardMemoryManager::getInstance().getStatePool().release(static_cast<State*>(ptr));
    }

    // Copy constructor with profiling
    OptimizedState(const OptimizedState& other) : State(other) {
        PROFILE_SCOPE("OptimizedState::copy");
    }

    // Move constructor
    OptimizedState(OptimizedState&& other) noexcept : State(std::move(other)) {
        PROFILE_SCOPE("OptimizedState::move");
    }
};

// Performance-optimized WitnessSet with caching
class CachedWitnessSet : public WitnessSet {
private:
    static WitnessSetCache& getCache() {
        return CacheManager::getInstance().getWitnessCache();
    }

public:
    // Cache-aware computation
    static WitnessSet computeWithCache(const Bag& bag, 
                                     std::function<WitnessSet()> compute_func,
                                     size_t computation_cost = 1) {
        PROFILE_SCOPE("CachedWitnessSet::computeWithCache");
        
        auto cached_result = getCache().get(bag);
        if (cached_result) {
            return *cached_result;
        }

        // Compute and cache result
        auto result = compute_func();
        getCache().insert(bag, WitnessSet(result), computation_cost);
        return result;
    }

    // Batch cache operations
    static void preloadCache(const std::vector<Bag>& bags,
                           std::function<WitnessSet(const Bag&)> compute_func) {
        PROFILE_SCOPE("CachedWitnessSet::preloadCache");
        
        NUMAThreadPool pool;
        std::vector<std::future<void>> futures;

        for (const auto& bag : bags) {
            if (!getCache().contains(bag)) {
                futures.push_back(pool.submit([&bag, &compute_func]() {
                    auto result = compute_func(bag);
                    getCache().insert(bag, std::move(result));
                }));
            }
        }

        // Wait for all preloading to complete
        for (auto& future : futures) {
            future.wait();
        }
    }
};

// Performance-optimized SearchStrategy base class
class OptimizedSearchStrategy : public SearchStrategy {
protected:
    std::unique_ptr<ParallelSearchCoordinator> parallel_coordinator;
    std::unique_ptr<OptimizedParallelBFS> parallel_bfs;
    bool use_parallel_execution;
    size_t num_threads;

public:
    explicit OptimizedSearchStrategy(DynamicKernel* kernel, 
                                   Conjecture* conjecture, 
                                   Flags* flags,
                                   size_t threads = 0)
        : SearchStrategy(kernel, conjecture, flags)
        , use_parallel_execution(threads > 1)
        , num_threads(threads > 0 ? threads : std::thread::hardware_concurrency()) {
        
        if (use_parallel_execution) {
            parallel_coordinator = std::make_unique<ParallelSearchCoordinator>(num_threads);
            parallel_bfs = std::make_unique<OptimizedParallelBFS>(num_threads);
        }
    }

    void search() override {
        PROFILE_FUNCTION();
        
        if (use_parallel_execution) {
            parallelSearch();
        } else {
            sequentialSearch();
        }
    }

protected:
    virtual void parallelSearch() {
        PROFILE_SCOPE("OptimizedSearchStrategy::parallelSearch");
        
        // Default implementation using parallel BFS
        auto initial_state = getInitialState();
        auto expand_func = [this](const State& state) { return expandState(state); };
        auto goal_test = [this](const State& state) { return isGoalState(state); };
        
        auto result = parallel_bfs->search(initial_state, expand_func, goal_test);
        
        if (result) {
            handleSolution(*result);
        } else {
            handleNoSolution();
        }
    }

    virtual void sequentialSearch() {
        PROFILE_SCOPE("OptimizedSearchStrategy::sequentialSearch");
        
        // Call original search implementation
        SearchStrategy::search();
    }

    // Virtual methods to be implemented by derived classes
    virtual State getInitialState() = 0;
    virtual std::vector<State> expandState(const State& state) = 0;
    virtual bool isGoalState(const State& state) = 0;
    virtual void handleSolution(const State& solution) = 0;
    virtual void handleNoSolution() = 0;
};

// Enhanced parallel breadth-first search with all optimizations
class EnhancedParallelBreadthFirstSearch : public OptimizedSearchStrategy {
private:
    std::vector<std::shared_ptr<TermNode<RunNodeContent<State::ptr, ConcreteNode>>>> work_queue;
    std::mutex queue_mutex;
    std::atomic<bool> solution_found;

public:
    EnhancedParallelBreadthFirstSearch(DynamicKernel* kernel, 
                                     Conjecture* conjecture, 
                                     Flags* flags,
                                     size_t threads = 0)
        : OptimizedSearchStrategy(kernel, conjecture, flags, threads)
        , solution_found(false) {}

protected:
    State getInitialState() override {
        PROFILE_SCOPE("EnhancedParallelBFS::getInitialState");
        // Implementation depends on TreeWidzard's state structure
        return State(); // Placeholder
    }

    std::vector<State> expandState(const State& state) override {
        PROFILE_SCOPE("EnhancedParallelBFS::expandState");
        
        std::vector<State> expanded;
        // Use cached witness set computation
        auto bag = state.get_bag();
        
        auto witnesses = CachedWitnessSet::computeWithCache(bag, [&]() {
            // Original witness computation logic
            return computeWitnessSet(state);
        });

        // Generate next states from witnesses
        for (const auto& witness : witnesses.getWitnesses()) {
            expanded.emplace_back(createNextState(state, witness));
        }

        return expanded;
    }

    bool isGoalState(const State& state) override {
        PROFILE_SCOPE("EnhancedParallelBFS::isGoalState");
        
        // Check if conjecture is satisfied
        return getConjecture()->evaluateConjectureOnState(state) > 0.5;
    }

    void handleSolution(const State& solution) override {
        PROFILE_SCOPE("EnhancedParallelBFS::handleSolution");
        
        solution_found = true;
        // Process solution
        generateWitnessFiles(solution);
    }

    void handleNoSolution() override {
        PROFILE_SCOPE("EnhancedParallelBFS::handleNoSolution");
        
        // Handle case where no solution is found
        std::cout << "No solution found within search space." << std::endl;
    }

private:
    WitnessSet computeWitnessSet(const State& state) {
        // Placeholder for actual witness set computation
        return WitnessSet();
    }

    State createNextState(const State& current, const Witness& witness) {
        // Placeholder for state transition
        return State();
    }

    void generateWitnessFiles(const State& solution) {
        // Generate output files for the solution
    }
};

// Performance monitoring integration
class PerformanceMonitor {
private:
    static std::unique_ptr<PerformanceMonitor> instance;
    std::thread monitoring_thread;
    std::atomic<bool> monitoring_active;
    std::chrono::seconds report_interval;

public:
    static PerformanceMonitor& getInstance() {
        if (!instance) {
            instance = std::unique_ptr<PerformanceMonitor>(new PerformanceMonitor());
        }
        return *instance;
    }

    void startMonitoring(std::chrono::seconds interval = std::chrono::seconds(30)) {
        if (monitoring_active.load()) return;

        report_interval = interval;
        monitoring_active = true;
        
        monitoring_thread = std::thread([this]() {
            while (monitoring_active.load()) {
                std::this_thread::sleep_for(report_interval);
                if (monitoring_active.load()) {
                    generatePeriodicReport();
                }
            }
        });
    }

    void stopMonitoring() {
        monitoring_active = false;
        if (monitoring_thread.joinable()) {
            monitoring_thread.join();
        }
    }

    void generateFinalReport(const std::string& filename = "performance_report.txt") {
        auto& profiler = PerformanceProfiler::getInstance();
        auto& cache_manager = CacheManager::getInstance();

        std::ofstream report(filename);
        if (report.is_open()) {
            report << "=== TreeWidzard Performance Report ===\n";
            report << "Generated at: " << getCurrentTimestamp() << "\n\n";

            // Profiling data
            profiler.printReport(report);

            // Cache statistics
            auto cache_stats = cache_manager.getGlobalStatistics();
            report << "\n=== Cache Performance ===\n";
            report << "Witness Cache Hit Ratio: " 
                   << cache_stats.witness_stats.hit_ratio * 100 << "%\n";
            report << "DP Cache Hit Ratio: " 
                   << cache_stats.dp_stats.hit_ratio * 100 << "%\n";
            report << "Total Cache Memory: " 
                   << cache_stats.total_memory_usage / (1024 * 1024) << " MB\n\n";

            // Memory pool statistics
            auto pool_stats = TreeWidzardMemoryManager::getInstance().getStatistics();
            report << "=== Memory Pool Performance ===\n";
            report << "State Pool Utilization: " 
                   << pool_stats.state_stats.utilization_ratio * 100 << "%\n";
            report << "WitnessSet Pool Utilization: " 
                   << pool_stats.witness_set_stats.utilization_ratio * 100 << "%\n";

            report.close();
        }

        // Also print to console
        profiler.printReport();
    }

private:
    PerformanceMonitor() : monitoring_active(false) {}

    void generatePeriodicReport() {
        auto& profiler = PerformanceProfiler::getInstance();
        auto sys_metrics = profiler.getSystemMetrics();
        
        std::cout << "[" << getCurrentTimestamp() << "] "
                  << "Memory: " << sys_metrics.current_memory_mb << " MB, "
                  << "CPU: " << sys_metrics.cpu_usage_percent << "%, "
                  << "Calls: " << sys_metrics.total_function_calls << std::endl;
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        return std::string(std::ctime(&time_t));
    }
};

// Convenience functions for easy integration
namespace Performance {
    
    // Initialize all performance optimizations
    inline void initialize(bool enable_profiling = true, 
                          bool enable_caching = true,
                          size_t num_threads = 0) {
        if (enable_profiling) {
            PerformanceProfiler::getInstance().enableProfiling(true, true);
            PerformanceMonitor::getInstance().startMonitoring();
        }

        if (enable_caching) {
            CacheManager::getInstance().enableCaching(true);
        }

        std::cout << "TreeWidzard performance optimizations initialized with "
                  << (num_threads > 0 ? num_threads : std::thread::hardware_concurrency())
                  << " threads." << std::endl;
    }

    // Finalize and generate reports
    inline void finalize(const std::string& report_file = "performance_report.txt") {
        PerformanceMonitor::getInstance().stopMonitoring();
        PerformanceMonitor::getInstance().generateFinalReport(report_file);
        
        // Print cache statistics
        auto& cache_manager = CacheManager::getInstance();
        auto stats = cache_manager.getGlobalStatistics();
        
        std::cout << "\nPerformance Summary:\n";
        std::cout << "  Witness Cache Hit Ratio: " 
                  << stats.witness_stats.hit_ratio * 100 << "%\n";
        std::cout << "  Memory Pool Peak Usage: " 
                  << TreeWidzardMemoryManager::getInstance().getStatistics().state_stats.total_allocated 
                  << " objects\n";
        std::cout << "  Report saved to: " << report_file << std::endl;
    }

    // Quick performance test
    inline void runPerformanceTest() {
        std::cout << "Running performance test..." << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Test memory pool performance
        {
            PROFILE_SCOPE("MemoryPoolTest");
            std::vector<PooledObject<State>> states;
            auto& manager = TreeWidzardMemoryManager::getInstance();
            
            for (int i = 0; i < 10000; ++i) {
                states.push_back(manager.createState());
            }
        }
        
        // Test cache performance
        {
            PROFILE_SCOPE("CacheTest");
            auto& cache = CacheManager::getInstance().getWitnessCache();
            
            for (int i = 0; i < 1000; ++i) {
                Bag bag;
                WitnessSet ws;
                cache.insert(bag, std::move(ws));
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Performance test completed in " << duration.count() << " ms" << std::endl;
        
        PerformanceProfiler::getInstance().printReport();
    }
}

} // namespace TreeWidzard

#endif // PERFORMANCE_INTEGRATION_H