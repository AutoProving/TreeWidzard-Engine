// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

///////////////////////////////////////////////////////////////////////////////
// Before starting to work on your core, please create a new branch, and rename
// these files according to the instructions in the file README-GenericCore.txt
// In particular "TopologicalMinor" should be renamed everywhere to as specified in the file.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Please use the following commands for standardization purposes
// To create a witness called "witness": TopologicalMinor_WitnessPointer witness(new TopologicalMinor_Witness);
// To copy "witness1" into "witness2": copy(witness1,witness2)
// To insert "witness" into "witnessSet":  witnessSet->insert(witness)
/////////////////////////////////////////////////////////////////////////////


#include "TopologicalMinor.h"

extern "C" {
DynamicCore * create() {
    return new TopologicalMinor_DynamicCore;
}
DynamicCore * create_multiGraph(MultiGraph multiGraph) {
    return new TopologicalMinor_DynamicCore(multiGraph);
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

bool TopologicalMinor_Witness::is_equal_implementation(const TopologicalMinor_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    return (this->found  == w->found and this->partitions == w->partitions
            and this->edgeFoundVector == w->edgeFoundVector);
    //*****************************
    //*****************************
}

bool TopologicalMinor_Witness::is_less_implementation(const TopologicalMinor_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    if(this->found != w->found) return found;
    if(this->partitions != w->partitions) return this->partitions<w->partitions;
    return this->edgeFoundVector < w->edgeFoundVector;
    //*****************************
    //*****************************
}

Witness& TopologicalMinor_Witness::set_equal_implementation(TopologicalMinor_WitnessPointer w){
    //*****************************
    //*****************************
    this->found = w->found;
    this->partitions = w->partitions;
    this->edgeFoundVector = w->edgeFoundVector;
    return *this;
    //*****************************
    //*****************************
}

//TODO: Implement this function
void TopologicalMinor_Witness::print() {
    //*****************************
    //*****************************
    // Only edit this part
    // Here you will implement the code that print the witness
    //*****************************
    //*****************************
}

TopologicalMinor_Witness::~TopologicalMinor_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

TopologicalMinor_DynamicCore::TopologicalMinor_DynamicCore(){

    //*****************************
    //*****************************

    addAttribute("CoreName","TopologicalMinor"); // Obligatory attribute. Replace GenericName by the name of the core.
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","InputFile"); // Obligatory attribute. Replace GenericType by the type of the core: "NoParameter", "UnsignedInt", "InputFile".
    //  addAttribute("PrimaryOperator","GenericOperator"); //  This line should be uncommented if the type of the core is "UnsignedInt".
    //  Replace GenericOperator with "AtLeast" or "AtMost". The correct initialization depends on the core.
    //  ...
    //  Additional optional attributes.
    //*****************************
    //*****************************

}

TopologicalMinor_DynamicCore::TopologicalMinor_DynamicCore(MultiGraph multiGraph){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    this->multigraph = multiGraph;
    createInitialWitnessSet();
}
void TopologicalMinor_DynamicCore::createInitialWitnessSet_implementation(){
    //*****************************
    //*****************************
    TopologicalMinor_WitnessPointer initialWitness = createWitness();
    vector<pair<unsigned ,bool>> edgeFoundVector;
    set<unsigned > edges = multigraph.getEdges();
    for(auto e:edges){
        edgeFoundVector.push_back(make_pair(e,false));
    }
    vector<tuple<set<unsigned >, set<set<unsigned >>,bool> > partitions;
    partitions.resize(multigraph.verticesSize());
    //TODO: Is the initial bool is false or true?
    initialWitness->found = false;
    initialWitness->partitions = partitions;
    initialWitness->edgeFoundVector = edgeFoundVector;
    //*****************************
    //*****************************
}

void TopologicalMinor_DynamicCore::intro_v_implementation(unsigned i, Bag &b, TopologicalMinor_WitnessPointer w, TopologicalMinor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    // Since we can either include "i" in the partitions or not, the immediate witness is 'w".
    witnessSet->insert(w);
    if(w->found){
        // If found is true, it means the TopologicalMinor has been found
    }else{
        // We can insert "i" into one the none-processed partition of the partitions.
        for(size_t m=0; m < w->partitions.size(); m++ ){
            if(!get<2>(w->partitions[m])){
                TopologicalMinor_WitnessPointer witness = createWitness();
                witness->set_equal(*w);
                get<0>(witness->partitions[m]).insert(i);
                set<unsigned > cell;
                cell.insert(i);// Add "{i}" as a cell into the partition.
                get<1>(witness->partitions[m]).insert(cell);
                witnessSet->insert(witness);
            }
        }
    }
    //*****************************
    //*****************************
}

void TopologicalMinor_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b, TopologicalMinor_WitnessPointer w, TopologicalMinor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found){
        witnessSet->insert(w);
    }else{
        int iIndex=-1;
        int jIndex=-1;
        for(size_t m=0; m < w->partitions.size(); m++ ){
            if(get<0>(w->partitions[m]).count(i) ){
                iIndex = m;
            }
            if(get<0>(w->partitions[m]).count(j)){
                jIndex = m;
            }
        }
        if( iIndex==jIndex and iIndex!=-1 ){
            // "i" and "j" are in a same partition
            // We introduce {i,j} to partitions[iIndex]
            TopologicalMinor_WitnessPointer witness = createWitness();
            witness->set_equal(*w);
            witness->addEdgeToPartition(i,j,get<1>(witness->partitions[iIndex]));
            witnessSet->insert(witness);
        }else if(iIndex!=jIndex and iIndex!=-1 and jIndex!=-1){
            // "i" and "j" belong to different partitions
            TopologicalMinor_WitnessPointer witness = createWitness();
            witness->set_equal(*w);
            // Get labels of "iIndex"th  and "jIndex"th vertices of the graph
            unsigned iLabel = multigraph.nthVertex(iIndex);
            unsigned jLabel = multigraph.nthVertex(jIndex);
            set<unsigned > edges = multigraph.edgesBetweenVertices(iLabel,jLabel);
            if(edges.size()==0){
                //nothing
            }else{
                // Change the Boolean value of the edges inside "edges" to true.
                for(auto e:witness->edgeFoundVector){
                    if(edges.count(e.first)){
                        e.second = true;
                    }
                }
            }
            witnessSet->insert(witness);
        }else{
            //TODO: iIndex or jIndex is -1, and it shouldn't happen, What I have to return?
        }
    }
    //*****************************
    //*****************************
}

void TopologicalMinor_DynamicCore::forget_v_implementation(unsigned i, Bag &b, TopologicalMinor_WitnessPointer w, TopologicalMinor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found){
        witnessSet->insert(w);
    }else{
        TopologicalMinor_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        for(auto partition: witness->partitions){
            if(get<0>(partition).count(i)){
                // Remove from the set
                get<0>(partition).erase(i);
                pair<bool, bool> result = witness->removeVertexFromPartition(i, get<1>(partition));
                if(result.first){
                    // Here, the partition is disconnected
                    break;
                }else{
                    // change the processed value of the partition
                    get<2>(partition) = result.second;
                    witnessSet->insert(witness);
                    break;
                }
            }
        }
    }
    //*****************************
    //*****************************
}
void TopologicalMinor_DynamicCore::join_implementation(Bag &b, TopologicalMinor_WitnessPointer w1, TopologicalMinor_WitnessPointer w2, TopologicalMinor_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************

    //*****************************
    //*****************************
}

shared_ptr<WitnessSet> TopologicalMinor_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    //*****************************
    //*****************************
    // In most cases, you will not need to change this function.
    //*****************************
    //*****************************
    return witnessSet;
}
bool TopologicalMinor_DynamicCore::is_final_witness_implementation(TopologicalMinor_WitnessPointer w) {
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


bool TopologicalMinor_Witness::is_equal(const Witness &rhs) const{
    if (TopologicalMinor_Witness const *e = dynamic_cast<TopologicalMinor_Witness const *>(&rhs)) {
        TopologicalMinor_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in TopologicalMinor_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}



bool TopologicalMinor_Witness::is_less(const Witness &rhs)const {
    if (TopologicalMinor_Witness const *e = dynamic_cast<TopologicalMinor_Witness const *>(&rhs)) {
        TopologicalMinor_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in TopologicalMinor_Witness::is_less cast error"<<endl;
        exit(20);
    }
}



Witness& TopologicalMinor_Witness::set_equal(Witness &witness) {
    if (TopologicalMinor_Witness *e = dynamic_cast<TopologicalMinor_Witness *>(&witness)) {
        TopologicalMinor_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in TopologicalMinor_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

void  TopologicalMinor_DynamicCore::createInitialWitnessSet() {
    TopologicalMinor_WitnessSetPointer witnessSet(new TopologicalMinor_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

TopologicalMinor_WitnessPointer TopologicalMinor_DynamicCore::createWitness(){
    TopologicalMinor_WitnessPointer w(new TopologicalMinor_Witness);
    return w;
}

void TopologicalMinor_DynamicCore::copyWitness(TopologicalMinor_WitnessPointer w_input, TopologicalMinor_WitnessPointer w_output){
    w_output->set_equal_implementation(w_input);
}


WitnessSetPointer TopologicalMinor_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (TopologicalMinor_Witness *e = dynamic_cast<TopologicalMinor_Witness *>(&witness)){
        TopologicalMinor_WitnessPointer w = e->shared_from_this();
        TopologicalMinor_WitnessSetPointer witnessSet (new TopologicalMinor_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet); // Witnesses are created and added to witnessSet in this funciton.
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in TopologicalMinor_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer TopologicalMinor_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (TopologicalMinor_Witness *e = dynamic_cast<TopologicalMinor_Witness *>(&witness)){
        TopologicalMinor_WitnessPointer w = e->shared_from_this();
        TopologicalMinor_WitnessSetPointer witnessSet (new TopologicalMinor_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in TopologicalMinor_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> TopologicalMinor_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (TopologicalMinor_Witness *e = dynamic_cast<TopologicalMinor_Witness *>(&witness)){
        TopologicalMinor_WitnessPointer w = e->shared_from_this();
        TopologicalMinor_WitnessSetPointer witnessSet (new TopologicalMinor_WitnessSet);
        forget_v_implementation(i, b, w, witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in TopologicalMinor_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}


shared_ptr<WitnessSet> TopologicalMinor_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(TopologicalMinor_Witness *e1 = dynamic_cast<TopologicalMinor_Witness *>(&witness1)){
        if(TopologicalMinor_Witness *e2 = dynamic_cast<TopologicalMinor_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            TopologicalMinor_WitnessPointer w1 = e1->shared_from_this();
            TopologicalMinor_WitnessPointer w2 = e2->shared_from_this();
            TopologicalMinor_WitnessSetPointer witnessSet (new TopologicalMinor_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in TopologicalMinor_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in TopologicalMinor_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

bool TopologicalMinor_DynamicCore::is_final_witness(Witness &witness) {
    if (TopologicalMinor_Witness *e = dynamic_cast<TopologicalMinor_Witness *>(&witness)) {
        TopologicalMinor_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in TopologicalMinor_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}