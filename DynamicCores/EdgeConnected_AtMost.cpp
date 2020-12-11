// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

///////////////////////////////////////////////////////////////////////////////
// Before starting to work on your core, please create a new branch, and rename
// these files according to the instructions in the file README-GenericCore.txt
// In particular "Generic_Operator" should be renamed everywhere to as specified in the file.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Please use the following commands for standardization purposes
// To create a witness called "witness": EdgeConnected_AtMost_WitnessPointer witness(new EdgeConnected_AtMost_Witness);
// To copy "witness1" into "witness2": copy(witness1,witness2)
// To insert "witness" into "witnessSet":  witnessSet->insert(witness)
/////////////////////////////////////////////////////////////////////////////


#include "EdgeConnected_AtMost.h"

extern "C" {
DynamicCore * create() {
    return new EdgeConnected_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new EdgeConnected_AtMost_DynamicCore(param);
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

bool EdgeConnected_AtMost_Witness::is_equal_implementation(const EdgeConnected_AtMost_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    return (this->found == w->found and this->size == w->size and this->partition == w->partition and
        this->processed == w->processed and this->disconnectedEdges == w->disconnectedEdges
    );

    //*****************************
    //*****************************
}
//TODO
bool EdgeConnected_AtMost_Witness::is_less_implementation(const EdgeConnected_AtMost_WitnessPointerConst w)const{

    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that determines when a witness is less than the other
    // Don't forget to return the result
    // return RESULT;
    //*****************************
    //*****************************

}

Witness& EdgeConnected_AtMost_Witness::set_equal_implementation(EdgeConnected_AtMost_WitnessPointer w){
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that attributes one witness to the other
    // This function should return *this, don't change the line below
    this->found = w->found;
    this->partition = w->partition;
    this->disconnectedEdges = w->disconnectedEdges;
    this->processed = w->processed;
    this->size = w->size;
    return *this;
    //*****************************
    //*****************************
}

//TODO
void EdgeConnected_AtMost_Witness::print() {
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that print the witness
    //*****************************
    //*****************************
}

EdgeConnected_AtMost_Witness::~EdgeConnected_AtMost_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_Witness::removeIncidentEdges(unsigned int i) {
    set<pair<unsigned ,unsigned > > temp; // temp disconnectedEdges
    for(auto e:disconnectedEdges){
        if(e.first != i and e.second != i){
            temp.insert(e);
        }
    }
    disconnectedEdges = temp;
}

pair<bool,bool> removeVertexFromPartition(unsigned int i, set<set<unsigned int>> &partition) {
    set<set<unsigned >> temp;
    bool found = false; // If found = true, it means the new partition is disconnected
    bool processed = false; // If processed = true, it means the new partition is empty
    for(auto cell:partition){
     if(cell.count(i) > 0 ){
         if( cell.size()==1 and partition.size()>1){
             found = true; // This means, partition will be disconnected
         }else if(cell.size()==1 and partition.size() == 1){
             processed = true;
         }else{
             cell.erase(i);
             temp.insert(cell);
         }
     }else{
         temp.insert(cell);
     }
    }
    partition = temp;
    return make_pair(found,processed);
}

void addEdgeToPartition(unsigned int i, unsigned int j, set<set<unsigned int>> &partition) {
    set<unsigned> s1, s2;
    //CHECK: change "part" with "cell"
    for(auto cell : partition) {
        if(cell.count(i) > 0) {
            s1 = cell;
        }
        if(cell.count(j) > 0) {
            s2 = cell;
        }
    }
    if(s1 != s2 and !s1.empty() and !s2.empty()) {
        set<unsigned> intermediate;
        set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
                  inserter(intermediate, intermediate.end()));
        partition.erase(s1);
        partition.erase(s2);
        partition.insert(intermediate);
    }

}

set<set<unsigned >> mergePartitions(set<set<unsigned int>> &partition1, set<set<unsigned int>> &partition2) {
    map<unsigned, int> eleToCellMap;
    int cells = 1;
    for(auto cell : partition1) {
        for(auto ele : cell) {
            eleToCellMap[ele] = cells;
        }
        cells++;
    }
    for(auto cell : partition2) {
        set<unsigned> s; // This set is initialized to empty
        for(auto ele : cell) {
            if(eleToCellMap.find(ele) != eleToCellMap.end()) {
                s = cell;
                break;
            }
        }
        if(!s.empty()) {
            set<unsigned> otherCellNumbers;
            unsigned parent = 1e9;
            for(auto i : s) {
                if(eleToCellMap.find(i) != eleToCellMap.end()) {
                    otherCellNumbers.insert(eleToCellMap[i]);
                    parent = min(parent, (unsigned) eleToCellMap[i]);
                } else {
                    eleToCellMap[i] = -1;
                }
            }
            // change the cell number
            for(auto pair : eleToCellMap) { // iterating through all pairs in the map.
                if(otherCellNumbers.find(pair.second) != otherCellNumbers.end() || pair.second == -1) {
                    // If the condition is not satisfied, then the element of the map is not related
                    // to the cell we are analysing at the moment.
                    pair.second = parent;
                }
            }
            cells = cells - otherCellNumbers.size() + 1;
        } else {
            // If the element does not occur in some cell of w1, then we create a cell specifically for that element.
            cells++;
            unsigned parent = cells;
            for(auto ele : cell) {
                eleToCellMap[ele] = parent;
            }
        }
    }
    // Now group all the elements according to their cell numbers and add them to resulting cell.
    map<int, set<unsigned>> result; //The first coordinate is the cell number. The second coordinate is the set of elements in that cell
    for(auto pair : eleToCellMap) {
        result[pair.second].insert(pair.first);
    }
    set<set<unsigned >> partition;
    for(auto pair : result) {
        partition.insert(pair.second);
    }
    return partition;
}

EdgeConnected_AtMost_DynamicCore::EdgeConnected_AtMost_DynamicCore(){

    //*****************************
    //*****************************
    // Only edit this part
    // In this function you can initialize the map attributes. These attributes are used to describe properties of the core.
    // Example:
    addAttribute("CoreName","EdgeConnected"); // Obligatory attribute. Replace GenericName by the name of the core.
    addAttribute("ParameterType","UnsignedInt"); // Obligatory attribute. Replace GenericType by the type of the core: "NoParameter", "UnsignedInt", "InputFile".
    addAttribute("PrimaryOperator","AtMost"); //  This line should be uncommented if the type of the core is "UnsignedInt".
    //  Replace GenericOperator with "AtLeast" or "AtMost". The correct initialization depends on the core.
    //  ...
    //  Additional optional attributes.
    //*****************************
    //*****************************

}

EdgeConnected_AtMost_DynamicCore::EdgeConnected_AtMost_DynamicCore(unsigned parameter){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    this->parameter = parameter;
    createInitialWitnessSet();
}
void EdgeConnected_AtMost_DynamicCore::createInitialWitnessSet_implementation(){
    //*****************************
    //*****************************
    // Here you will initialize initialWitnessSet by adding witnesses to it.
    // In most cases, there will be only one initial witness.
    // The next two lines show how to create a witness and add it to a witnessSet.
    // EdgeConnected_AtMost_WitnessPointer witness(new EdgeConnected_AtMost_Witness);
    // Here, you can modify the witness and then insert to the witnessSet.
    // this->insertIntoInitialWitnessSet(witness);
    // If there are more, these should also be added to initialWitnessSet.
    EdgeConnected_AtMost_WitnessPointer witness = createWitness();
    witness->found = false;
    witness->processed = false;
    this->insertIntoInitialWitnessSet(witness);
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_DynamicCore::intro_v_implementation(unsigned i, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found ){
        // In this case, a disconnectedness has been found, therefore, we will return a witness with "found=true",
        // and other attributes are empty
        EdgeConnected_AtMost_WitnessPointer witness = createWitness();
        witness->found = true;
        witness->processed = false;
        witnessSet->insert(witness);
        return ;
    }else if(w->processed){
        // In this case, since "processed" is true, by adding "i", the partition will be disconnected,
        // therefore, this is disconnectedness, and we set "found = true".
        EdgeConnected_AtMost_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        witness->found = true;
        // TODO: can we add "i" to the partition?
        set<unsigned > p; // new partition by adding "i"
        p.insert(i);
        witness->partition.insert(p);
        // TODO
        witnessSet->insert(witness);
        return;
    }else{
        // In this case, we can add "i" as a new cell to the partition.
        EdgeConnected_AtMost_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        set<unsigned > p; // new partition by adding "i"
        p.insert(i);
        witness->partition.insert(p);
        witnessSet->insert(witness);
        return;
    }
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    EdgeConnected_AtMost_WitnessPointer witness1 = createWitness();

    if(w->found){
        // In this case, a disconnectedness has been found, therefore, we will return a witness with "found=true",
        // and other attributes are empty
        witness1->found = true;
        witness1->processed = false;
        witnessSet->insert(witness1);
        return;
    }else{
        // In this case, we have two options, add {i,j} to disconnectedEdges or not.
        witness1->set_equal(*w);
        addEdgeToPartition(i,j,witness1->partition);
        witnessSet->insert(witness1);
        // Here, if size < parameter we want to add {i,j} to disconnectedEdges
        if(w->size < parameter){
            EdgeConnected_AtMost_WitnessPointer  witness2 = createWitness();
            witness2->set_equal(*w);
            witness2->disconnectedEdges.insert(make_pair(min(i,j),max(i,j)));
            witnessSet->insert(witness2);
        }
        return;
    }
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_DynamicCore::forget_v_implementation(unsigned i, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    EdgeConnected_AtMost_WitnessPointer witness = createWitness();
    if(w->found ){
        // In this case, a disconnectedness has been found, therefore, we will return a witness with "found=true",
        // and other attributes are empty
        witness->found = true;
        witness->processed = false;
        witnessSet->insert(witness);
        return;
    }else{
        // In this case, we remove all incident edges to "i" from disconnected edges set, and remove it from the partition
        witness->set_equal(*w);
        // Remove incident edges
        witness->removeIncidentEdges(i);
        pair<bool,bool> result;
        result = removeVertexFromPartition(i,witness->partition);
        witness->found = result.first;
        witness->processed = result.second;
        return;
    }
    //*****************************
    //*****************************
}
void EdgeConnected_AtMost_DynamicCore::join_implementation(Bag &b, EdgeConnected_AtMost_WitnessPointer w1, EdgeConnected_AtMost_WitnessPointer w2, EdgeConnected_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    EdgeConnected_AtMost_WitnessPointer witness = createWitness();
    if(w1->found or w2->found){
        witness->found = true;
        witness->processed = false;
        witnessSet->insert(witness);
        return;
    }else{
        // First Step, construct the union of the disconnectedEdges of both witnesses
        set<pair<unsigned ,unsigned >> edgeUnion;
        set_union(w1->disconnectedEdges.begin(),w1->disconnectedEdges.end(),
                  w2->disconnectedEdges.begin(),w2->disconnectedEdges.end(), inserter(edgeUnion,edgeUnion.begin()));
        // Check the size of the disconnectedEdge that both witnesses are create.
        // The size is w1->size + w2->size - 2*|edgeUnion|
        unsigned size  = w1->size + w2->size - 2*edgeUnion.size();
        if(size > parameter){
            // If size is bigger than parameter, then it is not valid
            return;
        }else{
            if(w1->processed and w2->processed){
                return;
            }else if(w1->processed and !w2->partition.empty()){
                return;
            }else if(w2->processed and !w1->partition.empty()){
                return;
            }
            witness->found = false;
            witness->disconnectedEdges = edgeUnion;
            witness->size = size;
            witness->partition = mergePartitions(w1->partition,w2->partition);
        }
    }
    //*****************************
    //*****************************
}

shared_ptr<WitnessSet> EdgeConnected_AtMost_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    //*****************************
    //*****************************
    // In most cases, you will not need to change this function.
    //*****************************
    //*****************************
    return witnessSet;
}
bool EdgeConnected_AtMost_DynamicCore::is_final_witness_implementation(EdgeConnected_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    return w->found;
    //*****************************
    //*****************************
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


bool EdgeConnected_AtMost_Witness::is_equal(const Witness &rhs) const{
    if (EdgeConnected_AtMost_Witness const *e = dynamic_cast<EdgeConnected_AtMost_Witness const *>(&rhs)) {
        EdgeConnected_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeConnected_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool EdgeConnected_AtMost_Witness::is_less(const Witness &rhs)const {
    if (EdgeConnected_AtMost_Witness const *e = dynamic_cast<EdgeConnected_AtMost_Witness const *>(&rhs)) {
        EdgeConnected_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeConnected_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness& EdgeConnected_AtMost_Witness::set_equal(Witness &witness) {
    if (EdgeConnected_AtMost_Witness *e = dynamic_cast<EdgeConnected_AtMost_Witness *>(&witness)) {
        EdgeConnected_AtMost_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeConnected_AtMost_Witness::operator= cast error"<<endl;
        exit(20);
    }
}


void  EdgeConnected_AtMost_DynamicCore::createInitialWitnessSet() {
    EdgeConnected_AtMost_WitnessSetPointer witnessSet(new EdgeConnected_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

EdgeConnected_AtMost_WitnessPointer EdgeConnected_AtMost_DynamicCore::createWitness(){
    EdgeConnected_AtMost_WitnessPointer w(new EdgeConnected_AtMost_Witness);
    return w;
}

void EdgeConnected_AtMost_DynamicCore::copyWitness(EdgeConnected_AtMost_WitnessPointer w_input, EdgeConnected_AtMost_WitnessPointer w_output){
    w_output->set_equal_implementation(w_input);
}


WitnessSetPointer EdgeConnected_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (EdgeConnected_AtMost_Witness *e = dynamic_cast<EdgeConnected_AtMost_Witness *>(&witness)){
        EdgeConnected_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeConnected_AtMost_WitnessSetPointer witnessSet (new EdgeConnected_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet); // Witnesses are created and added to witnessSet in this funciton.
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeConnected_AtMost_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer EdgeConnected_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (EdgeConnected_AtMost_Witness *e = dynamic_cast<EdgeConnected_AtMost_Witness *>(&witness)){
        EdgeConnected_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeConnected_AtMost_WitnessSetPointer witnessSet (new EdgeConnected_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in EdgeConnected_AtMost_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> EdgeConnected_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (EdgeConnected_AtMost_Witness *e = dynamic_cast<EdgeConnected_AtMost_Witness *>(&witness)){
        EdgeConnected_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeConnected_AtMost_WitnessSetPointer witnessSet (new EdgeConnected_AtMost_WitnessSet);
        forget_v_implementation(i, b, w, witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeConnected_AtMost_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}


shared_ptr<WitnessSet> EdgeConnected_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(EdgeConnected_AtMost_Witness *e1 = dynamic_cast<EdgeConnected_AtMost_Witness *>(&witness1)){
        if(EdgeConnected_AtMost_Witness *e2 = dynamic_cast<EdgeConnected_AtMost_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            EdgeConnected_AtMost_WitnessPointer w1 = e1->shared_from_this();
            EdgeConnected_AtMost_WitnessPointer w2 = e2->shared_from_this();
            EdgeConnected_AtMost_WitnessSetPointer witnessSet (new EdgeConnected_AtMost_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in EdgeConnected_AtMost_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in EdgeConnected_AtMost_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

bool EdgeConnected_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (EdgeConnected_AtMost_Witness *e = dynamic_cast<EdgeConnected_AtMost_Witness *>(&witness)) {
        EdgeConnected_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeConnected_AtMost_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}