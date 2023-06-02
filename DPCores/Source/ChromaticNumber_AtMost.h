// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include <bits/stdc++.h>
#include <iterator>

#include "../../CorePrelude.h"

using namespace std;

struct ChromaticNumber_AtMost_Witness : WitnessWrapper<ChromaticNumber_AtMost_Witness> {
	//////
  set<set<unsigned>> partialColoring;
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
									   const WitnessAlias &r){
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
	WitnessAlias relabel_implementation(
		const map<unsigned, unsigned> &relabelingMap) const {
    auto relabeled = WitnessAlias();
    for (const auto &cell : partialColoring) {
      set<unsigned> relabeledCell;
      for (const auto &vertex : cell)
        relabeledCell.insert(relabelingMap.at(vertex));
      relabeled.partialColoring.insert(relabeledCell);
    }
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
    for (auto s : partialColoring) {
      for (auto c : s) h << c;
      h << -1u;
    }
	}

	/**
	 * Get a human readable string
	 * representing the witness.
	 */
	void witness_info(std::ostream &os) const {
    os << "partialColoring:";
    for (auto cell : partialColoring) {
      os << "{ ";
      for (auto item : cell) {
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
	: CoreWrapper<ChromaticNumber_AtMost_Core, ChromaticNumber_AtMost_Witness, WitnessSetTypeTwo> {
  unsigned k;

	static std::map<std::string, std::string> metadata() {
		return {
			{"CoreName", "ChromaticNumber"},
			{"CoreType", "Bool"},
			{"ParameterType", "UnsignedInt"},
			{"PrimaryOperator", "AtMost"},
		};
	}

	ChromaticNumber_AtMost_Core(const parameterType &parameters) {
    auto [n] = unpack_typed_args<int>(parameters);
    k = n;
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
    for (auto cell : w.partialColoring) {
      auto witness = w.clone();
      witness->partialColoring.erase(cell);
      cell.insert(i);
      witness->partialColoring.insert(cell);
      witnessSet.insert(std::move(witness));
    }
    if (w.partialColoring.size() < this->k) {
      // i can be added as an separated cell to the partition
      auto witness = w.clone();
      // witness->numberUsedColors = witness->numberUsedColors +1;
      set<unsigned> iCell;
      iCell.insert(i);
      witness->partialColoring.insert(iCell);
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
    for (auto cell : w.partialColoring)
      if (cell.count(i) && cell.count(j))
        return;
    witnessSet.insert(w.clone());
	}

	/**
	 * Insert what the witness w becomes
	 * after forgetting the label of the vertex
	 * currently labeled i into the witnessSet.
	 */
	void forget_v_implementation(unsigned int i, const Bag &,
								 const WitnessAlias &w,
								 WitnessSet &witnessSet) {
    for (auto cell : w.partialColoring) {
      if (cell.count(i)) {
        auto witness = w.clone();
        witness->partialColoring.erase(cell);
        cell.erase(i);
        if (!cell.empty()) {
          witness->partialColoring.insert(cell);
        }
        witnessSet.insert(witness);
        break;
      }
    }
	}

	/**
	 * Optional
	 * Is used for treewidth, but not for pathwidth.
	 *
	 * Insert what the witness results from joining
	 * w1 and w2 into the witnessSet.
	 */

	WitnessSetPointer join(const Bag &, const WitnessSetPointer ws_ptr1,
						   WitnessSetPointer ws_ptr2) override {
    const auto &ws1 = as_witness_set(*ws_ptr1);
    const auto &ws2 = as_witness_set(*ws_ptr2);
    auto newWitnessSet = std::make_shared<WitnessSet>();

    std::vector<std::shared_ptr<Witness>> res;
    std::set_intersection(ws1.begin(), ws1.end(), ws2.begin(), ws2.end(),
                          std::back_inserter(res),
                          [](const auto &w1, const auto &w2) {
                            return WitnessAlias::as_witness(*w1).partialColoring < WitnessAlias::as_witness(*w2).partialColoring;
                          });
    for (auto w : res) {
      newWitnessSet->insert(w);
    }

    clean_implementation(*newWitnessSet);
    return newWitnessSet;
	}
	void join_implementation(const Bag &, const WitnessAlias &w1,
							 const WitnessAlias &w2, WitnessSet &witnessSet) {
    assert(false); // unused due to specialization above
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
