// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include <bits/stdc++.h>

#include "../CorePrelude.h"

using namespace std;

struct WitnessNameHere : WitnessWrapper<WitnessNameHere> {
  //////
  #error TODO: Specify variables held by every witness here
  //////

  /**
   * Check if the witnesses l and r are equal.
   */
  friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
    #error TODO: implement is_equal here
  }

  /**
   * Check if the witness l is "less" than r.
   * Less here can mean anything
   * as long as it's a strict total order.
   */
  friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
    #error TODO: implement is_less here
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
    #warning TODO: implement relabel here
    cerr << "relabel() is not implemented for this witness.\n";
    exit(20);
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
    #warning TODO: implement hash here
    cerr << "hash() is not implemented for this witness.\n";
    exit(20);
  }

  /**
   * Get a human readable string
   * representing the witness.
   */
  string witnessInformation() const override {
    #error TODO: implement witnessInformation here
  }

};

struct CoreNameHere : CoreWrapper<CoreNameHere, WitnessNameHere, WitnessSetTypeTwo> {

  #error TODO: Set the core properties here
  static constexpr char CoreName[] = "CoreNameHere";
  static constexpr char CoreType[] = "Bool";
  static constexpr char ParameterType[] = "ParameterLess";
  static constexpr char PrimaryOperator[] = "---";

  /**
   * Initialize the witnessset corresponding to leaf
   * nodes in the decomposition.
   */
  void initialize_leaf(WitnessSet &witnessSet) {
    #error TODO: implement initialize_leaf here

    // a usual implementation is to just insert
    // a default constructed witness, which looks like this:
    //witnessSet.insert(make_shared<WitnessAlias>());
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
    #error TODO: implement intro_v here
  }

  /**
   * Insert what the witness w becomes
   * after inserting a new edge between i and j,
   * into the witnessSet.
   */
  void intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                              const WitnessAlias &w, WitnessSet &witnessSet) {
    #error TODO: implement intro_e here
  }

  /**
   * Insert what the witness w becomes
   * after forgetting the label of the vertex
   * currently labeled i into the witnessSet.
   */
  void forget_v_implementation(unsigned int i, Bag &b, const WitnessAlias &w,
                               WitnessSet &witnessSet) {
    #error TODO: implement forget_v here
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
    #warning implement join here
    cerr << "join() is not implemented for this witness.\n";
    exit(20);
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
    #error TODO: implement is_final_witness here
  }

  int weight_implementation(const WitnessAlias &w) {
    #warning implement weight here
    return 0;
  }
};

extern "C" {
  DynamicCore *create() { return new CoreNameHere(); }
  //DynamicCore *create_int(unsigned param) { return new CoreNameHere(param); }
  //DynamicCore *create_multiGraph(MultiGraph multigraph) { return new CoreNameHere(multigraph); }
  //DynamicCore *create_parameters(vector<int> parameters) { return new CoreNameHere(parameters); }
}
