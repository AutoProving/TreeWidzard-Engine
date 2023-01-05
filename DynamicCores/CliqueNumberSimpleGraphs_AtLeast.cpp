#include "CliqueNumberSimpleGraphs_AtLeast.h"


/////////////////////////////Implementation////////////////////////////////////////////////

bool CliqueNumberSimpleGraphs_AtLeast_Witness::is_equal_implementation(const CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w) const{
  return (this->found == w->found and this->size==w->size and this->partialClique==w->partialClique);
}

bool CliqueNumberSimpleGraphs_AtLeast_Witness::is_less_implementation(const CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w) const{
  if(this->found >  w->found) return true;
  if(this->found == w->found and  this->size < w->size) return true;
  if(this->found == w->found and  this->size == w->size) return this->partialClique < w->partialClique;
  return false;
}

Witness& CliqueNumberSimpleGraphs_AtLeast_Witness::set_equal_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w){
  this->found = w->found;
  this->size = w->size;
  this->partialClique = w->partialClique;
  return *this;
}

shared_ptr<Witness> CliqueNumberSimpleGraphs_AtLeast_Witness::relabel(const map<unsigned int, unsigned int> &relabelingMap) const {
  // Here we can not call createWitness() because it is not accessible in this function.
  CliqueNumberSimpleGraphs_AtLeast_WitnessPointer relabeledWitness(new CliqueNumberSimpleGraphs_AtLeast_Witness);


    relabeledWitness->found = this->found;
    relabeledWitness->size = this->size;
    for(auto p:this->partialClique){
      auto it = relabelingMap.find(p.first);

      if(it != relabelingMap.end()){
        relabeledWitness->partialClique.insert(make_pair(it->second,p.second));
      }else{
        cout<<"Error: CliqueNumberSimpleGraphs_AtLeast_Witness::relabel "<< p.first<< "in the map"<<endl;
        print();
        for(auto l:relabelingMap){
          cout<<l.first<<"->"<<l.second<<endl;
        }
        exit(20);
      }
    }
    return relabeledWitness;
}

void CliqueNumberSimpleGraphs_AtLeast_Witness::print() const{
  cout << "found = " << found << " size: "<<size;
  cout<<"partialClique={";
  for(auto it = partialClique.begin() ; it != partialClique.end() ; ++it){
    cout<<it->first<<"->"<<it->second;
    if(it!=--partialClique.end()){
      cout<<", ";
    }
  }
  cout<<"}"<<endl;
}

string CliqueNumberSimpleGraphs_AtLeast_Witness::witnessInformation() const{
  string info;
  info =  "found = " +  to_string(found) + " size: "+ to_string(size);
  info = info + "partialClique={";
  for(auto it = partialClique.begin() ; it != partialClique.end() ; ++it){
    info = info  + to_string(it->first) + "->" + to_string(it->second);\
    if(it!= --partialClique.end()){
      info = info + ",";
    }
  }
  info = info + "}\n";
  return info;
}


void CliqueNumberSimpleGraphs_AtLeast_Witness::hash(Hasher &h) const {
  h << found;
  h << -1u;
  for (auto [i, j] : partialClique){
    h << i << j;
  }
  h << -1u;
  h << size;
}

CliqueNumberSimpleGraphs_AtLeast_Witness::CliqueNumberSimpleGraphs_AtLeast_Witness(){
  this->size = 0;
  this->found = false;
}

CliqueNumberSimpleGraphs_AtLeast_Witness::~CliqueNumberSimpleGraphs_AtLeast_Witness(){

}

CliqueNumberSimpleGraphs_AtLeast_DynamicCore::CliqueNumberSimpleGraphs_AtLeast_DynamicCore() {
  // Initializing attributes
  addAttribute("CoreName","CliqueNumberSimpleGraphs");
  addAttribute("CoreType","Bool");
  addAttribute("Restriction","SimpleGraphs");
  addAttribute("ParameterType","UnsignedInt");
  addAttribute("PrimaryOperator","AtLeast");

}

CliqueNumberSimpleGraphs_AtLeast_DynamicCore::CliqueNumberSimpleGraphs_AtLeast_DynamicCore(unsigned cliqueSize){
  this->cliqueSize = cliqueSize;
  createInitialWitnessSet();
  // Initializing attributes
  addAttribute("CoreName","CliqueNumberSimpleGraphs");
  addAttribute("CoreType","Bool");
  addAttribute("Restriction","SimpleGraphs");
  addAttribute("ParameterType","UnsignedInt");
  addAttribute("PrimaryOperator","AtLeast");
}

CliqueNumberSimpleGraphs_AtLeast_DynamicCore::CliqueNumberSimpleGraphs_AtLeast_DynamicCore(
	const parameterType &parameters) {
	if (parameters.size() != 1) {
		cerr << "ChromaticNumber Core received an invalid parameter list"
			 << endl;
		exit(20);
	}
	for (const auto &prop : parameters) {
		std::visit(overloaded{[&](char *p) {
								  cerr << "ChromaticNumber Core received an "
										  "invalid parameter list"
									   << endl;
								  exit(20);
							  },
							  [&](auto p) { this->cliqueSize = p; }},
				   prop);
	}
  createInitialWitnessSet();
  // Initializing attributes
  addAttribute("CoreName","CliqueNumberSimpleGraphs");
  addAttribute("CoreType","Bool");
  addAttribute("Restriction","SimpleGraphs");
  addAttribute("ParameterType","UnsignedInt");
  addAttribute("PrimaryOperator","AtLeast");
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::createInitialWitnessSet_implementation() {
  CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w = createWitness();
  w->found = false;
  w->size = 0;
  this->insertIntoInitialWitnessSet(w);
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
  CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
  if(w->found or w->size == this->cliqueSize){
    witness->set_equal(*w);
    witnessSet->insert(witness);
  }else{
    // Either i can be added to the clique or not.
    witness->set_equal(*w);
    witnessSet->insert(witness);
    // A new vertex only can be added to the clique when no vertex has been forgotten from the clique.
    // This means number of vertices in the current clique should be equal to size
    if(w->size == w->partialClique.size()){
      CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witnessTwo = createWitness();
      witnessTwo->set_equal(*w);
      witnessTwo->partialClique.insert(make_pair(i,0));
      witnessTwo->size = w->size+1;
      witnessSet->insert(witnessTwo);
    }
  }
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
  if (w->found){
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    witnessSet->insert(witness);
  }else{
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
    if(w->partialClique.count(i) and w->partialClique.count(j)){
      // Both i and j are in the clique, so that the counters of i and j will be increased by one.
      witness->set_equal(*w);
      unsigned iCounter = witness->partialClique[i];
      unsigned jCounter = witness->partialClique[j];
      if(iCounter<cliqueSize-1 and jCounter<cliqueSize-1){
        witness->partialClique.erase(i);
        witness->partialClique.erase(j);
        witness->partialClique.insert(make_pair(i,iCounter+1));
        witness->partialClique.insert(make_pair(j,jCounter+1));
        if(isCompleteClique(witness)){
          witness->found = true;
          witness->partialClique.clear();
        }
        witnessSet->insert(witness);
      }

    }else{
      // Either i or j is not in the clique, so that {i,j} cannot be added to the clique.
      witness->set_equal(*w);
      witnessSet->insert(witness);
    }
  }
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::forget_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
  CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
  if(w->found){
    witness->set_equal(*w);
    witnessSet->insert(witness);
  }else{
    map<unsigned,unsigned> partialCliqueW = w->partialClique;
    if(partialCliqueW.count(i)){
      // A vertex only can be forgotten when the size of the clique is equal to cliqueSize.
      // And the forgotten vertex has cliqueSize-1 neighbors.
      if(w->size==cliqueSize and partialCliqueW[i]==cliqueSize-1 ){
        witness->set_equal(*w);
        witness->partialClique.erase(i);
        witnessSet->insert(witness);
      }else{
        // invalid witness
        return;
      }
    }else{
      // i is not in the clique
      witness->set_equal(*w);
      witnessSet->insert(witness);
    }
  }
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::join_implementation(Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w1, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w2, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
  CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
  if (w1->found){
    witness->set_equal(*w1);
    witnessSet->insert(witness);
  } else if (w2->found){
    witness->set_equal(*w2);
    witnessSet->insert(witness);
  } else{
    // At the same time it is not possible to have forget vertices in both witnesses
    // If there is a forgotten vertex v in w1, and a forgotten vertex u in w2, then v and u can not be connected to together
    if (w1->partialClique.size()==w1->size or w2->partialClique.size() == w2->size) {
      // The domains of two partialCliques should be the same, and therefore, they should have the same size.
      // Do not confuse w->size with w->partialClique.size().
      if (w1->partialClique.size() == w2->partialClique.size()) {
        witness->set_equal(*w1);
        // The size of the new witness is the sum of the sizes of the two witnesses minus the size of the partialClique
        // Note that, w1->partialClique.size() = w2->partialClique.size().
        witness->size = w1->size + w2->size - w1->partialClique.size();
        // Now we check if the domains of the partialCliques are the same.
        // If the in the checking process we realize that they do not have the same domains, we return the empty set.
        for (auto p: w2->partialClique) {
          if (witness->partialClique.count(p.first)) {
            unsigned pCounter = p.second + witness->partialClique[p.first];
            witness->partialClique.erase(p.first);
            witness->partialClique.insert(make_pair(p.first, pCounter));
          } else {
            // w1 and w2 have different domains, so they cannot be joined.
            return;
          }
        }
        // Check that the clique is a complete clique or not,
        // if yes, found will be true and partialClique will be cleared.
        if (isCompleteClique(witness)) {
          witness->found = true;
          witness->partialClique.clear();
        }
        witnessSet->insert(witness);
      } else {
        return;
      }
    }else{
      return;
    }
  }
}

CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::clean_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
  for(auto witness:(*witnessSet)){
    if (CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w = dynamic_pointer_cast<CliqueNumberSimpleGraphs_AtLeast_Witness>(witness)) {
      if(w->found){
        CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer newWitnessSet (new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
        newWitnessSet->insert(w);
        return newWitnessSet;
      }
    }else{
      cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::clean_implementation cast error"<<endl;
      exit(20);
    }
  }
  return witnessSet;
}

bool CliqueNumberSimpleGraphs_AtLeast_DynamicCore::is_final_witness_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w){
  return w->found;
}

bool CliqueNumberSimpleGraphs_AtLeast_DynamicCore::isCompleteClique(CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w){
  if(w->size == cliqueSize){
    for(auto m: w->partialClique){
      if(m.second != cliqueSize-1){
        return false;
      }
    }
    return true;
  }else{
    return false;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CliqueNumberSimpleGraphs_AtLeast_Witness::is_equal(const Witness &rhs) const{
  if (CliqueNumberSimpleGraphs_AtLeast_Witness const *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness const *>(&rhs)) {
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w = e->shared_from_this();
    return is_equal_implementation(w);
  }else{
    cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::is_equal cast error"<<endl;
    exit(20);
  }
}

bool CliqueNumberSimpleGraphs_AtLeast_Witness::is_less(const Witness &rhs)const {
  if (CliqueNumberSimpleGraphs_AtLeast_Witness const *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness const *>(&rhs)) {
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w = e->shared_from_this();
    return is_less_implementation(w);
  }else{
    cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::is_less cast error"<<endl;
    exit(20);
  }
}

Witness &CliqueNumberSimpleGraphs_AtLeast_Witness::set_equal(const Witness &witness) {
  if (const CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<const CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)) {
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w = e->shared_from_this();
    return set_equal_implementation(w);
  }else{
    cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::operator= cast error"<<endl;
    exit(20);
  }
}

shared_ptr<WitnessSet> CliqueNumberSimpleGraphs_AtLeast_WitnessSet::createEmptyWitnessSet() {
  CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
  return witnessSet;
}

CliqueNumberSimpleGraphs_AtLeast_WitnessPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::createWitness() {
  CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w(new CliqueNumberSimpleGraphs_AtLeast_Witness);
  return w;
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::copyWitness(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_input,
  CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
  }

  void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::createInitialWitnessSet() {
    CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
  }


  WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, const Witness &witness  ) {
    if (const CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<const CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)){
      CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w = e->shared_from_this();
      CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet (new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
      intro_v_implementation(i,b,w,witnessSet);
      return clean(witnessSet);
    }else{
      cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_v cast error"<<endl;
      exit(20);
    }
  }



  WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness) {
    if (const CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<const CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)){
      CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w = e->shared_from_this();
      CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet (new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
      intro_e_implementation(i,j,b,w,witnessSet);
      return clean(witnessSet);
    } else{
      cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_e cast error"<< endl;
      exit(20);
    }
  }


  WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, const Witness &witness) {
    if (const CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<const CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)){
      CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w = e->shared_from_this();
      CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet (new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
      forget_v_implementation(i,b,w,witnessSet);
      return clean(witnessSet);
    }else{
      cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::forget_v cast error"<<endl;
      exit(20);
    }
  }


  WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::join(Bag &b, const Witness &witness1, const Witness &witness2) {
    if(const CliqueNumberSimpleGraphs_AtLeast_Witness *e1 = dynamic_cast<const CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness1)){
      if(const CliqueNumberSimpleGraphs_AtLeast_Witness *e2 = dynamic_cast<const CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness2)){
        // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w1 = e1->shared_from_this();
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w2 = e2->shared_from_this();
        CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet (new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
        join_implementation(b,w1,w2,witnessSet);
        return clean(witnessSet);
      }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::join cast error"<<endl;
        exit(20);
      }
    }else{
      cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::join cast error"<<endl;
      exit(20);
    }
  }

  WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer e = dynamic_pointer_cast<CliqueNumberSimpleGraphs_AtLeast_WitnessSet >(witnessSet)) {
      return clean_implementation(e);
    }else{
      cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::clean cast error"<<endl;
      exit(20);
    }
  }

  bool CliqueNumberSimpleGraphs_AtLeast_DynamicCore::is_final_witness(const Witness &witness) {
    if (const CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<const CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)) {
      CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w = e->shared_from_this();
      return is_final_witness_implementation(w);
    }else{
      cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_DynamicCore::is_final_witness cast error"<<endl;
      exit(20);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  extern "C" {
    DynamicCore * create() {
      return new CliqueNumberSimpleGraphs_AtLeast_DynamicCore;
    }
    DynamicCore * create_int(unsigned param) {
      return new CliqueNumberSimpleGraphs_AtLeast_DynamicCore(param);
    }
    DynamicCore *create_parameters(const parameterType &parameters) {
      return new CliqueNumberSimpleGraphs_AtLeast_DynamicCore(parameters);
    }
  }
