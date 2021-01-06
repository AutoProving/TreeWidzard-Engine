// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "IndependentSet_AtLeast.h"

///////////////////////Implementation/////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
bool IndependentSet_AtLeast_Witness::is_equal_implementation(
        const IndependentSet_AtLeast_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->partialIndSet == w->partialIndSet && this->size == w->size && this->found == w->found;
    //*****************************
    //*****************************
}
bool IndependentSet_AtLeast_Witness::is_less_implementation(const IndependentSet_AtLeast_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if (this->found != w->found) return this->found < w->found;
    if (this->partialIndSet != w->partialIndSet) return this->partialIndSet < w->partialIndSet;
    return this->size < w->size;
    //*****************************
    //*****************************
}
Witness & IndependentSet_AtLeast_Witness::set_equal_implementation(IndependentSet_AtLeast_WitnessPointer w) {
    //*****************************
    //*****************************
    this->found = w->found;
    this->partialIndSet = w->partialIndSet;
    this->size = w->size;
    return *this;
    //*****************************
    //*****************************
}
shared_ptr<Witness> IndependentSet_AtLeast_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {

    if(this->found){
        return this->shared_from_this();
    }else{
        IndependentSet_AtLeast_WitnessPointer relabeledWitness(new IndependentSet_AtLeast_Witness);
        relabeledWitness->found = false;
        relabeledWitness->size = this->size;
        for(auto s:this->partialIndSet){
            if(relabelingMap.count(s)){
                relabeledWitness->partialIndSet.insert(relabelingMap[s]);
            }else{
                cout<<"Error: IndependentSet_AtLeast_Witness::relabel "<< s<<" is not in the map."<<endl;
                exit(20);
            }
        }
        return relabeledWitness;
    }
}
void IndependentSet_AtLeast_Witness::print() {
    //*****************************
    //*****************************
    // Expected output example: Witness: ({1 2 3}, 5, false)
    set<unsigned>::iterator itr;
    cout << "Witness: ({ ";
    for (itr = this->partialIndSet.begin(); itr != this->partialIndSet.end(); ++itr) {
        cout << *itr << " ";
    }
    cout << "}, " << this->size << ", ";
    if (this->found == true) cout << "true)" << endl;
    else cout << "false)" << endl;
    //*****************************
    //*****************************
}

IndependentSet_AtLeast_Witness::~IndependentSet_AtLeast_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}
IndependentSet_AtLeast_DynamicCore::IndependentSet_AtLeast_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","IndependentSet");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");
}

IndependentSet_AtLeast_DynamicCore::IndependentSet_AtLeast_DynamicCore(unsigned parameter){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","IndependentSet");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");
    this->parameter = parameter;
    createInitialWitnessSet();
}

void IndependentSet_AtLeast_DynamicCore::createInitialWitnessSet_implementation() {
    //*****************************
    //*****************************
    // Should be empty.
    IndependentSet_AtLeast_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
    //*****************************
    //*****************************
}
void IndependentSet_AtLeast_DynamicCore::intro_v_implementation(unsigned int i, Bag &b,
                                                                IndependentSet_AtLeast_WitnessPointer w,
                                                                IndependentSet_AtLeast_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // witness == (S, r),
    // then intro_i results in the witnessSet: {(S, r), (S U {i}, r + 1)} (when r < k)
    //                                      or {(S, r)}                   (when r >= k)

    IndependentSet_AtLeast_WitnessPointer w1 = createWitness();
    // w1 = (S, r)
    w1->set_equal(*w);
    witnessSet->insert(w1);
    if (!w->found){
        IndependentSet_AtLeast_WitnessPointer w2 = createWitness();
        // w2 = (S U {i}, r + 1)
        w2->set_equal(*w);
        w2->partialIndSet.insert(i);
        w2->size += 1;
        // Verifying if r reached k. If it did, an indSet with size at least k was already found.
        if (w2->size == this->parameter) {
            w2->found = true;
            witnessSet->insert(w2);
        }
    }
    //*****************************
    //*****************************
}
void IndependentSet_AtLeast_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                                IndependentSet_AtLeast_WitnessPointer w,
                                                                IndependentSet_AtLeast_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // if i and j are in the witness
    if (w->partialIndSet.count(i) && w->partialIndSet.count(j)) {
        // do nothing. The witness is bad.
    }else{
        // it stays the same and should be added to the witnessSet.
        IndependentSet_AtLeast_WitnessPointer w1 = createWitness();
        w1->set_equal(*w);
        witnessSet->insert(w1);
    }
    //*****************************
    //*****************************
}
void IndependentSet_AtLeast_DynamicCore::forget_v_implementation(unsigned int i, Bag &b,
                                                                 IndependentSet_AtLeast_WitnessPointer w,
                                                                 IndependentSet_AtLeast_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // witness == (S, r) -> forget_i -> (S - {i}, r) is added to the witnessSet.
    IndependentSet_AtLeast_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    if (w1->partialIndSet.count(i)) {
        w1->partialIndSet.erase(i);
    }
    witnessSet->insert(w1);
    //*****************************
    //*****************************
}
void IndependentSet_AtLeast_DynamicCore::join_implementation(Bag &b, IndependentSet_AtLeast_WitnessPointer w1,
                                                             IndependentSet_AtLeast_WitnessPointer w2,
                                                             IndependentSet_AtLeast_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    if (w1->partialIndSet == w2->partialIndSet) {
        shared_ptr<IndependentSet_AtLeast_Witness> wPrime1(new IndependentSet_AtLeast_Witness);
        wPrime1->partialIndSet = w1->partialIndSet;
        wPrime1->size = w1->size + w2->size - w1->partialIndSet.size();
        if (wPrime1->size >= this->parameter){
            wPrime1->found = true;
        }else{
            wPrime1->found = false;
        }
        witnessSet->insert(wPrime1);
    }
    // else wPrime1 is not valid and should not be inserted to the witnessSet.
    //*****************************
    //*****************************
}

shared_ptr<WitnessSet> IndependentSet_AtLeast_DynamicCore::clean_implementation(IndependentSet_AtLeast_WitnessSetPointer  witnessSet) {
    for(auto witness:(*witnessSet)){
        if (IndependentSet_AtLeast_WitnessPointer w = dynamic_pointer_cast<IndependentSet_AtLeast_Witness>(witness)) {
            if(w->found){
                IndependentSet_AtLeast_WitnessSetPointer newWitnessSet (new IndependentSet_AtLeast_WitnessSet);
                newWitnessSet->insert(w);
                return newWitnessSet;
            }
        }else{
            cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::clean_implementation cast error"<<endl;
            exit(20);
        }
    }
    return witnessSet;
}

bool IndependentSet_AtLeast_DynamicCore::is_final_witness_implementation(IndependentSet_AtLeast_WitnessPointer w) {
    //*****************************
    //*****************************
    // True if size r is at least the parameter k. False otherwise.
    return w->found;
    //*****************************
    //*****************************
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

bool IndependentSet_AtLeast_Witness::is_equal(const Witness &rhs) const{
    if (IndependentSet_AtLeast_Witness const *e = dynamic_cast<IndependentSet_AtLeast_Witness const *>(&rhs)) {
        IndependentSet_AtLeast_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}
bool IndependentSet_AtLeast_Witness::is_less(const Witness &rhs)const {
    if (IndependentSet_AtLeast_Witness const *e = dynamic_cast<IndependentSet_AtLeast_Witness const *>(&rhs)) {
        IndependentSet_AtLeast_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &IndependentSet_AtLeast_Witness::set_equal(Witness &witness) {
    if (IndependentSet_AtLeast_Witness *e = dynamic_cast<IndependentSet_AtLeast_Witness *>(&witness)) {
        IndependentSet_AtLeast_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> IndependentSet_AtLeast_WitnessSet::createEmptyWitnessSet() {
    IndependentSet_AtLeast_WitnessSetPointer witnessSet(new IndependentSet_AtLeast_WitnessSet);
    return witnessSet;

}

void IndependentSet_AtLeast_DynamicCore::createInitialWitnessSet() {
    IndependentSet_AtLeast_WitnessSetPointer witnessSet(new IndependentSet_AtLeast_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}


IndependentSet_AtLeast_WitnessPointer IndependentSet_AtLeast_DynamicCore::createWitness() {
    IndependentSet_AtLeast_WitnessPointer w(new IndependentSet_AtLeast_Witness);
    return w;
}

void IndependentSet_AtLeast_DynamicCore::copyWitness(IndependentSet_AtLeast_WitnessPointer w_input,
                                                     IndependentSet_AtLeast_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

WitnessSetPointer IndependentSet_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (IndependentSet_AtLeast_Witness *e = dynamic_cast<IndependentSet_AtLeast_Witness *>(&witness)){
        IndependentSet_AtLeast_WitnessPointer w = e->shared_from_this();
        IndependentSet_AtLeast_WitnessSetPointer witnessSet(new IndependentSet_AtLeast_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer IndependentSet_AtLeast_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (IndependentSet_AtLeast_Witness *e = dynamic_cast<IndependentSet_AtLeast_Witness *>(&witness)){
        IndependentSet_AtLeast_WitnessPointer w = e->shared_from_this();
        IndependentSet_AtLeast_WitnessSetPointer witnessSet (new IndependentSet_AtLeast_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}
WitnessSetPointer IndependentSet_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (IndependentSet_AtLeast_Witness *e = dynamic_cast<IndependentSet_AtLeast_Witness *>(&witness)){
        IndependentSet_AtLeast_WitnessPointer w = e->shared_from_this();
        IndependentSet_AtLeast_WitnessSetPointer witnessSet (new IndependentSet_AtLeast_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer IndependentSet_AtLeast_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(IndependentSet_AtLeast_Witness *e1 = dynamic_cast<IndependentSet_AtLeast_Witness *>(&witness1)){
        if(IndependentSet_AtLeast_Witness *e2 = dynamic_cast<IndependentSet_AtLeast_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            IndependentSet_AtLeast_WitnessPointer w1 = e1->shared_from_this();
            IndependentSet_AtLeast_WitnessPointer w2 = e2->shared_from_this();
            IndependentSet_AtLeast_WitnessSetPointer witnessSet (new IndependentSet_AtLeast_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> IndependentSet_AtLeast_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    if (IndependentSet_AtLeast_WitnessSetPointer e = dynamic_pointer_cast<IndependentSet_AtLeast_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool IndependentSet_AtLeast_DynamicCore::is_final_witness(Witness &witness) {
    if (IndependentSet_AtLeast_Witness *e = dynamic_cast<IndependentSet_AtLeast_Witness *>(&witness)) {
        IndependentSet_AtLeast_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in IndependentSet_AtLeast_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore * create() {
    return new IndependentSet_AtLeast_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new IndependentSet_AtLeast_DynamicCore(param);
}
}