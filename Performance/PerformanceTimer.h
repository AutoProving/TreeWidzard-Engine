#ifndef TREEWIDZARD_PERFORMANCE_TIMER_H
#define TREEWIDZARD_PERFORMANCE_TIMER_H

#include <chrono>

namespace TreeWidzard {

/**
 * Simple performance timer for measuring execution time
 */
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_running;
    
public:
    PerformanceTimer() : is_running(false) {}
    
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        is_running = true;
    }
    
    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
        is_running = false;
    }
    
    double getElapsedMilliseconds() const {
        if (is_running) {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time);
            return duration.count() / 1000.0;
        } else {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            return duration.count() / 1000.0;
        }
    }
    
    double getElapsedSeconds() const {
        return getElapsedMilliseconds() / 1000.0;
    }
    
    bool isRunning() const {
        return is_running;
    }
};

} // namespace TreeWidzard

#endif // TREEWIDZARD_PERFORMANCE_TIMER_H