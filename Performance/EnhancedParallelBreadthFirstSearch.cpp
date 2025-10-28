#include "EnhancedParallelBreadthFirstSearch.h"
#include "PerformanceTimer.h"
#include <algorithm>
#include <thread>

namespace TreeWidzard {

EnhancedParallelBreadthFirstSearch::EnhancedParallelBreadthFirstSearch() 
    : operations_count(0), total_time(0.0) {
}

EnhancedParallelBreadthFirstSearch::~EnhancedParallelBreadthFirstSearch() = default;

std::vector<State> EnhancedParallelBreadthFirstSearch::expandState(const State& state) {
    PerformanceTimer timer;
    timer.start();
    
    operations_count++;
    
    // Basic state expansion implementation
    std::vector<State> expanded_states;
    
    // For now, create a simple expansion that generates variations of the current state
    const Bag& current_bag = state.get_bag();
    auto elements = current_bag.get_elements();
    
    // Generate expanded states by adding new elements
    for (unsigned element : elements) {
        std::set<unsigned> new_elements = elements;
        new_elements.insert(element + 100); // Simple transformation
        
        State new_state = state;
        Bag new_bag;
        new_bag.set_elements(new_elements);
        new_state.set_bag(new_bag);
        expanded_states.push_back(new_state);
    }
    
    // If no elements, create a default expansion
    if (elements.empty()) {
        std::set<unsigned> new_elements = {1, 2, 3};
        State new_state = state;
        Bag new_bag;
        new_bag.set_elements(new_elements);
        new_state.set_bag(new_bag);
        expanded_states.push_back(new_state);
    }
    
    timer.stop();
    total_time += timer.getElapsedMilliseconds();
    
    return expanded_states;
}

bool EnhancedParallelBreadthFirstSearch::isGoalState(const State& state) {
    // Simple goal checking - state with more than 10 elements is considered a goal
    const Bag& bag = state.get_bag();
    return bag.get_elements().size() > 10;
}

void EnhancedParallelBreadthFirstSearch::reset() {
    operations_count = 0;
    total_time = 0.0;
}

std::vector<State> EnhancedParallelBreadthFirstSearch::expandStateBatch(const std::vector<State>& states) {
    std::vector<State> all_expanded;
    
    for (const auto& state : states) {
        auto expanded = expandState(state);
        all_expanded.insert(all_expanded.end(), expanded.begin(), expanded.end());
    }
    
    return all_expanded;
}

std::vector<State> EnhancedParallelBreadthFirstSearch::optimizedExpansion(const State& state) {
    // Use parallel processing for complex states
    if (useParallelProcessing(state)) {
        // Simplified parallel processing simulation
        return expandState(state);
    } else {
        return expandState(state);
    }
}

bool EnhancedParallelBreadthFirstSearch::useParallelProcessing(const State& state) const {
    // Use parallel processing for states with larger bags
    const Bag& bag = state.get_bag();
    return bag.get_elements().size() > 5;
}

} // namespace TreeWidzard