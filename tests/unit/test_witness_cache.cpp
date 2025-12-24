#include "../utils/test_helpers.h"
#include "Kernel/Bag.h"
#include "Performance/WitnessCache.h"
#include <gtest/gtest.h>

using namespace TreeWidzard;
using namespace TreeWidzard::Testing;

// Simple test without WitnessSet instantiation
class WitnessCacheTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Tests will be simpler without actual WitnessSet objects
  }
};

// Test: Cache statistics initialization
TEST_F(WitnessCacheTest, CacheInitialization) {
  WitnessSetCache cache(10);
  auto stats = cache.getStatistics();

  EXPECT_EQ(stats.max_size, 10);
  EXPECT_EQ(stats.current_size, 0);
  EXPECT_EQ(stats.hits, 0);
  EXPECT_EQ(stats.misses, 0);
  EXPECT_EQ(stats.hit_ratio, 0.0);
}

// Test: Cache capacity
TEST_F(WitnessCacheTest, CacheCapacity) {
  WitnessSetCache small_cache(5);
  auto stats1 = small_cache.getStatistics();
  EXPECT_EQ(stats1.max_size, 5);

  WitnessSetCache large_cache(100);
  auto stats2 = large_cache.getStatistics();
  EXPECT_EQ(stats2.max_size, 100);
}

// Test: Clear cache
TEST_F(WitnessCacheTest, ClearCache) {
  WitnessSetCache cache(10);
  cache.clear();

  auto stats = cache.getStatistics();
  EXPECT_EQ(stats.current_size, 0);
}

// Test: Multiple cache instances
TEST_F(WitnessCacheTest, MultipleCaches) {
  WitnessSetCache cache1(10);
  WitnessSetCache cache2(20);

  auto stats1 = cache1.getStatistics();
  auto stats2 = cache2.getStatistics();

  EXPECT_EQ(stats1.max_size, 10);
  EXPECT_EQ(stats2.max_size, 20);
  EXPECT_NE(stats1.max_size, stats2.max_size);
}

