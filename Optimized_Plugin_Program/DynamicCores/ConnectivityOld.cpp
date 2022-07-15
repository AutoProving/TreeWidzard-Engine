// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "ConnectivityOld.h"

//////////////////////Implementation/////////////////////////////
////////////////////////////////////////////////////////////////
bool Connectivity_Witness_Old::is_equal_implementation(
    const Connectivity_Witness_OldPointerConst w) const {
  //*****************************
  //*****************************
  return tie(this->tag, this->partition) == tie(w->tag, w->partition);
  //*****************************
  //*****************************
}

bool Connectivity_Witness_Old::is_less_implementation(
    const Connectivity_Witness_OldPointerConst w) const {
  //*****************************
  //*****************************
  return tie(this->tag, this->partition) < tie(w->tag, w->partition);
  //*****************************
  //*****************************
}

Witness &Connectivity_Witness_Old::set_equal_implementation(
    Connectivity_Witness_OldPointerConst w) {
  //*****************************
  //*****************************
  this->partition = w->partition;
  this->tag = w->tag;
  return *this;
  //*****************************
  //*****************************
}

shared_ptr<Witness> Connectivity_Witness_Old::relabel(
    const map<unsigned int, unsigned int> &relabelingMap) const {
  Connectivity_Witness_OldPointer w(new Connectivity_Witness_Old);
  w->tag = this->tag;

  auto &newPartition = w->partition;
  for (const auto &cell : this->partition) {
    set<unsigned> newCell;
    for (auto v : cell) {
      auto it = relabelingMap.find(v);
      if (it == relabelingMap.end()) {
        cerr << "Error: Connectivity_Witness_Old::relabel " << v
             << " is not in the map\n";
        exit(20);
      }
      newCell.insert(it->second);
    }
    newPartition.insert(newCell);
  }
  return w;
}

void Connectivity_Witness_Old::hash(Hasher &h) const {
  h << tag << -1u;
  for (const auto &component : partition) {
    for (unsigned node : component) h << node;
    h << -1u;
  }
}

void Connectivity_Witness_Old::print() const {
  //*****************************
  //*****************************
  cout << witnessInformation();
  cout.flush();
  //*****************************
  //*****************************
}

string Connectivity_Witness_Old::witnessInformation() const {
  //*****************************
  //*****************************
  ostringstream info;
  info << "Tag=" << int(this->tag) << "\n";
  info << "Partition=\n";
  for (auto part : this->partition) {
    for (auto i : part) info << i << ' ';
    info << '\n';
  }
  return info.str();
  //*****************************
  //*****************************
}

Connectivity_Witness_Old::~Connectivity_Witness_Old() {
  //*****************************
  //*****************************
  // Only edit this part
  // In most cases this function can be empty, if you need a specific destructor
  // you can implement it here.
  //*****************************
  //*****************************
}

Connectivity_DynamicCore_Old::Connectivity_DynamicCore_Old() {
  // Initializing attributes
  addAttribute("CoreName", "ConnectivityOld");
  addAttribute("CoreType", "Bool");
  addAttribute("ParameterType", "ParameterLess");
  addAttribute("PrimaryOperator", "---");
  createInitialWitnessSet();
}

void Connectivity_DynamicCore_Old::createInitialWitnessSet_implementation() {
  Connectivity_Witness_OldPointer witness = createWitness();
  //*****************************
  //*****************************
  //*****************************
  //*****************************
  this->insertIntoInitialWitnessSet(witness);
}

void Connectivity_DynamicCore_Old::intro_v_implementation(
    unsigned int i, Bag &b, Connectivity_Witness_OldPointerConst w,
    Connectivity_Witness_OldSetPointer witnessSet) {
  //*****************************
  //*****************************
  Connectivity_Witness_OldPointer wPrime = createWitness();
  wPrime->set_equal(*w);
  wPrime->tag |= 1;
  wPrime->partition.insert({i});
  witnessSet->insert(wPrime);
  //*****************************
  //*****************************
}

void Connectivity_DynamicCore_Old::intro_e_implementation(
    unsigned int i, unsigned int j, Bag &b, Connectivity_Witness_OldPointerConst w,
    Connectivity_Witness_OldSetPointer witnessSet) {
  //*****************************
  //*****************************
  auto i_it = find_if(w->partition.begin(), w->partition.end(),
                      [i = i](const auto &cell) { return cell.count(i); });
  auto j_it = find_if(w->partition.begin(), w->partition.end(),
                      [j = j](const auto &cell) { return cell.count(j); });
  if (i_it == j_it) {
    Connectivity_Witness_OldPointer wPrime = createWitness();
    wPrime->set_equal(*w);
    witnessSet->insert(wPrime);
  } else {
    if (i_it == w->partition.end() || j_it == w->partition.end()) {
      cerr << "ERROR: labels were not in the witness introducing an edge "
              "between them.\n";
      exit(20);
    }

    Connectivity_Witness_OldPointer wPrime = createWitness();
    wPrime->set_equal(*w);

    wPrime->partition.erase(*i_it);
    wPrime->partition.erase(*j_it);

    set<unsigned> cell_union = *i_it;
    cell_union.insert(j_it->begin(), j_it->end());
    wPrime->partition.insert(cell_union);
    witnessSet->insert(wPrime);
  }
  //*****************************
  //*****************************
}

void Connectivity_DynamicCore_Old::forget_v_implementation(
    unsigned int i, Bag &b, Connectivity_Witness_OldPointerConst w,
    Connectivity_Witness_OldSetPointer witnessSet) {
  //*****************************
  //*****************************
  Connectivity_Witness_OldPointer wPrime = createWitness();
  wPrime->set_equal(*w);

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
      cerr << w->witnessInformation() << "i=" << i << '\n';
      cerr << "ERROR: label was not in the witness when forgetting it.\n";
      exit(20);
    }

    auto cell = *it;
    cell.erase(i);

    wPrime->partition.erase(it);
    wPrime->partition.insert(move(cell));
  }

  witnessSet->insert(wPrime);
  //*****************************
  //*****************************
}

void Connectivity_DynamicCore_Old::join_implementation(
    Bag &b, Connectivity_Witness_OldPointerConst w1,
    Connectivity_Witness_OldPointerConst w2,
    Connectivity_Witness_OldSetPointer witnessSet) {
  //*****************************
  //*****************************
  int q1 = w1->tag;
  int q2 = w2->tag;

  if (q1 == 0 && q2 == 0) {
    witnessSet->insert(createWitness());
    return;
  }

  int qPrime = 3;

  if (q1 <= 1 && q2 <= 1)
    qPrime = 1;
  else if ((q1 == 0 && q2 == 2) || (q1 == 2 && q2 == 0))
    qPrime = 2;

  // TODO change to vector?
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

  for (auto w : {&*w1, &*w2}) {
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

  Connectivity_Witness_OldPointer wPrime = createWitness();
  wPrime->tag = qPrime;
  for (auto &[root, cell] : partition) wPrime->partition.insert(move(cell));
  witnessSet->insert(wPrime);
  //*****************************
  //*****************************
}

WitnessSetPointer Connectivity_DynamicCore_Old::clean(
    WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  // In most cases, you will not need to change this function.
  //*****************************
  //*****************************
  return witnessSet;
}

bool Connectivity_DynamicCore_Old::is_final_witness_implementation(
    Connectivity_Witness_OldPointerConst w) {
  //*****************************
  //*****************************
  return w->tag != 3 && w->partition.size() <= 1;
  //*****************************
  //*****************************
}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

bool Connectivity_Witness_Old::is_equal(const Witness &rhs) const {
  if (Connectivity_Witness_Old const *e =
          dynamic_cast<Connectivity_Witness_Old const *>(&rhs)) {
    Connectivity_Witness_OldPointerConst w = e->shared_from_this();
    return is_equal_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_Witness_Old::is_equal cast error" << endl;
    exit(20);
  }
}

bool Connectivity_Witness_Old::is_less(const Witness &rhs) const {
  if (Connectivity_Witness_Old const *e =
          dynamic_cast<Connectivity_Witness_Old const *>(&rhs)) {
    Connectivity_Witness_OldPointerConst w = e->shared_from_this();
    return is_less_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_Witness_Old::is_less cast error" << endl;
    exit(20);
  }
}

Witness &Connectivity_Witness_Old::set_equal(const Witness &witness) {
  if (const Connectivity_Witness_Old *e =
          dynamic_cast<const Connectivity_Witness_Old *>(&witness)) {
    Connectivity_Witness_OldPointerConst w = e->shared_from_this();
    return set_equal_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_Witness_Old::operator= cast error" << endl;
    exit(20);
  }
}

void Connectivity_DynamicCore_Old::createInitialWitnessSet() {
  Connectivity_Witness_OldSetPointer witnessSet(new Connectivity_Witness_OldSet);
  this->setInitialWitnessSet(witnessSet);
  createInitialWitnessSet_implementation();
}

WitnessSetPointer Connectivity_DynamicCore_Old::intro_v(
    unsigned i, Bag &b, const Witness &witness) {
  if (const Connectivity_Witness_Old *e =
          dynamic_cast<const Connectivity_Witness_Old *>(&witness)) {
    Connectivity_Witness_OldPointerConst w = e->shared_from_this();
    Connectivity_Witness_OldSetPointer witnessSet(new Connectivity_Witness_OldSet);
    intro_v_implementation(i, b, w, witnessSet);
    return clean(witnessSet);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore_Old::intro_v cast error" << endl;
    exit(20);
  }
}

WitnessSetPointer Connectivity_DynamicCore_Old::intro_e(
    unsigned i, unsigned j, Bag &b, const Witness &witness) {
  if (const Connectivity_Witness_Old *e =
          dynamic_cast<const Connectivity_Witness_Old *>(&witness)) {
    Connectivity_Witness_OldPointerConst w = e->shared_from_this();
    Connectivity_Witness_OldSetPointer witnessSet(new Connectivity_Witness_OldSet);
    intro_e_implementation(i, j, b, w, witnessSet);
    return clean(witnessSet);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore_Old::intro_e cast error" << endl;
    exit(20);
  }
}

WitnessSetPointer Connectivity_DynamicCore_Old::forget_v(
    unsigned i, Bag &b, const Witness &witness) {
  if (const Connectivity_Witness_Old *e =
          dynamic_cast<const Connectivity_Witness_Old *>(&witness)) {
    Connectivity_Witness_OldPointerConst w = e->shared_from_this();
    Connectivity_Witness_OldSetPointer witnessSet(new Connectivity_Witness_OldSet);
    forget_v_implementation(i, b, w, witnessSet);
    return clean(witnessSet);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore_Old::forget_v cast error" << endl;
    exit(20);
  }
}

WitnessSetPointer Connectivity_DynamicCore_Old::join(
    Bag &b, const Witness &witness1, const Witness &witness2) {
  if (const Connectivity_Witness_Old *e1 =
          dynamic_cast<const Connectivity_Witness_Old *>(&witness1)) {
    if (const Connectivity_Witness_Old *e2 =
            dynamic_cast<const Connectivity_Witness_Old *>(&witness2)) {
      // Note: Do not merge the two "if" instructions above into a single "if".
      // Potential for errors
      Connectivity_Witness_OldPointerConst w1 = e1->shared_from_this();
      Connectivity_Witness_OldPointerConst w2 = e2->shared_from_this();
      Connectivity_Witness_OldSetPointer witnessSet(new Connectivity_Witness_OldSet);
      join_implementation(b, w1, w2, witnessSet);
      return clean(witnessSet);
    } else {
      cerr << "ERROR: in Connectivity_DynamicCore_Old::join cast error" << endl;
      exit(20);
    }
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore_Old::join cast error" << endl;
    exit(20);
  }
}

bool Connectivity_DynamicCore_Old::is_final_witness(const Witness &witness) {
  if (const Connectivity_Witness_Old *e =
          dynamic_cast<const Connectivity_Witness_Old *>(&witness)) {
    Connectivity_Witness_OldPointerConst w = e->shared_from_this();
    return is_final_witness_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore_Old::is_final_witness cast error"
         << endl;
    exit(20);
  }
}

Connectivity_Witness_OldPointer Connectivity_DynamicCore_Old::createWitness() {
  Connectivity_Witness_OldPointer w(new Connectivity_Witness_Old);
  return w;
}
shared_ptr<WitnessSet> Connectivity_Witness_OldSet::createEmptyWitnessSet() {
  Connectivity_Witness_OldSetPointer witnessSet(new Connectivity_Witness_OldSet);
  return witnessSet;
}

void Connectivity_DynamicCore_Old::copyWitness(
    Connectivity_Witness_OldPointer w_input, Connectivity_Witness_OldPointer w_output) {
  w_output->set_equal_implementation(w_input);
}

extern "C" {
DynamicCore *create() { return new Connectivity_DynamicCore_Old; }
}
