// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "VertexCover_AtMost.h"

///////////////////////Implementation////////////////////////////////
////////////////////////////////////////////////////////////////////
bool VertexCover_AtMost_Witness::is_equal_implementation(const VertexCover_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->partialCover == w->partialCover &&
           this->cost == w->cost;
    //*****************************
    //*****************************
}
/*
 * Determine if our witness is less than `rhs`.
 */
bool VertexCover_AtMost_Witness::is_less_implementation(const VertexCover_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if (this->partialCover != w->partialCover) {
        return (this->partialCover).size() < (w->partialCover).size();
    }
    return this->cost < w->cost;
    //*****************************
    //*****************************
}
/*
 * Attributes one witness to the other.
 */
Witness & VertexCover_AtMost_Witness::set_equal_implementation(VertexCover_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    this->partialCover = w->partialCover;
    this->cost = w->cost;
    return *this;
    //*****************************
    //*****************************
}

shared_ptr<Witness> VertexCover_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    VertexCover_AtMost_WitnessPointer relabeledWitness(new VertexCover_AtMost_Witness);
    relabeledWitness->cost = this->cost;
    for(auto item:this->partialCover){
        if(relabelingMap.count(item)){
            relabeledWitness->partialCover.insert(relabelingMap[item]);
        }else{
            cout<<"Error: VertexCover_AtMost_Witness::relabel "<<item<<" is not in the map."<<endl;
            exit(20);
        }
    }
    return relabeledWitness;
}

/*
 * Expected output example: "Witness: ({ 1 2 3 }, 10)"
 */
void VertexCover_AtMost_Witness::print()
{
    //*****************************
    //*****************************

    cout << "Witness: ({ ";

    for (auto v : partialCover) {
        cout << v << " ";
    }

    cout << " }, " << this->cost << ")" << endl;

    //*****************************
    //*****************************
}

string VertexCover_AtMost_Witness::witnessInformation() {
    //*****************************
    //*****************************
    string info;
    info = info + "Witness: ({ ";

    for (auto v : partialCover) {
        info = info + to_string(v) + " ";
    }

    info = info+  " }, " +  to_string(this->cost) + ")\n";
    return info;
    //*****************************
    //*****************************
}

VertexCover_AtMost_Witness::~VertexCover_AtMost_Witness()
{
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

VertexCover_AtMost_DynamicCore::VertexCover_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","VertexCover");
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

VertexCover_AtMost_DynamicCore::VertexCover_AtMost_DynamicCore(unsigned parameter)
{
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","VertexCover");
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
    this->parameter = parameter;
    createInitialWitnessSet();
}
/*
 * Create an initial witness and add it to the set `initialWitnessSet`
 * Usually only one initial witness exists, if there are more,
 * add them to `initialWitnessSet`.
 */
void VertexCover_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    //*****************************
    //*****************************
    shared_ptr<VertexCover_AtMost_Witness> witness = createWitness();
    witness->cost = 0;
    this->insertIntoInitialWitnessSet(witness);
    //*****************************
    //*****************************
}
/*
 * Two witnesses are introduced from adding vertex `i` to the bag `bag`, and
 * each witness will be added to `WitnessSet`, which is the result returned.
 *
 * (C, k) -> { (C \union {x}, k + 1) , (C, k) }
 */

void VertexCover_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, VertexCover_AtMost_WitnessPointer w,
                                                            VertexCover_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    VertexCover_AtMost_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    witnessSet->insert(w1);
    //*****************************
    //*****************************
}
/*
 * Introduce an edge between `i` and `j` in the bag `bag`, and modify `witness` set accordingly.
 *
 * RETURN (C, k) if intersection `edge` and `cover` != empty
 * RETURN {}     if intersection = empty.
 */
void VertexCover_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                            VertexCover_AtMost_WitnessPointer w,
                                                            VertexCover_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // If either node of an edge is in partial cover
    if (((w->partialCover).count(i) || (w->partialCover).count(j))) {
        VertexCover_AtMost_WitnessPointer w1 = createWitness();
        w1->set_equal(*w);
        witnessSet->insert(w1);
    } else if (w->cost < this->parameter) { // If the cover is not full
        VertexCover_AtMost_WitnessPointer w1 = createWitness();
        VertexCover_AtMost_WitnessPointer w2 = createWitness();
        (w1->partialCover).insert(i);
        (w2->partialCover).insert(j);
        ++(w1->cost);
        ++(w2->cost);
        witnessSet->insert(w1);
        witnessSet->insert(w2);
    }
    //*****************************
    //*****************************
}
/*
 * Remove vertex `i` from the bag `bag` and change `witness` accordingly.
 * Cost of the cover is not changed.
 *
 * (C, k) -> (c \ {x}, k)
 */
void VertexCover_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b,
                                                             VertexCover_AtMost_WitnessPointer w,
                                                             VertexCover_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    VertexCover_AtMost_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    // If `i` is in partialCover
    if ((w->partialCover).find(i) != (w->partialCover).end()) {
        (w1->partialCover).erase(i);
    }
    witnessSet->insert(w1);
    //*****************************
    //*****************************
}
/*
 * Join two witnesses into a new one.
 *
 * We have two witnesses (C1, k1) and (C2, k2):
 * If C1 == C2, then we have (C1, k1 + k2 - |C1|) to avoid double counting
 * If C1 != C2, then we return empty set
 */

// TODO: check join, is it correct that two vertex covers should be equal? example, suppose in one side we have two
// Isolated vertices and in the other side there is an edge between these two vertices.
void VertexCover_AtMost_DynamicCore::join_implementation(Bag &b, VertexCover_AtMost_WitnessPointer w1,
                                                         VertexCover_AtMost_WitnessPointer w2,
                                                         VertexCover_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    VertexCover_AtMost_WitnessPointer joined_witness = createWitness();
    if (w1->partialCover == w2->partialCover) {
        joined_witness->partialCover = w1->partialCover;
        joined_witness->cost = w1->cost + w2->cost - (w1->partialCover).size();
    }
    witnessSet->insert(joined_witness);
    //*****************************
    //*****************************
}

VertexCover_AtMost_WitnessSetPointer VertexCover_AtMost_DynamicCore::clean_implementation(
        VertexCover_AtMost_WitnessSetPointer witnessSet) {
    // default clean
    return witnessSet;
}

/*
 * Determine whether `witness` if final
 */
bool VertexCover_AtMost_DynamicCore::is_final_witness_implementation(VertexCover_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    return w->cost <= this->parameter;
    //*****************************
    //*****************************
}

////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
bool VertexCover_AtMost_Witness::is_equal(const Witness &rhs) const
{
    if (VertexCover_AtMost_Witness const *e = dynamic_cast<VertexCover_AtMost_Witness const *>(&rhs))
    {
        VertexCover_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_Witness::is_equal cast error" << endl;
        exit(20);
    }
}
bool VertexCover_AtMost_Witness::is_less(const Witness &rhs) const
{
    if (VertexCover_AtMost_Witness const *e = dynamic_cast<VertexCover_AtMost_Witness const *>(&rhs))
    {
        VertexCover_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_Witness::is_less cast error" << endl;
        exit(20);
    }
}
Witness &VertexCover_AtMost_Witness::set_equal(Witness &witness)
{
    if (VertexCover_AtMost_Witness *e = dynamic_cast<VertexCover_AtMost_Witness *>(&witness))
    {
        VertexCover_AtMost_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_Witness::operator= cast error" << endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> VertexCover_AtMost_WitnessSet::createEmptyWitnessSet() {
    VertexCover_AtMost_WitnessSetPointer witnessSet(new VertexCover_AtMost_WitnessSet);
    return witnessSet;
}

VertexCover_AtMost_WitnessPointer VertexCover_AtMost_DynamicCore::createWitness() {
    VertexCover_AtMost_WitnessPointer w(new VertexCover_AtMost_Witness);
    return w;
}

void VertexCover_AtMost_DynamicCore::copyWitness(VertexCover_AtMost_WitnessPointer w_input,
                                                 VertexCover_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void VertexCover_AtMost_DynamicCore::createInitialWitnessSet()
{
    VertexCover_AtMost_WitnessSetPointer witnessSet(new VertexCover_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

WitnessSetPointer VertexCover_AtMost_DynamicCore::intro_v(unsigned i, Bag &bag, Witness &witness)
{
    if (VertexCover_AtMost_Witness *e = dynamic_cast<VertexCover_AtMost_Witness *>(&witness))
    {
        VertexCover_AtMost_WitnessPointer w = e->shared_from_this(); // pointer to the input witness
        VertexCover_AtMost_WitnessSetPointer witnessSet(new VertexCover_AtMost_WitnessSet);
        intro_v_implementation(i,bag,w,witnessSet);
        return clean(witnessSet);
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_DynamicCore::intro_v cast error" << endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> VertexCover_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &bag, Witness &witness)
{
    if (VertexCover_AtMost_Witness *e = dynamic_cast<VertexCover_AtMost_Witness *>(&witness))
    {
        VertexCover_AtMost_WitnessPointer w = e->shared_from_this(); // pointer to the input witness
        VertexCover_AtMost_WitnessSetPointer witnessSet(new VertexCover_AtMost_WitnessSet);
        intro_e_implementation(i,j,bag, w,witnessSet);
        return clean(witnessSet);
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_DynamicCore::intro_e cast error" << endl;
        exit(20);
    }
}
WitnessSetPointer VertexCover_AtMost_DynamicCore::forget_v(unsigned i, Bag &bag, Witness &witness)
{
    if (VertexCover_AtMost_Witness *e = dynamic_cast<VertexCover_AtMost_Witness *>(&witness))
    {
        VertexCover_AtMost_WitnessPointer w = e->shared_from_this(); // pointer to the input witness
        VertexCover_AtMost_WitnessSetPointer witnessSet(new VertexCover_AtMost_WitnessSet);
        forget_v_implementation(i,bag, w, witnessSet);
        return clean(witnessSet);
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_DynamicCore::forget_v cast error" << endl;
        exit(20);
    }
}
shared_ptr<WitnessSet> VertexCover_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2)
{
    if (VertexCover_AtMost_Witness *e1 = dynamic_cast<VertexCover_AtMost_Witness *>(&witness1))
    {
        if (VertexCover_AtMost_Witness *e2 = dynamic_cast<VertexCover_AtMost_Witness *>(&witness2))
        {
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            VertexCover_AtMost_WitnessPointer w1 = e1->shared_from_this(); // pointer to the input witness
            VertexCover_AtMost_WitnessPointer w2 = e2->shared_from_this(); // pointer to the input witness
            VertexCover_AtMost_WitnessSetPointer witnessSet(new VertexCover_AtMost_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }
        else
        {
            cerr << "ERROR: in VertexCover_AtMost_DynamicCore::join cast error" << endl;
            exit(20);
        }
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_DynamicCore::join cast error" << endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> VertexCover_AtMost_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet)
{
    if (VertexCover_AtMost_WitnessSetPointer e = dynamic_pointer_cast<VertexCover_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in VertexCover_AtMost_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool VertexCover_AtMost_DynamicCore::is_final_witness(Witness &witness)
{
    if (VertexCover_AtMost_Witness *e = dynamic_cast<VertexCover_AtMost_Witness *>(&witness))
    {
        VertexCover_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }
    else
    {
        cerr << "ERROR: in VertexCover_AtMost_DynamicCore::is_final_witness cast error" << endl;
        exit(20);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore * create() {
    return new VertexCover_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new VertexCover_AtMost_DynamicCore(param);
}
}