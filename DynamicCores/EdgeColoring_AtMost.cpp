// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "EdgeColoring_AtMost.h"

bool EdgeColoring_AtMost_Witness::is_equal_implementation(const EdgeColoring_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->mapSetColors == w->mapSetColors;
    //*****************************
    //*****************************
}

bool EdgeColoring_AtMost_Witness::is_less_implementation(const EdgeColoring_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->mapSetColors < w->mapSetColors;
    //*****************************
    //*****************************
}

Witness & EdgeColoring_AtMost_Witness::set_equal_implementation(EdgeColoring_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    this->mapSetColors = w->mapSetColors;
    return *this;
    //*****************************
    //*****************************
}

shared_ptr<Witness> EdgeColoring_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    EdgeColoring_AtMost_WitnessPointer relabeledWitness(new EdgeColoring_AtMost_Witness);
    for(auto m:mapSetColors){
        if(relabelingMap.count(m.first)){
            relabeledWitness->mapSetColors.insert(make_pair(relabelingMap[m.first],m.second));
        }else{
            cout<<"Error: EdgeColoring_AtMost_Witness::relabel "<< m.first<< " is not in the map"<<endl;
            exit(20);
        }
    }
    return relabeledWitness;
}

void EdgeColoring_AtMost_Witness::print() {
    //*****************************
    //*****************************
    // EXPECTED OUTPUT EXAMPLE
    // Witness :
    // (vertex1, { (#edge1, #color1) (#edge2, #color2) } )
    // (vertex2, { (#edge1, #color1) (#edge3, #color4) } )
    cout << "EdgeColoring Witness:";
    for (auto &pairVertexSetColors: this->mapSetColors) {
        cout << "(" << pairVertexSetColors.first << ", " << "{ ";
        for (auto colorIt=pairVertexSetColors.second.begin(); colorIt!=pairVertexSetColors.second.end(); colorIt++) {
            cout << *colorIt;
            if (colorIt != pairVertexSetColors.second.end()--) cout << ",";
        }
        cout << "})";
    }
    //*****************************
    //*****************************
}
string EdgeColoring_AtMost_Witness::witnessInformation() {
    //*****************************
    //*****************************
    // EXPECTED OUTPUT EXAMPLE
    // Witness :
    // (vertex1, { (#edge1, #color1) (#edge2, #color2) } )
    // (vertex2, { (#edge1, #color1) (#edge3, #color4) } )
    string info;
    info = "EdgeColoring Witness:";
    for (auto &pairVertexSetColors: this->mapSetColors) {
        info = info +  "(" + to_string(pairVertexSetColors.first) + ", " + "{ ";
        for (auto colorIt=pairVertexSetColors.second.begin(); colorIt!=pairVertexSetColors.second.end(); colorIt++) {
            info = info + to_string(*colorIt);
            if (colorIt != pairVertexSetColors.second.end()--) info = info + ",";
        }
        info = info +"})";
    }
    return info;
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
    addAttribute("CoreType","Bool");

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
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
    this->parameter = parameter;
    createInitialWitnessSet();
}

void EdgeColoring_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    EdgeColoring_AtMost_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
}

void EdgeColoring_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b,
                                                             EdgeColoring_AtMost_WitnessPointer w,
                                                             EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // We add vertex i to the map with an empty set of used colors
    set<unsigned> emptyColorSet;
    EdgeColoring_AtMost_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    witness->mapSetColors[i]=emptyColorSet;
    witnessSet->insert(witness);
    //*****************************
    //*****************************
}

void EdgeColoring_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                             EdgeColoring_AtMost_WitnessPointer w,
                                                             EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    set<unsigned> allColors;
    for (unsigned color = 1; color<=this->parameter; color++){
        allColors.insert(color);
    }
    set<unsigned> usedColors;
    set_union(w->mapSetColors[i].begin(), w->mapSetColors[i].end(), w->mapSetColors[j].begin(),w->mapSetColors[j].end(),inserter(usedColors,usedColors.begin()));
    set<unsigned> availableColors;
    set_difference(allColors.begin(),allColors.end(),usedColors.begin(),usedColors.end(),inserter(availableColors, availableColors.begin()));
    // if availableColors is empty, then we are trying to assign a color to an edge, but there is no possibility of doing so. The witness in this case is empty.
    for (unsigned color:availableColors){
        EdgeColoring_AtMost_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        witness->mapSetColors[i].insert(color);
        witness->mapSetColors[j].insert(color);
        witnessSet->insert(witness);
    }
    //*****************************
    //*****************************
}

void EdgeColoring_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b,
                                                              EdgeColoring_AtMost_WitnessPointer w,
                                                              EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    EdgeColoring_AtMost_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    witness->mapSetColors.erase(i);
    witnessSet->insert(witness);
    //*****************************
    //*****************************
}
void EdgeColoring_AtMost_DynamicCore::join_implementation(Bag &b, EdgeColoring_AtMost_WitnessPointer w1,
                                                          EdgeColoring_AtMost_WitnessPointer w2,
                                                          EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    bool consistent = true;
    auto it1 = w1->mapSetColors.begin();
    for (auto it2 = w2->mapSetColors.begin(); it2!=w2->mapSetColors.end(); it2++){
        set<unsigned> intersection;
        // it1->first should be equal to it2->first, since all elements of the bag must be in the domain of each map.
        set_intersection(it1->second.begin(),it1->second.end(),it2->second.begin(),it2->second.end(), inserter(intersection,intersection.begin()));
        if (!intersection.empty()){
            consistent = false;
            break;
        }
        it1++; // move to next vertex in the the map of w1
    }
    if (consistent ==true){
        EdgeColoring_AtMost_WitnessPointer witness = createWitness();
        auto it1 = w1->mapSetColors.begin();
        for (auto it2 = w2->mapSetColors.begin(); it2!=w2->mapSetColors.end(); it2++){
            set<unsigned> unioncolors;
            // it1->first should be equal to it2->first, since all elements of the bag must be in the domain of each map.
            set_union(it1->second.begin(),it1->second.end(),it2->second.begin(),it2->second.end(), inserter(unioncolors,unioncolors.begin()));
            witness->mapSetColors[it1->first] = unioncolors;
            it1++; // move to next vertex in the the map of w1
        }
        witnessSet->insert(witness);
    }
    //*****************************
    //*****************************
}

EdgeColoring_AtMost_WitnessSetPointer EdgeColoring_AtMost_DynamicCore::clean_implementation(
        EdgeColoring_AtMost_WitnessSetPointer witnessSet) {
    return witnessSet;
}

bool EdgeColoring_AtMost_DynamicCore::is_final_witness_implementation(EdgeColoring_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    return true; // All witnesses are consistent, since we guarantee that no more than this->parameter colors occur at each step.
    //*****************************
    //*****************************
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool EdgeColoring_AtMost_Witness::is_equal(const Witness &rhs) const{
    if (EdgeColoring_AtMost_Witness const *e = dynamic_cast<EdgeColoring_AtMost_Witness const *>(&rhs)) {
        EdgeColoring_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
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

Witness &EdgeColoring_AtMost_Witness::set_equal(Witness &witness) {
    if (EdgeColoring_AtMost_Witness *e = dynamic_cast<EdgeColoring_AtMost_Witness *>(&witness)) {
        EdgeColoring_AtMost_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoring_AtMost_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> EdgeColoring_AtMost_WitnessSet::createEmptyWitnessSet() {
    EdgeColoring_AtMost_WitnessSetPointer witnessSet(new EdgeColoring_AtMost_WitnessSet);
    return witnessSet;
}

EdgeColoring_AtMost_WitnessPointer EdgeColoring_AtMost_DynamicCore::createWitness() {
    EdgeColoring_AtMost_WitnessPointer w(new EdgeColoring_AtMost_Witness);
    return w;
}

void EdgeColoring_AtMost_DynamicCore::copyWitness(EdgeColoring_AtMost_WitnessPointer w_input,
                                                  EdgeColoring_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void EdgeColoring_AtMost_DynamicCore::createInitialWitnessSet() {
    EdgeColoring_AtMost_WitnessSetPointer witnessSet(new EdgeColoring_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
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
    if (EdgeColoring_AtMost_WitnessSetPointer e = dynamic_pointer_cast<EdgeColoring_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in EdgeColoring_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
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

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new EdgeColoring_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new EdgeColoring_AtMost_DynamicCore(param);
}
}
