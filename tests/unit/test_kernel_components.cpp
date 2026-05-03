// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "Kernel/CertificateUtils.h"
#include "Kernel/CertificateWriter.h"
#include "Kernel/DynamicKernel.h"
#include "Kernel/SearchStrategy.h"
#include "Kernel/StateTree.h"
#include "Kernel/WitnessSet.h"
#include "Multigraph/MultiGraph.h"
#include "tests/utils/kernel_test_doubles.h"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace {

using TreeWidzardTest::CountingCore;
using TreeWidzardTest::CountingCoreTypeOne;
using TreeWidzardTest::IntWitness;
using TreeWidzardTest::PassthroughCore;
using TreeWidzardTest::StubSearchStrategy;
using TreeWidzardTest::first_witness_value;
using TreeWidzardTest::make_bag;
using TreeWidzardTest::make_int_witness;
using TreeWidzardTest::witness_values;

class ScopedEnvVar {
public:
  ScopedEnvVar(std::string name, std::string value) : name_(std::move(name)) {
    if (const char *existing = std::getenv(name_.c_str())) {
      original_ = existing;
    }
    set(name_, value);
  }

  ~ScopedEnvVar() {
    if (original_.has_value()) {
      set(name_, *original_);
    } else {
      clear(name_);
    }
  }

private:
  static void set(const std::string &name, const std::string &value) {
#ifdef _WIN32
    _putenv_s(name.c_str(), value.c_str());
#else
    setenv(name.c_str(), value.c_str(), 1);
#endif
  }

  static void clear(const std::string &name) {
#ifdef _WIN32
    _putenv_s(name.c_str(), "");
#else
    unsetenv(name.c_str());
#endif
  }

  std::string name_;
  std::optional<std::string> original_;
};

auto make_tree_width(unsigned value) -> Width {
  Width width;
  width.set_name("tree_width");
  width.set_value(value);
  return width;
}

class ConjectureHarness {
public:
  template <typename... Args> auto make(Args &&...args) -> ConjectureNode * {
    nodes_.push_back(
        std::make_unique<ConjectureNode>(std::forward<Args>(args)...));
    return nodes_.back().get();
  }

  static void attach(ConjectureNode *parent,
                     std::initializer_list<ConjectureNode *> children) {
    std::vector<ConjectureNode *> owned_children(children);
    parent->setChildren(owned_children);
    for (auto *child : owned_children) {
      child->setParent(parent);
    }
  }

private:
  std::vector<std::unique_ptr<ConjectureNode>> nodes_;
};

} // namespace

TEST(KernelComponents, DynamicCoreBaseMetadataAndDefaultClean) {
  PassthroughCore core;

  core.addAttribute("CoreName", "Passthrough");
  core.addAttribute("CoreType", "Max");
  core.setWidth(7);
  core.createInitialWitnessSet();

  ASSERT_NE(core.getInitialSet(), nullptr);
  EXPECT_EQ(first_witness_value(core.getInitialSet()), 0);
  EXPECT_TRUE(core.isAttribute("CoreName", "Passthrough"));
  EXPECT_FALSE(core.isAttribute("CoreName", "Other"));
  EXPECT_EQ(core.getAttributeValue("CoreName"), "Passthrough");
  EXPECT_EQ(core.getAttributeValue("Missing"), "NULL");
  EXPECT_EQ(core.getAttributes().at("CoreType"), "Max");
  EXPECT_EQ(core.getWidth(), 7);

  auto witness_set = std::make_shared<WitnessSetTypeTwo<IntWitness>>();
  witness_set->insert(make_int_witness(3));
  EXPECT_EQ(core.clean(witness_set), witness_set);
}

TEST(KernelComponents, WitnessWrapperAndWitnessSetTypeOneOperations) {
  IntWitness witness;
  witness.value = 4;
  auto cloned = witness.clone();
  ASSERT_NE(cloned, nullptr);
  EXPECT_EQ(cloned->value, 4);
  EXPECT_EQ(witness.witnessInformation(), "4\n");

  std::ostringstream printed;
  witness.witness_info(printed);
  EXPECT_EQ(printed.str(), "4\n");

  IntWitness assigned;
  assigned.value = 1;
  assigned.set_equal(witness);
  EXPECT_EQ(assigned.value, 4);
  EXPECT_FALSE(assigned != witness);

  WitnessSetTypeOne<IntWitness> lhs;
  lhs.insert(make_int_witness(4));
  lhs.insert(make_int_witness(1));
  lhs.insert(make_int_witness(4));
  EXPECT_EQ(lhs.size(), 2);

  WitnessSetTypeOne<IntWitness> rhs;
  rhs.insert(make_int_witness(2));
  rhs.insert(make_int_witness(4));
  lhs.union_set_witness(std::make_shared<WitnessSetTypeOne<IntWitness>>(rhs));

  auto values =
      witness_values(std::make_shared<WitnessSetTypeOne<IntWitness>>(lhs));
  std::sort(values.begin(), values.end());
  EXPECT_EQ(values, (std::vector<int>{1, 2, 4}));
  EXPECT_TRUE(lhs.isLess(rhs) == false);
  EXPECT_FALSE(lhs.isEqual(rhs));

  auto relabeled = lhs.relabel({});
  auto relabeled_values = witness_values(relabeled);
  std::sort(relabeled_values.begin(), relabeled_values.end());
  EXPECT_EQ(relabeled_values, (std::vector<int>{1, 2, 4}));

  Hasher lhs_hash(0);
  lhs.hash(lhs_hash);
  Hasher rhs_hash(0);
  rhs.hash(rhs_hash);
  EXPECT_NE(lhs_hash.get(), rhs_hash.get());

  WitnessSetTypeOne<IntWitness> copied;
  copied.setEqual(lhs);
  EXPECT_TRUE(copied.isEqual(lhs));
}

TEST(KernelComponents, WitnessSetTypeTwoOperations) {
  WitnessSetTypeTwo<IntWitness> lhs;
  lhs.insert(make_int_witness(3));
  lhs.insert(make_int_witness(1));
  lhs.insert(make_int_witness(3));

  WitnessSetTypeTwo<IntWitness> rhs;
  rhs.insert(make_int_witness(5));
  rhs.insert(make_int_witness(1));

  EXPECT_EQ(lhs.size(), 2);
  EXPECT_TRUE(lhs.isLess(rhs));
  EXPECT_FALSE(lhs.isEqual(rhs));

  lhs.union_set_witness(std::make_shared<WitnessSetTypeTwo<IntWitness>>(rhs));
  EXPECT_EQ(witness_values(std::make_shared<WitnessSetTypeTwo<IntWitness>>(lhs)),
            (std::vector<int>{1, 3, 5}));

  auto empty = lhs.createEmptyWitnessSet();
  EXPECT_EQ(empty->size(), 0);

  WitnessSetTypeTwo<IntWitness> copied;
  copied.setEqual(lhs);
  EXPECT_TRUE(copied.isEqual(lhs));
}

TEST(KernelComponents, DynamicKernelOperationsAndMappings) {
  CountingCore core_a;
  CountingCore core_b;
  DynamicKernel kernel;
  Width width = make_tree_width(2);

  kernel.set_width(width);
  kernel.addCore(core_a);
  kernel.addCore(core_b);
  kernel.setVarToNameAndIndex({
      {"x", {"CountingCore", 0}},
      {"y", {"CountingCore", 1}},
  });

  EXPECT_EQ(kernel.coreSize(), 2U);
  EXPECT_EQ(kernel.get_width().get_name(), "tree_width");
  EXPECT_EQ(kernel.get_width().get_value(), 2U);
  EXPECT_TRUE(kernel.isVarExists("x"));
  EXPECT_FALSE(kernel.isVarExists("missing"));
  EXPECT_EQ(kernel.getIndexByVar("y"), 1);
  EXPECT_EQ(kernel.getCoreByVar("x"), &core_a);
  EXPECT_EQ(kernel.pointerToCoreNumber(1), &core_b);
  EXPECT_EQ(kernel.getVarToNameAndIndex().at("x").first, "CountingCore");

  State::ptr initial = kernel.initialState();
  ASSERT_EQ(initial->numberOfComponents(), 2U);
  EXPECT_TRUE(initial->get_bag().get_elements().empty());
  EXPECT_EQ(first_witness_value(initial->getWitnessSet(0)), 0);
  EXPECT_EQ(first_witness_value(initial->getWitnessSet(1)), 0);

  State::ptr one_vertex = kernel.intro_v(initial, 1);
  EXPECT_EQ(one_vertex->get_bag().get_elements(), (std::set<unsigned>{1}));
  EXPECT_EQ(first_witness_value(one_vertex->getWitnessSet(0)), 1);

  State::ptr two_vertices = kernel.intro_v(one_vertex, 2);
  EXPECT_EQ(two_vertices->get_bag().get_elements(),
            (std::set<unsigned>{1, 2}));
  EXPECT_EQ(first_witness_value(two_vertices->getWitnessSet(0)), 2);

  State::ptr edge_state = kernel.intro_e(two_vertices, 1, 2);
  EXPECT_EQ(edge_state->get_bag().get_elements(), (std::set<unsigned>{1, 2}));
  EXPECT_EQ(first_witness_value(edge_state->getWitnessSet(0)), 12);

  State::ptr forgot = kernel.forget_v(edge_state, 2);
  EXPECT_EQ(forgot->get_bag().get_elements(), (std::set<unsigned>{1}));
  EXPECT_EQ(first_witness_value(forgot->getWitnessSet(0)), 12);

  State::ptr joined = kernel.join(two_vertices, edge_state);
  EXPECT_EQ(joined->get_bag().get_elements(), (std::set<unsigned>{1, 2}));
  EXPECT_EQ(first_witness_value(joined->getWitnessSet(0)), 14);
}

TEST(KernelComponents, DynamicKernelPointerLookupRejectsEmptyKernel) {
  DynamicKernel kernel;

  EXPECT_EXIT(
      {
        (void)kernel.pointerToCoreNumber(0);
        std::exit(0);
      },
      ::testing::ExitedWithCode(20), "bigger than the size of cores");
}

TEST(KernelComponents, DynamicKernelUsesPooledStateConstruction) {
  State::clearPool();
  State::initializePool(4);

  {
    CountingCore core;
    DynamicKernel kernel;
    Width width = make_tree_width(2);
    kernel.set_width(width);
    kernel.addCore(core);

    auto initial = kernel.initialState();
    EXPECT_EQ(State::getPoolStats(), 1U);
    EXPECT_TRUE(initial->get_bag().get_elements().empty());
    EXPECT_EQ(first_witness_value(initial->getWitnessSet(0)), 0);

    auto introduced = kernel.intro_v(initial, 1);
    EXPECT_EQ(State::getPoolStats(), 2U);
    EXPECT_EQ(introduced->get_bag().get_elements(),
              (std::set<unsigned>{1}));
    EXPECT_EQ(first_witness_value(introduced->getWitnessSet(0)), 1);

    auto forgotten = kernel.forget_v(introduced, 1);
    EXPECT_EQ(State::getPoolStats(), 3U);
    EXPECT_TRUE(forgotten->get_bag().get_elements().empty());
    EXPECT_EQ(first_witness_value(forgotten->getWitnessSet(0)), 1);
  }

  EXPECT_EQ(State::getPoolStats(), 0U);
  State::clearPool();
}

TEST(KernelComponents, SearchStrategyBaseStoresConfiguration) {
  DynamicKernel kernel;
  Conjecture conjecture;
  Flags flags;

  StubSearchStrategy strategy(&kernel, &conjecture, &flags);
  strategy.addAttribute("SearchName", "Stub");
  strategy.setPropertyFilePath("/tmp/property.tw");
  strategy.setOutputsPath("/tmp/out");
  strategy.search();

  EXPECT_TRUE(strategy.search_called);
  EXPECT_EQ(strategy.kernel_ptr(), &kernel);
  EXPECT_EQ(strategy.conjecture_ptr(), &conjecture);
  EXPECT_EQ(strategy.flags_ptr(), &flags);
  EXPECT_TRUE(strategy.isAttribute("SearchName", "Stub"));
  EXPECT_EQ(strategy.getAttributeValue("SearchName"), "Stub");
  EXPECT_EQ(strategy.getPropertyFilePath(), "/tmp/property.tw");
  EXPECT_EQ(strategy.getOutputsPath(), "/tmp/out");
  EXPECT_EQ(strategy.getAttributes().at("SearchName"), "Stub");
}

TEST(KernelComponents, SearchStrategyBaseSearchExits) {
  EXPECT_EXIT(
      {
        SearchStrategy strategy;
        strategy.search();
      },
      ::testing::ExitedWithCode(20), "");
}

TEST(KernelComponents, StateTreeNodeOperationsAndTreeOutput) {
  auto kernel = std::make_shared<DynamicKernel>();
  CountingCore core;
  Width width = make_tree_width(2);
  kernel->set_width(width);
  kernel->addCore(core);

  auto leaf = std::make_shared<StateTreeNode>("Leaf", kernel->initialState(),
                                              std::vector<StateTreeNodePointer>{},
                                              kernel);
  EXPECT_EQ(leaf->get_nodeType(), "Leaf");
  EXPECT_EQ(leaf->printITD(), "Leaf");
  EXPECT_NE(leaf->printStateTreeNode().find("Core 1 WitnessSet"), std::string::npos);

  StateTreeNode intro_one = leaf->introVertex(1);
  auto intro_one_ptr = std::make_shared<StateTreeNode>(intro_one);
  intro_one_ptr->set_children({leaf});
  leaf->set_parent(intro_one_ptr);

  StateTreeNode intro_two = intro_one_ptr->introVertex(2);
  auto intro_two_ptr = std::make_shared<StateTreeNode>(intro_two);
  intro_two_ptr->set_children({intro_one_ptr});
  intro_one_ptr->set_parent(intro_two_ptr);

  StateTreeNode edge = intro_two_ptr->introEdge(1, 2);
  auto edge_ptr = std::make_shared<StateTreeNode>(edge);
  edge_ptr->set_children({intro_two_ptr});
  intro_two_ptr->set_parent(edge_ptr);

  StateTreeNode forgotten = edge_ptr->forgetVertex(2);
  auto forgotten_ptr = std::make_shared<StateTreeNode>(forgotten);
  forgotten_ptr->set_children({edge_ptr});
  edge_ptr->set_parent(forgotten_ptr);

  StateTreeNode joined = join(*intro_two_ptr, *edge_ptr);
  auto joined_ptr = std::make_shared<StateTreeNode>(joined);
  joined_ptr->set_children({intro_two_ptr, edge_ptr});

  StateTree tree;
  tree.root = joined_ptr;

  unsigned tree_label = 0;
  const std::string tree_text = tree.printTreeRecursive(*tree.root, tree_label);
  EXPECT_NE(tree_text.find("Join"), std::string::npos);
  EXPECT_NE(tree_text.find("IntroVertex_1"), std::string::npos);
  EXPECT_NE(tree_text.find("IntroEdge_1_2"), std::string::npos);

  unsigned itd_label = 0;
  const std::string itd_text = tree.printITDRecursive(*tree.root, itd_label);
  EXPECT_NE(itd_text.find("Leaf"), std::string::npos);
  EXPECT_NE(itd_text.find("Join"), std::string::npos);

  testing::internal::CaptureStdout();
  tree.printITD();
  const std::string itd_stdout = testing::internal::GetCapturedStdout();
  EXPECT_NE(itd_stdout.find("Join"), std::string::npos);

  testing::internal::CaptureStdout();
  tree.printStateTree();
  const std::string tree_stdout = testing::internal::GetCapturedStdout();
  EXPECT_NE(tree_stdout.find("Core 1 WitnessSet"), std::string::npos);

  const fs::path temp_dir =
      fs::temp_directory_path() / "treewidzard-kernel-state-tree";
  fs::create_directories(temp_dir);
  const fs::path original_cwd = fs::current_path();
  fs::current_path(temp_dir);
  tree.writeToFile("sample.tree");
  fs::current_path(original_cwd);

  const fs::path expected_file =
      temp_dir / "Counterexample_StateTreeDec_sample.tree";
  std::ifstream in(expected_file);
  ASSERT_TRUE(in.is_open());
  std::ostringstream contents;
  contents << in.rdbuf();
  EXPECT_NE(contents.str().find("Join"), std::string::npos);

  MultiGraph graph;
  std::map<unsigned, unsigned> color_to_vertex;
  unsigned vertices = 3;
  unsigned edges = 4;
  tree.traverseNode(*tree.root, graph, color_to_vertex, vertices, edges);
  EXPECT_EQ(vertices, 3U);
  EXPECT_EQ(edges, 4U);
}

TEST(KernelComponents, StateTreeExtractMultiGraphExitsUntilImplemented) {
  StateTree tree;
  auto kernel = std::make_shared<DynamicKernel>();
  CountingCore core;
  kernel->addCore(core);
  tree.root = std::make_shared<StateTreeNode>(
      "Leaf", kernel->initialState(), std::vector<StateTreeNodePointer>{},
      kernel);

  EXPECT_EXIT(tree.extractMultiGraph(), ::testing::ExitedWithCode(20), "");
}

TEST(KernelComponents, MultiGraphPrintAndReadRoundTrip) {
  MultiGraph original;
  original.addVertex(1);
  original.addVertex(2);
  original.addVertex(3);
  original.addEdgeLabel(1);
  original.addIncidence(1, 1);
  original.addIncidence(1, 2);
  original.addEdgeLabel(2);
  original.addIncidence(2, 2);
  original.addIncidence(2, 3);

  const fs::path file =
      fs::temp_directory_path() / "treewidzard-multigraph-roundtrip.txt";
  original.printToFile(file.string());

  std::ifstream in(file);
  ASSERT_TRUE(in.is_open());
  MultiGraph restored;
  restored.readFromFile(in);

  EXPECT_EQ(restored.getVertices(), original.getVertices());
  EXPECT_EQ(restored.getEdges(), original.getEdges());
  EXPECT_EQ(restored.getIncidenceMap(), original.getIncidenceMap());
}

TEST(KernelComponents, CertificateWriterWritesExpectedTranscript) {
  const fs::path output =
      fs::temp_directory_path() / "treewidzard-kernel-certificate.twz";
  Width width = make_tree_width(2);

  TreeWidzard::Certificates::CertificateWriter writer(output.string());
  writer.writeHeader(width, "BreadthFirstSearch",
                     TreeWidzard::Certificates::CanonMode::BAG_MIN, true,
                     0x12ULL, "property.twz", 0x34ULL);
  writer.writeLeaf(0);
  writer.writeIntroVertex(1, 0, 3);
  writer.writeForgetVertex(2, 1, 3);
  writer.writeIntroEdge(3, 2, 1, 2);
  writer.writeJoin(4, 1, 3, std::map<unsigned, unsigned>{{1, 2}});
  writer.writeResultSatisfied();
  writer.writeResultNotSatisfied(4);
  writer.writeResultIncomplete();

  std::ifstream in(output);
  ASSERT_TRUE(in.is_open());
  std::ostringstream contents;
  contents << in.rdbuf();
  const std::string text = contents.str();

  EXPECT_NE(text.find("TWZCERT 1"), std::string::npos);
  EXPECT_NE(text.find("H SEARCH BreadthFirstSearch"), std::string::npos);
  EXPECT_NE(text.find("H CANON BAG_MIN"), std::string::npos);
  EXPECT_NE(text.find("H PREMISE 1"), std::string::npos);
  EXPECT_NE(text.find("S 0 LEAF"), std::string::npos);
  EXPECT_NE(text.find("S 1 INTRO_V 0 3"), std::string::npos);
  EXPECT_NE(text.find("S 2 FORGET_V 1 3"), std::string::npos);
  EXPECT_NE(text.find("S 3 INTRO_E 2 1 2"), std::string::npos);
  EXPECT_NE(text.find("S 4 JOIN 1 3 MAP 1 2"), std::string::npos);
  EXPECT_NE(text.find("R SATISFIED"), std::string::npos);
  EXPECT_NE(text.find("R NOT_SATISFIED 4"), std::string::npos);
  EXPECT_NE(text.find("R INCOMPLETE"), std::string::npos);
}

TEST(KernelComponents, CertificateUtilsCoverHashingAndDiscoveryHelpers) {
  using namespace TreeWidzard::Certificates;

  EXPECT_EQ(fnv1a64_bytes("abc"), fnv1a64_bytes(std::string_view("abc")));
  EXPECT_EQ(parse_hex_u64(hex_u64(0x1234abcdULL)), 0x1234abcdULL);
  EXPECT_FALSE(parse_hex_u64("not-hex").has_value());
#ifdef _WIN32
  EXPECT_EQ(split_paths("C:\\dpcores;D:\\search;;E:\\final"),
            (std::vector<std::string>{"C:\\dpcores", "D:\\search",
                                      "E:\\final"}));
#else
  EXPECT_EQ(split_paths("alpha:beta::gamma"),
            (std::vector<std::string>{"alpha", "beta", "gamma"}));
#endif

  const fs::path temp_dir =
      fs::temp_directory_path() / "treewidzard-kernel-dpcores";
  fs::create_directories(temp_dir);
  const fs::path lib_a = temp_dir / ("A" TREEWIDZARD_DYNAMIC_LIB_EXTENSION);
  const fs::path lib_b = temp_dir / ("B" TREEWIDZARD_DYNAMIC_LIB_EXTENSION);
  const fs::path ignored = temp_dir / "note.txt";
  std::ofstream(lib_a) << "a";
  std::ofstream(lib_b) << "bb";
  std::ofstream(ignored) << "ignore";

  ScopedEnvVar env("TREEWIDZARD_DPCORES", temp_dir.string());
  EXPECT_EQ(default_dp_core_paths(), (std::vector<std::string>{temp_dir.string()}));

  const auto files = list_dp_core_files();
  ASSERT_EQ(files.size(), 2U);
  EXPECT_EQ(files[0].name, lib_a.filename().string());
  EXPECT_EQ(files[1].name, lib_b.filename().string());
  EXPECT_EQ(files[0].file_hash, fnv1a64_file(lib_a));
  EXPECT_EQ(files[1].file_hash, fnv1a64_file(lib_b));
  EXPECT_EQ(dp_cores_fingerprint(), dp_cores_fingerprint());
}
