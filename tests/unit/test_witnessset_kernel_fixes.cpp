#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

#include "Kernel/WitnessSet.h"
#include "Kernel/DynamicKernel.h"
#include "Kernel/Width.h"
#include "../utils/test_helpers.h"
#include "../utils/kernel_test_doubles.h"

using namespace TreeWidzardTest;

// =============================================================================
// WITNESSSET TESTS - Thread Safety and Basic Operations
// =============================================================================

class WitnessSetThreadSafetyTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Clear static data before each test
    WitnessSetTypeOne<IntWitness>::allWitnesses.clear();
    WitnessSetTypeOne<IntWitness>::witnessVector.clear();
  }

  void TearDown() override {
    WitnessSetTypeOne<IntWitness>::allWitnesses.clear();
    WitnessSetTypeOne<IntWitness>::witnessVector.clear();
  }
};

// Test 1: WitnessSetTypeOne concurrent inserts from multiple threads
TEST_F(WitnessSetThreadSafetyTest, ConcurrentInsertsDoNotCorrupt) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  const int num_threads = 4;
  constexpr int inserts_per_thread = 25;
  std::vector<std::thread> threads;

  for (int t = 0; t < num_threads; ++t) {
    threads.emplace_back([witness_set, t]() {
      for (int i = 0; i < inserts_per_thread; ++i) {
        int value = t * inserts_per_thread + i;
        witness_set->insert(make_int_witness(value));
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  // Verify all inserts were recorded correctly
  EXPECT_EQ(witness_set->size(), num_threads * inserts_per_thread);

  // Verify we can iterate without crashes
  int count = 0;
  for (const auto &witness : *witness_set) {
    count++;
  }
  EXPECT_EQ(count, witness_set->size());
}

// Test 2: WitnessSetTypeOne concurrent inserts with duplicate values
TEST_F(WitnessSetThreadSafetyTest, ConcurrentDuplicateInsertsHandledCorrectly) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  const int num_threads = 5;
  constexpr int same_value = 42;

  std::vector<std::thread> threads;
  for (int t = 0; t < num_threads; ++t) {
    threads.emplace_back([witness_set]() {
      // All threads insert the same value multiple times
      for (int i = 0; i < 10; ++i) {
        witness_set->insert(make_int_witness(same_value));
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  // Only one unique witness should be in the set
  EXPECT_EQ(witness_set->size(), 1);
}

// Test 3: WitnessSetTypeOne begin/end on empty set
TEST_F(WitnessSetThreadSafetyTest, EmptySetIteratorsBehavCorrectly) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();

  auto begin_it = witness_set->begin();
  auto end_it = witness_set->end();

  EXPECT_EQ(begin_it, end_it);

  // Test iteration on empty set
  int count = 0;
  for (const auto &witness : *witness_set) {
    (void)witness;
    count++;
  }
  EXPECT_EQ(count, 0);
}

// Test 4: WitnessSetTypeTwo sorted insert and dedup
TEST_F(WitnessSetThreadSafetyTest, TypeTwoMaintainsSortedOrder) {
  auto witness_set = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  // Insert in non-sorted order
  witness_set->insert(make_int_witness(30));
  witness_set->insert(make_int_witness(10));
  witness_set->insert(make_int_witness(20));
  witness_set->insert(make_int_witness(10));  // Duplicate

  // Verify size is 3 (deduped)
  EXPECT_EQ(witness_set->size(), 3);

  // Verify sorted order
  std::vector<int> values;
  for (const auto &witness : *witness_set) {
    values.push_back(IntWitness::as_witness(*witness).value);
  }

  EXPECT_EQ(values.size(), 3);
  EXPECT_EQ(values[0], 10);
  EXPECT_EQ(values[1], 20);
  EXPECT_EQ(values[2], 30);
}

// Test 5: WitnessSetTypeTwo isEqual comparison
TEST_F(WitnessSetThreadSafetyTest, TypeTwoEqualityComparison) {
  auto set1 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  // Empty sets should be equal
  EXPECT_TRUE(set1->isEqual(*set2));

  // Add same elements
  set1->insert(make_int_witness(1));
  set1->insert(make_int_witness(2));
  set2->insert(make_int_witness(1));
  set2->insert(make_int_witness(2));

  EXPECT_TRUE(set1->isEqual(*set2));

  // Add different element
  set2->insert(make_int_witness(3));
  EXPECT_FALSE(set1->isEqual(*set2));
}

// Test 6: WitnessSetTypeTwo isLess comparison
TEST_F(WitnessSetThreadSafetyTest, TypeTwoLessComparison) {
  auto set1 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  set1->insert(make_int_witness(1));
  set1->insert(make_int_witness(2));

  set2->insert(make_int_witness(1));
  set2->insert(make_int_witness(2));
  set2->insert(make_int_witness(3));

  // set1 has fewer elements
  EXPECT_TRUE(set1->isLess(*set2));
  EXPECT_FALSE(set2->isLess(*set1));
}

// Test 7: WitnessSetTypeOne union_set_witness
TEST_F(WitnessSetThreadSafetyTest, TypeOneUnionOperation) {
  auto set1 = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeOne<IntWitness>>();

  set1->insert(make_int_witness(1));
  set1->insert(make_int_witness(2));

  set2->insert(make_int_witness(2));
  set2->insert(make_int_witness(3));

  set1->union_set_witness(set2);

  EXPECT_EQ(set1->size(), 3);

  // Verify all elements are present
  std::vector<int> values;
  for (const auto &witness : *set1) {
    values.push_back(IntWitness::as_witness(*witness).value);
  }
  std::sort(values.begin(), values.end());

  EXPECT_EQ(values[0], 1);
  EXPECT_EQ(values[1], 2);
  EXPECT_EQ(values[2], 3);
}

// Test 8: WitnessSetTypeTwo union_set_witness
TEST_F(WitnessSetThreadSafetyTest, TypeTwoUnionOperation) {
  auto set1 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  set1->insert(make_int_witness(1));
  set1->insert(make_int_witness(3));

  set2->insert(make_int_witness(2));
  set2->insert(make_int_witness(4));

  set1->union_set_witness(set2);

  EXPECT_EQ(set1->size(), 4);

  // Verify sorted order is maintained
  std::vector<int> values;
  for (const auto &witness : *set1) {
    values.push_back(IntWitness::as_witness(*witness).value);
  }

  EXPECT_EQ(values[0], 1);
  EXPECT_EQ(values[1], 2);
  EXPECT_EQ(values[2], 3);
  EXPECT_EQ(values[3], 4);
}

// Test 9: Hash consistency between equal WitnessSetTypeOne sets
TEST_F(WitnessSetThreadSafetyTest, TypeOneHashConsistency) {
  auto set1 = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeOne<IntWitness>>();

  set1->insert(make_int_witness(1));
  set1->insert(make_int_witness(2));
  set1->insert(make_int_witness(3));

  set2->insert(make_int_witness(1));
  set2->insert(make_int_witness(2));
  set2->insert(make_int_witness(3));

  Hasher h1(0), h2(0);
  set1->hash(h1);
  set2->hash(h2);

  EXPECT_EQ(h1.get(), h2.get());
}

// Test 10: Hash consistency between equal WitnessSetTypeTwo sets
TEST_F(WitnessSetThreadSafetyTest, TypeTwoHashConsistency) {
  auto set1 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  set1->insert(make_int_witness(1));
  set1->insert(make_int_witness(2));

  set2->insert(make_int_witness(1));
  set2->insert(make_int_witness(2));

  Hasher h1(0), h2(0);
  set1->hash(h1);
  set2->hash(h2);

  EXPECT_EQ(h1.get(), h2.get());
}

// Test 11: WitnessSetTypeOne setEqual cross-assignment
TEST_F(WitnessSetThreadSafetyTest, TypeOneSetEqual) {
  auto set1 = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeOne<IntWitness>>();

  set2->insert(make_int_witness(10));
  set2->insert(make_int_witness(20));
  set2->insert(make_int_witness(30));

  set1->setEqual(*set2);

  EXPECT_EQ(set1->size(), 3);
  EXPECT_TRUE(set1->isEqual(*set2));
}

// Test 12: WitnessSetTypeTwo setEqual cross-assignment
TEST_F(WitnessSetThreadSafetyTest, TypeTwoSetEqual) {
  auto set1 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  set2->insert(make_int_witness(5));
  set2->insert(make_int_witness(15));

  set1->setEqual(*set2);

  EXPECT_EQ(set1->size(), 2);
  EXPECT_TRUE(set1->isEqual(*set2));
}

// Test 13: WitnessSetTypeOne iteration consistency
TEST_F(WitnessSetThreadSafetyTest, TypeOneIterationConsistency) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();

  for (int i = 0; i < 10; ++i) {
    witness_set->insert(make_int_witness(i));
  }

  // Multiple iterations should yield consistent results
  std::vector<int> first_pass, second_pass;

  for (const auto &witness : *witness_set) {
    first_pass.push_back(IntWitness::as_witness(*witness).value);
  }

  for (const auto &witness : *witness_set) {
    second_pass.push_back(IntWitness::as_witness(*witness).value);
  }

  EXPECT_EQ(first_pass, second_pass);
}

// =============================================================================
// DYNAMICKERNEL TESTS
// =============================================================================

class DynamicKernelTest : public ::testing::Test {
 protected:
  void SetUp() override {
    WitnessSetTypeOne<IntWitness>::allWitnesses.clear();
    WitnessSetTypeOne<IntWitness>::witnessVector.clear();
  }

  void TearDown() override {
    WitnessSetTypeOne<IntWitness>::allWitnesses.clear();
    WitnessSetTypeOne<IntWitness>::witnessVector.clear();
  }
};

// Test 14: DynamicKernel addCore stores correct pointer
TEST_F(DynamicKernelTest, AddCoreStoresCorrectPointer) {
  DynamicKernel kernel;
  PassthroughCore core;

  EXPECT_EQ(kernel.coreSize(), 0);

  kernel.addCore(core);

  EXPECT_EQ(kernel.coreSize(), 1);
  EXPECT_EQ(kernel.pointerToCoreNumber(0), &core);
}

// Test 15: DynamicKernel with multiple cores
TEST_F(DynamicKernelTest, MultipleCoresStored) {
  DynamicKernel kernel;
  PassthroughCore core1, core2, core3;

  kernel.addCore(core1);
  kernel.addCore(core2);
  kernel.addCore(core3);

  EXPECT_EQ(kernel.coreSize(), 3);
  EXPECT_EQ(kernel.pointerToCoreNumber(0), &core1);
  EXPECT_EQ(kernel.pointerToCoreNumber(1), &core2);
  EXPECT_EQ(kernel.pointerToCoreNumber(2), &core3);
}

// Test 16: DynamicKernel initialState produces valid state
TEST_F(DynamicKernelTest, InitialStateCreationBasic) {
  DynamicKernel kernel;
  PassthroughCore core;
  kernel.addCore(core);

  auto state = kernel.initialState();

  // State::ptr always holds a valid state
  // Should have one witness set for the core
  EXPECT_EQ(state->numberOfComponents(), 1);

  auto witness_set = state->getWitnessSet(0);
  EXPECT_TRUE(witness_set != nullptr);
  EXPECT_EQ(witness_set->size(), 1);
}

// Test 17: DynamicKernel initialState with multiple cores
TEST_F(DynamicKernelTest, InitialStateMultipleCores) {
  DynamicKernel kernel;
  PassthroughCore core1, core2;
  kernel.addCore(core1);
  kernel.addCore(core2);

  auto state = kernel.initialState();

  // State::ptr always holds a valid state
  EXPECT_EQ(state->numberOfComponents(), 2);

  auto set1 = state->getWitnessSet(0);
  auto set2 = state->getWitnessSet(1);

  EXPECT_TRUE(set1 != nullptr);
  EXPECT_TRUE(set2 != nullptr);
  EXPECT_EQ(set1->size(), 1);
  EXPECT_EQ(set2->size(), 1);
}

// Test 18: DynamicKernel intro_v produces correct state
TEST_F(DynamicKernelTest, IntroVertexOperation) {
  DynamicKernel kernel;
  PassthroughCore core;
  kernel.addCore(core);

  auto initial_state = kernel.initialState();

  // Introduce vertex 0
  auto next_state = kernel.intro_v(initial_state, 0);

  // State::ptr always holds a valid state
  EXPECT_EQ(next_state->numberOfComponents(), 1);

  auto witness_set = next_state->getWitnessSet(0);
  EXPECT_EQ(witness_set->size(), 1);

  // Value should be incremented from intro_v operation (0 + 1 = 1)
  int value = first_witness_value(witness_set);
  EXPECT_EQ(value, 1);
}

// Test 19: DynamicKernel intro_e produces correct state
TEST_F(DynamicKernelTest, IntroEdgeOperation) {
  DynamicKernel kernel;
  PassthroughCore core;
  kernel.addCore(core);

  auto initial_state = kernel.initialState();

  // First introduce vertices
  auto state_after_v0 = kernel.intro_v(initial_state, 0);
  auto state_after_v1 = kernel.intro_v(state_after_v0, 1);

  // Now introduce edge
  auto state_after_e = kernel.intro_e(state_after_v1, 0, 1);

  // State::ptr always valid

  auto witness_set = state_after_e->getWitnessSet(0);
  EXPECT_EQ(witness_set->size(), 1);

  // Two intro_v steps produce 2, then intro_e adds 10.
  int value = first_witness_value(witness_set);
  EXPECT_EQ(value, 12);
}

// Test 20: DynamicKernel forget_v produces correct state
TEST_F(DynamicKernelTest, ForgetVertexOperation) {
  DynamicKernel kernel;
  PassthroughCore core;
  kernel.addCore(core);

  auto initial_state = kernel.initialState();

  // Introduce and then forget
  auto state_after_intro = kernel.intro_v(initial_state, 0);
  auto state_after_forget = kernel.forget_v(state_after_intro, 0);

  // State::ptr always valid

  auto witness_set = state_after_forget->getWitnessSet(0);
  // Value should remain from forget_v operation
  int value = first_witness_value(witness_set);
  EXPECT_EQ(value, 1);
}

// Test 21: DynamicKernel join produces correct state
TEST_F(DynamicKernelTest, JoinOperation) {
  DynamicKernel kernel;
  PassthroughCore core;
  kernel.addCore(core);

  auto initial_state = kernel.initialState();

  // Create two different states to join
  auto state1 = kernel.intro_v(initial_state, 0);
  auto state1_with_same_bag = kernel.intro_v(state1, 1);
  auto state2 = kernel.intro_v(initial_state, 1);
  auto state2_with_same_bag = kernel.intro_v(state2, 0);

  auto joined_state = kernel.join(state1_with_same_bag, state2_with_same_bag);

  // State::ptr always valid
  EXPECT_EQ(joined_state->numberOfComponents(), 1);

  auto witness_set = joined_state->getWitnessSet(0);
  // Each branch contributes 2 after introducing both vertices.
  int value = first_witness_value(witness_set);
  EXPECT_EQ(value, 4);
}

// Test 22: DynamicKernel getCoreByVar returns correct core
TEST_F(DynamicKernelTest, GetCoreByVar) {
  DynamicKernel kernel;
  PassthroughCore core1, core2;
  kernel.addCore(core1);
  kernel.addCore(core2);

  std::map<std::string, std::pair<std::string, int>> var_map;
  var_map["x"] = std::make_pair("CoreName1", 0);
  var_map["y"] = std::make_pair("CoreName2", 1);
  kernel.setVarToNameAndIndex(var_map);

  EXPECT_EQ(kernel.getCoreByVar("x"), &core1);
  EXPECT_EQ(kernel.getCoreByVar("y"), &core2);
}

// Test 23: DynamicKernel getIndexByVar returns correct index
TEST_F(DynamicKernelTest, GetIndexByVar) {
  DynamicKernel kernel;
  PassthroughCore core1, core2;
  kernel.addCore(core1);
  kernel.addCore(core2);

  std::map<std::string, std::pair<std::string, int>> var_map;
  var_map["a"] = std::make_pair("CoreName1", 0);
  var_map["b"] = std::make_pair("CoreName2", 1);
  kernel.setVarToNameAndIndex(var_map);

  EXPECT_EQ(kernel.getIndexByVar("a"), 0);
  EXPECT_EQ(kernel.getIndexByVar("b"), 1);
}

// Test 24: DynamicKernel pointerToCoreNumber bounds checking
TEST_F(DynamicKernelTest, PointerToCoreNumberBoundsCheck) {
  DynamicKernel kernel;
  PassthroughCore core;
  kernel.addCore(core);

  // Valid index
  EXPECT_EQ(kernel.pointerToCoreNumber(0), &core);

  // Invalid index should trigger error condition
  // (Note: In real code this exits, so we can't test it without mocking)
}

// Test 25: DynamicKernel operations with multiple cores
TEST_F(DynamicKernelTest, MultiCoreOperationConsistency) {
  DynamicKernel kernel;
  PassthroughCore core1, core2;
  kernel.addCore(core1);
  kernel.addCore(core2);

  auto state = kernel.initialState();
  EXPECT_EQ(state->numberOfComponents(), 2);

  auto state1 = kernel.intro_v(state, 0);
  EXPECT_EQ(state1->numberOfComponents(), 2);

  auto set0 = state1->getWitnessSet(0);
  auto set1 = state1->getWitnessSet(1);

  EXPECT_EQ(set0->size(), 1);
  EXPECT_EQ(set1->size(), 1);

  // Both cores should have processed the intro_v
  EXPECT_EQ(first_witness_value(set0), 1);
  EXPECT_EQ(first_witness_value(set1), 1);
}

// Test 26: WitnessSetTypeOne::size on populated set
TEST_F(WitnessSetThreadSafetyTest, TypeOneSizeAccuracy) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();

  EXPECT_EQ(witness_set->size(), 0);

  witness_set->insert(make_int_witness(1));
  EXPECT_EQ(witness_set->size(), 1);

  witness_set->insert(make_int_witness(2));
  EXPECT_EQ(witness_set->size(), 2);

  // Duplicate insert should not increase size
  witness_set->insert(make_int_witness(1));
  EXPECT_EQ(witness_set->size(), 2);
}

// Test 27: WitnessSetTypeTwo::size on populated set
TEST_F(WitnessSetThreadSafetyTest, TypeTwoSizeAccuracy) {
  auto witness_set = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  EXPECT_EQ(witness_set->size(), 0);

  witness_set->insert(make_int_witness(5));
  EXPECT_EQ(witness_set->size(), 1);

  witness_set->insert(make_int_witness(10));
  EXPECT_EQ(witness_set->size(), 2);
}

// Test 28: WitnessSetTypeOne::createEmptyWitnessSet
TEST_F(WitnessSetThreadSafetyTest, TypeOneCreateEmpty) {
  auto original = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  original->insert(make_int_witness(42));

  auto empty = original->createEmptyWitnessSet();

  EXPECT_TRUE(empty != nullptr);
  EXPECT_EQ(empty->size(), 0);
}

// Test 29: WitnessSetTypeTwo::createEmptyWitnessSet
TEST_F(WitnessSetThreadSafetyTest, TypeTwoCreateEmpty) {
  auto original = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  original->insert(make_int_witness(42));

  auto empty = original->createEmptyWitnessSet();

  EXPECT_TRUE(empty != nullptr);
  EXPECT_EQ(empty->size(), 0);
}

// Test 30: WitnessSetTypeOne union with self
TEST_F(WitnessSetThreadSafetyTest, TypeOneUnionWithSelf) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  witness_set->insert(make_int_witness(1));
  witness_set->insert(make_int_witness(2));

  int size_before = witness_set->size();
  witness_set->union_set_witness(witness_set);

  // Size should not change when union with self
  EXPECT_EQ(witness_set->size(), size_before);
}

// Test 31: DynamicKernel width manipulation
TEST_F(DynamicKernelTest, KernelWidthManipulation) {
  DynamicKernel kernel;
  Width width;

  kernel.set_width(width);
  Width &retrieved_width = kernel.get_width();

  // Should get back the same width object
  EXPECT_EQ(&retrieved_width, &kernel.get_width());
}

// Test 32: DynamicKernel var mapping
TEST_F(DynamicKernelTest, VarToNameAndIndexMapping) {
  DynamicKernel kernel;

  std::map<std::string, std::pair<std::string, int>> var_map;
  var_map["var1"] = std::make_pair("Name1", 10);
  var_map["var2"] = std::make_pair("Name2", 20);

  kernel.setVarToNameAndIndex(var_map);

  const auto &retrieved = kernel.getVarToNameAndIndex();
  EXPECT_EQ(retrieved.size(), 2);
  EXPECT_EQ(retrieved.at("var1").second, 10);
  EXPECT_EQ(retrieved.at("var2").second, 20);
}

// Test 33: WitnessSetTypeOne inequality operator
TEST_F(WitnessSetThreadSafetyTest, TypeOneInequalityOperator) {
  auto set1 = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeOne<IntWitness>>();

  set1->insert(make_int_witness(1));

  // Different sets should be unequal
  EXPECT_TRUE(*set1 != *set2);

  set2->insert(make_int_witness(1));
  EXPECT_FALSE(*set1 != *set2);
}

// Test 34: WitnessSetTypeTwo inequality operator
TEST_F(WitnessSetThreadSafetyTest, TypeTwoInequalityOperator) {
  auto set1 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  auto set2 = std::make_shared<WitnessSetTypeTwo<IntWitness>>();

  set1->insert(make_int_witness(5));

  EXPECT_TRUE(*set1 != *set2);

  set2->insert(make_int_witness(5));
  EXPECT_FALSE(*set1 != *set2);
}

// Test 35: WitnessSetTypeOne relabel operation
TEST_F(WitnessSetThreadSafetyTest, TypeOneRelabel) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  witness_set->insert(make_int_witness(42));

  std::map<unsigned, unsigned> relabel_map;
  relabel_map[0] = 1;

  auto relabeled = witness_set->relabel(relabel_map);

  EXPECT_TRUE(relabeled != nullptr);
  EXPECT_EQ(relabeled->size(), 1);
}

// Test 36: WitnessSetTypeOne print operation (smoke test)
TEST_F(WitnessSetThreadSafetyTest, TypeOnePrintOperation) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  witness_set->insert(make_int_witness(1));

  // Just verify it doesn't crash
  EXPECT_NO_THROW(witness_set->print());
}

// Test 37: WitnessSetTypeTwo print operation (smoke test)
TEST_F(WitnessSetThreadSafetyTest, TypeTwoPrintOperation) {
  auto witness_set = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  witness_set->insert(make_int_witness(1));

  EXPECT_NO_THROW(witness_set->print());
}

// Test 38: WitnessSetTypeOne witnessSetInformation
TEST_F(WitnessSetThreadSafetyTest, TypeOneWitnessSetInformation) {
  auto witness_set = std::make_shared<WitnessSetTypeOne<IntWitness>>();
  witness_set->insert(make_int_witness(1));

  std::string info = witness_set->witnessSetInformation();
  EXPECT_FALSE(info.empty());
}

// Test 39: WitnessSetTypeTwo witnessSetInformation
TEST_F(WitnessSetThreadSafetyTest, TypeTwoWitnessSetInformation) {
  auto witness_set = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  witness_set->insert(make_int_witness(1));

  std::string info = witness_set->witnessSetInformation();
  EXPECT_FALSE(info.empty());
}

// Test 40: Complex multi-threaded scenario with kernel operations
TEST_F(DynamicKernelTest, ComplexMultiCoreIntegration) {
  DynamicKernel kernel;
  PassthroughCore core1, core2, core3;

  kernel.addCore(core1);
  kernel.addCore(core2);
  kernel.addCore(core3);

  auto initial = kernel.initialState();
  EXPECT_EQ(initial->numberOfComponents(), 3);

  // Perform series of operations
  auto state1 = kernel.intro_v(initial, 0);
  auto state2 = kernel.intro_v(state1, 1);
  auto state3 = kernel.forget_v(state2, 0);

  EXPECT_EQ(state3->numberOfComponents(), 3);

  for (size_t i = 0; i < 3; ++i) {
    auto ws = state3->getWitnessSet(i);
    EXPECT_GT(ws->size(), 0);
  }
}
