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


#include "Mod.h"

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



Mod_Witness::Mod_Witness() {
    this->remainder = 0;
}

void Mod_Witness::print() {
    std::cout<<this->remainder;
}

string Mod_Witness::witnessInformation() {
    string info = std::to_string(this->remainder); 
    return info;
}

bool Mod_Witness::is_equal_implementation(const Mod_WitnessPointerConst w) const {
    if(this->remainder == w->remainder){
        return  true;
    } else{
        return false;
    }
}
bool Mod_Witness::is_less_implementation(const Mod_WitnessPointerConst w) const {
    // True is smaller than false. In this way, in the set container, the accepting witnesses come first.
    if(this->remainder < w->remainder){
        return true;
    }else{
        return false;
    }
}

Witness & Mod_Witness::set_equal_implementation(Mod_WitnessPointer w) {
    this->remainder  = w->remainder; 
    return *this;
}

shared_ptr<Witness> Mod_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    Mod_WitnessPointer w (new Mod_Witness);
    w->remainder = this->remainder; 
    return w;
}

Mod_Witness::~Mod_Witness() {

}

Mod_DynamicCore::Mod_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","Mod");
    addAttribute("ParameterType","MultiParameter");
    addAttribute("PrimaryOperator","-");
}

Mod_DynamicCore::Mod_DynamicCore(vector<int> parameters){
    if(parameters.size()!=2){
        cout<<"Error: Mod Core has a invalid parameters."
            <<"The parameters should be a vector of unsigned where the first cordinate is divisor,"
                <<"and the second cordinate is remainder"<<endl;
        exit(20);
    }
    this->divisor = parameters[0];
    this->remainder = parameters[1];
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","Mod");
    addAttribute("ParameterType","MultiParameter");
    addAttribute("PrimaryOperator","-");
}

void Mod_DynamicCore::createInitialWitnessSet_implementation() {
    Mod_WitnessPointer w = createWitness();
    w->remainder = 0;
    this->insertIntoInitialWitnessSet(w);
}

void Mod_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, Mod_WitnessPointer w,
                                                          Mod_WitnessSetPointer witnessSet) {
    shared_ptr<Mod_Witness> witness(new Mod_Witness);
    witness->remainder = (w->remainder+1)%divisor;
    witnessSet->insert(witness);
}

void Mod_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                          Mod_WitnessPointer w,
                                                          Mod_WitnessSetPointer witnessSet) {
    witnessSet->insert(w);
}

void Mod_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, Mod_WitnessPointer w,
                                                           Mod_WitnessSetPointer witnessSet) {

    witnessSet->insert(w);
}

void Mod_DynamicCore::join_implementation(Bag &b, Mod_WitnessPointer w1,
                                                       Mod_WitnessPointer w2,
                                                       Mod_WitnessSetPointer witnessSet) {
    Mod_WitnessPointer wPrime = createWitness();
    int remainderPrime  = (w1->remainder + w2->remainder - b.get_elements().size())%divisor;
    if(remainderPrime < 0 ){
        remainderPrime  = divisor - remainderPrime;
    } 
    wPrime->remainder = remainderPrime;
    witnessSet->insert(wPrime);
}

bool Mod_DynamicCore::is_final_witness_implementation(Mod_WitnessPointer w) {
    if(w->remainder == this->remainder){
        return true;
    }else{
        return false;
    }
}

shared_ptr<WitnessSet> Mod_DynamicCore::clean_implementation(Mod_WitnessSetPointer witnessSet) {
    return witnessSet;
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool Mod_Witness::is_equal(const Witness &rhs)const {
    if(Mod_Witness const *e = dynamic_cast<Mod_Witness const *>(&rhs)) {
        shared_ptr<Mod_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool Mod_Witness::is_less(const  Witness &rhs)const {
    if (Mod_Witness const *e = dynamic_cast<Mod_Witness const *>(&rhs)) {
        shared_ptr<Mod_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &Mod_Witness::set_equal(Witness &witness) {
    if (Mod_Witness  *e = dynamic_cast<Mod_Witness *>(&witness)) {
        shared_ptr<Mod_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> Mod_WitnessSet::createEmptyWitnessSet() {
    Mod_WitnessSetPointer witnessSet(new Mod_WitnessSet);
    return witnessSet;
}

Mod_WitnessPointer Mod_DynamicCore::createWitness() {
    Mod_WitnessPointer w(new Mod_Witness);
    return w;
}

void Mod_DynamicCore::copyWitness(Mod_WitnessPointer w_input,
                                               Mod_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void Mod_DynamicCore::createInitialWitnessSet() {
    shared_ptr<Mod_WitnessSet> witnessSet(new Mod_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

WitnessSetPointer Mod_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (Mod_Witness *e = dynamic_cast<Mod_Witness *>(&witness)) {
        // shared_ptr<Mod_Witness> p = make_shared<Mod_Witness>(*e);
        Mod_WitnessPointer w = e->shared_from_this();
        Mod_WitnessSetPointer witnessSet(new Mod_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer Mod_DynamicCore::intro_e(unsigned i , unsigned j, Bag &b, Witness &witness) {
    if (Mod_Witness *e = dynamic_cast<Mod_Witness *>(&witness)) {
        Mod_WitnessPointer w = e->shared_from_this();
        Mod_WitnessSetPointer witnessSet(new Mod_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}


WitnessSetPointer Mod_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (Mod_Witness *e = dynamic_cast<Mod_Witness *>(&witness)) {
        Mod_WitnessPointer w = e->shared_from_this();
        Mod_WitnessSetPointer witnessSet(new Mod_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer Mod_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (Mod_Witness  *e1 = dynamic_cast<Mod_Witness  *>(&witness1)){
        if(Mod_Witness  *e2 = dynamic_cast<Mod_Witness  *>(&witness2)) {
            shared_ptr<Mod_Witness> w1 = e1->shared_from_this();
            shared_ptr<Mod_Witness> w2 = e2->shared_from_this();
            shared_ptr<Mod_WitnessSet> witnessSet(new Mod_WitnessSet);
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


bool Mod_DynamicCore::is_final_witness(Witness &witness) {
    if (Mod_Witness *e = dynamic_cast<Mod_Witness *>(&witness)) {
        Mod_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> Mod_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    if (Mod_WitnessSetPointer e = dynamic_pointer_cast<Mod_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in Mod_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore * create() {
    return new Mod_DynamicCore;
}
DynamicCore * create_parameters(vector<int> parameters) {
    return new Mod_DynamicCore(parameters);
}
}


