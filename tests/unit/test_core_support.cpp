#include "Conjecture/PropertyAssignment.h"
#include "Kernel/Flags.h"
#include "Kernel/State.h"
#include "Kernel/Witness.h"
#include "Kernel/WitnessSet.h"
#include "Kernel/Width.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

class DummyWitness final : public Witness {
public:
  explicit DummyWitness(int value) : value_(value) {}

  std::shared_ptr<Witness>
  relabel(const std::map<unsigned, unsigned> &) const override {
    return std::make_shared<DummyWitness>(value_);
  }

  void print() const override { std::cout << "dummy:" << value_; }

  std::string witnessInformation() const override {
    return "dummy:" + std::to_string(value_);
  }

  void hash(Hasher &h) const override { h << static_cast<unsigned>(value_); }

private:
  bool is_equal(const Witness &rhs) const override {
    const auto *other = dynamic_cast<const DummyWitness *>(&rhs);
    return other != nullptr && value_ == other->value_;
  }

  bool is_less(const Witness &rhs) const override {
    const auto *other = dynamic_cast<const DummyWitness *>(&rhs);
    return other != nullptr && value_ < other->value_;
  }

  Witness &set_equal(const Witness &witness) override {
    value_ = dynamic_cast<const DummyWitness &>(witness).value_;
    return *this;
  }

  int value_;
};

auto make_witness_set(std::initializer_list<int> values)
    -> std::shared_ptr<WitnessSet> {
  auto witness_set = std::make_shared<WitnessSetTypeTwo<DummyWitness>>();
  for (const int value : values) {
    witness_set->insert(std::make_shared<DummyWitness>(value));
  }
  return witness_set;
}

auto make_bag(std::initializer_list<unsigned> values) -> Bag {
  Bag bag;
  bag.set_elements(std::set<unsigned>(values));
  return bag;
}

} // namespace

TEST(CoreSupport, WidthAssignmentComparisonAndPrint) {
  Width lhs;
  lhs.set_name("tree_width");
  lhs.set_value(3);

  Width rhs;
  rhs.set_name("path_width");
  rhs.set_value(1);
  rhs = lhs;

  EXPECT_EQ(rhs.get_name(), "tree_width");
  EXPECT_EQ(rhs.get_value(), 3U);
  EXPECT_EQ(lhs, rhs);
  EXPECT_FALSE(lhs != rhs);
  EXPECT_TRUE(lhs <= rhs);
  EXPECT_TRUE(lhs >= rhs);

  Width greater;
  greater.set_name("tree_width");
  greater.set_value(4);
  EXPECT_TRUE(lhs < greater);
  EXPECT_TRUE(greater > lhs);

  testing::internal::CaptureStdout();
  lhs.print();
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "tree_width 3\n");
}

TEST(CoreSupport, FlagsDefaultAndOverrideBehavior) {
  Flags flags;

  EXPECT_EQ(flags.get("PrintStates"), 0.0f);
  EXPECT_EQ(flags.get("Premise"), 0.0f);
  EXPECT_EQ(flags.get("StateTree"), 0.0f);
  EXPECT_EQ(flags.get("PrintDirectedBipartiteGraphNAUTY"), 0.0f);
  EXPECT_EQ(flags.get("MissingFlag"), -1.0f);

  flags.add_flag("Premise", 1.0f);
  flags.add_flag("Custom", 2.5f);
  flags.add_flag("Custom", 7.0f);

  EXPECT_EQ(flags.get("Premise"), 1.0f);
  EXPECT_EQ(flags.get("Custom"), 7.0f);
}

TEST(CoreSupport, PropertyAssignmentCoversGettersSettersAndPrinting) {
  PropertyAssignment assignment;
  assignment.setName("ChromaticNumber_AtMost");
  assignment.setOp("<=");
  assignment.setParameters({1, 2, 3});
  assignment.setParameterType("UnsignedInt");
  assignment.setType("Bool");
  assignment.setParameter(7);

  std::string label = "abc";
  std::vector<std::variant<char *, int, bool>> parameter_vec = {
      const_cast<char *>(label.c_str()), 7, true};
  assignment.setParametersVec(parameter_vec);

  EXPECT_EQ(assignment.getName(), "ChromaticNumber_AtMost");
  EXPECT_EQ(assignment.getOp(), "<=");
  EXPECT_EQ(assignment.getParameters(), std::vector<int>({1, 2, 3}));
  EXPECT_EQ(assignment.getParameterType(), "UnsignedInt");
  EXPECT_EQ(assignment.getType(), "Bool");
  EXPECT_EQ(assignment.getParameter(), 7);
  ASSERT_EQ(assignment.getParametersVec().size(), 3U);
  EXPECT_EQ(std::get<int>(assignment.getParametersVec()[1]), 7);
  EXPECT_TRUE(std::get<bool>(assignment.getParametersVec()[2]));

  testing::internal::CaptureStdout();
  assignment.printParameters();
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "\"abc\", 7, true");
}

TEST(CoreSupport, StatePoolLifecycleAndBasicAccessors) {
  State::clearPool();
  State::initializePool(2);
  EXPECT_EQ(State::getPoolStats(), 0U);

  {
    auto pooled = State::createPooled();
    EXPECT_EQ(State::getPoolStats(), 1U);
    pooled->set_bag(make_bag({1, 2}));
    pooled->setWitnessSetVector({make_witness_set({1}), make_witness_set({2})});

    EXPECT_EQ(pooled->get_bag().get_elements(), std::set<unsigned>({1, 2}));
    EXPECT_EQ(pooled->numberOfComponents(), 2U);
    EXPECT_EQ(pooled->getWitnessSet(0)->size(), 1);
    EXPECT_EQ(pooled->getWitnessSet(1)->size(), 1);
  }

  EXPECT_EQ(State::getPoolStats(), 0U);
}

TEST(CoreSupport, StateComparisonHashRelabelAndInformation) {
  State state_a;
  state_a.set_bag(make_bag({1, 2}));
  state_a.addWitnessSet(make_witness_set({1}));

  State state_b;
  state_b.set_bag(make_bag({1, 2}));
  state_b.addWitnessSet(make_witness_set({1}));

  State state_c;
  state_c.set_bag(make_bag({1, 3}));
  state_c.addWitnessSet(make_witness_set({2}));

  EXPECT_TRUE(state_a == state_b);
  EXPECT_FALSE(state_a != state_b);
  EXPECT_TRUE(state_a <= state_b);
  EXPECT_TRUE(state_a >= state_b);
  EXPECT_TRUE(state_a < state_c || state_c < state_a);
  EXPECT_NE(state_a(state_a), state_a(state_c));

  Hasher hash_a(0);
  Hasher hash_b(0);
  state_a.hash(hash_a);
  state_b.hash(hash_b);
  EXPECT_EQ(hash_a.get(), hash_b.get());

  const uint64_t cached_before = state_a.hashValue();
  state_a.set_bag(make_bag({1, 2, 3}));
  const uint64_t cached_after = state_a.hashValue();
  EXPECT_NE(cached_before, cached_after);
  EXPECT_EQ(cached_after, state_a.hashValue());

  testing::internal::CaptureStdout();
  state_a.print();
  const std::string printed = testing::internal::GetCapturedStdout();
  EXPECT_NE(printed.find("{1,2,3}"), std::string::npos);

  const std::string info = state_a.stateInformation();
  EXPECT_NE(info.find("Core 1 WitnessSet"), std::string::npos);
  EXPECT_NE(info.find("dummy:1"), std::string::npos);

  State relabel_source;
  Bag relabel_bag = make_bag({1, 3});
  relabel_bag.set_edge(1, 3);
  relabel_source.set_bag(relabel_bag);
  relabel_source.addWitnessSet(make_witness_set({5}));
  auto relabeled = relabel_source.relabel({{1, 4}, {3, 9}});
  EXPECT_EQ(relabeled->get_bag().get_elements(), std::set<unsigned>({4, 9}));
  EXPECT_EQ(relabeled->get_bag().get_edge(), std::make_pair(4U, 9U));
  EXPECT_EQ(relabeled->numberOfComponents(), 1U);
}
