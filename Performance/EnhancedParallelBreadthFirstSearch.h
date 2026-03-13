#ifndef TREEWIDZARD_ENHANCED_PARALLEL_BREADTH_FIRST_SEARCH_H
#define TREEWIDZARD_ENHANCED_PARALLEL_BREADTH_FIRST_SEARCH_H

#include "../Kernel/State.h"
#include <vector>
#include <memory>

namespace TreeWidzard {

/**
 * Simple search interface for MPS testing
 */
class SimpleSearchStrategy {
public:
    virtual ~SimpleSearchStrategy() = default;
    virtual std::vector<State> expandState(const State& state) = 0;
    virtual bool isGoalState(const State& state) = 0;
    virtual void reset() = 0;
};

/**
 * Enhanced parallel breadth-first search with optimizations
 * 
 * This is the optimized CPU search strategy that provides the baseline
 * performance for the current TreeWidzard solver.
 */
class EnhancedParallelBreadthFirstSearch : public SimpleSearchStrategy {
private:
    // Performance tracking
    size_t operations_count;
    double total_time;
    
public:
    EnhancedParallelBreadthFirstSearch();
    virtual ~EnhancedParallelBreadthFirstSearch();
    
    // SimpleSearchStrategy interface
    std::vector<State> expandState(const State& state) override;
    bool isGoalState(const State& state) override;
    void reset() override;
    
    // Performance monitoring
    size_t getOperationsCount() const { return operations_count; }
    double getTotalTime() const { return total_time; }
    
    // Batch processing for efficiency
    std::vector<State> expandStateBatch(const std::vector<State>& states);
    
protected:
    // Internal optimization methods
    std::vector<State> optimizedExpansion(const State& state);
    bool useParallelProcessing(const State& state) const;
};

} // namespace TreeWidzard

#endif // TREEWIDZARD_ENHANCED_PARALLEL_BREADTH_FIRST_SEARCH_H
