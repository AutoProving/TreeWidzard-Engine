// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Connectivity.h"

//////////////////////Implementation/////////////////////////////
////////////////////////////////////////////////////////////////
bool Connectivity_Witness::is_equal_implementation(
    const Connectivity_WitnessPointerConst w) const {
  //*****************************
  //*****************************
  return tie(this->tag, this->partition) == tie(w->tag, w->partition);
  //*****************************
  //*****************************
}

bool Connectivity_Witness::is_less_implementation(
    const Connectivity_WitnessPointerConst w) const {
  //*****************************
  //*****************************
  return tie(this->tag, this->partition) < tie(w->tag, w->partition);
  //*****************************
  //*****************************
}

Witness &Connectivity_Witness::set_equal_implementation(
    Connectivity_WitnessPointerConst w) {
  //*****************************
  //*****************************
  this->partition = w->partition;
  this->tag = w->tag;
  return *this;
  //*****************************
  //*****************************
}

shared_ptr<Witness> Connectivity_Witness::relabel(
    map<unsigned int, unsigned int> relabelingMap) const {
  Connectivity_WitnessPointer w(new Connectivity_Witness);
  w->tag = this->tag;

  auto &newPartition = w->partition;
  for (const auto &cell : this->partition) {
    set<unsigned> newCell;
    for (auto v : cell) {
      auto it = relabelingMap.find(v);
      if (it == relabelingMap.end()) {
        cerr << "Error: Connectivity_Witness::relabel " << v
             << " is not in the map\n";
        exit(20);
      }
      newCell.insert(it->second);
    }
    newPartition.insert(newCell);
  }
  return w;
}

void Connectivity_Witness::hash(Hasher &h) const {
  h << tag << -1u;
  for (const auto &component : partition) {
    for (unsigned node : component)
      h << node;
    h << -1u;
  }
}

void Connectivity_Witness::print() const {
  //*****************************
  //*****************************
  cout << witnessInformation();
  cout.flush();
  //*****************************
  //*****************************
}

string Connectivity_Witness::witnessInformation() const {
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

Connectivity_Witness::~Connectivity_Witness() {
  //*****************************
  //*****************************
  // Only edit this part
  // In most cases this function can be empty, if you need a specific destructor
  // you can implement it here.
  //*****************************
  //*****************************
}

Connectivity_DynamicCore::Connectivity_DynamicCore() {
  // Initializing attributes
  addAttribute("CoreName", "Connectivity");
  addAttribute("CoreType", "Bool");
  addAttribute("ParameterType", "ParameterLess");
  addAttribute("PrimaryOperator", "---");
  createInitialWitnessSet();
}

void Connectivity_DynamicCore::createInitialWitnessSet_implementation() {
  Connectivity_WitnessPointer witness = createWitness();
  //*****************************
  //*****************************
  //*****************************
  //*****************************
  this->insertIntoInitialWitnessSet(witness);
}

void Connectivity_DynamicCore::intro_v_implementation(
    unsigned int i, Bag &b, Connectivity_WitnessPointerConst w,
    Connectivity_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  Connectivity_WitnessPointer wPrime = createWitness();
  wPrime->set_equal(*w);
  wPrime->tag |= 1;
  wPrime->partition.insert({i});
  witnessSet->insert(wPrime);
  //*****************************
  //*****************************
}

void Connectivity_DynamicCore::intro_e_implementation(
    unsigned int i, unsigned int j, Bag &b, Connectivity_WitnessPointerConst w,
    Connectivity_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  auto i_it = find_if(w->partition.begin(), w->partition.end(),
                      [i=i](const auto &cell) { return cell.count(i); });
  auto j_it = find_if(w->partition.begin(), w->partition.end(),
                      [j=j](const auto &cell) { return cell.count(j); });
  if (i_it == j_it) {
    Connectivity_WitnessPointer wPrime = createWitness();
    wPrime->set_equal(*w);
    witnessSet->insert(wPrime);
  } else {
    if (i_it == w->partition.end() || j_it == w->partition.end()) {
      cerr << "ERROR: labels were not in the witness introducing an edge "
              "between them.\n";
      exit(20);
    }

    Connectivity_WitnessPointer wPrime = createWitness();
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

void Connectivity_DynamicCore::forget_v_implementation(
    unsigned int i, Bag &b, Connectivity_WitnessPointerConst w,
    Connectivity_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  Connectivity_WitnessPointer wPrime = createWitness();
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

void Connectivity_DynamicCore::join_implementation(
    Bag &b, Connectivity_WitnessPointerConst w1, Connectivity_WitnessPointerConst w2,
    Connectivity_WitnessSetPointer witnessSet) {
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
    while ((nxt = get_mp(i)) >= 0)
      i = nxt;
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
        for (auto i : cell)
          get_mp(i);
      }
    }
  }

  map<unsigned, set<unsigned>> partition;
  for (auto [v, up] : mp)
    partition[find(v).first].insert(v);

  Connectivity_WitnessPointer wPrime = createWitness();
  wPrime->tag = qPrime;
  for (auto &[root, cell] : partition) wPrime->partition.insert(move(cell));
  witnessSet->insert(wPrime);
  //*****************************
  //*****************************
}

WitnessSetPointer Connectivity_DynamicCore::clean(
    WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  // In most cases, you will not need to change this function.
  //*****************************
  //*****************************
  return witnessSet;
}

bool Connectivity_DynamicCore::is_final_witness_implementation(
    Connectivity_WitnessPointerConst w) {
  //*****************************
  //*****************************
  return w->tag != 3;
  //*****************************
  //*****************************
}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

bool Connectivity_Witness::is_equal(const Witness &rhs) const {
  if (Connectivity_Witness const *e =
          dynamic_cast<Connectivity_Witness const *>(&rhs)) {
    Connectivity_WitnessPointerConst w = e->shared_from_this();
    return is_equal_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_Witness::is_equal cast error" << endl;
    exit(20);
  }
}

bool Connectivity_Witness::is_less(const Witness &rhs) const {
  if (Connectivity_Witness const *e =
          dynamic_cast<Connectivity_Witness const *>(&rhs)) {
    Connectivity_WitnessPointerConst w = e->shared_from_this();
    return is_less_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_Witness::is_less cast error" << endl;
    exit(20);
  }
}

Witness &Connectivity_Witness::set_equal(const Witness &witness) {
  if (const Connectivity_Witness *e =
          dynamic_cast<const Connectivity_Witness *>(&witness)) {
    Connectivity_WitnessPointerConst w = e->shared_from_this();
    return set_equal_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_Witness::operator= cast error" << endl;
    exit(20);
  }
}

void Connectivity_DynamicCore::createInitialWitnessSet() {
  Connectivity_WitnessSetPointer witnessSet(new Connectivity_WitnessSet);
  this->setInitialWitnessSet(witnessSet);
  createInitialWitnessSet_implementation();
}

WitnessSetPointer Connectivity_DynamicCore::intro_v(unsigned i, Bag &b,
                                                    const Witness &witness) {
  if (const Connectivity_Witness *e =
          dynamic_cast<const Connectivity_Witness *>(&witness)) {
    Connectivity_WitnessPointerConst w = e->shared_from_this();
    Connectivity_WitnessSetPointer witnessSet(new Connectivity_WitnessSet);
    intro_v_implementation(i, b, w, witnessSet);
    return clean(witnessSet);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore::intro_v cast error" << endl;
    exit(20);
  }
}

WitnessSetPointer Connectivity_DynamicCore::intro_e(unsigned i, unsigned j,
                                                    Bag &b, const Witness &witness) {
  if (const Connectivity_Witness *e =
          dynamic_cast<const Connectivity_Witness *>(&witness)) {
    Connectivity_WitnessPointerConst w = e->shared_from_this();
    Connectivity_WitnessSetPointer witnessSet(new Connectivity_WitnessSet);
    intro_e_implementation(i, j, b, w, witnessSet);
    return clean(witnessSet);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore::intro_e cast error" << endl;
    exit(20);
  }
}

WitnessSetPointer Connectivity_DynamicCore::forget_v(unsigned i, Bag &b,
                                                     const Witness &witness) {
  if (const Connectivity_Witness *e =
          dynamic_cast<const Connectivity_Witness *>(&witness)) {
    Connectivity_WitnessPointerConst w = e->shared_from_this();
    Connectivity_WitnessSetPointer witnessSet(new Connectivity_WitnessSet);
    forget_v_implementation(i, b, w, witnessSet);
    return clean(witnessSet);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore::forget_v cast error" << endl;
    exit(20);
  }
}

WitnessSetPointer Connectivity_DynamicCore::join(Bag &b, const Witness &witness1,
                                                 const Witness &witness2) {
  if (const Connectivity_Witness *e1 =
          dynamic_cast<const Connectivity_Witness *>(&witness1)) {
    if (const Connectivity_Witness *e2 =
            dynamic_cast<const Connectivity_Witness *>(&witness2)) {
      // Note: Do not merge the two "if" instructions above into a single "if".
      // Potential for errors
      Connectivity_WitnessPointerConst w1 = e1->shared_from_this();
      Connectivity_WitnessPointerConst w2 = e2->shared_from_this();
      Connectivity_WitnessSetPointer witnessSet(new Connectivity_WitnessSet);
      join_implementation(b, w1, w2, witnessSet);
      return clean(witnessSet);
    } else {
      cerr << "ERROR: in Connectivity_DynamicCore::join cast error" << endl;
      exit(20);
    }
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore::join cast error" << endl;
    exit(20);
  }
}

bool Connectivity_DynamicCore::is_final_witness(const Witness &witness) {
  if (const Connectivity_Witness *e =
          dynamic_cast<const Connectivity_Witness *>(&witness)) {
    Connectivity_WitnessPointerConst w = e->shared_from_this();
    return is_final_witness_implementation(w);
  } else {
    cerr << "ERROR: in Connectivity_DynamicCore::is_final_witness cast error"
         << endl;
    exit(20);
  }
}

Connectivity_WitnessPointer Connectivity_DynamicCore::createWitness() {
  Connectivity_WitnessPointer w(new Connectivity_Witness);
  return w;
}
shared_ptr<WitnessSet> Connectivity_WitnessSet::createEmptyWitnessSet() {
  Connectivity_WitnessSetPointer witnessSet(new Connectivity_WitnessSet);
  return witnessSet;
}

void Connectivity_DynamicCore::copyWitness(
    Connectivity_WitnessPointer w_input, Connectivity_WitnessPointer w_output) {
  w_output->set_equal_implementation(w_input);
}

#include "Connectivity.h"
extern "C" {
DynamicCore *create() { return new Connectivity_DynamicCore; }
}
