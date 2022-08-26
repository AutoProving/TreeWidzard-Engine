// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include <bits/stdc++.h>

#include "../CorePrelude.h"

using namespace std;

struct TwoHamiltonianCycles_Witness : WitnessWrapper<TwoHamiltonianCycles_Witness> {
  struct VertexInfo {
    unsigned degree = -1U;
    unsigned match = -1U;
    bool operator<(const VertexInfo &other) const {
      return tie(degree, match) < tie(other.degree, other.match);
    }
    bool operator==(const VertexInfo &other) const {
      return tie(degree, match) == tie(other.degree, other.match);
    }
  };
  vector<VertexInfo> info[2];
  bool closed[2] = {false, false};
  bool was_different = false;

  /**
   * Check if the witnesses l and r are equal.
   */
  friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
    return l.was_different == r.was_different && l.closed[0] == r.closed[0] &&
           l.closed[1] == r.closed[1] && l.info[0] == r.info[0] &&
           l.info[1] == r.info[1];
  }

  /**
   * Check if the witness l is "less" than r.
   * Less here can mean anything
   * as long as it's a strict total order.
   */
  friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
    auto l_tuple_ref = std::tie(l.info[0], l.info[1], l.closed[0], l.closed[1],
                                l.was_different);
    auto r_tuple_ref = std::tie(r.info[0], r.info[1], r.closed[0], r.closed[1],
                                r.was_different);
    return l_tuple_ref < r_tuple_ref;
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
    WitnessAlias relabeledWitness;
    relabeledWitness.closed[0] = this->closed[0];
    relabeledWitness.closed[1] = this->closed[1];
    relabeledWitness.was_different = this->was_different;

    unsigned relabeled_size = 0;
    for (auto [oldlabel, newlabel] : relabelingMap)
      relabeled_size = std::max(relabeled_size, newlabel + 1);

    for (int v : {0, 1}) {
      relabeledWitness.info[v].resize(relabeled_size);

      for (unsigned i = 0; i < this->info[v].size(); ++i) {
        if (this->info[v][i].degree == -1U) continue;

        if (relabelingMap.count(i)) {
          unsigned j = relabelingMap.at(i);
          relabeledWitness.info[v][j] = this->info[v][i];

          unsigned mi = this->info[v][i].match;
          if (mi != -1U) {
            if (relabelingMap.count(mi)) {
              unsigned mj = relabelingMap.at(mi);
              relabeledWitness.info[v][j].match = mj;
            } else {
              cout << "Error: TwoHamiltonianCycles_Witness::relabel " << mi
                   << " from info[" << v << "] is not in the map." << endl;
              exit(20);
            }
          }
        } else {
          cout << "Error: TwoHamiltonianCycles_Witness::relabel " << i
               << " from info[" << v << "] is not in the map." << endl;
          exit(20);
        }
      }

      while (relabeledWitness.info[v].size() &&
             relabeledWitness.info[v].back().degree == -1U)
        relabeledWitness.info[v].pop_back();
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
    for (int version : {0, 1}) {
      h << closed[version] << -1u;
      for (int i = 0; i < info[version].size(); ++i) {
        if (info[version][i].degree != -1u) {
          h << i << info[version][i].degree << info[version][i].match;
        }
      }
    }
  }

  /**
   * Get a human readable string
   * representing the witness.
   */
  string witnessInformation() const override {
    stringstream out;
    for (int version : {0, 1}) {
      out << "V" << version << "{";

      out << "degree=[";
      for (unsigned i = 0; i < this->info[version].size(); ++i) {
        if (this->info[version][i].degree == -1U) {
          out << '-';
        } else {
          out << this->info[version][i].degree;
        }

        if (i + 1 != this->info[version].size()) out << ',';
      }

      out << "] matching=[";
      for (unsigned i = 0; i < this->info[version].size(); ++i) {
        if (this->info[version][i].match == -1U) {
          out << '-';
        } else {
          out << this->info[version][i].match;
        }

        if (i + 1 != this->info[version].size()) out << ',';
      }

      out << "] closed=" << this->closed[version] << "} ";
    }
    out << "was_different=" << this->was_different << '\n';
    return out.str();
  }
};

struct TwoHamiltonianCycles_Core : CoreWrapper<TwoHamiltonianCycles_Core, TwoHamiltonianCycles_Witness, WitnessSetTypeTwo> {
  static constexpr char CoreName[] = "TwoHamiltonianCycles";
  static constexpr char CoreType[] = "Bool";
  static constexpr char ParameterType[] = "ParameterLess";
  static constexpr char PrimaryOperator[] = "---";

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
  void intro_v_implementation(unsigned i, const Bag &b, const WitnessAlias &w,
                              WitnessSet &witnessSet) {
    if (w.closed[0] || w.closed[1]) return;  // impossible

    auto w1 = w.clone();

    for (int v : {0, 1})
      if (i >= w1->info[v].size()) w1->info[v].resize(i + 1);

    // Only accepts a vertex if it's new
    if (w1->info[0][i].degree == -1U && w1->info[1][i].degree == -1U) {
      w1->info[0][i].degree = 0;
      w1->info[1][i].degree = 0;
      witnessSet.insert(w1);
    }
  }

  /**
   * Insert what the witness w becomes
   * after inserting a new edge between i and j,
   * into the witnessSet.
   */
  void intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                              const WitnessAlias &w, WitnessSet &witnessSet) {
    auto try_use_edge = [&](WitnessAlias *w1, int v) -> bool {
      // attempt using the edge i,j in version v
      // returns true on success

      assert(w1->info[v][i].degree != -1U);
      assert(w1->info[v][j].degree != -1U);

      // Only introduces an edge if it's open
      if (w1->closed[v]) return false;

      if (w1->info[v][i].degree == 0 && w1->info[v][j].degree == 0) {
        w1->info[v][i].degree = 1;
        w1->info[v][j].degree = 1;
        w1->info[v][i].match = j;
        w1->info[v][j].match = i;
        return true;
      }

      if (w1->info[v][i].degree == 0 && w1->info[v][j].degree == 1) {
        w1->info[v][i].degree = 1;
        w1->info[v][j].degree = 2;
        unsigned m = w1->info[v][j].match;
        assert(m != -1U);
        w1->info[v][i].match = m;
        w1->info[v][m].match = i;
        w1->info[v][j].match = -1U;
        return true;
      }

      if (w1->info[v][i].degree == 1 && w1->info[v][j].degree == 0) {
        w1->info[v][i].degree = 2;
        w1->info[v][j].degree = 1;
        unsigned m = w1->info[v][i].match;
        assert(m != -1U);
        w1->info[v][j].match = m;
        w1->info[v][m].match = j;
        w1->info[v][i].match = -1U;
        return true;
      }

      if (w1->info[v][i].degree == 1 && w1->info[v][j].degree == 1) {
        if (w1->info[v][i].match != j) {
          assert(w1->info[v][j].match != i);

          w1->info[v][i].degree = 2;
          w1->info[v][j].degree = 2;

          unsigned mi = w1->info[v][i].match;
          unsigned mj = w1->info[v][j].match;
          assert(mi != -1U);
          assert(mj != -1U);

          w1->info[v][i].match = -1U;
          w1->info[v][j].match = -1U;
          w1->info[v][mi].match = mj;
          w1->info[v][mj].match = mi;
          return true;
        }

        // i is matched to j
        assert(w1->info[v][j].match == i);
        assert(w1->info[v][i].match == j);

        // does the path include every vertex?
        unsigned cnt_0 = 0, cnt_1 = 0;
        for (unsigned k = 0; k < w1->info[v].size(); ++k) {
          cnt_0 += w1->info[v][k].degree == 0;
          cnt_1 += w1->info[v][k].degree == 1;
        }

        if (cnt_0 == 0 && cnt_1 == 2) {
          // complete cycle
          w1->info[v][i].degree = 2;
          w1->info[v][j].degree = 2;
          w1->info[v][i].match = -1U;
          w1->info[v][j].match = -1U;
          w1->closed[v] = true;
          return true;
        }
      }

      return false;
    };

    for (int use_in_v0 : {0, 1}) {
      for (int use_in_v1 : {0, 1}) {
        auto w1 = w.clone();

        if (use_in_v0) {
          // attempt using the edge, and skip this case if it was impossible
          if (!try_use_edge(&*w1, 0)) continue;
        }

        if (use_in_v1) {
          // attempt using the edge, and skip this case if it was impossible
          if (!try_use_edge(&*w1, 1)) continue;
        }

        // remember that two different choices were made
        w1->was_different |= use_in_v0 != use_in_v1;

        witnessSet.insert(w1);
      }
    }
  }

  /**
   * Insert what the witness w becomes
   * after forgetting the label of the vertex
   * currently labeled i into the witnessSet.
   */
  void forget_v_implementation(unsigned int i, Bag &b, const WitnessAlias &w,
                               WitnessSet &witnessSet) {
    // Only forget vertices with degree 2
    if (w.info[0][i].degree == 2 && w.info[1][i].degree == 2) {
      auto w1 = w.clone();
      assert(w1->info[0][i].match == -1U);
      assert(w1->info[1][i].match == -1U);
      w1->info[0][i].degree = -1U;
      w1->info[1][i].degree = -1U;
      for (int v : {0, 1}) {
        while (w1->info[v].size() && w1->info[v].back().degree == -1U) {
          assert(w1->info[v].back().match == -1U);
          w1->info[v].pop_back();
        }
      }
      witnessSet.insert(w1);
    }
  }

  /**
   * Optional
   * Is used for treewidth, but not for pathwidth.
   *
   * Insert what the witness results from joining
   * w1 and w2 into the witnessSet.
   */
  void join_implementation(Bag &b, const WitnessAlias &w1, const WitnessAlias &w2,
                           WitnessSet &witnessSet) {
    // can only join closed if the other side didn't use any edges

    auto wPrime = make_shared<WitnessAlias>();
    wPrime->was_different |= w1.was_different;
    wPrime->was_different |= w2.was_different;

    unsigned sz = (unsigned)std::max({
        w1.info[0].size(),
        w2.info[0].size(),
        w1.info[1].size(),
        w2.info[1].size(),
    });

    auto merge_version = [&](int v) -> bool {
      wPrime->info[v].resize(sz);

      for (unsigned i = 0; i < wPrime->info[v].size(); ++i) {
        unsigned d1 = -1U;
        unsigned d2 = -1U;

        if (i < w1.info[v].size()) d1 = w1.info[v][i].degree;
        if (i < w2.info[v].size()) d2 = w2.info[v][i].degree;

        if (d1 == -1U && d2 == -1U) continue;

        if (d1 == -1U) d1 = 0;
        if (d2 == -1U) d2 = 0;

        wPrime->info[v][i].degree = d1 + d2;

        if (wPrime->info[v][i].degree > 2) return false;  // impossible
      }

      int cnt_0 = 0, cnt_1 = 0;

      std::vector<unsigned> endpoints;
      std::vector<unsigned> end_vis(sz);
      std::vector<unsigned> in_graph(sz);
      std::vector<unsigned> is_endpoint(sz);
      std::vector<std::vector<unsigned>> g(sz);

      for (unsigned i = 0; i < sz; ++i) {
        if (wPrime->info[v][i].degree == -1U) continue;

        cnt_0 += wPrime->info[v][i].degree == 0;
        cnt_1 += wPrime->info[v][i].degree == 1;

        if (wPrime->info[v][i].degree == 1) {
          endpoints.push_back(i);
          is_endpoint[i] = 1;
        }

        if (i < w1.info[v].size()) {
          unsigned j = w1.info[v][i].match;
          if (j != -1U) {
            in_graph[i] = 1;
            assert(w1.info[v][i].degree == 1);
            g[i].push_back(j);
          }
        }
        if (i < w2.info[v].size()) {
          unsigned j = w2.info[v][i].match;
          if (j != -1U) {
            in_graph[i] = 1;
            assert(w2.info[v][i].degree == 1);
            g[i].push_back(j);
          }
        }
      }

      while (endpoints.size()) {
        unsigned start = endpoints.back();
        endpoints.pop_back();
        if (end_vis[start]) continue;
        if (!in_graph[start]) continue;

        unsigned end = start;
        while (g[end].size()) {
          unsigned next = g[end].back();
          g[end].pop_back();

          assert(g[next].size());
          if (g[next].back() == end) {
            g[next].pop_back();
          } else {
            assert(g[next].front() == end);
            g[next].erase(g[next].begin());
          }

          end = next;
        }

        end_vis[start] = end_vis[end] = 1;
        assert(start != end);
        assert(is_endpoint[start]);
        assert(is_endpoint[end]);
        assert(wPrime->info[v][start].degree == 1);
        assert(wPrime->info[v][end].degree == 1);

        wPrime->info[v][start].match = end;
        wPrime->info[v][end].match = start;
      }

      int cycle_count = w1.closed[v] + w2.closed[v];

      unsigned start = sz - 1;
      while (start != -1U) {
        if (end_vis[start] || !in_graph[start] || g[start].size() == 0) {
          --start;
          continue;
        }

        ++cycle_count;

        unsigned end = start;
        while (g[end].size()) {
          unsigned next = g[end].back();
          g[end].pop_back();

          assert(g[next].size());
          if (g[next].back() == end) {
            g[next].pop_back();
          } else {
            assert(g[next].front() == end);
            g[next].erase(g[next].begin());
          }

          end = next;
        }

        assert(start == end);
      }

      for (const auto &adj : g)
        assert(adj.size() == 0);  // make sure all edges were used

      if (cycle_count > 1) return false;

      if (cycle_count > 0 && (cnt_0 > 0 || cnt_1 > 0)) return false;

      if (cycle_count == 1) wPrime->closed[v] = true;

      return true;
    };

    for (int version : {0, 1})
      if (!merge_version(version)) return;  // impossible

    witnessSet.insert(wPrime);
  }

  /**
   * Remove redundant witnesses from the witnessSet
   */
  void clean_implementation(WitnessSet &witnessSet) {
    // In most cases, you will not need to change this function.
  }

  /**
   * Return whether w is a final witness
   */
  bool is_final_witness_implementation(const WitnessAlias &w) {
    if (!w.was_different) return false;

    for (int version : {0, 1}) {
      if (!w.closed[version]) return false;

      for (auto [d, j] : w.info[version]) {
        assert(d == -1U || d == 2);
        assert(j == -1U);
      }
    }

    return true;
  }
};

extern "C" {
  DynamicCore *create() { return new TwoHamiltonianCycles_Core; }
}
