// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

///////////////////////////////////////////////////////////////////////////////
// Before starting to work on your core, please create a new branch, and rename
// these files according to the instructions in the file README-GenericCore.txt
// In particular "Minor" should be renamed everywhere to as specified in the file.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Please use the following commands for standardization purposes
// To create a witness called "witness": Minor_WitnessPointer witness(new Minor_Witness);
// To copy "witness1" into "witness2": copy(witness1,witness2)
// To insert "witness" into "witnessSet":  witnessSet->insert(witness)
/////////////////////////////////////////////////////////////////////////////


#include "MinDegree_AtMost.h"

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



MinDegree_AtMost_Witness::MinDegree_AtMost_Witness() {
    found =false;
}

void MinDegree_AtMost_Witness::print() {
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
bool MinDegree_AtMost_Witness::is_equal_implementation(const MinDegree_AtMost_WitnessPointerConst w) const {
    if(found == w->found and (degreeCounter==w->degreeCounter)){
        return  true;
    } else{
        return false;
    }
}
bool MinDegree_AtMost_Witness::is_less_implementation(const MinDegree_AtMost_WitnessPointerConst w) const {
    // True is smaller than false. In this way, in the set container, the accepting witnesses come first.
    if(this->found > w->found or (this->found == w->found and this->degreeCounter < w->degreeCounter)){
        return true;
    }else{
        return false;
    }
}

Witness & MinDegree_AtMost_Witness::set_equal_implementation(MinDegree_AtMost_WitnessPointer w) {
    found = w->found;
    degreeCounter = w->degreeCounter;
    return *this;
}

shared_ptr<Witness> MinDegree_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    MinDegree_AtMost_WitnessPointer w (new MinDegree_AtMost_Witness);
    map<unsigned ,unsigned > m;
    if(this->found){
        w->found = true;
    }else{
        for(auto element:this->degreeCounter){
            if(relabelingMap.count(element.first)){
                m.insert(make_pair(relabelingMap[element.first],element.second));
            }else{
                cout<<"Error: MinDegree_AtMost_Witness::relabel " << element.first<< " is not in the map"<<endl;
                cout<<" map:";
                exit(20);
            }
        }
        w->degreeCounter = m;
        w->found = this->found;
    }
    return w;
}

MinDegree_AtMost_Witness::~MinDegree_AtMost_Witness() {

}

MinDegree_AtMost_DynamicCore::MinDegree_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","MinDegree");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

MinDegree_AtMost_DynamicCore::MinDegree_AtMost_DynamicCore(unsigned maxDegree){
    this->parameter = maxDegree;
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","MinDegree");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

void MinDegree_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    MinDegree_AtMost_WitnessPointer w = createWitness();
    this->insertIntoInitialWitnessSet(w);
}

void MinDegree_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, MinDegree_AtMost_WitnessPointer w,
                                                           MinDegree_AtMost_WitnessSetPointer witnessSet) {
    if(w->found){
        witnessSet->insert(w);
    }else{
        shared_ptr<MinDegree_AtMost_Witness> witness(new MinDegree_AtMost_Witness);
        witness->found=false;
        witness->degreeCounter = w->degreeCounter;
        witness->degreeCounter.insert(make_pair(i,0));
        witnessSet->insert(witness);
    }

}

void MinDegree_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                           MinDegree_AtMost_WitnessPointer w,
                                                           MinDegree_AtMost_WitnessSetPointer witnessSet) {
    if(w->found){
        witnessSet->insert(w);
    }else if(i!=j){
        MinDegree_AtMost_WitnessPointer wPrime = createWitness();
        wPrime->degreeCounter = w->degreeCounter;
        wPrime->degreeCounter.erase(i);
        wPrime->degreeCounter.erase(j);
        wPrime->degreeCounter.insert(make_pair(i,min(w->degreeCounter.find(i)->second+1,parameter+1)));
        wPrime->degreeCounter.insert(make_pair(j,min(w->degreeCounter.find(j)->second+1,parameter+1)));
        wPrime->found = false;
        witnessSet->insert(wPrime);
    }else if(i==j){
        MinDegree_AtMost_WitnessPointer wPrime = createWitness();
        wPrime->degreeCounter = w->degreeCounter;
        wPrime->degreeCounter.erase(i);
        wPrime->degreeCounter.insert(make_pair(i,min(w->degreeCounter.find(i)->second+2,parameter+1)));
        witnessSet->insert(wPrime);
    }
}

void MinDegree_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, MinDegree_AtMost_WitnessPointer w,
                                                            MinDegree_AtMost_WitnessSetPointer witnessSet) {
    if(w->found){
        witnessSet->insert(w);
    }else if(w->degreeCounter.find(i)->second <= parameter){
        MinDegree_AtMost_WitnessPointer wPrime = createWitness();
        wPrime->found = true;
        witnessSet->insert(wPrime);
    }else{
        MinDegree_AtMost_WitnessPointer wPrime = createWitness();
        wPrime->found = false;
        wPrime->degreeCounter = w->degreeCounter;
        wPrime->degreeCounter.erase(i);
        witnessSet->insert(wPrime);
    }
}

void MinDegree_AtMost_DynamicCore::join_implementation(Bag &b, MinDegree_AtMost_WitnessPointer w1,
                                                        MinDegree_AtMost_WitnessPointer w2,
                                                        MinDegree_AtMost_WitnessSetPointer witnessSet) {
    MinDegree_AtMost_WitnessPointer wPrime = createWitness();
    if (w1->found ){
        witnessSet->insert(w1);
    } else if( w2->found ){
        witnessSet->insert(w2);
    }else {
        map<unsigned, unsigned> temp1 = w1->degreeCounter;
        map<unsigned, unsigned> temp2 = w2->degreeCounter;
        map<unsigned, unsigned> temp;
        bool found = false;
        map<unsigned, unsigned>::iterator it1 = temp1.begin();
        for (map<unsigned, unsigned>::iterator it2 = temp2.begin(); it2 != temp2.end(); ++it2) {
                temp.insert(make_pair(it1->first, min(it1->second + it2->second,parameter+1)));
                it1++;
        }
        wPrime->found = false;
        wPrime->degreeCounter = temp;
        witnessSet->insert(wPrime);
    }
}

bool MinDegree_AtMost_DynamicCore::is_final_witness_implementation(MinDegree_AtMost_WitnessPointer w) {
    if(w->found){
        return true;
    }else{
        for(auto p:w->degreeCounter)
        {
            if(p.second<= parameter){
                return true;
            }
        }
        return false;
    }
}

shared_ptr<WitnessSet> MinDegree_AtMost_DynamicCore::clean_implementation(MinDegree_AtMost_WitnessSetPointer witnessSet) {
    for(auto witness:(*witnessSet)){
        if (MinDegree_AtMost_WitnessPointer w = dynamic_pointer_cast<MinDegree_AtMost_Witness>(witness)) {
            if(w->found){
                MinDegree_AtMost_WitnessSetPointer newWitnessSet (new MinDegree_AtMost_WitnessSet);
                newWitnessSet->insert(w);
                return newWitnessSet;
            }
        }else{
            cerr<<"ERROR: in MinDegree_AtMost_DynamicCore::clean_implementation cast error"<<endl;
            exit(20);
        }
    }
    return witnessSet;
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool MinDegree_AtMost_Witness::is_equal(const Witness &rhs)const {
    if(MinDegree_AtMost_Witness const *e = dynamic_cast<MinDegree_AtMost_Witness const *>(&rhs)) {
        shared_ptr<MinDegree_AtMost_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool MinDegree_AtMost_Witness::is_less(const  Witness &rhs)const {
    if (MinDegree_AtMost_Witness const *e = dynamic_cast<MinDegree_AtMost_Witness const *>(&rhs)) {
        shared_ptr<MinDegree_AtMost_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &MinDegree_AtMost_Witness::set_equal(Witness &witness) {
    if (MinDegree_AtMost_Witness  *e = dynamic_cast<MinDegree_AtMost_Witness *>(&witness)) {
        shared_ptr<MinDegree_AtMost_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> MinDegree_AtMost_WitnessSet::createEmptyWitnessSet() {
    MinDegree_AtMost_WitnessSetPointer witnessSet(new MinDegree_AtMost_WitnessSet);
    return witnessSet;
}

MinDegree_AtMost_WitnessPointer MinDegree_AtMost_DynamicCore::createWitness() {
    MinDegree_AtMost_WitnessPointer w(new MinDegree_AtMost_Witness);
    return w;
}

void MinDegree_AtMost_DynamicCore::copyWitness(MinDegree_AtMost_WitnessPointer w_input,
                                                MinDegree_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void MinDegree_AtMost_DynamicCore::createInitialWitnessSet() {
    shared_ptr<MinDegree_AtMost_Witness> w(new MinDegree_AtMost_Witness);
    w->found=false;
    shared_ptr<MinDegree_AtMost_WitnessSet> witnessSet(new MinDegree_AtMost_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

WitnessSetPointer MinDegree_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (MinDegree_AtMost_Witness *e = dynamic_cast<MinDegree_AtMost_Witness *>(&witness)) {
       // shared_ptr<MinDegree_AtMost_Witness> p = make_shared<MinDegree_AtMost_Witness>(*e);
        MinDegree_AtMost_WitnessPointer w = e->shared_from_this();
        MinDegree_AtMost_WitnessSetPointer witnessSet(new MinDegree_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer MinDegree_AtMost_DynamicCore::intro_e(unsigned i , unsigned j, Bag &b, Witness &witness) {
    if (MinDegree_AtMost_Witness *e = dynamic_cast<MinDegree_AtMost_Witness *>(&witness)) {
        MinDegree_AtMost_WitnessPointer w = e->shared_from_this();
        MinDegree_AtMost_WitnessSetPointer witnessSet(new MinDegree_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}


WitnessSetPointer MinDegree_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (MinDegree_AtMost_Witness *e = dynamic_cast<MinDegree_AtMost_Witness *>(&witness)) {
        MinDegree_AtMost_WitnessPointer w = e->shared_from_this();
        MinDegree_AtMost_WitnessSetPointer witnessSet(new MinDegree_AtMost_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer MinDegree_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (MinDegree_AtMost_Witness  *e1 = dynamic_cast<MinDegree_AtMost_Witness  *>(&witness1)){
        if(MinDegree_AtMost_Witness  *e2 = dynamic_cast<MinDegree_AtMost_Witness  *>(&witness2)) {
            shared_ptr<MinDegree_AtMost_Witness> w1 = e1->shared_from_this();
            shared_ptr<MinDegree_AtMost_Witness> w2 = e2->shared_from_this();
            shared_ptr<MinDegree_AtMost_WitnessSet> witnessSet(new MinDegree_AtMost_WitnessSet);
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


bool MinDegree_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (MinDegree_AtMost_Witness *e = dynamic_cast<MinDegree_AtMost_Witness *>(&witness)) {
        MinDegree_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> MinDegree_AtMost_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    if (MinDegree_AtMost_WitnessSetPointer e = dynamic_pointer_cast<MinDegree_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in MinDegree_AtMost_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore * create() {
    return new MinDegree_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new MinDegree_AtMost_DynamicCore(param);
}
}


