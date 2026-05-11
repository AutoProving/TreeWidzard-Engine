// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef DPCores_ChromaticNumber_AtMost_h
#define DPCores_ChromaticNumber_AtMost_h

#include "../../CorePrelude.h"

using namespace std;

struct ChromaticNumber_AtMost_Witness
    : WitnessWrapper<ChromaticNumber_AtMost_Witness> {
  //////
  vector<set<unsigned>> partialColoring;
  //////

  /**
   * Check if the witnesses l and r are equal.
   */
  friend bool is_equal_implementation(const WitnessAlias &l,
                                      const WitnessAlias &r) {
    return l.partialColoring == r.partialColoring;
  }

  /**
   * Check if the witness l is "less" than r.
   * Less here can mean anything
   * as long as it's a strict total order.
   */
  friend bool is_less_implementation(const WitnessAlias &l,
                                     const WitnessAlias &r) {
    return l.partialColoring < r.partialColoring;
  }

  /**
   * Optional
   * used for isomorphism search and relabeling search,
   * but not regular bfs.
   *
   * Create a copy of the witness where the vertices
   * are renamed according to the relabeling map.
   */
  WitnessAlias
  relabel_implementation(const map<unsigned, unsigned> &relabelingMap) const {
    auto relabeled = WitnessAlias();
    for (const auto &cell : partialColoring) {
      set<unsigned> relabeledCell;
      for (const auto &vertex : cell)
        relabeledCell.insert(relabelingMap.at(vertex));
      relabeled.partialColoring.push_back(relabeledCell);
    }
    // Sort the vector to maintain canonical form for equality comparison
    std::sort(relabeled.partialColoring.begin(), relabeled.partialColoring.end());
    return relabeled;
  }

  /**
   * Optional
   * Is used for the parallel versions of atp.
   *
   * Feed hashable tokens to the hasher object,
   * to create a sequence that uniquely determines
   * the value of the witness.
   *
   * If two witnesses are equal they should supply
   * the same sequence to the hasher, and if they
   * are different they should probably supply
   * different hashes.
   */
  void hash(Hasher &h) const override {
    for (const auto &s : partialColoring) {
      for (const auto &c : s)
        h << c;
      h << -1u;
    }
  }

  /**
   * Get a human readable string
   * representing the witness.
   */
  void witness_info(std::ostream &os) const {
    os << "partialColoring:";
    for (const auto &cell : partialColoring) {
      os << "{ ";
      for (const auto &item : cell) {
        os << item;
        if (item != *cell.crbegin())
          os << ",";
      }
      os << "}";
    }
    os << "\n";
  }
};

struct ChromaticNumber_AtMost_Core
    : CoreWrapper<ChromaticNumber_AtMost_Core, ChromaticNumber_AtMost_Witness,
                  WitnessSetTypeTwo> {
  unsigned k;

  static std::map<std::string, std::string> metadata() {
    return {
        // Canonical public name: boolean threshold for chromatic number.
        {"CoreName", "ChromaticNumber_AtMost"},
        {"CoreType", "Bool"},
        {"ParameterType", "UnsignedInt"},
    };
  }

  ChromaticNumber_AtMost_Core(const parameterType &parameters) {
    auto [n] = unpack_typed_args<int>(parameters);
    k = static_cast<unsigned>(n);
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
  void intro_v_implementation(unsigned i, const Bag &, const WitnessAlias &w,
                              WitnessSet &witnessSet) {
    for (size_t idx = 0; idx < w.partialColoring.size(); ++idx) {
      auto witness = w.clone();
      witness->partialColoring[idx].insert(i);
      // Sort to maintain canonical form
      std::sort(witness->partialColoring.begin(), witness->partialColoring.end());
      witnessSet.insert(std::move(witness));
    }
    if (w.partialColoring.size() < this->k) {
      // i can be added as an separated cell to the partition
      auto witness = w.clone();
      // witness->numberUsedColors = witness->numberUsedColors +1;
      set<unsigned> iCell;
      iCell.insert(i);
      witness->partialColoring.push_back(iCell);
      // Sort to maintain canonical form
      std::sort(witness->partialColoring.begin(), witness->partialColoring.end());
      witnessSet.insert(std::move(witness));
    }
  }

  /**
   * Insert what the witness w becomes
   * after inserting a new edge between i and j,
   * into the witnessSet.
   */
  void intro_e_implementation(unsigned int i, unsigned int j, const Bag &,
                              const WitnessAlias &w, WitnessSet &witnessSet) {
    // If i and j are in different cell then w will be returned, otherwise, it
    // is invalid
    for (const auto &cell : w.partialColoring) {
      if (cell.count(i) && cell.count(j))
        return;
    }
    witnessSet.insert(w.clone());
  }

  /**
   * Insert what the witness w becomes
   * after forgetting the label of the vertex
   * currently labeled i into the witnessSet.
   */
  void forget_v_implementation(unsigned int i, const Bag &,
                               const WitnessAlias &w, WitnessSet &witnessSet) {
    for (size_t idx = 0; idx < w.partialColoring.size(); ++idx) {
      if (w.partialColoring[idx].count(i)) {
        auto witness = w.clone();
        witness->partialColoring[idx].erase(i);
        if (witness->partialColoring[idx].empty()) {
          // Remove the empty cell from the vector
          witness->partialColoring.erase(witness->partialColoring.begin() + static_cast<ptrdiff_t>(idx));
        }
        std::sort(witness->partialColoring.begin(),
                  witness->partialColoring.end());
        witnessSet.insert(std::move(witness));
        break;
      }
    }
  }

  /**
   * Join two witnesses from two subtrees.
   * For chromatic number, two partial colorings can be joined if they
   * agree on the vertices currently in the bag (i.e., the restriction
   * of both colorings to the bag vertices yields the same partition).
   * Since forget_v removes vertices from the coloring, at a join node
   * only bag vertices remain in the witness. Therefore, two witnesses
   * are compatible iff they are equal (same partition of bag vertices).
   */
  void join_implementation(const Bag &, const WitnessAlias &w1,
                           const WitnessAlias &w2, WitnessSet &witnessSet) {
    if (w1.partialColoring == w2.partialColoring) {
      witnessSet.insert(w1.clone());
    }
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
};

#endif
