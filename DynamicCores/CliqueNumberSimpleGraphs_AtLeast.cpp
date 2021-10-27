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

Witness& CliqueNumberSimpleGraphs_AtLeast_Witness::set_equal_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w){
    this->found = w->found;
    this->size = w->size;
    this->partialClique = w->partialClique;
    return *this;
}



shared_ptr<Witness> CliqueNumberSimpleGraphs_AtLeast_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    if(this->found){
        return this->shared_from_this();
    }else{
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointer relabeledWitness (new CliqueNumberSimpleGraphs_AtLeast_Witness);
        relabeledWitness->found = false;
        relabeledWitness->size = this->size;
        for(auto p:this->partialClique){
            if(relabelingMap.count(p.first)){
                relabeledWitness->partialClique.insert(make_pair(relabelingMap[p.first],p.second));
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
}

void CliqueNumberSimpleGraphs_AtLeast_Witness::print() {
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

string CliqueNumberSimpleGraphs_AtLeast_Witness::witnessInformation() {
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

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::createInitialWitnessSet_implementation() {
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w = createWitness();
    w->found = false;
    w->size = 0;
    this->insertIntoInitialWitnessSet(w);
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w,
            CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
     if(w->found or w->size == this->cliqueSize){
        witnessSet->insert(w);
     }else{
         // Either i can be added to the clique or not.
         witnessSet->insert(w);
         // A new vertex only can be added to the clique when no vertex has been forgotten from the clique.
        // This means number of vertices in the current clique should be equal to size
         if(w->size == w->partialClique.size()){
            CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
            witness->set_equal(*w);
            witness->partialClique.insert(make_pair(i,0));
            witness->size = w->size+1;
            witnessSet->insert(witness);
        }
     }
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b,
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
    if (w->found){
            witnessSet->insert(w);
    }else{
       if(w->partialClique.count(i) and w->partialClique.count(j)){
           // Both i and j are in the clique, so that the counters of i and j will be increased by one.
            CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
            witness->set_equal(*w);
            unsigned iCounter = w->partialClique[i];
            unsigned jCounter = w->partialClique[j];
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
           // Either i or j is not in the clique, so that {i,j} cannpt be added to the clique.
            witnessSet->insert(w);
       }
    }
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::forget_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w,
        CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
    if(w->found){
        witnessSet->insert(w);
    }else{
        if(w->partialClique.count(i)){
            // A vertex only can be forgotten when the size of the clique is equal to cliqueSize.
            // And the forgotten vertex has cliqueSize-1 neighbors.
            if(w->size==cliqueSize and w->partialClique[i]==cliqueSize-1 ){
                CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
                witness->set_equal(*w);
                witness->partialClique.erase(i);
                witnessSet->insert(witness);
            }else{
                // invalid witness
                return;
            }
        }else{
            // i is not in the clique
            witnessSet->insert(w);
        }
    }
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::join_implementation(Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w1,
                                                                       CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w2, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
    if (w1->found){
        witnessSet->insert(w1);
    } else if (w2->found){
        witnessSet->insert(w2);
    } else{
        // At the same time it is not possible to have forget vertices in both witnesses
        // If there is a forgotten vertex v in w1, and a forgotten vertex u in w2, then v and u can not be connected to together
        if (w1->partialClique.size()==w1->size or w2->partialClique.size() == w2->size) {
            if (w1->partialClique.size() == w2->partialClique.size()) {
                CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
                witness->set_equal(*w1);
                witness->size = w1->size + w2->size - w1->partialClique.size();
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
                // if yes found will be true and partialClique will be cleared.
                if (isCompleteClique(witness)) {
                    witness->found = true;
                }
                witnessSet->insert(witness);
            } else {
                return;
            }
        }else{
            return;
        }
//      if(w1->size == w1->partialClique.size() and w2->size == w2->partialClique.size() and w1->size == w2->size){
//          CliqueNumberSimpleGraphs_AtLeast_WitnessPointer witness = createWitness();
//          witness->set_equal(*w1);
//          for(auto p:w2->partialClique){
//              if(witness->partialClique.count(p.first)){
//                  unsigned pCounter = p.second + witness->partialClique[p.first];
//                  witness->partialClique.erase(p.first);
//                  witness->partialClique.insert(make_pair(p.first,pCounter));
//              }else{
//                  // w1 and w2 have different domains, so they cannot be joined.
//                  return;
//              }
//          }
//          // Check that the clique is a complete clique or not,
//          // if yes found will be true and partialClique will be cleared.
//          if(isCompleteClique(witness)){
//              witness->found = true;
//              witness->partialClique.clear();
//          }
//          witnessSet->insert(witness);
//       }
    }
}

CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::clean_implementation
                                                    (CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet){
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

bool CliqueNumberSimpleGraphs_AtLeast_DynamicCore::is_final_witness_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w){
    return w->found;
}

bool CliqueNumberSimpleGraphs_AtLeast_DynamicCore::isCompleteClique(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w){

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

///////////////////////////////////////////////////////////////////////////////////////////

bool CliqueNumberSimpleGraphs_AtLeast_Witness::is_equal(const Witness &rhs) const{
    if(CliqueNumberSimpleGraphs_AtLeast_Witness const *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool CliqueNumberSimpleGraphs_AtLeast_Witness::is_less(const Witness &rhs)const {
    if (CliqueNumberSimpleGraphs_AtLeast_Witness const *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &CliqueNumberSimpleGraphs_AtLeast_Witness::set_equal(Witness &witness) {
   if (CliqueNumberSimpleGraphs_AtLeast_Witness  *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)) {
        shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::is_less cast error"<<endl;
        exit(20);
    }
}



/*void CliqueNumberSimpleGraphs_AtLeast_WitnessSet::print() {
    for (WitnessSetIterator it = this->begin() ; it != this->end() ; ++it) {
        (*it)->print();
    }

}*/

/*WitnessSet *CliqueNumberSimpleGraphs_AtLeast_WitnessSet::createWitnessSet() {
    CliqueNumberSimpleGraphs_AtLeast_WitnessSet *w = new CliqueNumberSimpleGraphs_AtLeast_WitnessSet;
    return w;
}*/

/*bool CliqueNumberSimpleGraphs_AtLeast_WitnessSet::is_equal(WitnessSet &rhs) {
    if (CliqueNumberSimpleGraphs_AtLeast_WitnessSet *p = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_WitnessSet *>(&rhs)) {
        if(this->container.size() == p->container.size()){
            WitnessSetIterator itr = p->begin();
            for(WitnessSetIterator it = this->begin(); it != this->end() ; ++it){
                if((**it == **itr)){
                    itr++;
                }else{
                    return false;
                }
            }
            return true;
        }else{
            return false;
        }
    }else{
        cerr<<"ERROR: in Dynamic_CliqueNumberSimpleGraphs_WitnessSet::is_equal cast error"<<endl;
        exit(20);
    }
}*/



void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::createInitialWitnessSet() {
    CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

void CliqueNumberSimpleGraphs_AtLeast_DynamicCore::copyWitness(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_input,
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_output){

    w_output->set_equal_implementation(w_input);
}

CliqueNumberSimpleGraphs_AtLeast_WitnessPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::createWitness(){
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w (new CliqueNumberSimpleGraphs_AtLeast_Witness);
    return w;
}

WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
     if (CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)) {
       // shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> p = make_shared<CliqueNumberSimpleGraphs_AtLeast_Witness>(*e);
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w = e->shared_from_this();
        CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
     if (CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)) {
       // shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> p = make_shared<CliqueNumberSimpleGraphs_AtLeast_Witness>(*e);
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w = e->shared_from_this();
        CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
   if (CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)) {
       // shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> p = make_shared<CliqueNumberSimpleGraphs_AtLeast_Witness>(*e);
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w = e->shared_from_this();
        CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer CliqueNumberSimpleGraphs_AtLeast_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (CliqueNumberSimpleGraphs_AtLeast_Witness  *e1 = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness  *>(&witness1)){
          if(CliqueNumberSimpleGraphs_AtLeast_Witness  *e2 = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness  *>(&witness2)) {
              shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> w1 = e1->shared_from_this();
              shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> w2 = e2->shared_from_this();
              shared_ptr<CliqueNumberSimpleGraphs_AtLeast_WitnessSet> witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
              join_implementation(b,w1,w2,witnessSet);
              return clean(witnessSet);
          }else{
              cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::join cast error"<<endl;
              exit(20);
          }
      }else{
          cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::join cast error"<<endl;
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

bool CliqueNumberSimpleGraphs_AtLeast_DynamicCore::is_final_witness(Witness &witness) {

    if (CliqueNumberSimpleGraphs_AtLeast_Witness *e = dynamic_cast<CliqueNumberSimpleGraphs_AtLeast_Witness *>(&witness)) {
        CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumberSimpleGraphs_AtLeast_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> CliqueNumberSimpleGraphs_AtLeast_WitnessSet::createEmptyWitnessSet() {
    CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet(new CliqueNumberSimpleGraphs_AtLeast_WitnessSet);
    return witnessSet;
}
///////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new CliqueNumberSimpleGraphs_AtLeast_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new CliqueNumberSimpleGraphs_AtLeast_DynamicCore(param);
}
}
