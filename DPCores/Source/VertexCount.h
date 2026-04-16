#ifndef DPCores_VertexCount_h
#define DPCores_VertexCount_h

// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "../../CorePrelude.h"

using namespace std;

struct VertexCount_Witness : WitnessWrapper<VertexCount_Witness> {

  // ATRIBUTES DEFINITION
  int vertexCount = 0;

  friend bool is_equal_implementation(const WitnessAlias &l,
                                      const WitnessAlias &r) {
    return l.vertexCount == r.vertexCount;
  }

  friend bool is_less_implementation(const WitnessAlias &l,
                                     const WitnessAlias &r) {
    return l.vertexCount < r.vertexCount;
  }

  WitnessAlias relabel_implementation(
      const std::map<unsigned, unsigned> &) const {
    auto relabeled = WitnessAlias();
    relabeled.vertexCount = vertexCount;
    return relabeled;
  }
  void hash(Hasher &h) const override { h << vertexCount; }
  void witness_info(std::ostream &os) const { os << vertexCount << '\n'; }
};

struct VertexCount_Core
    : CoreWrapper<VertexCount_Core, VertexCount_Witness, WitnessSetTypeTwo> {
  static std::map<std::string, std::string> metadata() {
    return {
        // Canonical public name: numeric invariant for the number of vertices.
        {"CoreName", "VertexCount"},
        {"CoreType", "Min"},
        {"ParameterType", "None"},
    };
  }

  VertexCount_Core(const parameterType &parameters) {
    assert(parameters.empty());
    (void)parameters;
  }

  /**
   * Initialize the witnessset corresponding to leaf
   * nodes in the decomposition.
   */
  void initialize_leaf(WitnessSet &witnessSet) {
    witnessSet.insert(make_shared<WitnessAlias>());
  }

  /**
   * Insert what the witness w becomes
   * after inserting the new vertex i,
   * into the witnessSet.
   *
   * Multiple results can be inserted if there are multiple
   * choices for what to do with the new vertex, and if there
   * is no way to get a valid witness after inserting the vertex,
   * insert no results.
   */
  void intro_v_implementation(unsigned, const Bag &, const WitnessAlias &w,
                              WitnessSet &witnessSet) {
    auto wPrime = w.clone();
    ++wPrime->vertexCount;
    witnessSet.insert(wPrime);
  }

  /**
   * Insert what the witness w becomes
   * after inserting a new edge between i and j,
   * into the witnessSet.
   */
  void intro_e_implementation(unsigned int, unsigned int, const Bag &,
                              const WitnessAlias &w, WitnessSet &witnessSet) {

    witnessSet.insert(w.clone());
  }

  /**
   * Insert what the witness w becomes
   * after forgetting the label of the vertex
   * currently labeled i into the witnessSet.
   */
  void forget_v_implementation(unsigned int, const Bag &,
                               const WitnessAlias &w, WitnessSet &witnessSet) {

    witnessSet.insert(w.clone());
  }

  /**
   * Optional
   * Is used for treewidth, but not for pathwidth.
   *
   * Insert what the witness results from joining
   * w1 and w2 into the witnessSet.
   */
  void join_implementation(const Bag &b, const WitnessAlias &w1,
                           const WitnessAlias &w2, WitnessSet &witnessSet) {
    int joinedCount =
        w1.vertexCount + w2.vertexCount - static_cast<int>(b.get_elements().size());
    auto wPrime = std::make_shared<WitnessAlias>();
    wPrime->vertexCount = joinedCount;
    witnessSet.insert(wPrime);
  }

  /**
   * Remove redundant witnesses from the witnessSet
   */
  void clean_implementation(WitnessSet &) {
    // In most cases, you will not need to change this function.
  }

  /**
   * Return whether w is a final witness
   */
  bool is_final_witness_implementation(const Bag &, const WitnessAlias &) {
    return true;
  }

  int inv_implementation(const Bag &, const WitnessAlias &w) {
    return static_cast<int>(w.vertexCount);
  }
};

#endif
