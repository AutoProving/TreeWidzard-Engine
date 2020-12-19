// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "HamiltonianCycle.h"


///////////////////////Implementation////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool HamiltonianCycle_Witness::is_equal_implementation(const HamiltonianCycle_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if(this->degree_0 == w->degree_0 &&
       this->degree_1 == w->degree_1 &&
       this->degree_2 == w->degree_2 &&
       this->matching == w->matching &&
       this->closed == w->closed
            ){
        return true;
    }else{
        return false;
    }
    //*****************************
    //*****************************
}
bool HamiltonianCycle_Witness::is_less_implementation(const HamiltonianCycle_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if(this->degree_0 == w->degree_0){
        if(this->degree_1 == w->degree_1){
            if(this->degree_2 == w->degree_2){
                if(this->matching == w->matching){
                    return this->closed < w->closed;
                }else return this->matching < w->matching;
            }else return this->degree_2 < w->degree_2;
        }else return this->degree_1 < w->degree_1;
    }else return this->degree_0 < w->degree_0;
    //*****************************
    //*****************************
}

Witness & HamiltonianCycle_Witness::set_equal_implementation(HamiltonianCycle_WitnessPointer w) {
    //*****************************
    //*****************************
    this->degree_0 = w->degree_0;
    this->degree_1 = w->degree_1;
    this->degree_2 = w->degree_2;
    this->matching = w->matching;
    this->closed = w->closed;
    return *this;
    //*****************************
    //*****************************
}

void HamiltonianCycle_Witness::print() {
    //*****************************
    //*****************************
    cout << "Vertices of degree 0:" << endl << "\t";
    for(auto &v: this->degree_0) cout << v << " ";
    cout << endl;

    cout << "Vertices of degree 1:" << endl << "\t";
    for(auto &v: this->degree_1) cout << v << " ";
    cout << endl;

    cout << "Vertices of degree 2:" << endl << "\t";
    for(auto &v: this->degree_2) cout << v << " ";
    cout << endl;

    cout << "Matching:" << endl << "\t";
    for(auto &v: this->matching) cout << "(" << v.first << ", " << v.second << ") ";
    cout << endl;

    cout << "Closed: " << this->closed << endl;
    //*****************************
    //*****************************
}

HamiltonianCycle_Witness::~HamiltonianCycle_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

HamiltonianCycle_DynamicCore::HamiltonianCycle_DynamicCore(){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","HamiltonianCycle");
    addAttribute("ParameterType","NoParameter");
    addAttribute("PrimaryOperator","---");
    createInitialWitnessSet();
}

void HamiltonianCycle_DynamicCore::createInitialWitnessSet_implementation() {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
    //*****************************
    //*****************************
}
void HamiltonianCycle_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, HamiltonianCycle_WitnessPointer w,
                                                          HamiltonianCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    //Only accepts a vertex if it's new and the witness is open
    if(w1->degree_0.count(i) == 0 && w1->degree_1.count(i) == 0 && w1->degree_2.count(i) == 0 && !w1->closed){
        w1->degree_0.insert(i);
        witnessSet->insert(w1);
    }
    //*****************************
    //*****************************
}

void HamiltonianCycle_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                          HamiltonianCycle_WitnessPointer w,
                                                          HamiltonianCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer w0 = createWitness();
    w0->set_equal(*w);
    //New witness without this edge
    witnessSet->insert(w0);

    //Only introduces an edge if it's open
    if(w0->closed){
        // nothing
    }else if(w->degree_0.count(i) && w->degree_0.count(j)){
        //Both vertices with degree 0
        HamiltonianCycle_WitnessPointer w1=createWitness();
        w1->set_equal(*w);
        w1->degree_0.erase(i);
        w1->degree_0.erase(j);
        w1->degree_1.insert(i);
        w1->degree_1.insert(j);
        w1->matching[i] = j;
        w1->matching[j] = i;
        witnessSet->insert(w1);
    }else if(w->degree_0.count(i) && w->degree_1.count(j)){
        //Vertex i with degree 0 and j degree 1
        HamiltonianCycle_WitnessPointer w1=createWitness();
        w1->set_equal(*w);
        w1->degree_0.erase(i);
        w1->degree_1.erase(j);
        w1->degree_1.insert(i);
        w1->degree_2.insert(j);
        //Changing the matching
        unsigned  m = w1->matching[j];
        w1->matching.erase(j);
        w1->matching[m] = i;
        w1->matching[i] = m;
        witnessSet->insert(w1);
    }else if(w->degree_1.count(i) && w->degree_0.count(j)){
        //Vertex j with degree 0 and i degree 1
        HamiltonianCycle_WitnessPointer w1=createWitness();
        w1->set_equal(*w);
        w1->degree_1.erase(i);
        w1->degree_0.erase(j);
        w1->degree_2.insert(i);
        w1->degree_1.insert(j);
        //Changing the matching
        unsigned  m = w1->matching[i];
        w1->matching.erase(i);
        w1->matching[m] = j;
        w1->matching[j] = m;
        witnessSet->insert(w1);
    }else if(w->degree_1.count(i) && w->degree_1.count(j)){
        //Vertices j an i with degree 1
        if(w->matching[i] != j){
            //Not in the match
            HamiltonianCycle_WitnessPointer w1=createWitness();
            w1->set_equal(*w);
            w1->degree_1.erase(i);
            w1->degree_1.erase(j);
            w1->degree_2.insert(i);
            w1->degree_2.insert(j);
            unsigned  mi = w1->matching[i];
            unsigned  mj = w1->matching[j];
            w1->matching.erase(i);
            w1->matching.erase(j);
            w1->matching[mj] = mi;
            w1->matching[mi] = mj;
            witnessSet->insert(w1);
        }else if(w->matching[i] == j && w->degree_0.size() == 0 && w->degree_1.size() == 2){
            //They are matching
            HamiltonianCycle_WitnessPointer w1=createWitness();
            w1->set_equal(*w);
            w1->degree_1.erase(i);
            w1->degree_1.erase(j);
            w1->degree_2.insert(i);
            w1->degree_2.insert(j);
            w1->matching.erase(i);
            w1->matching.erase(j);
            w1->closed = true;
            witnessSet->insert(w1);
        }
    }
    //*****************************
    //*****************************
}
void HamiltonianCycle_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, HamiltonianCycle_WitnessPointer w,
                                                           HamiltonianCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer w1=createWitness();
    w1->set_equal(*w);
    //Only forget vertices with degree 2
    if(w1->degree_2.count(i) == 1){
        w1->degree_2.erase(i);
        witnessSet->insert(w1);
    }
    //*****************************
    //*****************************
}
void HamiltonianCycle_DynamicCore::join_implementation(Bag &b, HamiltonianCycle_WitnessPointer w1,
                                                       HamiltonianCycle_WitnessPointer w2,
                                                       HamiltonianCycle_WitnessSetPointer witnessSet) {
//*****************************
    //*****************************
    HamiltonianCycle_WitnessPointer wPrime1 = createWitness();
    wPrime1->set_equal(*w1); // Working Witness
    bool impossible = false;
    //We have copied the input witness1 into w1 and the input witness2 into w2.
    //The final witness will be w1.
    //Treating vertices of degree 0 in p1
    for (unsigned i: w1->degree_0)
    {
        // If i has degree 0 in p2 then it will have degree 0 in w1. Therefore this case does need to be treated.
        if (w1->degree_2.count(i))
        {
            // In this case, i will have degree 2 in the new w1
            wPrime1->degree_0.erase(i);
            wPrime1->degree_2.insert(i);
        }
    }
    //Treating vertices of degree 2 in p1
    for (unsigned i: w1->degree_2)
    {
        // If i has degree 0 in p2 then it will have degree 2 in w1. Therefore this case does need to be treated.
        if (w2->degree_2.count(i))
        {
            // in this case, i is being overused. This means that there is no way of combining the input witnesses into a valid witness
            impossible = true;
            break;
        }
    }

    if (not impossible){
        //Treating the edges of the matching.
        map<unsigned,unsigned> auxMatching = w2->matching;
        while(auxMatching.size())
        {
            pair<unsigned,unsigned> edge = *(auxMatching.begin());
            auxMatching.erase(edge.first);
            auxMatching.erase(edge.second);
            //We will treat case by case
            if(wPrime1->degree_0.count(edge.first) and wPrime1->degree_0.count(edge.second))
            {
                // degrees 0 and 0
                wPrime1->degree_0.erase(edge.first);
                wPrime1->degree_1.insert(edge.first);
                wPrime1->degree_0.erase(edge.second);
                wPrime1->degree_1.insert(edge.second);
                wPrime1->matching[edge.first]=edge.second;
                wPrime1->matching[edge.second]=edge.first;
            }
            else if(wPrime1->degree_0.count(edge.first) and wPrime1->degree_1.count(edge.second))
            {
                // degrees 0 and 1
                wPrime1->degree_0.erase(edge.first);
                wPrime1->degree_1.insert(edge.first);
                const unsigned m = wPrime1->matching[edge.second];
                wPrime1->matching[edge.first] = m;
                wPrime1->matching[m] = edge.first;
                wPrime1->matching.erase(edge.second);
                wPrime1->degree_1.erase(edge.second);
                wPrime1->degree_2.insert(edge.second);
            }
            else if(wPrime1->degree_0.count(edge.first) and wPrime1->degree_2.count(edge.second))
            {
                // degrees 0 and 2
                // vertices in overuse
                // BAD WITNESS JOIN...
                impossible = true;
                break;
            }
            else if(wPrime1->degree_1.count(edge.first) and wPrime1->degree_0.count(edge.second))
            {
                // degrees 1 and 0
                wPrime1->degree_0.erase(edge.second);
                wPrime1->degree_1.insert(edge.second);
                const unsigned m = wPrime1->matching[edge.first];
                wPrime1->matching[edge.second] = m;
                wPrime1->matching[m] = edge.second;
                wPrime1->matching.erase(edge.first);
                wPrime1->degree_1.erase(edge.first);
                wPrime1->degree_2.insert(edge.first);
            }
            else if(wPrime1->degree_1.count(edge.first) and wPrime1->degree_1.count(edge.second))
            {
                // degrees 1 and 1
                const unsigned m1 = wPrime1->matching[edge.first]; // matching of edge.first
                const unsigned m2 = wPrime1->matching[edge.second]; // matching of edge.second
                // Checking for closing cycles
                if(m1 == edge.second)
                {
                    if(not wPrime1->closed)
                    {
                        // if not yet closed
                        wPrime1->closed = true;
                        wPrime1->matching.erase(m1);
                        wPrime1->matching.erase(m2);
                        wPrime1->degree_1.erase(m1);
                        wPrime1->degree_1.erase(m2);
                        wPrime1->degree_2.insert(m1);
                        wPrime1->degree_2.insert(m2);
                    }else{
                        // impossible to close again
                        // BAD WITNESS JOIN...
                        impossible = true;
                        break;
                    }
                }else
                {
                    // Not closing cycle
                    wPrime1->degree_1.erase(edge.first);
                    wPrime1->degree_1.erase(edge.second);
                    wPrime1->degree_2.insert(edge.first);
                    wPrime1->degree_2.insert(edge.second);
                    wPrime1->matching.erase(edge.first);
                    wPrime1->matching.erase(edge.second);
                    wPrime1->matching[m1] = m2;
                    wPrime1->matching[m2] = m1;
                }
            }
            else if(wPrime1->degree_1.count(edge.first) and wPrime1->degree_2.count(edge.second))
            {
                // degrees 1 and 2
                // vertices in overuse
                // BAD WITNESS JOIN...
                impossible = true;
                break;
            }
            else if(wPrime1->degree_2.count(edge.first) and wPrime1->degree_0.count(edge.second))
            {
                // degrees 2 and 2
                // vertices in overuse
                // BAD WITNESS JOIN...
                impossible = true;
                break;
            }
            else if(wPrime1->degree_2.count(edge.first) and wPrime1->degree_1.count(edge.second))
            {
                // degrees 2 and 2
                // vertices in overuse
                // BAD WITNESS JOIN...
                impossible = true;
                break;
            }
            else if(wPrime1->degree_2.count(edge.first) and wPrime1->degree_2.count(edge.second))
            {
                // degrees 2 and 2
                // vertices in overuse
                // BAD WITNESS JOIN...
                impossible = true;
                break;
            }
        }

        // Verifying if it is a valid join
        if (not impossible)
        {
            if(wPrime1->closed)
            {
                if(wPrime1->degree_0.size() == 0 and wPrime1->degree_1.size() == 0)
                {
                    // Only finishes closed if it's empty
                    witnessSet->insert(wPrime1);
                }
            }
            else
            {
                witnessSet->insert(wPrime1);
            }
        }
    }
    //*****************************
    //*****************************
}

HamiltonianCycle_WitnessSetPointer HamiltonianCycle_DynamicCore::clean_implementation(
        HamiltonianCycle_WitnessSetPointer witnessSet) {

    return witnessSet;
}

bool HamiltonianCycle_DynamicCore::is_final_witness_implementation(HamiltonianCycle_WitnessPointer w) {
    //*****************************
    //*****************************
    // Verifies if there is no node with degree 0 or 1, and checks if it's already closed
    if(w->degree_0.size() == 0 &&
       w->degree_1.size() == 0 &&
       w->matching.size() == 0 &&
       w->closed
            ){
        return true;
    }else{
        return false;
    }
    //*****************************
    //*****************************
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool HamiltonianCycle_Witness::is_equal(const Witness &rhs) const{
    if (HamiltonianCycle_Witness const *e = dynamic_cast<HamiltonianCycle_Witness const *>(&rhs)) {
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool HamiltonianCycle_Witness::is_less(const Witness &rhs)const {
    if (HamiltonianCycle_Witness const *e = dynamic_cast<HamiltonianCycle_Witness const *>(&rhs)) {
        HamiltonianCycle_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &HamiltonianCycle_Witness::set_equal(Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)) {
        HamiltonianCycle_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> HamiltonianCycle_WitnessSet::createEmptyWitnessSet() {
    HamiltonianCycle_WitnessSetPointer witnessSet(new HamiltonianCycle_WitnessSet);
    return witnessSet;
}

HamiltonianCycle_WitnessPointer HamiltonianCycle_DynamicCore::createWitness() {
    HamiltonianCycle_WitnessPointer w(new HamiltonianCycle_Witness);
    return w;
}

void HamiltonianCycle_DynamicCore::copyWitness(HamiltonianCycle_WitnessPointer w_input,
                                               HamiltonianCycle_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void HamiltonianCycle_DynamicCore::createInitialWitnessSet() {
    HamiltonianCycle_WitnessSetPointer witnessSet(new HamiltonianCycle_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}


WitnessSetPointer HamiltonianCycle_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness  ) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)){
        HamiltonianCycle_WitnessPointer w = e->shared_from_this();
        HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}



WitnessSetPointer HamiltonianCycle_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)){
        HamiltonianCycle_WitnessPointer w = e->shared_from_this();
        HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}


WitnessSetPointer HamiltonianCycle_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)){
        HamiltonianCycle_WitnessPointer w = e->shared_from_this();
        HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}


WitnessSetPointer HamiltonianCycle_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(HamiltonianCycle_Witness *e1 = dynamic_cast<HamiltonianCycle_Witness *>(&witness1)){
        if(HamiltonianCycle_Witness *e2 = dynamic_cast<HamiltonianCycle_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            HamiltonianCycle_WitnessPointer w1 = e1->shared_from_this();
            HamiltonianCycle_WitnessPointer w2 = e2->shared_from_this();
            HamiltonianCycle_WitnessSetPointer witnessSet (new HamiltonianCycle_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in HamiltonianCycle_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer HamiltonianCycle_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (HamiltonianCycle_WitnessSetPointer e = dynamic_pointer_cast<HamiltonianCycle_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool HamiltonianCycle_DynamicCore::is_final_witness(Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)) {
        HamiltonianCycle_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new HamiltonianCycle_DynamicCore;
}
}
