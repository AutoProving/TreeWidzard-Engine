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


#include "Minor.h"

extern "C" {
DynamicCore * create() {
    return new Minor_DynamicCore;
}
DynamicCore * create_multiGraph(MultiGraph multiGraph) {
    return new Minor_DynamicCore(multiGraph);
}
}

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

bool Minor_Witness::is_equal_implementation(const Minor_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that determines when a witness is equal to other
    // Don't forget to return the result
    // return RESULT;
    //*****************************
    //*****************************
}

bool Minor_Witness::is_less_implementation(const Minor_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that determines when a witness is less than the other
    // Don't forget to return the result
    // return RESULT;
    //*****************************
    //*****************************
}

Witness& Minor_Witness::set_equal_implementation(Minor_WitnessPointer w){
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that attributes one witness to the other
    // This function should return *this, don't change the line below
    return *this;
    //*****************************
    //*****************************
}


void Minor_Witness::print() {
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that print the witness
    //*****************************
    //*****************************
}

Minor_Witness::~Minor_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

Minor_DynamicCore::Minor_DynamicCore(){

    //*****************************
    //*****************************
    // Only edit this part
    // In this function you can initialize the map attributes. These attributes are used to describe properties of the core.
    // Example:
    addAttribute("CoreName","Minor"); // Obligatory attribute. Replace GenericName by the name of the core.
    addAttribute("ParameterType","InputFile"); // Obligatory attribute. Replace GenericType by the type of the core: "NoParameter", "UnsignedInt", "InputFile".
    //  addAttribute("PrimaryOperator","GenericOperator"); //  This line should be uncommented if the type of the core is "UnsignedInt".
    //  Replace GenericOperator with "AtLeast" or "AtMost". The correct initialization depends on the core.
    //  ...
    //  Additional optional attributes.
    //*****************************
    //*****************************

}

Minor_DynamicCore::Minor_DynamicCore(MultiGraph multiGraph){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    this->multigraph = multiGraph;
    createInitialWitnessSet();
}
void Minor_DynamicCore::createInitialWitnessSet_implementation(){
    //*****************************
    //*****************************
    Minor_WitnessPointer initialWitness = createWitness();
    vector<bool> edgeFoundVector;
    edgeFoundVector.resize(multigraph.edgesSize());
    vector<tuple<set<unsigned >, set<set<unsigned >>,bool> > partitions;
    partitions.resize(multigraph.edgesSize());
    //TODO: Is the initial bool is false or true?
    initialWitness->found = false;
    initialWitness->partitions = partitions;
    initialWitness->edgeFoundVector = edgeFoundVector;
    //*****************************
    //*****************************
}

void Minor_DynamicCore::intro_v_implementation(unsigned i, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************

    //*****************************
    //*****************************
}

void Minor_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    // w is a pointer to the input witness.
    // Here, you will define which witnesses are obtained from the witness pointed by w when introducing edge (i,j) to the bag b.
    // Each of these witnesses should be added to witnessSet.
    // Example of how to create witnesses and how to add them to witnessSet:
    // Creating witness w1:
    // Minor_WitnessPointer w1 = createWitness();
    // Do something with w1.
    // copyWitness(w,w1); // Copying w to w1
    // witnessSet->insert(w1);
    // Creating witness w2:
    // Minor_WitnessPointer w2 = createWitness();
    // Do something with w2.
    // witnessSet->insert(w2);
    // And so on ...
    //*****************************
    //*****************************
}

void Minor_DynamicCore::forget_v_implementation(unsigned i, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    // w is a pointer to the input witness.
    // Here, you will define which witnesses are obtained from the witness pointed by w when removing vertex i to the bag b.
    // Each of these witnesses should be added to witnessSet.
    // Example of how to create witnesses and how to add them to witnessSet:
    // Creating witness w1:
    // Minor_WitnessPointer w1 = createWitness();
    // Do something with w1.
    // copyWitness(w,w1); // Copying w to w1
    // witnessSet->insert(w1);
    // Creating witness w2:
    // Minor_WitnessPointer w2 = createWitness();
    // Do something with w2.
    // witnessSet->insert(w2);
    // And so on ...
    //*****************************
    //*****************************
}
void Minor_DynamicCore::join_implementation(Bag &b, Minor_WitnessPointer w1, Minor_WitnessPointer w2, Minor_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // w1 and w2 are pointers to the input witnesses.
    // Here, you will define which witnesses are obtained from the witnesses pointed by w1 and w2 when applying the join operation to the bag b.
    // Each of these new witnesses should be added to witnessSet.
    // Example of how to create witnesses and how to add them to witnessSet:
    // Creating witness w:
    // Minor_WitnessPointer w = createWitness();
    // Do something with w.
    // copyWitness(w1,w); // Copying w1 to w
    // witnessSet->insert(w);
    // Creating witness wPrime:
    // Minor_WitnessPointer wPrime = createWitness();
    // Do something with wPrime.
    // witnessSet->insert(wPrime);
    // And so on ...
    //*****************************
    //*****************************
}

shared_ptr<WitnessSet> Minor_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    //*****************************
    //*****************************
    // In most cases, you will not need to change this function.
    //*****************************
    //*****************************
    return witnessSet;
}
bool Minor_DynamicCore::is_final_witness_implementation(Minor_WitnessPointer w) {
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that determines when a witness is a final witness.
    // Don't forget to return the result
    // return RESULT;
    //*****************************
    //*****************************
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


bool Minor_Witness::is_equal(const Witness &rhs) const{
    if (Minor_Witness const *e = dynamic_cast<Minor_Witness const *>(&rhs)) {
        Minor_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in Minor_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}



bool Minor_Witness::is_less(const Witness &rhs)const {
    if (Minor_Witness const *e = dynamic_cast<Minor_Witness const *>(&rhs)) {
        Minor_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in Minor_Witness::is_less cast error"<<endl;
        exit(20);
    }
}



Witness& Minor_Witness::set_equal(Witness &witness) {
    if (Minor_Witness *e = dynamic_cast<Minor_Witness *>(&witness)) {
        Minor_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in Minor_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

void  Minor_DynamicCore::createInitialWitnessSet() {
    Minor_WitnessSetPointer witnessSet(new Minor_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

Minor_WitnessPointer Minor_DynamicCore::createWitness(){
    Minor_WitnessPointer w(new Minor_Witness);
    return w;
}

void Minor_DynamicCore::copyWitness(Minor_WitnessPointer w_input, Minor_WitnessPointer w_output){
    w_output->set_equal_implementation(w_input);
}


WitnessSetPointer Minor_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (Minor_Witness *e = dynamic_cast<Minor_Witness *>(&witness)){
        Minor_WitnessPointer w = e->shared_from_this();
        Minor_WitnessSetPointer witnessSet (new Minor_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet); // Witnesses are created and added to witnessSet in this funciton.
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in Minor_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer Minor_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (Minor_Witness *e = dynamic_cast<Minor_Witness *>(&witness)){
        Minor_WitnessPointer w = e->shared_from_this();
        Minor_WitnessSetPointer witnessSet (new Minor_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in Minor_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> Minor_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (Minor_Witness *e = dynamic_cast<Minor_Witness *>(&witness)){
        Minor_WitnessPointer w = e->shared_from_this();
        Minor_WitnessSetPointer witnessSet (new Minor_WitnessSet);
        forget_v_implementation(i, b, w, witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in Minor_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}


shared_ptr<WitnessSet> Minor_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(Minor_Witness *e1 = dynamic_cast<Minor_Witness *>(&witness1)){
        if(Minor_Witness *e2 = dynamic_cast<Minor_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            Minor_WitnessPointer w1 = e1->shared_from_this();
            Minor_WitnessPointer w2 = e2->shared_from_this();
            Minor_WitnessSetPointer witnessSet (new Minor_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in Minor_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in Minor_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

bool Minor_DynamicCore::is_final_witness(Witness &witness) {
    if (Minor_Witness *e = dynamic_cast<Minor_Witness *>(&witness)) {
        Minor_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in Minor_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}