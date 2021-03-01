// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "ChromaticNumber_AtMost.h"



bool ChromaticNumber_AtMost_Witness::is_equal_implementation(const ChromaticNumber_AtMost_WitnessPointerConst w) const{
    return this->coloring == w->coloring;
}

bool ChromaticNumber_AtMost_Witness::is_less_implementation(const ChromaticNumber_AtMost_WitnessPointerConst w) const{
       return this->coloring < w->coloring;
}

Witness& ChromaticNumber_AtMost_Witness::set_equal_implementation(ChromaticNumber_AtMost_WitnessPointer w){
    this->coloring = w->coloring; 
    return *this;
}

void ChromaticNumber_AtMost_Witness::print() {
    cout<<"{";
    for (map<unsigned,unsigned>::iterator it = coloring.begin(); it != coloring.end() ; ++it) {
        cout<< it->first <<" -> "<< it->second << ", ";
    }
    cout<<"}";
}


string ChromaticNumber_AtMost_Witness::witnessInformation() {
    string info;
    info = "{";
    for (map<unsigned,unsigned>::iterator it = coloring.begin(); it != coloring.end() ; ++it) {
        info = info + to_string( it->first) + " -> " + to_string( it->second) + ", ";
    }
    info= info + "}";
    return info;
}

shared_ptr<Witness> ChromaticNumber_AtMost_Witness::relabel(map<unsigned,unsigned> relabelingMap){
    shared_ptr<ChromaticNumber_AtMost_Witness> relabeledWitness(new ChromaticNumber_AtMost_Witness);
    for (auto it = this->coloring.begin(); it!=this->coloring.end();it++){
        pair<unsigned,unsigned> relabeledPair;
        if(relabelingMap.count(it->first)){
            relabeledPair.first = relabelingMap[it->first];
            relabeledPair.second = it->second;
            relabeledWitness->coloring.insert(relabeledPair);
        }else{
            cout<<"Error: ChromaticNumber_AtMost_Witness::relabel "<< it->first << " is not in the map"<<endl;
            exit(20);
        }
    }
    return relabeledWitness;
}


ChromaticNumber_AtMost_DynamicCore::ChromaticNumber_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","ChromaticNumber");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

ChromaticNumber_AtMost_DynamicCore::ChromaticNumber_AtMost_DynamicCore(unsigned k){
    this->k = k;
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","ChromaticNumber");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

void ChromaticNumber_AtMost_DynamicCore::createInitialWitnessSet_implementation(){
    shared_ptr<ChromaticNumber_AtMost_Witness> witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
}

void ChromaticNumber_AtMost_DynamicCore::intro_v_implementation(unsigned i, Bag &b,
                                                                ChromaticNumber_AtMost_WitnessPointer w, ChromaticNumber_AtMost_WitnessSetPointer witnessSet){
    for (unsigned color = 1; color < k+1 ; color++) {
        ChromaticNumber_AtMost_WitnessPointer witness = createWitness();
        witness->coloring = w->coloring;
        witness->coloring.insert(make_pair(i,color));
        witnessSet->insert(witness);
    }
}

void ChromaticNumber_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                                ChromaticNumber_AtMost_WitnessPointer w,
                                                                ChromaticNumber_AtMost_WitnessSetPointer witnessSet) {
    if(w->coloring.find(i)->second != w->coloring.find(j)->second){
        witnessSet->insert(w);
    }
}

void ChromaticNumber_AtMost_DynamicCore::forget_v_implementation(unsigned i, Bag &b,
                                                                 ChromaticNumber_AtMost_WitnessPointer w,ChromaticNumber_AtMost_WitnessSetPointer witnessSet){
    ChromaticNumber_AtMost_WitnessPointer witness = createWitness();
    witness->coloring = w->coloring;
    witness->coloring.erase(i);
    witnessSet->insert(witness);
}

void ChromaticNumber_AtMost_DynamicCore::join_implementation(Bag &b, ChromaticNumber_AtMost_WitnessPointer w1,
                                                             ChromaticNumber_AtMost_WitnessPointer w2, ChromaticNumber_AtMost_WitnessSetPointer witnessSet){
    if(w1->coloring == w2->coloring){
        witnessSet->insert(w1);
    }
}

bool ChromaticNumber_AtMost_DynamicCore::is_final_witness_implementation(ChromaticNumber_AtMost_WitnessPointer w){
    return true;
}

ChromaticNumber_AtMost_WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::clean_implementation(
        ChromaticNumber_AtMost_WitnessSetPointer witnessSet){
    return witnessSet;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ChromaticNumber_AtMost_Witness::is_equal(const Witness &rhs) const{
    if(ChromaticNumber_AtMost_Witness const *e = dynamic_cast<ChromaticNumber_AtMost_Witness const *>(&rhs)) {
        shared_ptr<ChromaticNumber_AtMost_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool ChromaticNumber_AtMost_Witness::is_less(const Witness &rhs)const {
    if (ChromaticNumber_AtMost_Witness const *e = dynamic_cast<ChromaticNumber_AtMost_Witness const *>(&rhs)) {
        shared_ptr<ChromaticNumber_AtMost_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &ChromaticNumber_AtMost_Witness::set_equal(Witness &witness) {
   if (ChromaticNumber_AtMost_Witness  *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
        shared_ptr<ChromaticNumber_AtMost_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}


void ChromaticNumber_AtMost_DynamicCore::createInitialWitnessSet() {
    createInitialWitnessSet_implementation();  
}

void ChromaticNumber_AtMost_DynamicCore::copyWitness(ChromaticNumber_AtMost_WitnessPointer w_input,
        ChromaticNumber_AtMost_WitnessPointer w_output){

    w_output->set_equal_implementation(w_input);
}

ChromaticNumber_AtMost_WitnessPointer ChromaticNumber_AtMost_DynamicCore::createWitness(){
    ChromaticNumber_AtMost_WitnessPointer w (new ChromaticNumber_AtMost_Witness);
    return w;
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (ChromaticNumber_AtMost_Witness *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
       // shared_ptr<ChromaticNumber_AtMost_Witness> p = make_shared<ChromaticNumber_AtMost_Witness>(*e);
        ChromaticNumber_AtMost_WitnessPointer w = e->shared_from_this();
        ChromaticNumber_AtMost_WitnessSetPointer witnessSet(new ChromaticNumber_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
     if (ChromaticNumber_AtMost_Witness *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
       // shared_ptr<ChromaticNumber_AtMost_Witness> p = make_shared<ChromaticNumber_AtMost_Witness>(*e);
        ChromaticNumber_AtMost_WitnessPointer w = e->shared_from_this();
        ChromaticNumber_AtMost_WitnessSetPointer witnessSet(new ChromaticNumber_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
     }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::intro_e cast error"<<endl;
        exit(20);
     }
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (ChromaticNumber_AtMost_Witness *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
       // shared_ptr<ChromaticNumber_AtMost_Witness> p = make_shared<ChromaticNumber_AtMost_Witness>(*e);
        ChromaticNumber_AtMost_WitnessPointer w = e->shared_from_this();
        ChromaticNumber_AtMost_WitnessSetPointer witnessSet(new ChromaticNumber_AtMost_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::forget_v cast error"<<endl;
        exit(20);
    } 
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (ChromaticNumber_AtMost_Witness  *e1 = dynamic_cast<ChromaticNumber_AtMost_Witness  *>(&witness1)){
          if(ChromaticNumber_AtMost_Witness  *e2 = dynamic_cast<ChromaticNumber_AtMost_Witness  *>(&witness2)) {
              shared_ptr<ChromaticNumber_AtMost_Witness> w1 = e1->shared_from_this();
              shared_ptr<ChromaticNumber_AtMost_Witness> w2 = e2->shared_from_this();
              shared_ptr<ChromaticNumber_AtMost_WitnessSet> witnessSet(new ChromaticNumber_AtMost_WitnessSet);
              join_implementation(b,w1,w2,witnessSet);
              return clean(witnessSet);
          }else{
              cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::join cast error"<<endl;
              exit(20);
          }
      }else{
          cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::join cast error"<<endl;
          exit(20);
      }
}

bool ChromaticNumber_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (ChromaticNumber_AtMost_Witness *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
        ChromaticNumber_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::clean(WitnessSetPointer witnessSet) {
     if (ChromaticNumber_AtMost_WitnessSetPointer e = dynamic_pointer_cast<ChromaticNumber_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> ChromaticNumber_AtMost_WitnessSet::createEmptyWitnessSet() {
    ChromaticNumber_AtMost_WitnessSetPointer witnessSet(new ChromaticNumber_AtMost_WitnessSet);
    return witnessSet;
}
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new ChromaticNumber_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new ChromaticNumber_AtMost_DynamicCore(param);
}
}