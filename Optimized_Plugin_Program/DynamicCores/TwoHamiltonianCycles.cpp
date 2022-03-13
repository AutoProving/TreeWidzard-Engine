// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "TwoHamiltonianCycles.h"
#include <algorithm>
#include <cassert>


///////////////////////Implementation////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool TwoHamiltonianCycles_Witness::is_equal_implementation(const TwoHamiltonianCycles_WitnessPointerConst w) const{
  //*****************************
  //*****************************
  if(this->degree[0] == w->degree[0] &&
      this->degree[1] == w->degree[1] &&
      this->matching[0] == w->matching[0] &&
      this->matching[1] == w->matching[1] &&
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
      this->degree[0],
      this->degree[1],
      this->matching[0],
      this->matching[1],
      this->closed[0],
      this->closed[1],
      this->was_different
      );

  auto w_tuple_ref = std::tie(
      w->degree[0],
      w->degree[1],
      w->matching[0],
      w->matching[1],
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
  this->degree[0] = w->degree[0];
  this->degree[1] = w->degree[1];
  this->matching[0] = w->matching[0];
  this->matching[1] = w->matching[1];
  this->closed[0] = w->closed[0];
  this->closed[1] = w->closed[1];
  this->was_different = w->was_different;
  return *this;
  //*****************************
  //*****************************
}

shared_ptr<Witness> TwoHamiltonianCycles_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
  auto check_empty = [](const auto &v) {
    return std::all_of(v.begin(), v.end(), [](const auto &ele) { return ele == -1U; });
  };

  if (check_empty(this->degree[0]) && check_empty(this->degree[1]) &&
      check_empty(this->matching[0]) && check_empty(this->matching[1])
      && closed[0] && closed[1]) {
    return this->shared_from_this();
  }

  TwoHamiltonianCycles_WitnessPointer relabeledWitness(new TwoHamiltonianCycles_Witness);
  relabeledWitness->closed[0] = this->closed[0];
  relabeledWitness->closed[1] = this->closed[1];
  relabeledWitness->was_different = this->was_different;

  // Relabeling degrees
  for (int version : {0, 1}) {
    for (unsigned i = 0; i < this->degree[version].size(); ++i) {
      if (this->degree[version][i] == -1U)
        continue;

      if (relabelingMap.count(i)){
        unsigned j = relabelingMap[i];
        if (j >= relabeledWitness->degree[version].size())
          degree[version].resize(j+1, -1U);
        relabeledWitness->degree[version][j] = this->degree[version][i];
      } else {
        cout<<"Error: TwoHamiltonianCycles_Witness::relabel " << i
          <<" from degree["<<version<<"] is not in the map."<<endl;
        exit(20);
      }
    }
  }

  // Relabeling matching
  for (int version : {0, 1}) {
    for (unsigned i0 = 0; i0 < this->matching[version].size(); ++i0) {
      unsigned i1 = this->matching[version][i0];
      if (i1 == -1U)
        continue;

      if (relabelingMap.count(i0) && relabelingMap.count(i1)){
        unsigned j0 = relabelingMap[i0];
        unsigned j1 = relabelingMap[i1];
        relabeledWitness->matching[version][j0] = j1;
      } else {
        cout<<"Error: TwoHamiltonianCycles_Witness::relabel "<<i0<<" or "<<i1
            <<" from matching is not in the map."<<endl;
        exit(20);
      }
    }
  }
  return relabeledWitness;
}

void TwoHamiltonianCycles_Witness::print() {
  //*****************************
  //*****************************
  for (int version : {0, 1}) {
    cout << "V" << version << "{";

    cout << "degree=[";
    for (unsigned i = 0; i < this->degree[version].size(); ++i) {
      if (this->degree[version][i] == -1U) {
        cout << '-';
      } else {
        cout << this->degree[version][i];
      }

      if (i+1 != this->degree[version].size())
        cout << ',';
    }

    cout << "] matching=[";
    for (unsigned i = 0; i < this->matching[version].size(); ++i) {
      if (this->matching[version][i] == -1U) {
        cout << '-';
      } else {
        cout << this->matching[version][i];
      }

      if (i+1 != this->matching[version].size())
        cout << ',';
    }

    cout << "closed=" << this->closed << "} ";
  }
  cout << "was_different=" << this->was_different << endl;
  //*****************************
  //*****************************
}
string TwoHamiltonianCycles_Witness::witnessInformation() {
  //*****************************
  //*****************************
  stringstream info;
  for (int version : {0, 1}) {
    info << "V" << version << "{";

    info << "degree=[";
    for (unsigned i = 0; i < this->degree[version].size(); ++i) {
      if (this->degree[version][i] == -1U) {
        info << '-';
      } else {
        info << this->degree[version][i];
      }

      if (i+1 != this->degree[version].size())
        info << ',';
    }

    info << "] matching=[";
    for (unsigned i = 0; i < this->matching[version].size(); ++i) {
      if (this->matching[version][i] == -1U) {
        info << '-';
      } else {
        info << this->matching[version][i];
      }

      if (i+1 != this->matching[version].size())
        info << ',';
    }

    info << "closed=" << this->closed << "} ";
  }
  info << "was_different=" << this->was_different << endl;
  return info.str();
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
  TwoHamiltonianCycles_WitnessPointer w1 = createWitness();
  w1->set_equal(*w);

  for (int version : {0, 1}) {
    if (i >= w1->degree[version].size())
      w1->degree[version].resize(i+1, -1U);

    if (i >= w1->matching[version].size())
      w1->matching[version].resize(i+1, -1U);
  }

  //Only accepts a vertex if it's new and the witness is open
  if (w1->degree[0][i] == -1U && w1->degree[1][i] == -1U && !w1->closed[0] && !w1->closed[1]) {
    w1->degree[0][i] = 0;
    w1->degree[1][i] = 0;
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

    // Only introduces an edge if it's open
    if (w1->closed[v])
      return false;

    if (w1->degree[v][i] == 0 && w1->degree[v][j] == 0) {
      w1->degree[v][i] = 1;
      w1->degree[v][j] = 1;
      w1->matching[v][i] = j;
      w1->matching[v][j] = i;
      return true;
    }

    if (w1->degree[v][i] == 0 && w1->degree[v][j] == 1) {
      w1->degree[v][i] = 1;
      w1->degree[v][j] = 2;
      unsigned m = w1->matching[v][j];
      w1->matching[v][i] = m;
      w1->matching[v][m] = i;
      w1->matching[v][j] = -1U;
      return true;
    }

    if (w1->degree[v][i] == 1 && w1->degree[v][j] == 0) {
      w1->degree[v][i] = 2;
      w1->degree[v][j] = 1;
      unsigned m = w1->matching[v][i];
      w1->matching[v][j] = m;
      w1->matching[v][m] = j;
      w1->matching[v][i] = -1U;
      return true;
    }

    if (w1->degree[v][i] == 1 && w1->degree[v][j] == 1) {

      if (w1->matching[v][i] != j) {
        w1->degree[v][i] = 2;
        w1->degree[v][j] = 2;

        unsigned mi = w1->matching[v][i];
        unsigned mj = w1->matching[v][j];

        w1->matching[v][i] = -1U;
        w1->matching[v][j] = -1U;
        w1->matching[v][mi] = mj;
        w1->matching[v][mj] = mi;
        return true;
      }

      // i is matched to j

      // does the path include every vertex?
      unsigned cnt_0 = 0, cnt_1 = 0;
      for (unsigned k = 0; k < w1->degree[v].size(); ++k) {
        cnt_0 += w1->degree[v][k] == 0;
        cnt_1 += w1->degree[v][k] == 1;
      }

      if (cnt_0 == 0 && cnt_1 == 2) {
        // complete cycle
        w1->degree[v][i] = 2;
        w1->degree[v][j] = 2;
        w1->matching[v][i] = -1U;
        w1->matching[v][j] = -1U;
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
  if (w->degree[0][i] == 2 && w->degree[1][i] == 2) {
    TwoHamiltonianCycles_WitnessPointer w1=createWitness();
    w1->set_equal(*w);
    w1->degree[0][i] = -1U;
    w1->degree[1][i] = -1U;
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
  TwoHamiltonianCycles_WitnessPointer wPrime1 = createWitness();
  wPrime1->set_equal(*w1); // Working Witness
  wPrime1->was_different |= w2->was_different;

  auto merge_version = [&](int v) -> bool {
    if (wPrime1->degree[v].size() < w2->degree[v].size())
      wPrime1->degree[v].resize(w2->degree[v].size(), -1U);

    wPrime1->matching[v].assign(
      std::max(w1->matching[v].size(), w2->matching[v].size()),
      -1U
    );

    for (unsigned i = 0; i < w2->degree[v].size(); ++i) {
      if (w2->degree[v][i] == -1U)
        continue;

      if (wPrime1->degree[v][i] == -1U)
        wPrime1->degree[v][i] = 0;

      wPrime1->degree[v][i] += w2->degree[v][i];

      if (wPrime1->degree[v][i] > 2)
        return false; // impossible
    }

    int cnt_0 = 0, cnt_1 = 0;

    std::vector<unsigned> endpoints;
    std::vector<unsigned> vis(wPrime1->degree[v].size());

    for (unsigned i = 0; i < wPrime1->degree[v].size(); ++i) {
      cnt_0 += wPrime1->degree[v][i] == 0;
      cnt_1 += wPrime1->degree[v][i] == 1;

      if (wPrime1->degree[v][i] == 1)
        endpoints.push_back(i);
    }

    while (endpoints.size()) {
      unsigned start = endpoints.back();
      endpoints.pop_back();
      if (vis[start])
        continue;

      unsigned current = start;
      bool found = true;
      while (found) {
        vis[current] = 1;
        unsigned w1j = w1->matching[v][current];
        unsigned w2j = w2->matching[v][current];
        found = false;
        for (unsigned j : {w1j, w2j}) {
          if (j == -1U) continue;
          if (vis[j]) continue;

          current = j;
          found = true;
          break;
        }
      }

      assert(start != current);
      assert(wPrime1->degree[v][start] == 1);
      assert(wPrime1->degree[v][current] == 1);

      wPrime1->matching[v][start] = current;
      wPrime1->matching[v][current] = start;
    }

    int cycle_count = 0;

    unsigned last_unvis = (unsigned)vis.size() - 1;
    while (last_unvis != -1U) {
      if (vis[last_unvis]) {
        --last_unvis;
        continue;
      }

      ++cycle_count;

      unsigned current = last_unvis;
      bool found = true;
      while (found) {
        vis[current] = 1;
        unsigned w1j = w1->matching[v][current];
        unsigned w2j = w2->matching[v][current];
        found = false;
        for (unsigned j : {w1j, w2j}) {
          if (j == -1U) continue;
          if (vis[j]) continue;

          current = j;
          found = true;
          break;
        }
      }
    }

    if (cycle_count > 1)
      return false;

    if (cycle_count > 0 && (cnt_0 > 0 || cnt_1 > 0))
      return false;

    return true;
  };

  for (int version : {0, 1})
    if (!merge_version(version))
      return; // impossible

  witnessSet->insert(wPrime1);
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

    for (unsigned d : w->degree[version])
      if (d == 0 || d == 1)
        return false;

    for (unsigned j : w->matching[version])
      if (j != -1U)
        return false;
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
