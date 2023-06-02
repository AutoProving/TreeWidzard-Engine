// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include <bits/stdc++.h>

#include "../../CorePrelude.h"

struct CliqueNumberSimpleGraphs_AtLeast_Witness : WitnessWrapper<CliqueNumberSimpleGraphs_AtLeast_Witness> {
	//////
  bool found = false; // Set to true if and only if a clique of the right size was
				// found
  std::map<unsigned, unsigned> partialClique;
	unsigned size = 0; // size of the clique has been found
                     // 216
	//////

	/**
	 * Check if the witnesses l and r are equal.
	 */
	friend bool is_equal_implementation(const WitnessAlias &l,
										const WitnessAlias &r) {
    return std::tie(l.found, l.partialClique, l.size) ==
           std::tie(r.found, r.partialClique, r.size);
	}

	/**
	 * Check if the witness l is "less" than r.
	 * Less here can mean anything
	 * as long as it's a strict total order.
	 */
	friend bool is_less_implementation(const WitnessAlias &l,
									   const WitnessAlias &r){
    return std::tie(l.found, l.partialClique, l.size) <
           std::tie(r.found, r.partialClique, r.size);
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
		const std::map<unsigned, unsigned> &relabelingMap) const {
    auto relabeledWitness = WitnessAlias();

    relabeledWitness.found = this->found;
    relabeledWitness.size = this->size;
    for (auto p : this->partialClique) {
      auto it = relabelingMap.find(p.first);

      if (it != relabelingMap.end()) {
        relabeledWitness.partialClique.insert(
          std::make_pair(it->second, p.second));
      } else {
        std::cerr << "Error: CliqueNumberSimpleGraphs_AtLeast_Witness::relabel "
           << p.first << "in the map\n";
        print();
        for (auto l : relabelingMap) {
          std::cerr << l.first << "->" << l.second << '\n';
        }
        exit(20);
      }
    }
    return relabeledWitness;
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
    h << found;
    h << -1u;
    for (auto [i, j] : partialClique) {
      h << i << j;
    }
    h << -1u;
    h << size;
	}

	/**
	 * Get a human readable string
	 * representing the witness.
	 */
	void witness_info(std::ostream &os) const {
    os << "found = " << found << " size: " << size;
    os << "partialClique={";
    for (auto it = partialClique.begin(); it != partialClique.end(); ++it) {
      os << it->first << "->" << it->second;
      if (it != --partialClique.end()) {
        os << ",";
      }
    }
    os << "}\n";
	}
};

struct CliqueNumberSimpleGraphs_AtLeast_Core
	: CoreWrapper<CliqueNumberSimpleGraphs_AtLeast_Core, CliqueNumberSimpleGraphs_AtLeast_Witness, WitnessSetTypeTwo> {
  unsigned cliqueSize;

	static std::map<std::string, std::string> metadata() {
		return {
			{"CoreName", "CliqueNumberSimpleGraphs"},
			{"CoreType", "Bool"},
			{"ParameterType", "UnsignedInt"},
			{"PrimaryOperator", "AtLeast"},
		};
	}

	CliqueNumberSimpleGraphs_AtLeast_Core(const parameterType &parameters) {
    auto [n] = unpack_typed_args<int>(parameters);
    cliqueSize = n;
  }

	/**
	 * Initialize the witnessset corresponding to leaf
	 * nodes in the decomposition.
	 */
	void initialize_leaf(WitnessSet &witnessSet) {
		witnessSet.insert(std::make_shared<WitnessAlias>());
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
    // Choose not to add i to the clique
    witnessSet.insert(w.clone());

    if (w.found or w.size == this->cliqueSize)
      return; // can not be added to the clique

    // A new vertex only can be added to the clique when no vertex has been
    // forgotten from the clique. This means number of vertices in the
    // current clique should be equal to size
    if (w.size == w.partialClique.size()) {
      auto witness = w.clone();
      witness->partialClique.insert(std::make_pair(i, 0));
      witness->size = w.size + 1;
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
    if (w.found) {
      witnessSet.insert(w.clone());
    } else {
      auto witness = w.clone();

      if (w.partialClique.count(i) and w.partialClique.count(j)) {
        // Both i and j are in the clique, so that the counters of i and j
        // will be increased by one.
        unsigned iCounter = witness->partialClique[i];
        unsigned jCounter = witness->partialClique[j];
        if (iCounter < cliqueSize - 1 and jCounter < cliqueSize - 1) {
          witness->partialClique.erase(i);
          witness->partialClique.erase(j);
          witness->partialClique.insert(std::make_pair(i, iCounter + 1));
          witness->partialClique.insert(std::make_pair(j, jCounter + 1));
          if (isCompleteClique(*witness)) {
            witness->found = true;
            witness->partialClique.clear();
          }
          witnessSet.insert(std::move(witness));
        }

      } else {
        // Either i or j is not in the clique, so that {i,j} cannot be added
        // to the clique.
        witnessSet.insert(std::move(witness));
      }
    }
	}

	/**
	 * Insert what the witness w becomes
	 * after forgetting the label of the vertex
	 * currently labeled i into the witnessSet.
	 */
	void forget_v_implementation(unsigned int i, const Bag &,
								 const WitnessAlias &w,
								 WitnessSet &witnessSet) {
    if (w.found) {
      witnessSet.insert(w.clone());
    } else {
      std::map<unsigned, unsigned> partialCliqueW = w.partialClique;
      if (partialCliqueW.count(i)) {
        // A vertex only can be forgotten when the size of the clique is
        // equal to cliqueSize. And the forgotten vertex has cliqueSize-1
        // neighbors.
        if (w.size == cliqueSize and partialCliqueW[i] == cliqueSize - 1) {
          auto witness = w.clone();
          witness->partialClique.erase(i);
          witnessSet.insert(std::move(witness));
        } else {
          // invalid witness
          return;
        }
      } else {
        // i is not in the clique
        witnessSet.insert(w.clone());
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
	void join_implementation(const Bag &, const WitnessAlias &w1,
							 const WitnessAlias &w2, WitnessSet &witnessSet) {
    if (w1.found) {
      witnessSet.insert(w1.clone());
    } else if (w2.found) {
      witnessSet.insert(w2.clone());
    } else {
      // At the same time it is not possible to have forget vertices in both
      // witnesses If there is a forgotten vertex v in w1, and a forgotten
      // vertex u in w2, then v and u can not be connected to together
      if (w1.partialClique.size() == w1.size or
        w2.partialClique.size() == w2.size) {
        // The domains of two partialCliques should be the same, and
        // therefore, they should have the same size. Do not confuse w->size
        // with w->partialClique.size().
        if (w1.partialClique.size() == w2.partialClique.size()) {
          auto witness = w1.clone();
          // The size of the new witness is the sum of the sizes of the
          // two witnesses minus the size of the partialClique Note that,
          // w1->partialClique.size() = w2->partialClique.size().
          witness->size = w1.size + w2.size - w1.partialClique.size();
          // Now we check if the domains of the partialCliques are the
          // same. If the in the checking process we realize that they do
          // not have the same domains, we return the empty set.
          for (auto p : w2.partialClique) {
            if (witness->partialClique.count(p.first)) {
              unsigned pCounter =
                p.second + witness->partialClique[p.first];
              witness->partialClique.erase(p.first);
              witness->partialClique.insert(
                std::make_pair(p.first, pCounter));
            } else {
              // w1 and w2 have different domains, so they cannot be
              // joined.
              return;
            }
          }
          // Check that the clique is a complete clique or not,
          // if yes, found will be true and partialClique will be cleared.
          if (isCompleteClique(*witness)) {
            witness->found = true;
            witness->partialClique.clear();
          }
          witnessSet.insert(witness);
        } else {
          return;
        }
      } else {
        return;
      }
    }
	}

	/**
	 * Remove redundant witnesses from the witnessSet
	 */
	void clean_implementation(WitnessSet &witnessSet) {
    for (auto witness : witnessSet) {
      auto &w = WitnessAlias::as_witness(*witness);

      if (w.found) {
        auto newWitnessSet = std::make_shared<WitnessSet>();
        newWitnessSet->insert(witness);
        witnessSet.setEqual(*newWitnessSet);
        return;
      }
    }
	}

	/**
	 * Return whether w is a final witness
	 */
	bool is_final_witness_implementation(const Bag &, const WitnessAlias &w) {
    return w.found;
	}

  bool isCompleteClique(const WitnessAlias &w) {
    if (w.size == cliqueSize) {
      for (auto m : w.partialClique) {
        if (m.second != cliqueSize - 1) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }
};
