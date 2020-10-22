// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "EdgeColoring_AtMost.h"
extern "C" {
DynamicCore * create() {
    return new EdgeColoring_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new EdgeColoring_AtMost_DynamicCore(param);
}
}
//********************
//********************
// Implementations of specific functions:

unsigned EdgeColoring_AtMost_Witness::next_available_edge_number() {
    // Getting the greatest edgeNumber:
    auto maxNumberIterator = this->availableEdgeNumbers.rbegin();
    unsigned maxNumber = maxNumberIterator->first;

    // In the range of (1, maxNumber), if a number is not in availableEdgeNumbers,
    // then this number is the next available one.
    for (unsigned i = 1; i <= maxNumber; ++i) {
        if (!this->availableEdgeNumbers.count(i)) return i;
    }
    // if all numbers until maxNumber are being used, add a new edgeNumber.
    return maxNumber + 1;
}

void EdgeColoring_AtMost_Witness::print_pair(pair<unsigned,unsigned> edge) {
    cout << "(" << edge.first << ", " << edge.second << ")";
}
//********************
//*******************
//////////////////////////////Implementation//////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

bool EdgeColoring_AtMost_Witness::is_equal_implementation(const EdgeColoring_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->availableEdgeNumbers == w->availableEdgeNumbers &&
           this->edgesPerVertex == w->edgesPerVertex;
    //*****************************
    //*****************************
}

bool EdgeColoring_AtMost_Witness::is_equal(const Witness &rhs) const{
    if (EdgeColoring_AtMost_Witness const *e = dynamic_cast<EdgeColoring_AtMost_Witness const *>(&rhs)) {
        EdgeColoring_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool EdgeColoring_AtMost_Witness::is_less_implementation(const EdgeColoring_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->edgesPerVertex < w->edgesPerVertex;
    //*****************************
    //*****************************
}

bool EdgeColoring_AtMost_Witness::is_less(const Witness &rhs)const {
    if (EdgeColoring_AtMost_Witness const *e = dynamic_cast<EdgeColoring_AtMost_Witness const *>(&rhs)) {
        EdgeColoring_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness & EdgeColoring_AtMost_Witness::set_equal_implementation(EdgeColoring_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    this->availableEdgeNumbers = w->availableEdgeNumbers;
    this->edgesPerVertex = w->edgesPerVertex;
    return *this;
    //*****************************
    //*****************************
}
Witness &EdgeColoring_AtMost_Witness::set_equal(Witness &witness) {
    if (EdgeColoring_AtMost_Witness *e = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness)) {
        EdgeColoring_AtMost_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

void EdgeColoring_AtMost_Witness::print() {
    //*****************************
    //*****************************
    // EXPECTED OUTPUT EXAMPLE
    // Witness :
    // (vertex1, { (#edge1, #color1) (#edge2, #color2) } )
    // (vertex2, { (#edge1, #color1) (#edge3, #color4) } )
    cout << "Witness:\n";
    for (auto &pair: this->edgesPerVertex) {
        cout << "(" << pair.first << ", " << "{ ";
        for (auto &element: pair.second) {
            this->print_pair(element);
            cout << " ";
        }
        cout << "} )\n";
    }
    //*****************************
    //*****************************
}

EdgeColoring_AtMost_Witness::~EdgeColoring_AtMost_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************

}
EdgeColoring_AtMost_DynamicCore::EdgeColoring_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","EdgeColoring");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

EdgeColoring_AtMost_DynamicCore::EdgeColoring_AtMost_DynamicCore(unsigned parameter){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","EdgeColoring");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
    this->parameter = parameter;
    createInitialWitnessSet();
}

void EdgeColoring_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    EdgeColoring_AtMost_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
}

void EdgeColoring_AtMost_DynamicCore::createInitialWitnessSet() {
    EdgeColoring_AtMost_WitnessSetPointer witnessSet(new EdgeColoring_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

void EdgeColoring_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b,
                                                             EdgeColoring_AtMost_WitnessPointer w,
                                                             EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // When introducing a vertex into a bag, no edge changes, so, the witness
    // is the same as before.
    EdgeColoring_AtMost_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    witnessSet->insert(w1);
    //*****************************
    //*****************************
}

WitnessSetPointer EdgeColoring_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (EdgeColoring_AtMost_Witness *e = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness)){
        EdgeColoring_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeColoring_AtMost_WitnessSetPointer witnessSet (new EdgeColoring_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

void EdgeColoring_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                             EdgeColoring_AtMost_WitnessPointer w,
                                                             EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    //Getting iterators for keys i and j in the edgesPerVertex map.
    auto iItr = w->edgesPerVertex.find(i);
    auto jItr = w->edgesPerVertex.find(j);

    set<unsigned> possibleEdgeColors;
    set<unsigned> aux; // will hold colors that can't be assigned to the new edge.

    // Getting all possible colors:
    for (auto &edge: iItr->second) {
        aux.insert(edge.second);
    }
    for (auto &edge: jItr->second) {
        aux.insert(edge.second);
    }
    for (int k = 1; k <= this->parameter; ++k) {
        if (!aux.count(k)) possibleEdgeColors.insert(k);
    }

    // If there are possible colors:
    if (!possibleEdgeColors.empty()) {
        // The number of this edge will be the next available in p:
        unsigned edgeNumber = w->next_available_edge_number();
        // For each possible color, introduce an edge colored with
        // this color in w1 and insert it into the witness set:
        for (auto &color: possibleEdgeColors) {
            EdgeColoring_AtMost_WitnessPointer w1 = createWitness();
            w1->set_equal(*w);
            auto iItr1 = w1->edgesPerVertex.find(i);
            auto jItr1 = w1->edgesPerVertex.find(j);
            // Introducing edge <i,j>:
            iItr1->second.insert(pair<unsigned,unsigned>(edgeNumber,color));
            jItr1->second.insert(pair<unsigned,unsigned>(edgeNumber,color));
            w1->availableEdgeNumbers.insert(pair<unsigned,unsigned>(edgeNumber, 2));
            witnessSet->insert(w1);
        }
    }
    // Else, there is no possible witness.
    //*****************************
    //*****************************
}

WitnessSetPointer EdgeColoring_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (EdgeColoring_AtMost_Witness *e = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness)){
        EdgeColoring_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeColoring_AtMost_WitnessSetPointer witnessSet (new EdgeColoring_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}

void EdgeColoring_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b,
                                                              EdgeColoring_AtMost_WitnessPointer w,
                                                              EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // Unique witness: equals to p, but with i vertex removed from edgesPerVertex. If i is the
    // only endpoint of an edge, this edge should be removed from availableEdgeNumbers as well.

    EdgeColoring_AtMost_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);

    // Saving the edges related to the forgotten vertex:
    auto forgottenVertexIterator = w1->edgesPerVertex.find(i);
    map<unsigned,unsigned> forgottenEdges(forgottenVertexIterator->second);

    // Deleting vertex i:
    w1->edgesPerVertex.erase(i);

    // Fixing the number of endpoints of an edge related to the forgotten vertex:
    for (auto &forgottenEdge: forgottenEdges) {
        w1->availableEdgeNumbers[forgottenEdge.first]--;
        // If this edge has no endpoints anymore, it should not be represented in availableEdgeNumbers.
        // Thus, delete it:
        if(w1->availableEdgeNumbers[forgottenEdge.first] == 0) w1->availableEdgeNumbers.erase(forgottenEdge.first);
    }

    witnessSet->insert(w1);
    //*****************************
    //*****************************
}

WitnessSetPointer EdgeColoring_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (EdgeColoring_AtMost_Witness *e = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness)){
        EdgeColoring_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeColoring_AtMost_WitnessSetPointer witnessSet (new EdgeColoring_AtMost_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}

void EdgeColoring_AtMost_DynamicCore::join_implementation(Bag &b, EdgeColoring_AtMost_WitnessPointer w1,
                                                          EdgeColoring_AtMost_WitnessPointer w2,
                                                          EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // For each vertex in the bag, verify if some color appears in an edge incident in
    // the left witness and in the right witness. Obs:The join can only be applied when
    // the bags of the left-side and right-side witnesses are the same.

    auto it1 = w1->edgesPerVertex.begin();
    auto it2 = w2->edgesPerVertex.begin();
    bool contradiction = false;

    // Checking colors:
    while (it1 != w1->edgesPerVertex.end() and !contradiction) {
        for (auto edge1: it1->second) {
            for (auto edge2: it2->second) {
                // If there is an edge with same color in the same vertex, join is impossible.
                // We have a contradiction, so, break.
                if (edge1.second == edge2.second) {
                    contradiction = true;
                    break;
                } // else, continue. There is no contradiction.
            }
        }
        it1++;
        it2++;
    }

    // If there is no contradiction, merge p1 and p2.
    if (!contradiction) {
        EdgeColoring_AtMost_WitnessPointer wPrime1 = createWitness();
        wPrime1->set_equal(*w1); // will represent the joined witness of p1 and p2.

        // Transferring edges from p2 to p1.
        it1 = w1->edgesPerVertex.begin();
        it2 = w2->edgesPerVertex.begin();

        while (it2 != w2->edgesPerVertex.end()) {
            // For each edge from p2, insert it in wPrime1 with its color, but with a different edge number.
            for (auto &edge: it2->second) {
                pair<unsigned,unsigned> mergedEdge;
                // The i-eth edge in p2, will be the i-eth available edge in p1:
                mergedEdge = make_pair(w1->next_available_edge_number(), edge.second);
                wPrime1->edgesPerVertex[mergedEdge.first].insert(mergedEdge);
                wPrime1->availableEdgeNumbers[mergedEdge.first] = w2->availableEdgeNumbers[mergedEdge.first];
            }
            it1++;
            it2++;
        }
        witnessSet->insert(wPrime1);
    }else{
        // do nothing. There's no way to join, so, the witnessSet will be empty.
    }
    //*****************************
    //*****************************
}
WitnessSetPointer EdgeColoring_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(EdgeColoring_AtMost_Witness *e1 = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness1)){
        if(EdgeColoring_AtMost_Witness *e2 = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            EdgeColoring_AtMost_WitnessPointer w1 = e1->shared_from_this();
            EdgeColoring_AtMost_WitnessPointer w2 = e2->shared_from_this();
            EdgeColoring_AtMost_WitnessSetPointer witnessSet (new EdgeColoring_AtMost_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in EdgeColoring_AtMost_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}
WitnessSetPointer EdgeColoring_AtMost_DynamicCore::clean(WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // In most cases, you will not need to change this function.
    //*****************************
    //*****************************
    return witnessSet;
}

bool EdgeColoring_AtMost_DynamicCore::is_final_witness_implementation(EdgeColoring_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    // Count the number of colors.
    set<unsigned> colors;
    for (auto vertex: w->edgesPerVertex) {
        for (auto edge: vertex.second) {
            colors.insert(edge.second);
        }
    }
    // If the number of colors is at most this->parameter, this is a final
    // witness. Otherwise, it is not.
    return colors.size() <= this->parameter;
    //*****************************
    //*****************************
}

bool EdgeColoring_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (EdgeColoring_AtMost_Witness *e = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness)) {
        EdgeColoring_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}
EdgeColoring_AtMost_WitnessPointer EdgeColoring_AtMost_DynamicCore::createWitness() {
    EdgeColoring_AtMost_WitnessPointer w(new EdgeColoring_AtMost_Witness);
    return w;
}

void EdgeColoring_AtMost_DynamicCore::copyWitness(EdgeColoring_AtMost_WitnessPointer w_input,
                                                  EdgeColoring_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}