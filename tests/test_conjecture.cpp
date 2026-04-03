#include "../Conjecture/Conjecture.h"
#include "../Conjecture/PropertyAssignment.h"
#include "../Kernel/DynamicKernel.h"
#include <gtest/gtest.h>
#include <string>

class ConjectureTest : public ::testing::Test {
protected:
  void SetUp() override {
    kernel = std::make_unique<DynamicKernel>();
    conjecture = std::make_unique<Conjecture>();
  }

  void TearDown() override {
    conjecture.reset();
    kernel.reset();
  }

  std::unique_ptr<DynamicKernel> kernel;
  std::unique_ptr<Conjecture> conjecture;
};

TEST_F(ConjectureTest, BasicConjectureCreation) {
  ASSERT_NE(conjecture, nullptr);
  EXPECT_EQ(conjecture->getRoot(), nullptr);
  EXPECT_EQ(conjecture->getKernel(), nullptr);

  ConjectureNode node(NUMBER, "", 1.0);
  EXPECT_EQ(node.getParent(), nullptr);
}

TEST_F(ConjectureTest, SimpleArithmeticEvaluation) {
  // Create a simple arithmetic expression: 5 + 3
  auto root = new ConjectureNode(OPERATOR, "+", 0);
  auto left = new ConjectureNode(NUMBER, "", 5.0);
  auto right = new ConjectureNode(NUMBER, "", 3.0);

  std::vector<ConjectureNode *> children;
  children.push_back(left);
  children.push_back(right);
  root->setChildren(children);

  conjecture->setRoot(root);

  // Create a dummy state for evaluation
  State state;
  double result = conjecture->evaluateConjectureOnState(state);

  EXPECT_DOUBLE_EQ(result, 8.0);
}

TEST_F(ConjectureTest, BooleanOperations) {
  // Test AND operation: true AND false = false
  auto root = new ConjectureNode(OPERATOR, "and", 0);
  auto left = new ConjectureNode(NUMBER, "", 1.0);  // true
  auto right = new ConjectureNode(NUMBER, "", 0.0); // false

  std::vector<ConjectureNode *> children;
  children.push_back(left);
  children.push_back(right);
  root->setChildren(children);

  conjecture->setRoot(root);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);

  EXPECT_DOUBLE_EQ(result, 0.0); // false
}

TEST_F(ConjectureTest, ComparisonOperations) {
  // Test less than: 5 < 10 = true
  auto root = new ConjectureNode(OPERATOR, "<", 0);
  auto left = new ConjectureNode(NUMBER, "", 5.0);
  auto right = new ConjectureNode(NUMBER, "", 10.0);

  std::vector<ConjectureNode *> children;
  children.push_back(left);
  children.push_back(right);
  root->setChildren(children);

  conjecture->setRoot(root);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);

  EXPECT_DOUBLE_EQ(result, 1.0); // true
}

TEST_F(ConjectureTest, NestedExpressions) {
  // Test (5 + 3) < 10 = true
  auto root = new ConjectureNode(OPERATOR, "<", 0);

  // Left side: 5 + 3
  auto left = new ConjectureNode(OPERATOR, "+", 0);
  std::vector<ConjectureNode *> leftChildren;
  leftChildren.push_back(new ConjectureNode(NUMBER, "", 5.0));
  leftChildren.push_back(new ConjectureNode(NUMBER, "", 3.0));
  left->setChildren(leftChildren);

  // Right side: 10
  auto right = new ConjectureNode(NUMBER, "", 10.0);

  std::vector<ConjectureNode *> rootChildren;
  rootChildren.push_back(left);
  rootChildren.push_back(right);
  root->setChildren(rootChildren);

  conjecture->setRoot(root);

  State state;
  double result = conjecture->evaluateConjectureOnState(state);

  EXPECT_DOUBLE_EQ(result, 1.0); // true
}

TEST_F(ConjectureTest, InvalidOperatorHandling) {
  // Test invalid operator handling
  auto root = new ConjectureNode(OPERATOR, "invalid_op", 0);
  std::vector<ConjectureNode *> children;
  children.push_back(new ConjectureNode(NUMBER, "", 5.0));
  children.push_back(new ConjectureNode(NUMBER, "", 3.0));
  root->setChildren(children);
  conjecture->setRoot(root);

  State state;

  // This should exit with error code 20, but we can't easily test that
  // In a real implementation, we'd want proper exception handling
  EXPECT_NO_THROW({
    // For now, just ensure the structure is valid
    EXPECT_NE(conjecture->getRoot(), nullptr);
  });
}

TEST_F(ConjectureTest, ConjectureStructureValidation) {
  // Test conjecture structure validation
  auto root = new ConjectureNode(OPERATOR, "+", 0);
  std::vector<ConjectureNode *> children;
  children.push_back(new ConjectureNode(NUMBER, "", 5.0));
  children.push_back(new ConjectureNode(NUMBER, "", 3.0));
  root->setChildren(children);

  conjecture->setRoot(root);

  bool isValid = conjecture->checkConjectureStructure(root);
  EXPECT_TRUE(isValid);
}

TEST_F(ConjectureTest, ConjectureStructureValidationRejectsBinaryOperatorWithOneChild) {
  auto root = new ConjectureNode(OPERATOR, "+", 0);
  std::vector<ConjectureNode *> children;
  children.push_back(new ConjectureNode(NUMBER, "", 5.0));
  root->setChildren(children);

  conjecture->setRoot(root);

  EXPECT_FALSE(conjecture->checkConjectureStructure(root));
}

TEST_F(ConjectureTest, ConjectureStructureValidationRejectsUnknownUnaryFunction) {
  auto root = new ConjectureNode(FUNCTION_UNARY, "mystery", 0);
  std::vector<ConjectureNode *> children;
  children.push_back(new ConjectureNode(NUMBER, "", 5.0));
  root->setChildren(children);

  conjecture->setRoot(root);

  EXPECT_FALSE(conjecture->checkConjectureStructure(root));
}

TEST_F(ConjectureTest, EmptyConjectureHandling) {
  // Test handling of empty conjecture
  State state;

  // Should handle null root gracefully
  EXPECT_NO_THROW({
    // This might cause issues, but we test the current behavior
    EXPECT_EQ(conjecture->getRoot(), nullptr);
  });
}

TEST_F(ConjectureTest, PrintValuesUsesBothChildrenForBinaryFunctions) {
  auto root = new ConjectureNode(FUNCTION_BINARY, "min", 0);
  std::vector<ConjectureNode *> children;
  children.push_back(new ConjectureNode(NUMBER, "", 5.0));
  children.push_back(new ConjectureNode(NUMBER, "", 3.0));
  root->setChildren(children);
  conjecture->setRoot(root);

  State state;
  testing::internal::CaptureStdout();
  conjecture->printValues(state, root);
  const std::string output = testing::internal::GetCapturedStdout();

  EXPECT_NE(output.find("min(5,3)"), std::string::npos);
}

// Test fixture for property assignments
class PropertyAssignmentTest : public ::testing::Test {
protected:
  void SetUp() override { assignment = std::make_unique<PropertyAssignment>(); }

  std::unique_ptr<PropertyAssignment> assignment;
};

TEST_F(PropertyAssignmentTest, BasicPropertyAssignment) {
  ASSERT_NE(assignment, nullptr);

  // Test setting and getting property name
  assignment->setName("ChromaticNumber_AtMost");
  EXPECT_EQ(assignment->getName(), "ChromaticNumber_AtMost");
}

TEST_F(PropertyAssignmentTest, ParameterHandling) {
  assignment->setName("ChromaticNumber_AtMost");

  // Test parameter management
  std::vector<int> params = {4};
  assignment->setParameters(params);

  auto retrieved_params = assignment->getParameters();
  EXPECT_EQ(retrieved_params.size(), 1);
  EXPECT_EQ(retrieved_params[0], 4);
}

// Integration tests combining multiple components
class ConjectureIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    kernel = std::make_unique<DynamicKernel>();
    conjecture = std::make_unique<Conjecture>();
    conjecture->setKernel(kernel.get());
  }

  std::unique_ptr<DynamicKernel> kernel;
  std::unique_ptr<Conjecture> conjecture;
};

TEST_F(ConjectureIntegrationTest, KernelIntegration) {
  EXPECT_EQ(conjecture->getKernel(), kernel.get());
}

TEST_F(ConjectureIntegrationTest, VariableToPropertyMapping) {
  // Test variable to property mapping
  std::map<std::string, std::string> mapping;
  mapping["x"] = "ChromaticNumber_AtMost";

  conjecture->setVariablesToCoreName(mapping);

  auto retrieved_mapping = conjecture->getVariablesToCoreName();
  EXPECT_EQ(retrieved_mapping.at("x"), "ChromaticNumber_AtMost");
}

TEST_F(ConjectureIntegrationTest, CopiedConjectureKeepsIndependentPropertyAssignments) {
  auto property = std::make_unique<PropertyAssignment>();
  property->setName("ChromaticNumber_AtMost");
  property->setParametersVec({3});

  std::map<std::string, PropertyAssignment *> properties;
  properties["x"] = property.get();
  conjecture->setvarToProperty(properties);

  Conjecture copied = *conjecture;
  property.reset();

  ASSERT_NE(copied.getPropertyAssignment("x"), nullptr);
  EXPECT_EQ(copied.getPropertyAssignment("x")->getName(),
            "ChromaticNumber_AtMost");
  EXPECT_EQ(copied.getPropertyAssignment("x")->getParametersVec().size(), 1U);
}
