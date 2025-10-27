#ifndef PERFORMANCE_PROFILER_H
#define PERFORMANCE_PROFILER_H

#include <chrono>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <fstream>
#include <atomic>
#include <iostream>
#include <iomanip>

namespace TreeWidzard {

// High-resolution timer for performance measurements
class HighResolutionTimer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_running;

public:
    HighResolutionTimer() : is_running(false) {}

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        is_running = true;
    }

    void stop() {
        if (is_running) {
            end_time = std::chrono::high_resolution_clock::now();
            is_running = false;
        }
    }

    double getElapsedSeconds() const {
        auto end = is_running ? std::chrono::high_resolution_clock::now() : end_time;
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_time);
        return duration.count() / 1e9;
    }

    double getElapsedMilliseconds() const {
        return getElapsedSeconds() * 1000.0;
    }

    double getElapsedMicroseconds() const {
        return getElapsedSeconds() * 1e6;
    }
};

// Performance metrics collection
struct PerformanceMetrics {
    double total_time_seconds;
    double min_time_seconds;
    double max_time_seconds;
    double avg_time_seconds;
    uint64_t call_count;
    uint64_t total_memory_allocated;
    uint64_t peak_memory_usage;
    double cpu_utilization;
    
    PerformanceMetrics() : total_time_seconds(0), min_time_seconds(DBL_MAX), 
                          max_time_seconds(0), avg_time_seconds(0), call_count(0),
                          total_memory_allocated(0), peak_memory_usage(0), 
                          cpu_utilization(0) {}

    void update(double time_seconds, uint64_t memory_used = 0) {
        total_time_seconds += time_seconds;
        min_time_seconds = std::min(min_time_seconds, time_seconds);
        max_time_seconds = std::max(max_time_seconds, time_seconds);
        call_count++;
        avg_time_seconds = total_time_seconds / call_count;
        
        total_memory_allocated += memory_used;
        peak_memory_usage = std::max(peak_memory_usage, memory_used);
    }
};

// Thread-safe performance profiler
class PerformanceProfiler {
private:
    std::unordered_map<std::string, PerformanceMetrics> metrics_map;
    mutable std::mutex metrics_mutex;
    std::atomic<bool> profiling_enabled;
    std::atomic<bool> detailed_profiling;
    
    // Memory tracking
    std::atomic<uint64_t> current_memory_usage;
    std::atomic<uint64_t> peak_memory_usage;
    
    // CPU tracking
    std::thread cpu_monitor_thread;
    std::atomic<bool> cpu_monitoring_active;
    std::atomic<double> current_cpu_usage;

    static std::unique_ptr<PerformanceProfiler> instance;
    static std::once_flag init_flag;

    PerformanceProfiler() : profiling_enabled(false), detailed_profiling(false),
                           current_memory_usage(0), peak_memory_usage(0),
                           cpu_monitoring_active(false), current_cpu_usage(0.0) {}

public:
    static PerformanceProfiler& getInstance() {
        std::call_once(init_flag, []() {
            instance = std::unique_ptr<PerformanceProfiler>(new PerformanceProfiler());
        });
        return *instance;
    }

    ~PerformanceProfiler() {
        stopCPUMonitoring();
    }

    void enableProfiling(bool enable = true, bool detailed = false) {
        profiling_enabled = enable;
        detailed_profiling = detailed;
        
        if (enable) {
            startCPUMonitoring();
        } else {
            stopCPUMonitoring();
        }
    }

    bool isProfilingEnabled() const { return profiling_enabled.load(); }
    bool isDetailedProfilingEnabled() const { return detailed_profiling.load(); }

    void recordExecution(const std::string& function_name, double time_seconds, 
                        uint64_t memory_used = 0) {
        if (!profiling_enabled.load()) return;

        std::lock_guard<std::mutex> lock(metrics_mutex);
        metrics_map[function_name].update(time_seconds, memory_used);
    }

    void recordMemoryAllocation(uint64_t bytes) {
        if (!profiling_enabled.load()) return;
        
        current_memory_usage += bytes;
        peak_memory_usage = std::max(peak_memory_usage.load(), current_memory_usage.load());
    }

    void recordMemoryDeallocation(uint64_t bytes) {
        if (!profiling_enabled.load()) return;
        
        current_memory_usage -= std::min(current_memory_usage.load(), bytes);
    }

    PerformanceMetrics getMetrics(const std::string& function_name) const {
        std::lock_guard<std::mutex> lock(metrics_mutex);
        auto it = metrics_map.find(function_name);
        return it != metrics_map.end() ? it->second : PerformanceMetrics{};
    }

    std::unordered_map<std::string, PerformanceMetrics> getAllMetrics() const {
        std::lock_guard<std::mutex> lock(metrics_mutex);
        return metrics_map;
    }

    void clearMetrics() {
        std::lock_guard<std::mutex> lock(metrics_mutex);
        metrics_map.clear();
        current_memory_usage = 0;
        peak_memory_usage = 0;
    }

    struct SystemMetrics {
        uint64_t current_memory_mb;
        uint64_t peak_memory_mb;
        double cpu_usage_percent;
        size_t active_threads;
        uint64_t total_function_calls;
    };

    SystemMetrics getSystemMetrics() const {
        std::lock_guard<std::mutex> lock(metrics_mutex);
        
        uint64_t total_calls = 0;
        for (const auto& pair : metrics_map) {
            total_calls += pair.second.call_count;
        }

        return {
            current_memory_usage.load() / (1024 * 1024),
            peak_memory_usage.load() / (1024 * 1024),
            current_cpu_usage.load(),
            std::thread::hardware_concurrency(),
            total_calls
        };
    }

    void printReport(std::ostream& os = std::cout) const {
        std::lock_guard<std::mutex> lock(metrics_mutex);
        
        os << "\n=== TreeWidzard Performance Report ===\n";
        os << std::fixed << std::setprecision(6);
        
        // System metrics
        auto sys_metrics = getSystemMetrics();
        os << "\nSystem Metrics:\n";
        os << "  Current Memory: " << sys_metrics.current_memory_mb << " MB\n";
        os << "  Peak Memory: " << sys_metrics.peak_memory_mb << " MB\n";
        os << "  CPU Usage: " << sys_metrics.cpu_usage_percent << "%\n";
        os << "  Total Function Calls: " << sys_metrics.total_function_calls << "\n\n";

        // Function metrics
        os << "Function Performance:\n";
        os << std::setw(30) << "Function" 
           << std::setw(10) << "Calls" 
           << std::setw(12) << "Total(s)" 
           << std::setw(12) << "Avg(ms)" 
           << std::setw(12) << "Min(ms)" 
           << std::setw(12) << "Max(ms)" 
           << std::setw(15) << "Memory(MB)\n";
        os << std::string(90, '-') << "\n";

        // Sort by total time
        std::vector<std::pair<std::string, PerformanceMetrics>> sorted_metrics;
        for (const auto& pair : metrics_map) {
            sorted_metrics.emplace_back(pair);
        }
        
        std::sort(sorted_metrics.begin(), sorted_metrics.end(),
                  [](const auto& a, const auto& b) {
                      return a.second.total_time_seconds > b.second.total_time_seconds;
                  });

        for (const auto& pair : sorted_metrics) {
            const auto& name = pair.first;
            const auto& metrics = pair.second;
            
            os << std::setw(30) << (name.length() > 29 ? name.substr(0, 29) : name)
               << std::setw(10) << metrics.call_count
               << std::setw(12) << metrics.total_time_seconds
               << std::setw(12) << metrics.avg_time_seconds * 1000
               << std::setw(12) << metrics.min_time_seconds * 1000
               << std::setw(12) << metrics.max_time_seconds * 1000
               << std::setw(15) << metrics.total_memory_allocated / (1024 * 1024)
               << "\n";
        }
        
        os << "\n";
    }

    void saveReportToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (file.is_open()) {
            printReport(file);
            file.close();
        }
    }

private:
    void startCPUMonitoring() {
        if (cpu_monitoring_active.load()) return;
        
        cpu_monitoring_active = true;
        cpu_monitor_thread = std::thread([this]() {
            while (cpu_monitoring_active.load()) {
                current_cpu_usage = getCurrentCPUUsage();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }

    void stopCPUMonitoring() {
        cpu_monitoring_active = false;
        if (cpu_monitor_thread.joinable()) {
            cpu_monitor_thread.join();
        }
    }

    double getCurrentCPUUsage() const {
        // Simplified CPU usage calculation
        // In a real implementation, this would read from /proc/stat or use platform-specific APIs
        return 0.0; // Placeholder
    }
};

// RAII profiling scope for automatic timing
class ProfileScope {
private:
    std::string function_name;
    HighResolutionTimer timer;
    uint64_t initial_memory;

public:
    explicit ProfileScope(const std::string& name) 
        : function_name(name) {
        
        auto& profiler = PerformanceProfiler::getInstance();
        if (profiler.isProfilingEnabled()) {
            initial_memory = profiler.getSystemMetrics().current_memory_mb * 1024 * 1024;
            timer.start();
        }
    }

    ~ProfileScope() {
        auto& profiler = PerformanceProfiler::getInstance();
        if (profiler.isProfilingEnabled()) {
            timer.stop();
            uint64_t final_memory = profiler.getSystemMetrics().current_memory_mb * 1024 * 1024;
            uint64_t memory_used = final_memory > initial_memory ? 
                                 final_memory - initial_memory : 0;
            
            profiler.recordExecution(function_name, timer.getElapsedSeconds(), memory_used);
        }
    }
};

// Profiling macros for convenience
#define PROFILE_FUNCTION() \
    ProfileScope _prof_scope(__PRETTY_FUNCTION__)

#define PROFILE_SCOPE(name) \
    ProfileScope _prof_scope_##__LINE__(name)

#define PROFILE_BEGIN(name) \
    auto _timer_##name = HighResolutionTimer(); \
    _timer_##name.start()

#define PROFILE_END(name) \
    _timer_##name.stop(); \
    PerformanceProfiler::getInstance().recordExecution(#name, _timer_##name.getElapsedSeconds())

// Memory profiler for tracking allocations
class MemoryProfiler {
private:
    struct AllocationInfo {
        size_t size;
        std::string location;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    std::unordered_map<void*, AllocationInfo> allocations;
    std::mutex allocations_mutex;
    std::atomic<uint64_t> total_allocated;
    std::atomic<uint64_t> current_allocated;
    std::atomic<uint64_t> peak_allocated;

public:
    void recordAllocation(void* ptr, size_t size, const std::string& location = "") {
        std::lock_guard<std::mutex> lock(allocations_mutex);
        
        allocations[ptr] = {
            size, 
            location, 
            std::chrono::high_resolution_clock::now()
        };
        
        total_allocated += size;
        current_allocated += size;
        peak_allocated = std::max(peak_allocated.load(), current_allocated.load());
    }

    void recordDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(allocations_mutex);
        
        auto it = allocations.find(ptr);
        if (it != allocations.end()) {
            current_allocated -= it->second.size;
            allocations.erase(it);
        }
    }

    struct MemoryStatistics {
        uint64_t total_allocated;
        uint64_t current_allocated;
        uint64_t peak_allocated;
        size_t active_allocations;
    };

    MemoryStatistics getStatistics() const {
        std::lock_guard<std::mutex> lock(allocations_mutex);
        return {
            total_allocated.load(),
            current_allocated.load(),
            peak_allocated.load(),
            allocations.size()
        };
    }

    void printMemoryReport(std::ostream& os = std::cout) const {
        std::lock_guard<std::mutex> lock(allocations_mutex);
        
        auto stats = getStatistics();
        os << "\n=== Memory Usage Report ===\n";
        os << "Total Allocated: " << stats.total_allocated / (1024 * 1024) << " MB\n";
        os << "Current Allocated: " << stats.current_allocated / (1024 * 1024) << " MB\n";
        os << "Peak Allocated: " << stats.peak_allocated / (1024 * 1024) << " MB\n";
        os << "Active Allocations: " << stats.active_allocations << "\n";
        
        if (!allocations.empty()) {
            os << "\nTop 10 Largest Active Allocations:\n";
            
            std::vector<std::pair<void*, AllocationInfo>> sorted_allocs;
            for (const auto& alloc : allocations) {
                sorted_allocs.emplace_back(alloc);
            }
            
            std::sort(sorted_allocs.begin(), sorted_allocs.end(),
                      [](const auto& a, const auto& b) {
                          return a.second.size > b.second.size;
                      });
            
            for (size_t i = 0; i < std::min(size_t(10), sorted_allocs.size()); ++i) {
                const auto& alloc = sorted_allocs[i];
                os << "  " << alloc.second.size / 1024 << " KB at " 
                   << alloc.first << " (" << alloc.second.location << ")\n";
            }
        }
        os << "\n";
    }
};

} // namespace TreeWidzard

#endif // PERFORMANCE_PROFILER_H