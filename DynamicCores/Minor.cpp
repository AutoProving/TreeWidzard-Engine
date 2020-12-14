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
    return (this->found  == w->found and this->partitions == w->partitions
            and this->foundEdges == w->foundEdges);
    //*****************************
    //*****************************
}

bool Minor_Witness::is_less_implementation(const Minor_WitnessPointerConst w)const{
    //*****************************
    //*****************************
    if(this->found != w->found) return found;
    if(this->partitions != w->partitions) return this->partitions<w->partitions;
    return this->foundEdges < w->foundEdges;
    //*****************************
    //*****************************
}

Witness& Minor_Witness::set_equal_implementation(Minor_WitnessPointer w){
    //*****************************
    //*****************************
    this->found = w->found;
    this->partitions = w->partitions;
    this->foundEdges = w->foundEdges;
    return *this;
    //*****************************
    //*****************************
}

//TODO: Implement this function
void Minor_Witness::print(){
    //TODO
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
    // vector<pair<unsigned ,bool>> foundEdges;
    set<unsigned int> foundEdges; 
    //set<unsigned > edges = multigraph.getEdges();
    //for(auto e:edges){
    //    foundEdges.push_back(make_pair(e,false));
    //}
    vector<tuple<set<unsigned >, set<set<unsigned >>,bool,bool> > partitions;
    partitions.resize(multigraph.verticesSize());
    initialWitness->found = false;
    initialWitness->partitions = partitions;
    initialWitness->foundEdges = foundEdges;
    //*****************************
    //*****************************
}

void Minor_DynamicCore::intro_v_implementation(unsigned i, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    // There are two options. Either we add 'i' the the component corresponding to
    // a vertex of the pattern graph or we do nothing with 'i'
    // If 'i' is not added, the witness remains the same, so w is always included in the partition.   
    witnessSet->insert(w);
    if(!(w->found)){
        // We will add 'i' to each non-processed partition. 
        for(size_t m=0; m < w->partitions.size(); m++ ){
	    //if T is a tuple get<i>(T) is the i-th coordinate of T
            if(!get<2>(w->partitions[m])){ //If the partition at position m has not been processed (i.e. the processed flag is 0)
                Minor_WitnessPointer witness = createWitness();
                witness->set_equal(*w);
                get<0>(witness->partitions[m]).insert(i); // we add 
                set<unsigned int> cell;
                cell.insert(i);// Add "{i}" as a cell into the partition.
                get<1>(witness->partitions[m]).insert(cell);
                //get<3>(witness->partitions[m]) = true; //TODO REMOVE THIS. 
                witnessSet->insert(witness);
            }
        }
    }
    //*****************************
    //*****************************
}

void Minor_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found){
        witnessSet->insert(w);
    }else{
        int iIndex=-1; // index of the vector corresponding to the component where i belongs to. -1 if i belongs to no such component
        int jIndex=-1; // index of the vector corresponding to the component where j belongs to. -1 if j belongs to no such component
	// The next for loop is used to find the correct values of iIndex and jIndex
        for(size_t m=0; m < w->partitions.size(); m++ ){
            if(get<0>(w->partitions[m]).count(i) ){
                iIndex = m;
            }
            if(get<0>(w->partitions[m]).count(j)){
                jIndex = m;
            }
        }
	// There are three cases to be considered.
	// One: if i and j belong to the same component
	// Two: if i and j belong to distinct components
	// Three: either i or j belong to no component 
        if( iIndex==jIndex and iIndex!=-1 ){
            // "i" and "j" are in a same partition
            // We introduce {i,j} to partitions[iIndex]
   
    	    Minor_WitnessPointer witness = createWitness();
            witness->set_equal(*w);
            witness->addEdgeToPartition(i,j,get<1>(witness->partitions[iIndex]));
	    //TODO CHECK IF MINOR WAS FOUND
            witnessSet->insert(witness);
        }else if(iIndex!=jIndex and iIndex!=-1 and jIndex!=-1){
            // "i" and "j" belong to different partitions
            Minor_WitnessPointer witness = createWitness();
            witness->set_equal(*w);
            // Get labels of "iIndex"th  and "jIndex"th vertices of the graph
            unsigned iLabel = multigraph.nthVertex(iIndex);
            unsigned jLabel = multigraph.nthVertex(jIndex);
            set<unsigned int> incidentEdges = multigraph.edgesBetweenVertices(iLabel,jLabel);
	    set<unsigned int> notYetFound; 
	    set_difference(incidentEdges.begin(),incidentEdges.end(),foundEdges.begin(),foundEdges.end(),inserter(notYetFound,notYetFound.begin())); 
            if(notYetFound.size()==0){
                witnessSet->insert(w); // The new introduced edge is simply not part of the minor
            }else{
	    	int newedge = *notYetFound.begin(); 
	    	foundEdges.insert(newEdge); 
	    	//TODO CHECK IF MINOR WAS FOUND
            	witnessSet->insert(witness);
            }
        }else{
		// Case Three: either i or j belong to no component 
		// In this case, {i,j} is simply not added to the minor
		// and therefore only the input witness is returned
	   	witnessSet.insert(w); 
        }
    }
    //*****************************
    //*****************************
}

void Minor_DynamicCore::forget_v_implementation(unsigned i, Bag &b, Minor_WitnessPointer w, Minor_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found){
        witnessSet->insert(w);
    }else{
        Minor_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        bool isInSomeComponent = false;
	for(auto partition: witness->partitions){
            if(get<0>(partition).count(i)){
		isInSomeComponent = true; 
                // Remove from the set
                get<0>(partition).erase(i);
                pair<bool, bool> result = witness->removeVertexFromPartition(i, get<1>(partition)); // the first coordinate is true if the partition becomes disconnected (bad case)
                if(!result.first){ //Meaning that removing the vertex does not disconnect the component represented by the partition 
                    // change the processed value of the partition
                    get<2>(partition) = result.second;
                    witnessSet->insert(witness);
                    break;
                } else {
		    break; // In this case the component where i belongs to becomes disconnected. No wintess is returned in this case.
		}
            }
        }
	// If 'i' is not in any component, just return the singleton with the input witness
	if (!isInSomeComponent){
		witnessSet.insert(w); 	
	}

    }
    //*****************************
    //*****************************
}
void Minor_DynamicCore::join_implementation(Bag &b, Minor_WitnessPointer w1, Minor_WitnessPointer w2, Minor_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    if(w1->found){
        witnessSet->insert(w1);
    }else if(w2->found){
        witnessSet->insert(w2);
    }else{
        Minor_WitnessPointer witness = createWitness();
        set<unsigned > visitedVertices;
        bool successFlag = true;
        for(size_t n=0; n< w1->partitions.size(); n++){
            if((get<2>(w1->partitions[n]) and get<2>(w2->partitions[n])) or
               (get<2>(w1->partitions[n]) and !get<1>(w2->partitions[n]).empty()) or
               (get<2>(w2->partitions[n]) and !get<1>(w1->partitions[n]).empty()) ){
                // not valid
                successFlag = false;
                break;
            }else{
                // Check existence of a vertex which has been visited
                set<unsigned > unionSet;
                set_union(get<0>(w1->partitions[n]).begin(),get<0>(w1->partitions[n]).end(),
                          get<0>(w2->partitions[n]).begin(), get<0>(w2->partitions[n]).end(),
                          inserter(unionSet, unionSet.begin()));
                set<unsigned > intersectionSet;
                set_intersection(unionSet.begin(),unionSet.end(),visitedVertices.begin(),visitedVertices.end(),
                                 inserter(intersectionSet,intersectionSet.begin()));
                if(!intersectionSet.empty()){
                    // not valid
                    successFlag = false;
                    break;
                }else{
                    //  Add setUnion to visited vertices
                    set<unsigned > temp;
                    set_union(visitedVertices.begin(),visitedVertices.end(),unionSet.begin(),unionSet.end(),
                              inserter(temp,temp.end()));
                    visitedVertices = temp;
                    // merge the partitions[n] of w1 and w2
                    set<set<unsigned >> newPartition;
                    newPartition = witness->mergePartitions(get<1>(w1->partitions[n]), get<1>(w2->partitions[n]));
                    get<0>(witness->partitions[n]) = unionSet;
                    get<1>(witness->partitions[n]) = newPartition;
                    get<2>(witness->partitions[n]) = (get<2>(w1->partitions[n]) or get<2>(w1->partitions[n]) );
                    //get<3>(witness->partitions[n]) = (get<3>(w1->partitions[n]) or get<3>(w1->partitions[n]) ); TODO remove this.
                }
            }
        }
        if(successFlag){
            // update edges' value
            for(size_t n=0; n < multigraph.edgesSize(); n++){
                witness->foundEdges[n].first = w1->foundEdges[n].first;
                witness->foundEdges[n].second = (w1->foundEdges[n].second or w2->foundEdges[n].second);
            }
            //TODO: How check found?
            witnessSet->insert(witness);
        }
    }
    //*****************************
    //*****************************
}

bool Minor_DynamicCore::is_final_witness_implementation(Minor_WitnessPointer w) {
    //*****************************
    //*****************************
    return w->found;
    //*****************************
    //*****************************
}

shared_ptr<WitnessSet> Minor_DynamicCore::clean_implementation(Minor_WitnessSetPointer witnessSet) {
    for(auto witness:(*witnessSet)){
        if (Minor_WitnessPointer w = dynamic_pointer_cast<Minor_Witness>(witness)) {
            if(w->found){
                Minor_WitnessSetPointer newWitnessSet (new Minor_WitnessSet);
                newWitnessSet->insert(w);
                return newWitnessSet;
            }
        }else{
            cerr<<"ERROR: in Minor_DynamicCore::clean_implementation cast error"<<endl;
            exit(20);
        }
    }
    return witnessSet;
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

shared_ptr<WitnessSet> Minor_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {

    if (Minor_WitnessSetPointer e = dynamic_pointer_cast<Minor_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in Minor_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new Minor_DynamicCore;
}
DynamicCore * create_multiGraph(MultiGraph multiGraph) {
    return new Minor_DynamicCore(multiGraph);
}
}
