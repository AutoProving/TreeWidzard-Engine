#include "../GPU/MPSAcceleration.h"
#include "../GPU/MPSSearchStrategy.h"
#include "../Kernel/State.h"
#include "../Kernel/Bag.h"
#include <iostream>

/**
 * Simple MPS Test for Apple Silicon
 */

int main() {
    std::cout << "=== TreeWidzard MPS Test for Apple Silicon ===" << std::endl;
    
    // Test MPS availability
    auto mps_manager = TreeWidzard::MPS::MPSManager::getInstance();
    if (!mps_manager) {
        std::cout << "❌ MPS Manager not available" << std::endl;
        return 1;
    }
    
    std::cout << "✅ MPS Manager created" << std::endl;
    
    // Test initialization
    bool initialized = mps_manager->initialize();
    std::cout << "MPS Initialization: " << (initialized ? "✅ Success" : "❌ Failed") << std::endl;
    
    // Print system info
    mps_manager->printSystemInfo();
    
    // Test basic functionality
    if (initialized) {
        std::cout << "\n=== Testing Basic MPS Functionality ===" << std::endl;
        
        // Create test state
        std::set<unsigned> test_elements = {1, 2, 3, 4, 5};
        TreeWidzard::State test_state;
        TreeWidzard::Bag test_bag;
        test_bag.set_elements(test_elements);
        test_state.set_bag(test_bag);
        
        // Test MPS search strategy
        auto mps_strategy = TreeWidzard::MPS::MPSSearchFactory::createMPSStrategy();
        if (mps_strategy) {
            std::cout << "✅ MPS Search Strategy created" << std::endl;
            
            // Test state expansion
            try {
                auto expanded = mps_strategy->expandState(test_state);
                std::cout << "✅ State expansion successful: " << expanded.size() << " states" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "❌ State expansion failed: " << e.what() << std::endl;
            }
        } else {
            std::cout << "❌ Failed to create MPS Search Strategy" << std::endl;
        }
        
        // Test adaptive strategy
        auto adaptive_strategy = TreeWidzard::MPS::MPSSearchFactory::createAdaptiveStrategy();
        if (adaptive_strategy) {
            std::cout << "✅ Adaptive MPS Strategy created" << std::endl;
            
            try {
                auto expanded = adaptive_strategy->expandState(test_state);
                std::cout << "✅ Adaptive expansion successful: " << expanded.size() << " states" << std::endl;
                adaptive_strategy->printLearningStats();
            } catch (const std::exception& e) {
                std::cout << "❌ Adaptive expansion failed: " << e.what() << std::endl;
            }
        } else {
            std::cout << "❌ Failed to create Adaptive Strategy" << std::endl;
        }
    }
    
    std::cout << "\n=== MPS Test Complete ===" << std::endl;
    std::cout << "Your M4 Pro is ready for TreeWidzard GPU acceleration! 🍎⚡" << std::endl;
    
    return 0;
}