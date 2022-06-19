// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "HamiltonianCycle.h"


///////////////////////Implementation////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool HamiltonianCycle_Witness::is_equal_implementation(const HamiltonianCycle_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if(this->degree_0 == w->degree_0 &&
       this->degree_1 == w->degree_1 &&
       this->degree_2 == w->degree_2 &&
       this->matching == w->matching &&
       this->closed == w->closed
            ){
        return true;
    }else{
        return false;
    }
    //*****************************
    //*****************************
}
bool HamiltonianCycle_Witness::is_less_implementation(const HamiltonianCycle_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if(this->degree_0 == w->degree_0){
        if(this->degree_1 == w->degree_1){
            if(this->degree_2 == w->degree_2){
                if(this->matching == w->matching){
                    return this->closed < w->closed;
                }else return this->matching < w->matching;
            }else return this->degree_2 < w->degree_2;
        }else return this->degree_1 < w->degree_1;
    }else return this->degree_0 < w->degree_0;
    //*****************************
    //*****************************
}

Witness & HamiltonianCycle_Witness::set_equal_implementation(HamiltonianCycle_WitnessPointerConst w) {
    //*****************************
    //*****************************
    this->degree_0 = w->degree_0;
    this->degree_1 = w->degree_1;
    this->degree_2 = w->degree_2;
    this->matching = w->matching;
    this->closed = w->closed;
    return *this;
    //*****************************
    //*****************************
}

shared_ptr<Witness> HamiltonianCycle_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) const {
    if(this->degree_0.empty() and this->degree_1.empty() and this->degree_2.empty()
            and this->matching.empty() and this->closed){
        return make_shared<Witness>();
    }else{
        HamiltonianCycle_WitnessPointer relabeledWitness(new HamiltonianCycle_Witness);
        relabeledWitness->closed = this->closed;
        // Relabeling degree_0
        for(auto d:this->degree_0){
            if(relabelingMap.count(d)){
                relabeledWitness->degree_0.insert(relabelingMap[d]);
            }else{
                cout<<"Error: HamiltonianCycle_Witness::relabel "<< d<<" from degree_0 is not in the map."<<endl;
                exit(20);
            }
        }
        // Relabeling degree_1
        for(auto d:this->degree_1){
            if(relabelingMap.count(d)){
                relabeledWitness->degree_1.insert(relabelingMap[d]);
            }else{
                cout<<"Error: HamiltonianCycle_Witness::relabel "<< d<<" from degree_1 is not in the map."<<endl;
                exit(20);
            }
        }
        // Relabeling degree_2
        for(auto d:this->degree_2){
            if(relabelingMap.count(d)){
                relabeledWitness->degree_2.insert(relabelingMap[d]);
            }else{
                cout<<"Error: HamiltonianCycle_Witness::relabel "<< d<<" from degree_2 is not in the map."<<endl;
                exit(20);
            }
        }
        // Relabeling matching
        for(auto m:matching){
                if(relabelingMap.count(m.first) and relabelingMap.count(m.second)){
                    relabeledWitness->matching.insert(make_pair(relabelingMap[m.first],relabelingMap[m.second]));
                }else{
                    cout<<"Error: HamiltonianCycle_Witness::relabel "<< m.first << " or "<< m.second<<" from matching is not in the map."<<endl;
                    exit(20);
                }
        }
        return relabeledWitness;
    }
}

void HamiltonianCycle_Witness::print() const {
    //*****************************
    //*****************************
    cout << "Vertices of degree 0:" << endl << "\t";
    for(auto &v: this->degree_0) cout << v << " ";
    cout << endl;

    cout << "Vertices of degree 1:" << endl << "\t";
    for(auto &v: this->degree_1) cout << v << " ";
    cout << endl;

    cout << "Vertices of degree 2:" << endl << "\t";
    for(auto &v: this->degree_2) cout << v << " ";
    cout << endl;

    cout << "Matching:" << endl << "\t";
    for(auto &v: this->matching) cout << "(" << v.first << ", " << v.second << ") ";
    cout << endl;

    cout << "Closed: " << this->closed << endl;
    //*****************************
    //*****************************
}
string HamiltonianCycle_Witness::witnessInformation() const {
    //*****************************
    //*****************************
    string info="";
    info =  info +"Vertices of degree 0:" + "  \t";
    for(auto &v: this->degree_0) info = info + to_string(v) + " ";
    info = info + "\n";

    info =  info +  "Vertices of degree 1:" + "\t";
    for(auto &v: this->degree_1) info = info + to_string(v) + " ";
    info = info + "\n";

    info =  info + "Vertices of degree 2:" + " \t";
    for(auto &v: this->degree_2) info = info + to_string(v) + " ";
    info = info + "\n";

    info = info + "Matching:" + "\t";
    for(auto &v: this->matching) info = info + "(" + to_string(v.first) + ", " + to_string(v.second) + ") ";
    info = info + "\n";

    info = info +  "Closed: " + to_string(this->closed)+"\n";
    return info;
    //*****************************
    //*****************************
}

void HamiltonianCycle_Witness::hash(Hasher &h) const {
    h << closed;
    for (unsigned i : degree_0)
        h << i;
    h << -1u;
    for (unsigned i : degree_1)
        h << i;
    h << -1u;
    for (unsigned i : degree_2)
        h << i;
    h << -1u;
    for (auto [i, j] : matching)
        h << i << j;
}

HamiltonianCycle_Witness::~HamiltonianCycle_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

HamiltonianCycle_DynamicCore::HamiltonianCycle_DynamicCore(){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","HamiltonianCycle");
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","ParameterLess");
    addAttribute("PrimaryOperator","---");
    createInitialWitnessSet();
}

void HamiltonianCycle_DynamicCore::createInitialWitnessSet_implementation() {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
    //*****************************
    //*****************************
}
void HamiltonianCycle_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, HamiltonianCycle_WitnessPointerConst w,
                                                          HamiltonianCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    //Only accepts a vertex if it's new and the witness is open
    if(w1->degree_0.count(i) == 0 && w1->degree_1.count(i) == 0 && w1->degree_2.count(i) == 0 && !w1->closed){
        w1->degree_0.insert(i);
        witnessSet->insert(w1);
    }
    //*****************************
    //*****************************
}

void HamiltonianCycle_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                          HamiltonianCycle_WitnessPointerConst w,
                                                          HamiltonianCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer w0 = createWitness();
    w0->set_equal(*w);
    //New witness without this edge
    witnessSet->insert(w0);

    //Only introduces an edge if it's open
    if(w0->closed){
        // nothing
    }else if(w->degree_0.count(i) && w->degree_0.count(j)){
        //Both vertices with degree 0
        HamiltonianCycle_WitnessPointer w1=createWitness();
        w1->set_equal(*w);
        w1->degree_0.erase(i);
        w1->degree_0.erase(j);
        w1->degree_1.insert(i);
        w1->degree_1.insert(j);
        w1->matching[i] = j;
        w1->matching[j] = i;
        witnessSet->insert(w1);
    }else if(w->degree_0.count(i) && w->degree_1.count(j)){
        //Vertex i with degree 0 and j degree 1
        HamiltonianCycle_WitnessPointer w1=createWitness();
        w1->set_equal(*w);
        w1->degree_0.erase(i);
        w1->degree_1.erase(j);
        w1->degree_1.insert(i);
        w1->degree_2.insert(j);
        //Changing the matching
        unsigned  m = w1->matching[j];
        w1->matching.erase(j);
        w1->matching[m] = i;
        w1->matching[i] = m;
        witnessSet->insert(w1);
    }else if(w->degree_1.count(i) && w->degree_0.count(j)){
        //Vertex j with degree 0 and i degree 1
        HamiltonianCycle_WitnessPointer w1=createWitness();
        w1->set_equal(*w);
        w1->degree_1.erase(i);
        w1->degree_0.erase(j);
        w1->degree_2.insert(i);
        w1->degree_1.insert(j);
        //Changing the matching
        unsigned  m = w1->matching[i];
        w1->matching.erase(i);
        w1->matching[m] = j;
        w1->matching[j] = m;
        witnessSet->insert(w1);
    }else if(w->degree_1.count(i) && w->degree_1.count(j)){
        //Vertices j an i with degree 1
        if(w->matching.at(i) != j){
            //Not in the match
            HamiltonianCycle_WitnessPointer w1=createWitness();
            w1->set_equal(*w);
            w1->degree_1.erase(i);
            w1->degree_1.erase(j);
            w1->degree_2.insert(i);
            w1->degree_2.insert(j);
            unsigned  mi = w1->matching[i];
            unsigned  mj = w1->matching[j];
            w1->matching.erase(i);
            w1->matching.erase(j);
            w1->matching[mj] = mi;
            w1->matching[mi] = mj;
            witnessSet->insert(w1);
        }else if(w->matching.at(i) == j && w->degree_0.size() == 0 && w->degree_1.size() == 2){
            //They are matching
            HamiltonianCycle_WitnessPointer w1=createWitness();
            w1->set_equal(*w);
            w1->degree_1.erase(i);
            w1->degree_1.erase(j);
            w1->degree_2.insert(i);
            w1->degree_2.insert(j);
            w1->matching.erase(i);
            w1->matching.erase(j);
            w1->closed = true;
            witnessSet->insert(w1);
        }
    }
    //*****************************
    //*****************************
}
void HamiltonianCycle_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, HamiltonianCycle_WitnessPointerConst w,
                                                           HamiltonianCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer w1=createWitness();
    w1->set_equal(*w);
    //Only forget vertices with degree 2
    if(w1->degree_2.count(i) == 1){
        w1->degree_2.erase(i);
        witnessSet->insert(w1);
    }
    //*****************************
    //*****************************
}
void HamiltonianCycle_DynamicCore::join_implementation(Bag &b, HamiltonianCycle_WitnessPointerConst w1,
                                                       HamiltonianCycle_WitnessPointerConst w2,
                                                       HamiltonianCycle_WitnessSetPointer witnessSet) {
  //*****************************
  //*****************************
  HamiltonianCycle_WitnessPointer wPrime = createWitness();
  map<unsigned, int> new_degree;

  for (auto w : {w1, w2}) {
    for (unsigned i : w->degree_0)
      new_degree[i] += 0;
    for (unsigned i : w->degree_1)
      new_degree[i] += 1;
    for (unsigned i : w->degree_2)
      new_degree[i] += 2;
  }

  for (auto [i, d] : new_degree) {
    if (d > 2)
      return; // impossible

    if (d == 0)
      wPrime->degree_0.insert(i);
    if (d == 1)
      wPrime->degree_1.insert(i);
    if (d == 2)
      wPrime->degree_2.insert(i);
  }

  map<unsigned, vector<unsigned>> graph;

  for (auto w : {w1, w2})
    for (auto [i, j] : w->matching)
      if (graph[i].size() == 0 || graph[i][0] != j)
        graph[i].push_back(j);

  set<unsigned> endpoints;
  for (auto [i, d] : new_degree)
    if (d == 1)
      endpoints.insert(i);


  while (endpoints.size()) {
    unsigned start = *endpoints.begin();
    endpoints.erase(endpoints.begin());

    unsigned end = start;
    while (graph.count(end)) {
      unsigned next = graph[end].back();
      graph[end].pop_back();
      if (graph[end].size() == 0)
        graph.erase(end);

      if (graph[next].back() == end) {
        graph[next].pop_back();
      } else {
        graph[next].erase(graph[next].begin());
      }

      if (graph[next].size() == 0)
        graph.erase(next);

      end = next;
    }
    
    endpoints.erase(end);

    wPrime->matching[start] = end;
    wPrime->matching[end] = start;
  }

  int cycle_count = w1->closed + w2->closed;
  while (graph.size()) {
    ++cycle_count;
    unsigned start = graph.begin()->first;

    unsigned end = start;
    while (graph.count(end)) {
      unsigned next = graph[end].back();
      graph[end].pop_back();
      if (graph[end].size() == 0)
        graph.erase(end);

      if (graph[next].back() == end) {
        graph[next].pop_back();
      } else {
        graph[next].erase(graph[next].begin());
      }

      if (graph[next].size() == 0)
        graph.erase(next);

      end = next;
    }
  }

  if (cycle_count >= 2)
    return; // impossible

  if (cycle_count == 1) {
    // check if allowed to close

    for (auto [i, d] : new_degree)
      if (d != 2) return; // impossible

    wPrime->closed = true;
  }

  witnessSet->insert(wPrime);
  //*****************************
  //*****************************
}

HamiltonianCycle_WitnessSetPointer HamiltonianCycle_DynamicCore::clean_implementation(
        HamiltonianCycle_WitnessSetPointer witnessSet) {

    return witnessSet;
}

bool HamiltonianCycle_DynamicCore::is_final_witness_implementation(HamiltonianCycle_WitnessPointerConst w) {
    //*****************************
    //*****************************
    // Verifies if there is no node with degree 0 or 1, and checks if it's already closed
    if(w->degree_0.size() == 0 &&
       w->degree_1.size() == 0 &&
       w->matching.size() == 0 &&
       w->closed
            ){
        return true;
    }else{
        return false;
    }
    //*****************************
    //*****************************
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool HamiltonianCycle_Witness::is_equal(const Witness &rhs) const{
    if (HamiltonianCycle_Witness const *e = dynamic_cast<HamiltonianCycle_Witness const *>(&rhs)) {
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool HamiltonianCycle_Witness::is_less(const Witness &rhs)const {
    if (HamiltonianCycle_Witness const *e = dynamic_cast<HamiltonianCycle_Witness const *>(&rhs)) {
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &HamiltonianCycle_Witness::set_equal(const Witness &witness) {
    if (const HamiltonianCycle_Witness *e = dynamic_cast<const HamiltonianCycle_Witness *>(&witness)) {
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> HamiltonianCycle_WitnessSet::createEmptyWitnessSet() {
    HamiltonianCycle_WitnessSetPointer witnessSet(new HamiltonianCycle_WitnessSet);
    return witnessSet;
}

HamiltonianCycle_WitnessPointer HamiltonianCycle_DynamicCore::createWitness() {
    HamiltonianCycle_WitnessPointer w(new HamiltonianCycle_Witness);
    return w;
}

void HamiltonianCycle_DynamicCore::copyWitness(HamiltonianCycle_WitnessPointer w_input,
                                               HamiltonianCycle_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void HamiltonianCycle_DynamicCore::createInitialWitnessSet() {
    HamiltonianCycle_WitnessSetPointer witnessSet(new HamiltonianCycle_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}


WitnessSetPointer HamiltonianCycle_DynamicCore::intro_v(unsigned i, Bag &b, const Witness &witness  ) {
    if (const HamiltonianCycle_Witness *e = dynamic_cast<const HamiltonianCycle_Witness *>(&witness)){
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}



WitnessSetPointer HamiltonianCycle_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness) {
    if (const HamiltonianCycle_Witness *e = dynamic_cast<const HamiltonianCycle_Witness *>(&witness)){
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}


WitnessSetPointer HamiltonianCycle_DynamicCore::forget_v(unsigned i, Bag &b, const Witness &witness) {
    if (const HamiltonianCycle_Witness *e = dynamic_cast<const HamiltonianCycle_Witness *>(&witness)){
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}


WitnessSetPointer HamiltonianCycle_DynamicCore::join(Bag &b, const Witness &witness1, const Witness &witness2) {
    if(const HamiltonianCycle_Witness *e1 = dynamic_cast<const HamiltonianCycle_Witness *>(&witness1)){
        if(const HamiltonianCycle_Witness *e2 = dynamic_cast<const HamiltonianCycle_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            HamiltonianCycle_WitnessPointerConst w1 = e1->shared_from_this();
            HamiltonianCycle_WitnessPointerConst w2 = e2->shared_from_this();
            HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in HamiltonianCycle_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer HamiltonianCycle_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (HamiltonianCycle_WitnessSetPointer e = dynamic_pointer_cast<HamiltonianCycle_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool HamiltonianCycle_DynamicCore::is_final_witness(const Witness &witness) {
    if (const HamiltonianCycle_Witness *e = dynamic_cast<const HamiltonianCycle_Witness *>(&witness)) {
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new HamiltonianCycle_DynamicCore;
}
}
