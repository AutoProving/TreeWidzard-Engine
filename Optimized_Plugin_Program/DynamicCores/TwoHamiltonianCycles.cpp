// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "TwoHamiltonianCycles.h"
#include <algorithm>
#include <cassert>


///////////////////////Implementation////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool TwoHamiltonianCycles_Witness::is_equal_implementation(const TwoHamiltonianCycles_WitnessPointerConst w) const{
  //*****************************
  //*****************************
  if(this->info[0] == w->info[0] &&
      this->info[1] == w->info[1] &&
      this->closed[0] == w->closed[0] && 
      this->closed[1] == w->closed[1] && 
      this->was_different == w->was_different){
    return true;
  } else {
    return false;
  }
  //*****************************
  //*****************************
}
bool TwoHamiltonianCycles_Witness::is_less_implementation(const TwoHamiltonianCycles_WitnessPointerConst w) const {
  //*****************************
  //*****************************
  auto this_tuple_ref = std::tie(
      this->info[0],
      this->info[1],
      this->closed[0],
      this->closed[1],
      this->was_different
      );

  auto w_tuple_ref = std::tie(
      w->info[0],
      w->info[1],
      w->closed[0],
      w->closed[1],
      w->was_different
      );

  return this_tuple_ref < w_tuple_ref;
  //*****************************
  //*****************************
}

Witness & TwoHamiltonianCycles_Witness::set_equal_implementation(TwoHamiltonianCycles_WitnessPointer w) {
  //*****************************
  //*****************************
  this->info[0] = w->info[0];
  this->info[1] = w->info[1];
  this->closed[0] = w->closed[0];
  this->closed[1] = w->closed[1];
  this->was_different = w->was_different;
  return *this;
  //*****************************
  //*****************************
}

shared_ptr<Witness> TwoHamiltonianCycles_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
  if (this->info[0].empty() && this->info[1].empty()) {
    return this->shared_from_this();
  }

  TwoHamiltonianCycles_WitnessPointer relabeledWitness(new TwoHamiltonianCycles_Witness);
  relabeledWitness->closed[0] = this->closed[0];
  relabeledWitness->closed[1] = this->closed[1];
  relabeledWitness->was_different = this->was_different;

  for (int v : {0, 1}) {
    for (unsigned i = 0; i < this->info[v].size(); ++i) {
      if (this->info[v][i].degree == -1U) {
        assert(this->info[v][i].degree == -1U);
        continue;
      }

      if (relabelingMap.count(i)){
        unsigned j = relabelingMap[i];
        if (j >= relabeledWitness->info[v].size())
          relabeledWitness->info[v].resize(j+1);
        relabeledWitness->info[v][j] = this->info[v][i];

        unsigned mi = this->info[v][i].match;
        if (mi != -1U) {
          if (relabelingMap.count(mi)){
            unsigned mj = relabelingMap[mi];
            relabeledWitness->info[v][j].match = mj;
          } else {
            cout<<"Error: TwoHamiltonianCycles_Witness::relabel " << mi
              <<" from info["<<v<<"] is not in the map."<<endl;
            exit(20);
          }
        }
      } else {
        cout<<"Error: TwoHamiltonianCycles_Witness::relabel " << i
          <<" from info["<<v<<"] is not in the map."<<endl;
        exit(20);
      }
    }

    while (relabeledWitness->info[v].size() && relabeledWitness->info[v].back().degree -1U)
      relabeledWitness->info[v].pop_back();
  }

  return relabeledWitness;
}

void TwoHamiltonianCycles_Witness::print() {
  //*****************************
  //*****************************
  cout << this->witnessInformation() << flush;
  //*****************************
  //*****************************
}
string TwoHamiltonianCycles_Witness::witnessInformation() {
  //*****************************
  //*****************************
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

      if (i+1 != this->info[version].size())
        out << ',';
    }

    out << "] matching=[";
    for (unsigned i = 0; i < this->info[version].size(); ++i) {
      if (this->info[version][i].match == -1U) {
        out << '-';
      } else {
        out << this->info[version][i].match;
      }

      if (i+1 != this->info[version].size())
        out << ',';
    }

    out << "] closed=" << this->closed[version] << "} ";
  }
  out << "was_different=" << this->was_different << '\n';
  return out.str();
  //*****************************
  //*****************************
}
TwoHamiltonianCycles_Witness::~TwoHamiltonianCycles_Witness() {
  //*****************************
  //*****************************
  // Only edit this part
  // In most cases this function can be empty, if you need a specific destructor you can implement it here.
  //*****************************
  //*****************************
}

TwoHamiltonianCycles_DynamicCore::TwoHamiltonianCycles_DynamicCore(){
  //*****************************
  //*****************************
  // In most cases, you will not to need to change this function.
  //*****************************
  //*****************************
  // Initializing attributes
  addAttribute("CoreName","TwoHamiltonianCycles");
  addAttribute("CoreType","Bool");
  addAttribute("ParameterType","ParameterLess");
  addAttribute("PrimaryOperator","---");
  createInitialWitnessSet();
}

void TwoHamiltonianCycles_DynamicCore::createInitialWitnessSet_implementation() {
  //*****************************
  //*****************************
  TwoHamiltonianCycles_WitnessPointer witness = createWitness();
  this->insertIntoInitialWitnessSet(witness);
  //*****************************
  //*****************************
}
void TwoHamiltonianCycles_DynamicCore::intro_v_implementation(unsigned int i, Bag &,
    TwoHamiltonianCycles_WitnessPointer w, TwoHamiltonianCycles_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  if (w->closed[0] || w->closed[1])
    return; // impossible

  TwoHamiltonianCycles_WitnessPointer w1 = createWitness();
  w1->set_equal(*w);

  for (int v : {0, 1})
    if (i >= w1->info[v].size())
      w1->info[v].resize(i+1);

  //Only accepts a vertex if it's new
  if (w1->info[0][i].degree == -1U && w1->info[1][i].degree == -1U) {
    w1->info[0][i].degree = 0;
    w1->info[1][i].degree = 0;
    witnessSet->insert(w1);
  }
  //*****************************
  //*****************************
}

void TwoHamiltonianCycles_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &,
    TwoHamiltonianCycles_WitnessPointer w,
    TwoHamiltonianCycles_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  auto try_use_edge = [&](TwoHamiltonianCycles_WitnessPointer w1, int v) -> bool {
    // attempt using the edge i,j in version v
    // returns true on success
    assert(w1->info[v][i].degree != -1U);
    assert(w1->info[v][j].degree != -1U);

    // Only introduces an edge if it's open
    if (w1->closed[v])
      return false;

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
      TwoHamiltonianCycles_WitnessPointer w1=createWitness();
      w1->set_equal(*w);

      if (use_in_v0) {
        // attempt using the edge, and skip this case if it was impossible
        if (!try_use_edge(w1, 0))
          continue;
      }

      if (use_in_v1) {
        // attempt using the edge, and skip this case if it was impossible
        if (!try_use_edge(w1, 1))
          continue;
      }

      // remember that two different choices were made
      w1->was_different |= use_in_v0 != use_in_v1;

      witnessSet->insert(w1);
    }
  }
  //*****************************
  //*****************************
}
void TwoHamiltonianCycles_DynamicCore::forget_v_implementation(unsigned int i, Bag &,
    TwoHamiltonianCycles_WitnessPointer w, TwoHamiltonianCycles_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  //Only forget vertices with degree 2
  if (w->info[0][i].degree == 2 && w->info[1][i].degree == 2) {
    TwoHamiltonianCycles_WitnessPointer w1=createWitness();
    w1->set_equal(*w);
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
    witnessSet->insert(w1);
  }
  //*****************************
  //*****************************
}
void TwoHamiltonianCycles_DynamicCore::join_implementation(Bag &, TwoHamiltonianCycles_WitnessPointer w1,
    TwoHamiltonianCycles_WitnessPointer w2,
    TwoHamiltonianCycles_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  TwoHamiltonianCycles_WitnessPointer wPrime = createWitness();
  wPrime->was_different |= w1->was_different;
  wPrime->was_different |= w2->was_different;

  unsigned sz = (unsigned)std::max({
    w1->info[0].size(),  w2->info[0].size(),
    w1->info[1].size(),  w2->info[1].size(),
  });

  auto merge_version = [&](int v) -> bool {
    wPrime->info[v].resize(sz);

    for (unsigned i = 0; i < wPrime->info[v].size(); ++i) {
      unsigned d1 = -1U;
      unsigned d2 = -1U;

      if (i < w1->info[v].size())
        d1 = w1->info[v][i].degree;
      if (i < w2->info[v].size())
        d2 = w2->info[v][i].degree;

      if (d1 == -1U && d2 == -1U)
        continue;

      if (d1 == -1U) d1 = 0;
      if (d2 == -1U) d2 = 0;

      wPrime->info[v][i].degree = d1+d2;

      if (wPrime->info[v][i].degree > 2)
        return false; // impossible
    }

    int cnt_0 = 0, cnt_1 = 0;

    std::vector<unsigned> endpoints;
    std::vector<unsigned> end_vis(sz);
    std::vector<unsigned> in_graph(sz);
    std::vector<unsigned> is_endpoint(sz);
    std::vector<std::vector<unsigned>> g(sz);

    for (unsigned i = 0; i < sz; ++i) {
      if (wPrime->info[v][i].degree == -1U)
        continue;

      cnt_0 += wPrime->info[v][i].degree == 0;
      cnt_1 += wPrime->info[v][i].degree == 1;

      if (wPrime->info[v][i].degree == 1) {
        endpoints.push_back(i);
        is_endpoint[i] = 1;
      }

      if (i < w1->info[v].size()) {
        unsigned j = w1->info[v][i].match;
        if (j != -1U) {
          in_graph[i] = 1;
          assert(w1->info[v][i].degree == 1);
          g[i].push_back(j);
        }
      }
      if (i < w2->info[v].size()) {
        unsigned j = w2->info[v][i].match;
        if (j != -1U) {
          in_graph[i] = 1;
          assert(w2->info[v][i].degree == 1);
          g[i].push_back(j);
        }
      }
    }

    while (endpoints.size()) {
      unsigned start = endpoints.back();
      endpoints.pop_back();
      if (end_vis[start])
        continue;
      if (!in_graph[start])
        continue;

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

    int cycle_count = 0;

    unsigned start = sz-1;
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
      assert(adj.size() == 0); // make sure all edges were used

    if (cycle_count > 1)
      return false;

    if (cycle_count > 0 && (cnt_0 > 0 || cnt_1 > 0))
      return false;

    if (cycle_count == 1)
      wPrime->closed[v] = true;

    return true;
  };

  for (int version : {0, 1})
    if (!merge_version(version))
      return; // impossible

  witnessSet->insert(wPrime);
  //*****************************
  //*****************************
}

TwoHamiltonianCycles_WitnessSetPointer TwoHamiltonianCycles_DynamicCore::clean_implementation(
    TwoHamiltonianCycles_WitnessSetPointer witnessSet) {
  return witnessSet;
}

bool TwoHamiltonianCycles_DynamicCore::is_final_witness_implementation(TwoHamiltonianCycles_WitnessPointer w) {
  //*****************************
  //*****************************
  if (!w->was_different)
    return false;

  for (int version : {0, 1}) {
    if (!w->closed[version])
      return false;

    for (auto [d, j] : w->info[version]) {
      assert(d == -1U || d == 2);
      assert(j == -1U);
    }
  }

  return true;
  //*****************************
  //*****************************
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool TwoHamiltonianCycles_Witness::is_equal(const Witness &rhs) const{
  if (TwoHamiltonianCycles_Witness const *e = dynamic_cast<TwoHamiltonianCycles_Witness const *>(&rhs)) {
    TwoHamiltonianCycles_WitnessPointerConst w = e->shared_from_this();
    return is_equal_implementation(w);
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_Witness::is_equal cast error"<<endl;
    exit(20);
  }
}

bool TwoHamiltonianCycles_Witness::is_less(const Witness &rhs)const {
  if (TwoHamiltonianCycles_Witness const *e = dynamic_cast<TwoHamiltonianCycles_Witness const *>(&rhs)) {
    TwoHamiltonianCycles_WitnessPointerConst w = e->shared_from_this();
    return is_less_implementation(w);
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_Witness::is_less cast error"<<endl;
    exit(20);
  }
}

Witness &TwoHamiltonianCycles_Witness::set_equal(Witness &witness) {
  if (TwoHamiltonianCycles_Witness *e = dynamic_cast<TwoHamiltonianCycles_Witness *>(&witness)) {
    TwoHamiltonianCycles_WitnessPointer w = e->shared_from_this();
    return set_equal_implementation(w);
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_Witness::operator= cast error"<<endl;
    exit(20);
  }
}

shared_ptr<WitnessSet> TwoHamiltonianCycles_WitnessSet::createEmptyWitnessSet() {
  TwoHamiltonianCycles_WitnessSetPointer witnessSet(new TwoHamiltonianCycles_WitnessSet);
  return witnessSet;
}

TwoHamiltonianCycles_WitnessPointer TwoHamiltonianCycles_DynamicCore::createWitness() {
  TwoHamiltonianCycles_WitnessPointer w(new TwoHamiltonianCycles_Witness);
  return w;
}

void TwoHamiltonianCycles_DynamicCore::copyWitness(TwoHamiltonianCycles_WitnessPointer w_input,
    TwoHamiltonianCycles_WitnessPointer w_output) {
  w_output->set_equal_implementation(w_input);
}

void TwoHamiltonianCycles_DynamicCore::createInitialWitnessSet() {
  TwoHamiltonianCycles_WitnessSetPointer witnessSet(new TwoHamiltonianCycles_WitnessSet);
  this->setInitialWitnessSet(witnessSet);
  createInitialWitnessSet_implementation();
}


WitnessSetPointer TwoHamiltonianCycles_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness  ) {
  if (TwoHamiltonianCycles_Witness *e = dynamic_cast<TwoHamiltonianCycles_Witness *>(&witness)){
    TwoHamiltonianCycles_WitnessPointer w = e->shared_from_this();
    TwoHamiltonianCycles_WitnessSetPointer witnessSet (new TwoHamiltonianCycles_WitnessSet);
    intro_v_implementation(i,b,w,witnessSet);
    return clean(witnessSet);
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_DynamicCore::intro_v cast error"<<endl;
    exit(20);
  }
}



WitnessSetPointer TwoHamiltonianCycles_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
  if (TwoHamiltonianCycles_Witness *e = dynamic_cast<TwoHamiltonianCycles_Witness *>(&witness)){
    TwoHamiltonianCycles_WitnessPointer w = e->shared_from_this();
    TwoHamiltonianCycles_WitnessSetPointer witnessSet (new TwoHamiltonianCycles_WitnessSet);
    intro_e_implementation(i,j,b,w,witnessSet);
    return clean(witnessSet);
  } else{
    cerr<<"ERROR: in TwoHamiltonianCycles_DynamicCore::intro_e cast error"<< endl;
    exit(20);
  }
}


WitnessSetPointer TwoHamiltonianCycles_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
  if (TwoHamiltonianCycles_Witness *e = dynamic_cast<TwoHamiltonianCycles_Witness *>(&witness)){
    TwoHamiltonianCycles_WitnessPointer w = e->shared_from_this();
    TwoHamiltonianCycles_WitnessSetPointer witnessSet (new TwoHamiltonianCycles_WitnessSet);
    forget_v_implementation(i,b,w,witnessSet);
    return clean(witnessSet);
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_DynamicCore::forget_v cast error"<<endl;
    exit(20);
  }
}


WitnessSetPointer TwoHamiltonianCycles_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
  if(TwoHamiltonianCycles_Witness *e1 = dynamic_cast<TwoHamiltonianCycles_Witness *>(&witness1)){
    if(TwoHamiltonianCycles_Witness *e2 = dynamic_cast<TwoHamiltonianCycles_Witness *>(&witness2)){
      // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
      TwoHamiltonianCycles_WitnessPointer w1 = e1->shared_from_this();
      TwoHamiltonianCycles_WitnessPointer w2 = e2->shared_from_this();
      TwoHamiltonianCycles_WitnessSetPointer witnessSet (new TwoHamiltonianCycles_WitnessSet);
      join_implementation(b,w1,w2,witnessSet);
      return clean(witnessSet);
    }else{
      cerr<<"ERROR: in TwoHamiltonianCycles_DynamicCore::join cast error"<<endl;
      exit(20);
    }
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_DynamicCore::join cast error"<<endl;
    exit(20);
  }
}

WitnessSetPointer TwoHamiltonianCycles_DynamicCore::clean(WitnessSetPointer witnessSet) {
  if (TwoHamiltonianCycles_WitnessSetPointer e = dynamic_pointer_cast<TwoHamiltonianCycles_WitnessSet >(witnessSet)) {
    return clean_implementation(e);
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_DynamicCore::clean cast error"<<endl;
    exit(20);
  }
}

bool TwoHamiltonianCycles_DynamicCore::is_final_witness(Witness &witness) {
  if (TwoHamiltonianCycles_Witness *e = dynamic_cast<TwoHamiltonianCycles_Witness *>(&witness)) {
    TwoHamiltonianCycles_WitnessPointer w = e->shared_from_this();
    return is_final_witness_implementation(w);
  }else{
    cerr<<"ERROR: in TwoHamiltonianCycles_DynamicCore::is_final_witness cast error"<<endl;
    exit(20);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
  DynamicCore * create() {
    return new TwoHamiltonianCycles_DynamicCore;
  }
}
