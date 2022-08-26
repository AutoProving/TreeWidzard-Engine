// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include <bits/stdc++.h>

#include "../CorePrelude.h"
using namespace std;

struct Connectivity_Witness : WitnessWrapper<Connectivity_Witness> {
  set<set<unsigned>> partition;
  uint8_t tag = 0;

  /**
   * Check if the witnesses l and r are equal.
   */
  friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
    return tie(l.tag, l.partition) == tie(r.tag, r.partition);
  }

  /**
   * Check if the witness l is "less" than r.
   * Less here can mean anything
   * as long as it's a strict total order.
   */
  friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
    return tie(l.tag, l.partition) < tie(r.tag, r.partition);
  }

  /**
   * Optional
   *
   * Create a copy of the witness where the vertices
   * are renamed according to the relabeling map.
   */
  WitnessAlias relabel_implementation(
      const map<unsigned, unsigned> &relabelingMap) const {
    WitnessAlias res;
    res.tag = tag;
    for (const auto &component : partition) {
      set<unsigned> new_component;

      for (unsigned x : component) {
        auto it = relabelingMap.find(x);
        if (it == relabelingMap.end()) {
          cerr << "Missing node " << x << " in the relabeling map.\n";
          exit(20);
        }
        new_component.insert(it->second);
      }

      res.partition.insert(move(new_component));
    }
    return res;
  }

  /**
   * Optional
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
    h << tag;
    for (const auto &component : partition) {
      for (unsigned node : component) h << node;
      h << -1u;
    }
  }

  /**
   * Get a human readable string
   * representing the witness.
   */
  string witnessInformation() const override {
    ostringstream info;
    info << "Tag=" << int(tag) << "\n";
    info << "Partition=\n";
    for (auto part : this->partition) {
      for (auto i : part) info << i << ' ';
      info << '\n';
    }
    return info.str();
  }

  /*
  Assign the value of w to this. C++ implements
  this automatically, so in most cases it is not needed.
  WitnessImpl &operator=(const WitnessImpl &w) {
    partition = w.partition;
    tag = w.tag;
    return *this;
  }
  // */
};

struct Connectivity_Core : CoreWrapper<Connectivity_Core, Connectivity_Witness, WitnessSetTypeTwo> {
  static constexpr char CoreName[] = "Connectivity";
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
    auto wPrime = w.clone();
    wPrime->tag |= 1;
    wPrime->partition.insert({i});
    witnessSet.insert(wPrime);
  }

  /**
   * Insert what the witness w becomes
   * after inserting a new edge between i and j,
   * into the witnessSet.
   */
  void intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                              const WitnessAlias &w, WitnessSet &witnessSet) {
    auto i_it = find_if(w.partition.begin(), w.partition.end(),
                        [i = i](const auto &cell) { return cell.count(i); });
    auto j_it = find_if(w.partition.begin(), w.partition.end(),
                        [j = j](const auto &cell) { return cell.count(j); });
    if (i_it == j_it) {
      auto wPrime = w.clone();
      witnessSet.insert(wPrime);
    } else {
      if (i_it == w.partition.end() || j_it == w.partition.end()) {
        cerr << "ERROR: labels were not in the witness introducing an edge "
                "between them.\n";
        exit(20);
      }

      auto wPrime = w.clone();

      wPrime->partition.erase(*i_it);
      wPrime->partition.erase(*j_it);

      set<unsigned> cell_union = *i_it;
      cell_union.insert(j_it->begin(), j_it->end());
      wPrime->partition.insert(cell_union);
      witnessSet.insert(wPrime);
    }
  }

  /**
   * Insert what the witness w becomes
   * after forgetting the label of the vertex
   * currently labeled i into the witnessSet.
   */
  void forget_v_implementation(unsigned int i, Bag &b, const WitnessAlias &w,
                               WitnessSet &witnessSet) {
    auto wPrime = w.clone();

    // 0 = empty graph
    // 1 = nonempty witness, not broken
    // 2 = nonempty connected graph, empty witness (breaks upon intro_v)
    // 3 = broken

    if (auto singularCell = wPrime->partition.find({i});
        singularCell != wPrime->partition.end()) {
      if (wPrime->tag == 1) {
        if (wPrime->partition.size() >= 2)
          wPrime->tag = 3;
        else
          wPrime->tag = 2;
      }

      wPrime->partition.erase(singularCell);

    } else {
      auto it = find_if(wPrime->partition.begin(), wPrime->partition.end(),
                        [i = i](const auto &cell) { return cell.count(i); });
      if (it == wPrime->partition.end()) {
        cerr << w.witnessInformation() << "i=" << i << '\n';
        cerr << "ERROR: label was not in the witness when forgetting it.\n";
        exit(20);
      }

      auto cell = *it;
      cell.erase(i);

      wPrime->partition.erase(it);
      wPrime->partition.insert(move(cell));
    }

    witnessSet.insert(wPrime);
  }

  void join_implementation(Bag &b, const WitnessAlias &w1, const WitnessAlias &w2,
                           WitnessSet &witnessSet) {
    int q1 = w1.tag;
    int q2 = w2.tag;

    if (q1 == 0 && q2 == 0) {
      witnessSet.insert(make_shared<WitnessAlias>());
      return;
    }

    int qPrime = 3;

    if (q1 <= 1 && q2 <= 1)
      qPrime = 1;
    else if ((q1 == 0 && q2 == 2) || (q1 == 2 && q2 == 0))
      qPrime = 2;

    // TODO: change to vector?
    map<unsigned, int> mp;
    auto get_mp = [&](unsigned i) {
      auto [it, placed] = mp.emplace(i, -1);
      return it->second;
    };

    auto set_mp = [&](unsigned i, int v) {
      auto it = mp.find(i);
      it->second = v;
    };

    auto find = [&](int i) {
      int nxt;
      while ((nxt = get_mp(i)) >= 0) i = nxt;
      return make_pair(i, -nxt);
    };

    auto unite = [&](int u, int v) {
      auto [i_root, i_sz] = find(u);
      auto [j_root, j_sz] = find(v);
      if (i_root == j_root) return;
      if (i_sz < j_sz) {
        swap(i_root, j_root);
        swap(i_sz, j_sz);
      }
      set_mp(i_root, -(i_sz + j_sz));
      set_mp(j_root, i_root);
    };

    for (auto w : {&w1, &w2}) {
      for (const auto &cell : w->partition) {
        if (cell.size() >= 2) {
          for (auto it = cell.begin(); next(it) != cell.end(); ++it)
            unite(*it, *next(it));
        } else {
          // make sure lonely vertices are not forgotten
          for (auto i : cell) get_mp(i);
        }
      }
    }

    map<unsigned, set<unsigned>> partition;
    for (auto [v, up] : mp) partition[find(v).first].insert(v);

    auto wPrime = make_shared<WitnessAlias>();
    wPrime->tag = qPrime;
    for (auto &[root, cell] : partition) wPrime->partition.insert(move(cell));
    witnessSet.insert(wPrime);
  }

  void clean_implementation(WitnessSet &witnessSet) {
    // In most cases, you will not need to change this function.
  }

  bool is_final_witness_implementation(const WitnessAlias &w) {
    return w.tag != 3 && w.partition.size() <= 1;
  }
};

extern "C" {
  DynamicCore *create() { return (DynamicCore*) new Connectivity_Core; }
}
