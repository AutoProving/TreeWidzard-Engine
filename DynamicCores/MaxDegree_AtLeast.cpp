#include "MaxDegree_AtLeast.h"

////////////////////////////// AUXILIARY FUNCTIONS //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//
// If you need to implement auxiliary functions, please define them in the header
// file (Generic_Core.h) and implement them here.
//
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////// IMPLEMENTATION OF CORES ////////////////////////////////////
//////////////////////////// ONLY CHANGE THE GREY AREA ///////////////////////////////////




void MaxDegree_AtLeast_Witness::print() const {
  cout<<"{";
  map<unsigned,unsigned> wDegreeCounter = degreeCounter;
  for(map<unsigned,unsigned>::iterator it = wDegreeCounter.begin(); it != wDegreeCounter.end() ; it++){
    auto tempIt = it;
    cout << it->first << " -> " << it->second;
    if(++tempIt != wDegreeCounter.end()){
      cout<<", ";
    }
  }
  cout<<"} "<<"found : "<<found;
}

string MaxDegree_AtLeast_Witness::witnessInformation() const {
  map<unsigned,unsigned> wDegreeCounter = degreeCounter;
  string info;
  info = "{";
  for(map<unsigned,unsigned>::iterator it = wDegreeCounter.begin(); it != wDegreeCounter.end() ; it++){
    auto tempIt = it;
    info = info + to_string(it->first) + " -> " + to_string(it->second);
    if(++tempIt != wDegreeCounter.end()){
      info = info + ", ";
    }
  }
  info = info  +"} " + "found : " + to_string(found);
  return info;
}

bool MaxDegree_AtLeast_Witness::is_equal_implementation(const MaxDegree_AtLeast_WitnessPointerConst w) const {
  if(found == w->found and (degreeCounter==w->degreeCounter)){
    return  true;
  } else{
    return false;
  }
}

bool MaxDegree_AtLeast_Witness::is_less_implementation(const MaxDegree_AtLeast_WitnessPointerConst w) const {
  // True is smaller than false. In this way, in the set container, the accepting witnesses come first.
  if(this->found > w->found or (this->found == w->found and this->degreeCounter < w->degreeCounter)){
    return true;
  }else{
    return false;
  }
}

Witness & MaxDegree_AtLeast_Witness::set_equal_implementation(MaxDegree_AtLeast_WitnessPointerConst w) {
  found = w->found;
  degreeCounter = w->degreeCounter;
  return *this;
}

shared_ptr<Witness> MaxDegree_AtLeast_Witness::relabel(const map<unsigned int, unsigned int> &relabelingMap) const{
  MaxDegree_AtLeast_WitnessPointer w (new MaxDegree_AtLeast_Witness);
  map<unsigned ,unsigned > m;
  if(this->found){
    w->found = true;
  }else{
    for(auto element:this->degreeCounter){
      auto it = relabelingMap.find(element.first);
      if(it != relabelingMap.end()){
        m.insert(make_pair(it->second,element.second));
      }else{
        cout<<"Error: MaxDegree_AtLeast_Witness::relabel " << element.first<< " is not in the map"<<endl;
        cout<<" map:";
        exit(20);
      }
    }
    w->degreeCounter = m;
    w->found = this->found;
  }
  return w;
}

MaxDegree_AtLeast_Witness::~MaxDegree_AtLeast_Witness() {

}


MaxDegree_AtLeast_Witness::MaxDegree_AtLeast_Witness() {
  found = false;
}

void MaxDegree_AtLeast_Witness::hash(Hasher &h) const {
  h << found;
  h << -1u;
  for (auto [i, j] : degreeCounter){
    h << i << j;
  }
  h << -1u;
}

MaxDegree_AtLeast_DynamicCore::MaxDegree_AtLeast_DynamicCore() {
  // Initializing attributes
  addAttribute("CoreName","MaxDegree");
  addAttribute("CoreType","Bool");
  addAttribute("ParameterType","UnsignedInt");
  addAttribute("PrimaryOperator","AtLeast");
}

MaxDegree_AtLeast_DynamicCore::MaxDegree_AtLeast_DynamicCore(unsigned maxDegree){
  this->maxDegree = maxDegree;
  createInitialWitnessSet();
  // Initializing attributes
  addAttribute("CoreName","MaxDegree");
  addAttribute("CoreType","Bool");
  addAttribute("ParameterType","UnsignedInt");
  addAttribute("PrimaryOperator","AtLeast");
}

void MaxDegree_AtLeast_DynamicCore::createInitialWitnessSet_implementation() {
  MaxDegree_AtLeast_WitnessPointer w = createWitness();
  w->found = false;
  this->insertIntoInitialWitnessSet(w);
}

void MaxDegree_AtLeast_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, MaxDegree_AtLeast_WitnessPointerConst w,
  MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
    if(w->found){
      MaxDegree_AtLeast_WitnessPointer witness = createWitness();
      witness->set_equal(*w);
      witnessSet->insert(witness);
    }else{
      MaxDegree_AtLeast_WitnessPointer witness = createWitness();
      witness->found=false;
      witness->degreeCounter = w->degreeCounter;
      witness->degreeCounter.insert(make_pair(i,0));
      witnessSet->insert(witness);
    }
  }

  void MaxDegree_AtLeast_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
    MaxDegree_AtLeast_WitnessPointerConst w,
    MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
      if(w->found){
        MaxDegree_AtLeast_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        witnessSet->insert(witness);
      }else if( i!=j and  w->degreeCounter.find(i)->second < this->maxDegree-1 and w->degreeCounter.find(j)->second < this->maxDegree-1 ){
        MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
        wPrime->degreeCounter = w->degreeCounter;
        wPrime->degreeCounter.erase(i);
        wPrime->degreeCounter.erase(j);
        wPrime->degreeCounter.insert(make_pair(i,w->degreeCounter.find(i)->second+1));
        wPrime->degreeCounter.insert(make_pair(j,w->degreeCounter.find(j)->second+1));
        wPrime->found = false;
        witnessSet->insert(wPrime);
      }else if(i==j and w->degreeCounter.find(i)->second < this->maxDegree-2){
        // This case is inactive at the moment. It would be active if self-loops were allowed.
        MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
        wPrime->degreeCounter = w->degreeCounter;
        wPrime->degreeCounter.erase(i);
        wPrime->degreeCounter.insert(make_pair(i,w->degreeCounter.find(i)->second+2));
        witnessSet->insert(wPrime);
      }else{
        MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
        wPrime->found = true;
        witnessSet->insert(wPrime);
      }

    }

    void MaxDegree_AtLeast_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, MaxDegree_AtLeast_WitnessPointerConst w,
      MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
        if(w->found){
          MaxDegree_AtLeast_WitnessPointer witness = createWitness();
          witness->set_equal(*w);
          witnessSet->insert(witness);
        }else{
          MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
          wPrime->found = false;
          wPrime->degreeCounter = w->degreeCounter;
          wPrime->degreeCounter.erase(i);
          witnessSet->insert(wPrime);
        }
      }

      void MaxDegree_AtLeast_DynamicCore::join_implementation(Bag &b, MaxDegree_AtLeast_WitnessPointerConst w1,
        MaxDegree_AtLeast_WitnessPointerConst w2,
        MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
          MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
          if (w1->found){
            MaxDegree_AtLeast_WitnessPointer witness = createWitness();
            witness->set_equal(*w1);
            witnessSet->insert(witness);
          } else if( w2->found){
            MaxDegree_AtLeast_WitnessPointer witness = createWitness();
            witness->set_equal(*w2);
            witnessSet->insert(witness);
          }else {
            map<unsigned, unsigned> temp1 = w1->degreeCounter;
            map<unsigned, unsigned> temp2 = w2->degreeCounter;
            map<unsigned, unsigned> temp;
            bool found = false;
            map<unsigned, unsigned>::iterator it1 = temp1.begin();
            for (map<unsigned, unsigned>::iterator it2 = temp2.begin(); it2 != temp2.end(); ++it2) {
              if (it1->second + it2->second < this->maxDegree) {
                temp.insert(make_pair(it1->first, it1->second + it2->second));
                it1++;
              } else {
                found = true;
                break;
              }
            }
            if (found == false) {
              wPrime->found = false;
              wPrime->degreeCounter = temp;
              witnessSet->insert(wPrime);
            } else {
              wPrime->found = true;
              wPrime->degreeCounter.clear();
              witnessSet->insert(wPrime);
            }
          }
        }

        bool MaxDegree_AtLeast_DynamicCore::is_final_witness_implementation(MaxDegree_AtLeast_WitnessPointerConst w) {
          return w->found;
        }

        MaxDegree_AtLeast_WitnessSetPointer MaxDegree_AtLeast_DynamicCore::clean_implementation(MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
          for(auto witness:(*witnessSet)){
            if (MaxDegree_AtLeast_WitnessPointer w = dynamic_pointer_cast<MaxDegree_AtLeast_Witness>(witness)) {
              if(w->found){
                MaxDegree_AtLeast_WitnessSetPointer newWitnessSet (new MaxDegree_AtLeast_WitnessSet);
                newWitnessSet->insert(w);
                return newWitnessSet;
              }
            }else{
              cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::clean_implementation cast error"<<endl;
              exit(20);
            }
          }
          return witnessSet;
        }
/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MaxDegree_AtLeast_Witness::is_equal(const Witness &rhs) const{
  if (MaxDegree_AtLeast_Witness const *e = dynamic_cast<MaxDegree_AtLeast_Witness const *>(&rhs)) {
    MaxDegree_AtLeast_WitnessPointerConst w = e->shared_from_this();
    return is_equal_implementation(w);
  }else{
    cerr<<"ERROR: in MaxDegree_AtLeast_Witness::is_equal cast error"<<endl;
    exit(20);
  }
}

bool MaxDegree_AtLeast_Witness::is_less(const Witness &rhs)const {
  if (MaxDegree_AtLeast_Witness const *e = dynamic_cast<MaxDegree_AtLeast_Witness const *>(&rhs)) {
    MaxDegree_AtLeast_WitnessPointerConst w = e->shared_from_this();
    return is_less_implementation(w);
  }else{
    cerr<<"ERROR: in MaxDegree_AtLeast_Witness::is_less cast error"<<endl;
    exit(20);
  }
}

Witness &MaxDegree_AtLeast_Witness::set_equal(const Witness &witness) {
  if (const MaxDegree_AtLeast_Witness *e = dynamic_cast<const MaxDegree_AtLeast_Witness *>(&witness)) {
    MaxDegree_AtLeast_WitnessPointerConst w = e->shared_from_this();
    return set_equal_implementation(w);
  }else{
    cerr<<"ERROR: in MaxDegree_AtLeast_Witness::operator= cast error"<<endl;
    exit(20);
  }
}

shared_ptr<WitnessSet> MaxDegree_AtLeast_WitnessSet::createEmptyWitnessSet() {
  MaxDegree_AtLeast_WitnessSetPointer witnessSet(new MaxDegree_AtLeast_WitnessSet);
  return witnessSet;
}

MaxDegree_AtLeast_WitnessPointer MaxDegree_AtLeast_DynamicCore::createWitness() {
  MaxDegree_AtLeast_WitnessPointer w(new MaxDegree_AtLeast_Witness);
  return w;
}

void MaxDegree_AtLeast_DynamicCore::copyWitness(MaxDegree_AtLeast_WitnessPointer w_input,
  MaxDegree_AtLeast_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
  }

  void MaxDegree_AtLeast_DynamicCore::createInitialWitnessSet() {
    MaxDegree_AtLeast_WitnessSetPointer witnessSet(new MaxDegree_AtLeast_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
  }


  WitnessSetPointer MaxDegree_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, const Witness &witness  ) {
    if (const MaxDegree_AtLeast_Witness *e = dynamic_cast<const MaxDegree_AtLeast_Witness *>(&witness)){
      MaxDegree_AtLeast_WitnessPointerConst w = e->shared_from_this();
      MaxDegree_AtLeast_WitnessSetPointer witnessSet (new MaxDegree_AtLeast_WitnessSet);
      intro_v_implementation(i,b,w,witnessSet);
      return clean(witnessSet);
    }else{
      cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::intro_v cast error"<<endl;
      exit(20);
    }
  }



  WitnessSetPointer MaxDegree_AtLeast_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness) {
    if (const MaxDegree_AtLeast_Witness *e = dynamic_cast<const MaxDegree_AtLeast_Witness *>(&witness)){
      MaxDegree_AtLeast_WitnessPointerConst w = e->shared_from_this();
      MaxDegree_AtLeast_WitnessSetPointer witnessSet (new MaxDegree_AtLeast_WitnessSet);
      intro_e_implementation(i,j,b,w,witnessSet);
      return clean(witnessSet);
    } else{
      cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::intro_e cast error"<< endl;
      exit(20);
    }
  }


  WitnessSetPointer MaxDegree_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, const Witness &witness) {
    if (const MaxDegree_AtLeast_Witness *e = dynamic_cast<const MaxDegree_AtLeast_Witness *>(&witness)){
      MaxDegree_AtLeast_WitnessPointerConst w = e->shared_from_this();
      MaxDegree_AtLeast_WitnessSetPointer witnessSet (new MaxDegree_AtLeast_WitnessSet);
      forget_v_implementation(i,b,w,witnessSet);
      return clean(witnessSet);
    }else{
      cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::forget_v cast error"<<endl;
      exit(20);
    }
  }


  WitnessSetPointer MaxDegree_AtLeast_DynamicCore::join(Bag &b, const Witness &witness1, const Witness &witness2) {
    if(const MaxDegree_AtLeast_Witness *e1 = dynamic_cast<const MaxDegree_AtLeast_Witness *>(&witness1)){
      if(const MaxDegree_AtLeast_Witness *e2 = dynamic_cast<const MaxDegree_AtLeast_Witness *>(&witness2)){
        // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
        MaxDegree_AtLeast_WitnessPointerConst w1 = e1->shared_from_this();
        MaxDegree_AtLeast_WitnessPointerConst w2 = e2->shared_from_this();
        MaxDegree_AtLeast_WitnessSetPointer witnessSet (new MaxDegree_AtLeast_WitnessSet);
        join_implementation(b,w1,w2,witnessSet);
        return clean(witnessSet);
      }else{
        cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::join cast error"<<endl;
        exit(20);
      }
    }else{
      cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::join cast error"<<endl;
      exit(20);
    }
  }

  WitnessSetPointer MaxDegree_AtLeast_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (MaxDegree_AtLeast_WitnessSetPointer e = dynamic_pointer_cast<MaxDegree_AtLeast_WitnessSet >(witnessSet)) {
      return clean_implementation(e);
    }else{
      cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::clean cast error"<<endl;
      exit(20);
    }
  }

  bool MaxDegree_AtLeast_DynamicCore::is_final_witness(const Witness &witness) {
    if (const MaxDegree_AtLeast_Witness *e = dynamic_cast<const MaxDegree_AtLeast_Witness *>(&witness)) {
      MaxDegree_AtLeast_WitnessPointerConst w = e->shared_from_this();
      return is_final_witness_implementation(w);
    }else{
      cerr<<"ERROR: in MaxDegree_AtLeast_DynamicCore::is_final_witness cast error"<<endl;
      exit(20);
    }
  }

  //////////////////// The functions below are used by the plugin handler /////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  extern "C" {
    DynamicCore * create() {
      return new MaxDegree_AtLeast_DynamicCore;
    }
    DynamicCore * create_int(unsigned param) {
      return new MaxDegree_AtLeast_DynamicCore(param);
    }
  }
