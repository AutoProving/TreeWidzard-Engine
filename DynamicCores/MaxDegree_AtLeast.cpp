// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "MaxDegree_AtLeast.h"
extern "C" {
    DynamicCore * create() {
        return new MaxDegree_AtLeast_DynamicCore;
    }
    DynamicCore * create_int(unsigned param) {
        return new MaxDegree_AtLeast_DynamicCore(param);
    }
}

MaxDegree_AtLeast_Witness::MaxDegree_AtLeast_Witness() {
    found =false;
}

void MaxDegree_AtLeast_Witness::print() {
    cout<<"{";

    for(map<unsigned,unsigned>::iterator it = degreeCounter.begin(); it != degreeCounter.end() ; it++){
        auto tempIt = it;
        cout<<it->first << " -> "<< it->second;
        if(++tempIt != degreeCounter.end()){
            cout<<", ";
        }
    }
    cout<<"} "<<"found : "<<found;
}
bool MaxDegree_AtLeast_Witness::is_equal_implementation(const MaxDegree_AtLeast_WitnessPointerConst w) const {
    if(found == w->found and (degreeCounter==w->degreeCounter)){
        return  true;
    } else{
        return false;
    }
}
bool MaxDegree_AtLeast_Witness::is_equal(const Witness &rhs)const {
    if(MaxDegree_AtLeast_Witness const *e = dynamic_cast<MaxDegree_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<MaxDegree_AtLeast_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}
bool MaxDegree_AtLeast_Witness::is_less_implementation(const MaxDegree_AtLeast_WitnessPointerConst w) const {
    if(this->found < w->found or (this->found == w->found and this->degreeCounter < w->degreeCounter)){
        return true;
    }else{
        return false;
    }
}

bool MaxDegree_AtLeast_Witness::is_less(const  Witness &rhs)const {
    if (MaxDegree_AtLeast_Witness const *e = dynamic_cast<MaxDegree_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<MaxDegree_AtLeast_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness & MaxDegree_AtLeast_Witness::set_equal_implementation(MaxDegree_AtLeast_WitnessPointer w) {
    found = w->found;
    degreeCounter = w->degreeCounter;
    return *this;
}
Witness &MaxDegree_AtLeast_Witness::set_equal(Witness &witness) {
    if (MaxDegree_AtLeast_Witness  *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        shared_ptr<MaxDegree_AtLeast_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

MaxDegree_AtLeast_Witness::~MaxDegree_AtLeast_Witness() {

}

MaxDegree_AtLeast_DynamicCore::MaxDegree_AtLeast_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","MaxDegree");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");
}

MaxDegree_AtLeast_DynamicCore::MaxDegree_AtLeast_DynamicCore(unsigned maxDegree){
    this->maxDegree = maxDegree;
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","MaxDegree");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");
}
void MaxDegree_AtLeast_DynamicCore::createInitialWitnessSet_implementation() {
    MaxDegree_AtLeast_WitnessPointer w = createWitness();
    this->insertIntoInitialWitnessSet(w);
}

void MaxDegree_AtLeast_DynamicCore::createInitialWitnessSet() {
    shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
    w->found=false;
    shared_ptr<MaxDegree_AtLeast_WitnessSet> witnessSet(new MaxDegree_AtLeast_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

void MaxDegree_AtLeast_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, MaxDegree_AtLeast_WitnessPointer w,
                                                           MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
    if(w->found==true){
        witnessSet->insert(w);
    }else{
        shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
        w->found=false;
        w->degreeCounter = w->degreeCounter;
        w->degreeCounter.insert(make_pair(i,0));
        witnessSet->insert(w);
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
       // shared_ptr<MaxDegree_AtLeast_Witness> p = make_shared<MaxDegree_AtLeast_Witness>(*e);
        MaxDegree_AtLeast_WitnessPointer w = e->shared_from_this();
        MaxDegree_AtLeast_WitnessSetPointer witnessSet(new MaxDegree_AtLeast_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

void MaxDegree_AtLeast_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                           MaxDegree_AtLeast_WitnessPointer w,
                                                           MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
    if(w->found == true){
        witnessSet->insert(w);
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

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::intro_e(unsigned i , unsigned j, Bag &b, Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        MaxDegree_AtLeast_WitnessPointer w = e->shared_from_this();
        MaxDegree_AtLeast_WitnessSetPointer witnessSet(new MaxDegree_AtLeast_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}

void MaxDegree_AtLeast_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, MaxDegree_AtLeast_WitnessPointer w,
                                                            MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
    if(w->found == true){
        witnessSet->insert(w);
    }else{
        MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
        wPrime->found = false;
        wPrime->degreeCounter = w->degreeCounter;
        wPrime->degreeCounter.erase(i);
        witnessSet->insert(wPrime);
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        MaxDegree_AtLeast_WitnessPointer w = e->shared_from_this();
        MaxDegree_AtLeast_WitnessSetPointer witnessSet(new MaxDegree_AtLeast_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

void MaxDegree_AtLeast_DynamicCore::join_implementation(Bag &b, MaxDegree_AtLeast_WitnessPointer w1,
                                                        MaxDegree_AtLeast_WitnessPointer w2,
                                                        MaxDegree_AtLeast_WitnessSetPointer witnessSet) {
    MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
    if (w1->found == true ){
        witnessSet->insert(w1);
    } else if( w2->found == true){
        witnessSet->insert(w2);
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
            witnessSet->insert(wPrime);
        }
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (MaxDegree_AtLeast_Witness  *e1 = dynamic_cast<MaxDegree_AtLeast_Witness  *>(&witness1)){
        if(MaxDegree_AtLeast_Witness  *e2 = dynamic_cast<MaxDegree_AtLeast_Witness  *>(&witness2)) {
            shared_ptr<MaxDegree_AtLeast_Witness> w1 = e1->shared_from_this();
            shared_ptr<MaxDegree_AtLeast_Witness> w2 = e2->shared_from_this();
            shared_ptr<MaxDegree_AtLeast_WitnessSet> witnessSet(new MaxDegree_AtLeast_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::join cast error"<<endl;
        exit(20);
    }
}

bool MaxDegree_AtLeast_DynamicCore::is_final_witness_implementation(MaxDegree_AtLeast_WitnessPointer w) {
    return w->found;
}

bool MaxDegree_AtLeast_DynamicCore::is_final_witness(Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        MaxDegree_AtLeast_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::clean(WitnessSetPointer witnessSet) {
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
