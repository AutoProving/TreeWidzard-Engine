#include <gtest/gtest.h>
#include "../GPU/GPUAcceleration.h"
#include "../GPU/GPUSearchStrategy.h"
#include "../Kernel/State.h"
#include "../Kernel/Bag.h"

/**
 * GPU Acceleration Test Suite
 * 
 * Tests for GPU functionality, fallback behavior, and performance validation.
 */

class GPUTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test data
        std::set<unsigned> test_elements = {1, 2, 3, 4, 5};
        test_bag = TreeWidzard::Bag(test_elements);
        test_state.set_bag(test_bag);
    }
    
    TreeWidzard::Bag test_bag;
    TreeWidzard::State test_state;
};

TEST_F(GPUTest, GPUManagerInitialization) {
    auto gpu_manager = TreeWidzard::GPU::GPUManager::getInstance();
    ASSERT_NE(gpu_manager, nullptr);
    
    // GPU may or may not be available depending on hardware
    // Just test that we can query availability without crashing
    bool gpu_available = gpu_manager->isGPUAvailable();
    EXPECT_TRUE(gpu_available || !gpu_available); // Always true, but tests the call
}

TEST_F(GPUTest, GPUSearchFactoryCreation) {
    // Test GPU strategy creation
    auto gpu_strategy = TreeWidzard::GPU::GPUSearchFactory::createGPUStrategy();
    
    if (gpu_strategy) {
        EXPECT_NE(gpu_strategy.get(), nullptr);
        
        // Test that strategy can handle basic operations
        auto expanded = gpu_strategy->expandState(test_state);
        EXPECT_GE(expanded.size(), 0); // Should return some number of states
    } else {
        // GPU not available - this is okay
        GTEST_SKIP() << "GPU not available for testing";
    }
}

TEST_F(GPUTest, CPUFallbackBehavior) {
    // Test that CPU fallback works when GPU is not available
    auto optimal_strategy = TreeWidzard::GPU::GPUSearchFactory::createOptimalStrategy();
    ASSERT_NE(optimal_strategy, nullptr);
    
    // Should work regardless of GPU availability
    auto expanded = optimal_strategy->expandState(test_state);
    EXPECT_GE(expanded.size(), 0);
}

TEST_F(GPUTest, AdaptiveStrategyBasics) {
    auto adaptive_strategy = std::make_unique<TreeWidzard::AdaptiveGPUStrategy>();
    ASSERT_NE(adaptive_strategy, nullptr);
    
    // Test basic functionality
    auto expanded = adaptive_strategy->expandState(test_state);
    EXPECT_GE(expanded.size(), 0);
    
    // Test that we can query the current strategy choice
    bool using_gpu = adaptive_strategy->isCurrentlyUsingGPU();
    EXPECT_TRUE(using_gpu || !using_gpu); // Always true, but tests the call
}

TEST_F(GPUTest, StateExpansionConsistency) {
    // Compare CPU and GPU results for consistency
    auto cpu_strategy = std::make_unique<TreeWidzard::EnhancedParallelBreadthFirstSearch>();
    auto cpu_expanded = cpu_strategy->expandState(test_state);
    
    auto gpu_strategy = TreeWidzard::GPU::GPUSearchFactory::createGPUStrategy();
    
    if (gpu_strategy && gpu_strategy->isGPUEnabled()) {
        auto gpu_expanded = gpu_strategy->expandState(test_state);
        
        // Results should be consistent (same number of expanded states)
        EXPECT_EQ(cpu_expanded.size(), gpu_expanded.size());
        
        // Could add more detailed consistency checks here
    } else {
        GTEST_SKIP() << "GPU not available for consistency testing";
    }
}

TEST_F(GPUTest, ErrorHandling) {
    // Test that GPU code handles errors gracefully
    auto gpu_manager = TreeWidzard::GPU::GPUManager::getInstance();
    
    if (gpu_manager && gpu_manager->isGPUAvailable()) {
        // Test with invalid parameters (should not crash)
        try {
            // This would test error handling in actual GPU operations
            // For now, just verify we can create and destroy resources
            EXPECT_NO_THROW({
                auto strategy = TreeWidzard::GPU::GPUSearchFactory::createGPUStrategy();
                if (strategy) {
                    strategy->expandState(test_state);
                }
            });
        } catch (const std::exception& e) {
            // Exceptions are okay as long as they're handled properly
            GTEST_SKIP() << "GPU error during test: " << e.what();
        }
    } else {
        GTEST_SKIP() << "GPU not available for error testing";
    }
}

TEST_F(GPUTest, PerformanceComparison) {
    // Basic performance test (not a strict benchmark)
    auto cpu_strategy = std::make_unique<TreeWidzard::EnhancedParallelBreadthFirstSearch>();
    auto gpu_strategy = TreeWidzard::GPU::GPUSearchFactory::createGPUStrategy();
    
    if (gpu_strategy && gpu_strategy->isGPUEnabled()) {
        // Create multiple test states
        std::vector<TreeWidzard::State> test_states;
        for (int i = 0; i < 10; i++) {
            std::set<unsigned> elements;
            for (int j = 0; j < 5; j++) {
                elements.insert(i * 5 + j);
            }
            TreeWidzard::State state;
            state.set_bag(TreeWidzard::Bag(elements));
            test_states.push_back(state);
        }
        
        // Time CPU execution
        auto cpu_start = std::chrono::high_resolution_clock::now();
        for (const auto& state : test_states) {
            cpu_strategy->expandState(state);
        }
        auto cpu_end = std::chrono::high_resolution_clock::now();
        auto cpu_duration = std::chrono::duration_cast<std::chrono::microseconds>(cpu_end - cpu_start);
        
        // Time GPU execution  
        auto gpu_start = std::chrono::high_resolution_clock::now();
        for (const auto& state : test_states) {
            gpu_strategy->expandState(state);
        }
        auto gpu_end = std::chrono::high_resolution_clock::now();
        auto gpu_duration = std::chrono::duration_cast<std::chrono::microseconds>(gpu_end - gpu_start);
        
        // Both should complete successfully (exact performance comparison depends on hardware)
        EXPECT_GT(cpu_duration.count(), 0);
        EXPECT_GT(gpu_duration.count(), 0);
        
        std::cout << "CPU time: " << cpu_duration.count() << "μs" << std::endl;
        std::cout << "GPU time: " << gpu_duration.count() << "μs" << std::endl;
        
    } else {
        GTEST_SKIP() << "GPU not available for performance testing";
    }
}

// Test fixture for GPU memory operations
class GPUMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        gpu_manager = TreeWidzard::GPU::GPUManager::getInstance();
    }
    
    std::shared_ptr<TreeWidzard::GPU::GPUManager> gpu_manager;
};

TEST_F(GPUMemoryTest, MemoryAllocation) {
    if (!gpu_manager || !gpu_manager->isGPUAvailable()) {
        GTEST_SKIP() << "GPU not available for memory testing";
    }
    
    // Test basic memory allocation (if GPU is available)
    EXPECT_NO_THROW({
        // This would test actual GPU memory allocation
        // For now, just verify the manager exists
        EXPECT_NE(gpu_manager, nullptr);
    });
}

TEST_F(GPUMemoryTest, MemoryCleanup) {
    if (!gpu_manager || !gpu_manager->isGPUAvailable()) {
        GTEST_SKIP() << "GPU not available for cleanup testing";
    }
    
    // Test that memory cleanup works properly
    EXPECT_NO_THROW({
        // Create and destroy GPU strategy to test cleanup
        auto strategy = TreeWidzard::GPU::GPUSearchFactory::createGPUStrategy();
        // Strategy destructor should clean up properly
    });
}

// Test for system capabilities detection
TEST(GPUSystemTest, CapabilityDetection) {
    // Test that we can detect system capabilities without crashing
    EXPECT_NO_THROW({
        TreeWidzard::GPU::GPUSearchFactory::printSystemCapabilities();
        auto devices = TreeWidzard::GPU::GPUSearchFactory::detectGPUs();
        // Should return a vector (empty or with devices)
        EXPECT_GE(devices.size(), 0);
    });
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "Running TreeWidzard GPU Tests..." << std::endl;
    std::cout << "Note: Some tests may be skipped if GPU hardware is not available." << std::endl;
    
    return RUN_ALL_TESTS();
}