
#include "HasMultipleEdges.h"

bool HasMultipleEdges_Witness::is_equal_implementation(const HasMultipleEdges_WitnessPointerConst w)const {
    return found == w->found and edgeSet == w->edgeSet;
}

bool HasMultipleEdges_Witness::is_less_implementation(const HasMultipleEdges_WitnessPointerConst w )const {
        if(found < w->found){
            return true;
        } else if (found == w->found){
            if(edgeSet < w->edgeSet){
                return true;
            }
        }
        return false;
}

Witness &HasMultipleEdges_Witness::set_equal_implementation(HasMultipleEdges_WitnessPointerConst w) {
        this->found = w->found;
        this->edgeSet = w->edgeSet;
        return *this;
}

shared_ptr<Witness> HasMultipleEdges_Witness::relabel(const map<unsigned int, unsigned int> &relabelingMap) const {
    HasMultipleEdges_WitnessPointer w(new HasMultipleEdges_Witness);
    if(this->found ){
        w->found = true;
        return w;
    }else{
        set<pair<unsigned,unsigned>> newEdgeSet;
        for(auto p:this->edgeSet){
            auto it = relabelingMap.find(p.first);
            auto itr = relabelingMap.find(p.second);
            if(it != relabelingMap.end() and itr != relabelingMap.end()){
                if(it->second < itr->second){
                    newEdgeSet.insert(make_pair(it->second,itr->second));
                }else{
                    newEdgeSet.insert(make_pair(itr->second,it->second));
                }
            }else{
                cout<<"Error: HasMultipleEdges_Witness::relabel "<< p.first <<" or "<< p.second << " is not in the map"<<endl;
                print();
                cout<<"\nmap"<<endl;
                for(auto item:relabelingMap){
                    cout<<item.first<<"->"<<item.second<<endl;
                }
                exit(20);
            }
        }
        w->edgeSet = newEdgeSet;
        w->found = false;
        return w;
    }
}

void HasMultipleEdges_Witness::print() const{
    set<pair<unsigned,unsigned>> wEdgeSet = this->edgeSet;
    cout<<"{";
    for (set<pair<unsigned,unsigned>>::iterator it = wEdgeSet.begin() ; it != wEdgeSet.end() ; ++it) {
        cout<<"("<<it->first<<","<<it->second<<"),";
    }
    cout<<"} found: "<<this->found;
}

string HasMultipleEdges_Witness::witnessInformation() const{
    set<pair<unsigned,unsigned>> wEdgeSet = this->edgeSet;

    string info;
    info = "{";
    for (set<pair<unsigned,unsigned>>::iterator it = wEdgeSet.begin() ; it != wEdgeSet.end() ; ++it) {
        info=info + "(" + to_string(it->first)+","+to_string(it->second)+"),";
    }
    info=info+"} found: "+to_string(this->found);
    return info;
}

void HasMultipleEdges_Witness::hash(Hasher &h) const {
  h << found;
  h << -1u;
  for (auto p : edgeSet){
    h << p.first << p.second;
  }
  h << -1u;
}

HasMultipleEdges_Witness::HasMultipleEdges_Witness() {
    found =false;
}

HasMultipleEdges_Witness::~HasMultipleEdges_Witness() {

}


HasMultipleEdges_DynamicCore::HasMultipleEdges_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","HasMultipleEdges");
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","ParameterLess");
    addAttribute("PrimaryOperator","---");
    createInitialWitnessSet();
}

void HasMultipleEdges_DynamicCore::createInitialWitnessSet_implementation() {
    HasMultipleEdges_WitnessPointer w = createWitness();
    w->found = false;
    this->insertIntoInitialWitnessSet(w);
}

void HasMultipleEdges_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, HasMultipleEdges_WitnessPointerConst w, HasMultipleEdges_WitnessSetPointer witnessSet) {
    HasMultipleEdges_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    witnessSet->insert(witness);
}

void HasMultipleEdges_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b, HasMultipleEdges_WitnessPointerConst w, HasMultipleEdges_WitnessSetPointer witnessSet) {
  HasMultipleEdges_WitnessPointer witness = createWitness();

  if(i!=j and !w->found){
      pair<int,int> newEdge;
      if(i<j){
          newEdge = make_pair(i,j);
      }else{
          newEdge = make_pair(j,i);
      }
      if(w->edgeSet.find(newEdge)!= w->edgeSet.end()){
          witness->found = true;
          witnessSet->insert(witness);
      }else{
          witness->found =false;
          witness->edgeSet = w->edgeSet;
          witness->edgeSet.insert(newEdge);
          witnessSet->insert(witness);

      }

  }else{
    witness->set_equal(*w);
      witnessSet->insert(witness);

  }
}

void HasMultipleEdges_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, HasMultipleEdges_WitnessPointerConst w, HasMultipleEdges_WitnessSetPointer witnessSet) {
    HasMultipleEdges_WitnessPointer witness = createWitness();

    if(w->found){
    witness->set_equal(*w);
    witnessSet->insert(witness);

    }else{
      witness->found = w->found;
      witness->edgeSet = w->edgeSet;
      set<unsigned> tempBag = b.get_elements();
      for(set<unsigned>::iterator it = tempBag.begin(); it != tempBag.end() ; it++){
        pair<unsigned,unsigned> e = make_pair(i,*it);
        witness->edgeSet.erase(e);
        e = make_pair(*it,i);
        witness->edgeSet.erase(e);
        witnessSet->insert(witness);
      }
    }
  }

void HasMultipleEdges_DynamicCore::join_implementation(Bag &b, HasMultipleEdges_WitnessPointerConst w1, HasMultipleEdges_WitnessPointerConst w2, HasMultipleEdges_WitnessSetPointer witnessSet) {
  HasMultipleEdges_WitnessPointer witness = createWitness();

            if(w1->found ){
              witness->set_equal(*w1);
                witnessSet->insert(witness);

            }else if(w2->found){
                witness->set_equal(*w2);
                witnessSet->insert(witness);
            }else{
                set<pair<unsigned,unsigned>> intersection_set;
                set<pair<unsigned,unsigned>> wOneSet = w1->edgeSet;
                set<pair<unsigned,unsigned>> wTwoSet = w2->edgeSet;
                set_intersection(wOneSet.begin(),wOneSet.end(),wTwoSet.begin(),wTwoSet.end(), std::inserter(intersection_set,intersection_set.begin()));
                if(intersection_set.size()>0){
                    witness->found = true;
                    witnessSet->insert(witness);

                }else{
                    set<pair<unsigned,unsigned>> union_set;
                    set<pair<unsigned,unsigned>> wOneSet = w1->edgeSet;
                    set<pair<unsigned,unsigned>> wTwoSet = w2->edgeSet;

                    set_union(wOneSet.begin(),wOneSet.end(),wTwoSet.begin(),wTwoSet.end(),std::inserter(union_set,union_set.begin()));
                    witness->found = false;
                    witness->edgeSet = union_set;
                    witnessSet->insert(witness);

                }
              }

}

bool HasMultipleEdges_DynamicCore::is_final_witness_implementation(HasMultipleEdges_WitnessPointerConst w) {
  return w->found;
}

HasMultipleEdges_WitnessSetPointer HasMultipleEdges_DynamicCore::clean_implementation(HasMultipleEdges_WitnessSetPointer witnessSet) {
    return witnessSet;
}



/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HasMultipleEdges_Witness::is_equal(const Witness &rhs) const{
    if (HasMultipleEdges_Witness const *e = dynamic_cast<HasMultipleEdges_Witness const *>(&rhs)) {
        HasMultipleEdges_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool HasMultipleEdges_Witness::is_less(const Witness &rhs)const {
    if (HasMultipleEdges_Witness const *e = dynamic_cast<HasMultipleEdges_Witness const *>(&rhs)) {
        HasMultipleEdges_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &HasMultipleEdges_Witness::set_equal(const Witness &witness) {
    if (const HasMultipleEdges_Witness *e = dynamic_cast<const HasMultipleEdges_Witness *>(&witness)) {
        HasMultipleEdges_WitnessPointerConst w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> HasMultipleEdges_WitnessSet::createEmptyWitnessSet() {
    HasMultipleEdges_WitnessSetPointer witnessSet(new HasMultipleEdges_WitnessSet);
    return witnessSet;
}

HasMultipleEdges_WitnessPointer HasMultipleEdges_DynamicCore::createWitness() {
    HasMultipleEdges_WitnessPointer w(new HasMultipleEdges_Witness);
    return w;
}

void HasMultipleEdges_DynamicCore::copyWitness(HasMultipleEdges_WitnessPointer w_input,
                                               HasMultipleEdges_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void HasMultipleEdges_DynamicCore::createInitialWitnessSet() {
    HasMultipleEdges_WitnessSetPointer witnessSet(new HasMultipleEdges_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}


WitnessSetPointer HasMultipleEdges_DynamicCore::intro_v(unsigned i, Bag &b, const Witness &witness  ) {
    if (const HasMultipleEdges_Witness *e = dynamic_cast<const HasMultipleEdges_Witness *>(&witness)){
        HasMultipleEdges_WitnessPointerConst w = e->shared_from_this();
        HasMultipleEdges_WitnessSetPointer witnessSet (new HasMultipleEdges_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}



WitnessSetPointer HasMultipleEdges_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness) {
    if (const HasMultipleEdges_Witness *e = dynamic_cast<const HasMultipleEdges_Witness *>(&witness)){
        HasMultipleEdges_WitnessPointerConst w = e->shared_from_this();
        HasMultipleEdges_WitnessSetPointer witnessSet (new HasMultipleEdges_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}


WitnessSetPointer HasMultipleEdges_DynamicCore::forget_v(unsigned i, Bag &b, const Witness &witness) {
    if (const HasMultipleEdges_Witness *e = dynamic_cast<const HasMultipleEdges_Witness *>(&witness)){
        HasMultipleEdges_WitnessPointerConst w = e->shared_from_this();
        HasMultipleEdges_WitnessSetPointer witnessSet (new HasMultipleEdges_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}


WitnessSetPointer HasMultipleEdges_DynamicCore::join(Bag &b, const Witness &witness1, const Witness &witness2) {
    if(const HasMultipleEdges_Witness *e1 = dynamic_cast<const HasMultipleEdges_Witness *>(&witness1)){
        if(const HasMultipleEdges_Witness *e2 = dynamic_cast<const HasMultipleEdges_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            HasMultipleEdges_WitnessPointerConst w1 = e1->shared_from_this();
            HasMultipleEdges_WitnessPointerConst w2 = e2->shared_from_this();
            HasMultipleEdges_WitnessSetPointer witnessSet (new HasMultipleEdges_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in HasMultipleEdges_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer HasMultipleEdges_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (HasMultipleEdges_WitnessSetPointer e = dynamic_pointer_cast<HasMultipleEdges_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool HasMultipleEdges_DynamicCore::is_final_witness(const Witness &witness) {
    if (const HasMultipleEdges_Witness *e = dynamic_cast<const HasMultipleEdges_Witness *>(&witness)) {
        HasMultipleEdges_WitnessPointerConst w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}


extern "C" {
DynamicCore * create() {
    return new HasMultipleEdges_DynamicCore;
}
}
