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


#include "NVertices_AtMost.h"

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



NVertices_AtMost_Witness::NVertices_AtMost_Witness() {
    this->nVertices = 0;
}

void NVertices_AtMost_Witness::print() {
    std::cout<<this->nVertices;
}

string NVertices_AtMost_Witness::witnessInformation() {
    string info = std::to_string(this->nVertices); 
    return info;
}

bool NVertices_AtMost_Witness::is_equal_implementation(const NVertices_AtMost_WitnessPointerConst w) const {
    if(this->nVertices == w->nVertices){
        return  true;
    } else{
        return false;
    }
}
bool NVertices_AtMost_Witness::is_less_implementation(const NVertices_AtMost_WitnessPointerConst w) const {
    // True is smaller than false. In this way, in the set container, the accepting witnesses come first.
    if(this->nVertices < w->nVertices){
        return true;
    }else{
        return false;
    }
}

Witness & NVertices_AtMost_Witness::set_equal_implementation(NVertices_AtMost_WitnessPointer w) {
    this->nVertices  = w->nVertices; 
    return *this;
}

shared_ptr<Witness> NVertices_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    NVertices_AtMost_WitnessPointer w (new NVertices_AtMost_Witness);
    w->nVertices = this->nVertices; 
    return w;
}

NVertices_AtMost_Witness::~NVertices_AtMost_Witness() {

}

NVertices_AtMost_DynamicCore::NVertices_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","NVertices");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

NVertices_AtMost_DynamicCore::NVertices_AtMost_DynamicCore(unsigned nVertices){
    this->nVertices = nVertices;
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","NVertices");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

void NVertices_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    NVertices_AtMost_WitnessPointer w = createWitness();
    w->nVertices = 0;
    this->insertIntoInitialWitnessSet(w);
}

void NVertices_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, NVertices_AtMost_WitnessPointer w,
                                                          NVertices_AtMost_WitnessSetPointer witnessSet) {
    shared_ptr<NVertices_AtMost_Witness> witness(new NVertices_AtMost_Witness);
    witness->nVertices = (w->nVertices+1);
    witnessSet->insert(witness);
}

void NVertices_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                          NVertices_AtMost_WitnessPointer w,
                                                          NVertices_AtMost_WitnessSetPointer witnessSet) {
    witnessSet->insert(w);
}

void NVertices_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, NVertices_AtMost_WitnessPointer w,
                                                           NVertices_AtMost_WitnessSetPointer witnessSet) {

    witnessSet->insert(w);
}

void NVertices_AtMost_DynamicCore::join_implementation(Bag &b, NVertices_AtMost_WitnessPointer w1,
                                                       NVertices_AtMost_WitnessPointer w2,
                                                       NVertices_AtMost_WitnessSetPointer witnessSet) {
    NVertices_AtMost_WitnessPointer wPrime = createWitness();
    int nVerticesPrime  = (w1->nVertices + w2->nVertices - b.get_elements().size());
    wPrime->nVertices = nVerticesPrime;
    witnessSet->insert(wPrime);
}

bool NVertices_AtMost_DynamicCore::is_final_witness_implementation(NVertices_AtMost_WitnessPointer w) {
    if(w->nVertices <= this->nVertices){
        return true;
    }else{
        return false;
    }
}

shared_ptr<WitnessSet> NVertices_AtMost_DynamicCore::clean_implementation(NVertices_AtMost_WitnessSetPointer witnessSet) {
    return witnessSet;
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool NVertices_AtMost_Witness::is_equal(const Witness &rhs)const {
    if(NVertices_AtMost_Witness const *e = dynamic_cast<NVertices_AtMost_Witness const *>(&rhs)) {
        shared_ptr<NVertices_AtMost_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool NVertices_AtMost_Witness::is_less(const  Witness &rhs)const {
    if (NVertices_AtMost_Witness const *e = dynamic_cast<NVertices_AtMost_Witness const *>(&rhs)) {
        shared_ptr<NVertices_AtMost_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &NVertices_AtMost_Witness::set_equal(Witness &witness) {
    if (NVertices_AtMost_Witness  *e = dynamic_cast<NVertices_AtMost_Witness *>(&witness)) {
        shared_ptr<NVertices_AtMost_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> NVertices_AtMost_WitnessSet::createEmptyWitnessSet() {
    NVertices_AtMost_WitnessSetPointer witnessSet(new NVertices_AtMost_WitnessSet);
    return witnessSet;
}

NVertices_AtMost_WitnessPointer NVertices_AtMost_DynamicCore::createWitness() {
    NVertices_AtMost_WitnessPointer w(new NVertices_AtMost_Witness);
    return w;
}

void NVertices_AtMost_DynamicCore::copyWitness(NVertices_AtMost_WitnessPointer w_input,
                                               NVertices_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void NVertices_AtMost_DynamicCore::createInitialWitnessSet() {
    shared_ptr<NVertices_AtMost_WitnessSet> witnessSet(new NVertices_AtMost_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

WitnessSetPointer NVertices_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (NVertices_AtMost_Witness *e = dynamic_cast<NVertices_AtMost_Witness *>(&witness)) {
        // shared_ptr<NVertices_AtMost_Witness> p = make_shared<NVertices_AtMost_Witness>(*e);
        NVertices_AtMost_WitnessPointer w = e->shared_from_this();
        NVertices_AtMost_WitnessSetPointer witnessSet(new NVertices_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer NVertices_AtMost_DynamicCore::intro_e(unsigned i , unsigned j, Bag &b, Witness &witness) {
    if (NVertices_AtMost_Witness *e = dynamic_cast<NVertices_AtMost_Witness *>(&witness)) {
        NVertices_AtMost_WitnessPointer w = e->shared_from_this();
        NVertices_AtMost_WitnessSetPointer witnessSet(new NVertices_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}


WitnessSetPointer NVertices_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (NVertices_AtMost_Witness *e = dynamic_cast<NVertices_AtMost_Witness *>(&witness)) {
        NVertices_AtMost_WitnessPointer w = e->shared_from_this();
        NVertices_AtMost_WitnessSetPointer witnessSet(new NVertices_AtMost_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer NVertices_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (NVertices_AtMost_Witness  *e1 = dynamic_cast<NVertices_AtMost_Witness  *>(&witness1)){
        if(NVertices_AtMost_Witness  *e2 = dynamic_cast<NVertices_AtMost_Witness  *>(&witness2)) {
            shared_ptr<NVertices_AtMost_Witness> w1 = e1->shared_from_this();
            shared_ptr<NVertices_AtMost_Witness> w2 = e2->shared_from_this();
            shared_ptr<NVertices_AtMost_WitnessSet> witnessSet(new NVertices_AtMost_WitnessSet);
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


bool NVertices_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (NVertices_AtMost_Witness *e = dynamic_cast<NVertices_AtMost_Witness *>(&witness)) {
        NVertices_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> NVertices_AtMost_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    if (NVertices_AtMost_WitnessSetPointer e = dynamic_pointer_cast<NVertices_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in NVertices_AtMost_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore * create() {
    return new NVertices_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned nVertices) {
    return new NVertices_AtMost_DynamicCore(nVertices);
}
}


