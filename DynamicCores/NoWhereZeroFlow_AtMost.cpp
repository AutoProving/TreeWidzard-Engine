// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

///////////////////////////////////////////////////////////////////////////////
// Before starting to work on your core, please create a new branch, and rename
// these files according to the instructions in the file README-GenericCore.txt
// In particular "Generic_Operator" should be renamed everywhere to as specified in the file.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Please use the following commands for standardization purposes
// To create a witness called "witness": NoWhereZeroFlow_AtMost_WitnessPointer witness(new NoWhereZeroFlow_AtMost_Witness);
// To copy "witness1" into "witness2": copy(witness1,witness2)
// To insert "witness" into "witnessSet":  witnessSet->insert(witness)
/////////////////////////////////////////////////////////////////////////////


#include "NoWhereZeroFlow_AtMost.h"


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

bool NoWhereZeroFlow_AtMost_Witness::is_equal_implementation(const NoWhereZeroFlow_AtMost_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    return this->flowSum == w->flowSum;
    //*****************************
    //*****************************
}


bool NoWhereZeroFlow_AtMost_Witness::is_less_implementation(const NoWhereZeroFlow_AtMost_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    return this->flowSum < w->flowSum;
    //*****************************
    //*****************************
}

Witness& NoWhereZeroFlow_AtMost_Witness::set_equal_implementation(NoWhereZeroFlow_AtMost_WitnessPointer w){
    //*****************************
    //*****************************
    this->flowSum = w->flowSum;
    return *this;
    //*****************************
    //*****************************
}

shared_ptr<Witness> NoWhereZeroFlow_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    NoWhereZeroFlow_AtMost_WitnessPointer relabeledWitness(new NoWhereZeroFlow_AtMost_Witness);
    for(auto item:this->flowSum){
        if(relabelingMap.count(item.first)){
            relabeledWitness->flowSum.insert(make_pair(relabelingMap[item.first],item.second));
        }else{
            cout<<"Error: NoWhereZeroFlow_AtMost_Witness::relabel "<< item.first <<" is not in the map."<<endl;
            print();
            for(auto m:relabelingMap){
                cout<<m.first<<"->"<<m.second<<endl;
            }
            exit(20);
        }
    }
    return relabeledWitness;
}

void NoWhereZeroFlow_AtMost_Witness::print() {
    //*****************************
    //*****************************
    cout << "FlowSum: ";
    for(auto f:flowSum){
        cout<<"("<<f.first<<"->"<<f.second<<")";
        if(f != *(--flowSum.end())){
            cout<<",";
        }
    }
    //*****************************
    //*****************************
}

string NoWhereZeroFlow_AtMost_Witness::witnessInformation() {
    //*****************************
    //*****************************
    string info;
    info = "FlowSum: ";
    for(auto f:flowSum){
        info = info + "(" + to_string(f.first) + "->" + to_string(f.second)+")";
        if(f != *(--flowSum.end())){
            info = info + ",";
        }
    }
    return info;

    //*****************************
    //*****************************
}

NoWhereZeroFlow_AtMost_Witness::~NoWhereZeroFlow_AtMost_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

NoWhereZeroFlow_AtMost_DynamicCore::NoWhereZeroFlow_AtMost_DynamicCore(){

    //*****************************
    //*****************************
    // Only edit this part
    // In this function you can initialize the map attributes. These attributes are used to describe properties of the core.
    // Example:
    addAttribute("CoreName","NoWhereZeroFlow"); // Obligatory attribute. Replace GenericName by the name of the core.
    addAttribute("ParameterType","UnsignedInt"); // Obligatory attribute. Replace GenericType by the type of the core: "NoParameter", "UnsignedInt", "InputFile".
    addAttribute("PrimaryOperator","AtMost"); //  This line should be uncommented if the type of the core is "UnsignedInt".
    //  Replace GenericOperator with "AtLeast" or "AtMost". The correct initialization depends on the core.
    //  ...
    //  Additional optional attributes.
    //*****************************
    //*****************************

}

NoWhereZeroFlow_AtMost_DynamicCore::NoWhereZeroFlow_AtMost_DynamicCore(unsigned parameter){
    //*****************************
    //*****************************
    addAttribute("CoreName","NoWhereZeroFlow"); // Obligatory attribute. Replace GenericName by the name of the core.
    addAttribute("ParameterType","UnsignedInt"); // Obligatory attribute. Replace GenericType by the type of the core: "NoParameter", "UnsignedInt", "InputFile".
    addAttribute("PrimaryOperator","AtMost"); //  This line should be uncommented if the type of the core is "UnsignedInt".
    //*****************************
    //*****************************
    this->parameter = parameter;
    createInitialWitnessSet();
}
void NoWhereZeroFlow_AtMost_DynamicCore::createInitialWitnessSet_implementation(){
    //*****************************
    //*****************************
    // Here you will initialize initialWitnessSet by adding witnesses to it.
    // In most cases, there will be only one initial witness.
    // The next two lines show how to create a witness and add it to a witnessSet.
    // NoWhereZeroFlow_AtMost_WitnessPointer witness(new NoWhereZeroFlow_AtMost_Witness);
    // Here, you can modify the witness and then insert to the witnessSet.
    // this->insertIntoInitialWitnessSet(witness);
    // If there are more, these should also be added to initialWitnessSet.
    NoWhereZeroFlow_AtMost_WitnessPointer witness = createWitness(); // creates a pointer to a new object of type NoWhereZeroFlow_AtMost_Witness. The flowSum map is empty by default. 
    this->insertIntoInitialWitnessSet(witness);
    //*****************************
    //*****************************
}

void NoWhereZeroFlow_AtMost_DynamicCore::intro_v_implementation(unsigned i, Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    NoWhereZeroFlow_AtMost_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    witness->flowSum.insert(make_pair(i,0));
    witnessSet->insert(witness);
    //*****************************
    //*****************************
}

void NoWhereZeroFlow_AtMost_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    //The existence of a nowhere zero flow does not depend on the direction of the edges.
    //therefore, we choose arbitrarily the direction to be from the smallest vertex to the 
    //biggest vertex: (minVertex,maxVertex)
    auto minVertex = w->flowSum.find(min(i,j));
    auto maxVertex = w->flowSum.find(max(i,j));
    for(unsigned x = 1; x<=this->parameter-1; x++){
	    //The flow is negative whne leaving minVertex
	    unsigned minVertexSum = (minVertex->second - x) % this->parameter; //obs: In C++, the % operator is not the mod operator but rather the remainder operator, which can be a negative number
	    if (minVertexSum < 0) minVertexSum = this->parameter - minVertexSum; 
	    //The flow is positive whne entering maxVertex
	    unsigned maxVertexSum = (maxVertex->second + x) % this->parameter; // here we don't need to test for positivity, since the remainder of a positive number will be positive
	    //Create a fresh copy of w
	    NoWhereZeroFlow_AtMost_WitnessPointer witness = createWitness();
	    witness->set_equal(*w);
	    //Update flowSum in this copy

	    witness->flowSum[minVertex->first] = minVertexSum;
	    witness->flowSum[maxVertex->first] = maxVertexSum;
	    witnessSet->insert(witness);
    }
    //*****************************
    //*****************************
}

void NoWhereZeroFlow_AtMost_DynamicCore::forget_v_implementation(unsigned i, Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    auto it = w->flowSum.find(i);
    if(it->second==0){
        NoWhereZeroFlow_AtMost_WitnessPointer  witness = createWitness();
        witness->set_equal(*w);
        witness->flowSum.erase(i);
        witnessSet->insert(witness);
    }
    //*****************************
    //*****************************
}
void NoWhereZeroFlow_AtMost_DynamicCore::join_implementation(Bag &b, NoWhereZeroFlow_AtMost_WitnessPointer w1, NoWhereZeroFlow_AtMost_WitnessPointer w2, NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    NoWhereZeroFlow_AtMost_WitnessPointer witness = createWitness();
    set<unsigned > bagSet = b.get_elements();
    for(unsigned int x : bagSet){
        witness->flowSum.insert({x, (w1->flowSum[x] + w2->flowSum[x]) % this->parameter});
    }
    witnessSet->insert(witness);
    //*****************************
    //*****************************
}

shared_ptr<WitnessSet> NoWhereZeroFlow_AtMost_DynamicCore::clean_implementation(NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet) {
    return witnessSet;
}

bool NoWhereZeroFlow_AtMost_DynamicCore::is_final_witness_implementation(NoWhereZeroFlow_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    for(auto & it : w->flowSum){
	// The summation of the flows in the incomming edges minus the summation of the flows in the outgoing edges is equal to zero.
	// If this condition is not satisfied, the witness is not valid. 
        if(it.second != 0){
            return false;
        }
    }
    return true;
    //*****************************
    //*****************************
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


bool NoWhereZeroFlow_AtMost_Witness::is_equal(const Witness &rhs) const{
    if (NoWhereZeroFlow_AtMost_Witness const *e = dynamic_cast<NoWhereZeroFlow_AtMost_Witness const *>(&rhs)) {
        NoWhereZeroFlow_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool NoWhereZeroFlow_AtMost_Witness::is_less(const Witness &rhs)const {
    if (NoWhereZeroFlow_AtMost_Witness const *e = dynamic_cast<NoWhereZeroFlow_AtMost_Witness const *>(&rhs)) {
        NoWhereZeroFlow_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness& NoWhereZeroFlow_AtMost_Witness::set_equal(Witness &witness) {
    if (NoWhereZeroFlow_AtMost_Witness *e = dynamic_cast<NoWhereZeroFlow_AtMost_Witness *>(&witness)) {
        NoWhereZeroFlow_AtMost_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> NoWhereZeroFlow_AtMost_WitnessSet::createEmptyWitnessSet() {
    NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet(new NoWhereZeroFlow_AtMost_WitnessSet);
    return witnessSet;
}

void  NoWhereZeroFlow_AtMost_DynamicCore::createInitialWitnessSet() {
    NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet(new NoWhereZeroFlow_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

NoWhereZeroFlow_AtMost_WitnessPointer NoWhereZeroFlow_AtMost_DynamicCore::createWitness(){
    NoWhereZeroFlow_AtMost_WitnessPointer w(new NoWhereZeroFlow_AtMost_Witness);
    return w;
}

void NoWhereZeroFlow_AtMost_DynamicCore::copyWitness(NoWhereZeroFlow_AtMost_WitnessPointer w_input, NoWhereZeroFlow_AtMost_WitnessPointer w_output){
    w_output->set_equal_implementation(w_input);
}


WitnessSetPointer NoWhereZeroFlow_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (NoWhereZeroFlow_AtMost_Witness *e = dynamic_cast<NoWhereZeroFlow_AtMost_Witness *>(&witness)){
        NoWhereZeroFlow_AtMost_WitnessPointer w = e->shared_from_this();
        NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet (new NoWhereZeroFlow_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet); // Witnesses are created and added to witnessSet in this funciton.
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer NoWhereZeroFlow_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (NoWhereZeroFlow_AtMost_Witness *e = dynamic_cast<NoWhereZeroFlow_AtMost_Witness *>(&witness)){
        NoWhereZeroFlow_AtMost_WitnessPointer w = e->shared_from_this();
        NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet (new NoWhereZeroFlow_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> NoWhereZeroFlow_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (NoWhereZeroFlow_AtMost_Witness *e = dynamic_cast<NoWhereZeroFlow_AtMost_Witness *>(&witness)){
        NoWhereZeroFlow_AtMost_WitnessPointer w = e->shared_from_this();
        NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet (new NoWhereZeroFlow_AtMost_WitnessSet);
        forget_v_implementation(i, b, w, witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}


shared_ptr<WitnessSet> NoWhereZeroFlow_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(NoWhereZeroFlow_AtMost_Witness *e1 = dynamic_cast<NoWhereZeroFlow_AtMost_Witness *>(&witness1)){
        if(NoWhereZeroFlow_AtMost_Witness *e2 = dynamic_cast<NoWhereZeroFlow_AtMost_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            NoWhereZeroFlow_AtMost_WitnessPointer w1 = e1->shared_from_this();
            NoWhereZeroFlow_AtMost_WitnessPointer w2 = e2->shared_from_this();
            NoWhereZeroFlow_AtMost_WitnessSetPointer witnessSet (new NoWhereZeroFlow_AtMost_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in NoWhereZeroFlow_AtMost_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> NoWhereZeroFlow_AtMost_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {

    if (NoWhereZeroFlow_AtMost_WitnessSetPointer e = dynamic_pointer_cast<NoWhereZeroFlow_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool NoWhereZeroFlow_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (NoWhereZeroFlow_AtMost_Witness *e = dynamic_cast<NoWhereZeroFlow_AtMost_Witness *>(&witness)) {
        NoWhereZeroFlow_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in NoWhereZeroFlow_AtMost_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new NoWhereZeroFlow_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new NoWhereZeroFlow_AtMost_DynamicCore(param);
}
}
