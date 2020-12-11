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
    return (this->found == w->found and 
	    this->processed == w->processed and
            this->size == w->size and 
	    this->partition == w->partition and
	    this->disconnectingEdges == w->disconnectingEdges);
    //*****************************
    //*****************************
}


bool EdgeConnected_AtMost_Witness::is_less_implementation(const EdgeConnected_AtMost_WitnessPointerConst w)const{
    //*****************************
    //*****************************
	if (this->found != w->found) return (this->found); // true is smaller than false in this case 
	if (this->processed != w->processed) return (this->processed < w->processed); 
	if (this->size != w->size) return (this->size < w->size); 
	if (this->partition != w->partition) return (this->partition < w->partition); 
	return this->disconnectingEdges < w->disconnectingEdges;
    //*****************************
    //*****************************
}

Witness& EdgeConnected_AtMost_Witness::set_equal_implementation(EdgeConnected_AtMost_WitnessPointer w){
    //*****************************
    //*****************************
    this->found = w->found;
    this->partition = w->partition;
    this->disconnectingEdges = w->disconnectingEdges;
    this->processed = w->processed;
    this->size = w->size;
    return *this;
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_Witness::print() {
    //*****************************
    //*****************************
    cout << "found: " << this->found << "  processed: "<< this->processed << "  size: " << this->size << endl;
    cout << "disconnecting edges: {"; 
    for (auto it:this->disconnectingEdges){
	cout<< "(" << *it.first << "," << *it.second << ")";  
    	if (it!=disconnectingEdges.end()-1) cout << ","; 
    }
    cout<<"}" << endl;
    cout << "partition: ("; 
    for (auto it:this->partition){
    	cout << "{" ;
	for (auto itPrime:(*it)){
		cout << *itPrime ; 
		if (itPrime!=(*it).end()-1) cout << ","; 
	}
	cout << "}" ; 
    }
    cout << ")" << endl; 
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
    set<pair<unsigned ,unsigned > > temp; // temp disconnectingEdges
    for(auto e:disconnectingEdges){
        if(e.first != i and e.second != i){
            temp.insert(e);
        }
    }
    disconnectingEdges = temp;
}

pair<bool,bool> removeVertexFromPartition(unsigned int i, set<set<unsigned int>> &partition) {
    set<set<unsigned >> temp;
    bool found = false; // If found = true, it means the new partition is disconnected
    bool processed = false; // If processed = true, it means the new partition is empty
    for(auto cell:partition){
    	if(cell.count(i) > 0 ){
        	if(cell.size()==1){
			if (partition.size()>1){
             			found = true; // This means, partition will be disconnected
	         	}else if(partition.size() == 1){
		             processed = true;
			} 
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


//
//// TODO: Implement a generic merge function for partitions. 
//set<set<unsigned >> mergePartitions(set<set<unsigned int>> &partition1, set<set<unsigned int>> &partition2) {
//    map<unsigned, int> eleToCellMap;
//    int ncells = 1;
//    for(auto cell : partition1) {
//        for(auto ele : cell) {
//            eleToCellMap[ele] = ncells;
//        }
//        ncells++;
//    }
//    for(auto cell : partition2) {
//        set<unsigned> s; // This set is initialized to empty
//        for(auto ele : cell) {
//            if(eleToCellMap.find(ele) != eleToCellMap.end()) {
//                s = cell;
//                break;
//            }
//        }
//        if(!s.empty()) {
//            set<unsigned> otherCellNumbers;
//            unsigned parent = 1e9;
//            for(auto i : s) {
//                if(eleToCellMap.find(i) != eleToCellMap.end()) {
//                    otherCellNumbers.insert(eleToCellMap[i]);
//                    parent = min(parent, (unsigned) eleToCellMap[i]);
//                } else {
//                    eleToCellMap[i] = -1;
//                }
//            }
//            // change the cell number
//            for(auto pair : eleToCellMap) { // iterating through all pairs in the map.
//                if(otherCellNumbers.find(pair.second) != otherCellNumbers.end() || pair.second == -1) {
//                    // If the condition is not satisfied, then the element of the map is not related
//                    // to the cell we are analysing at the moment.
//                    pair.second = parent;
//                }
//            }
//            ncells = ncells - otherCellNumbers.size() + 1;
//        } else {
//            // If the element does not occur in some cell of w1, then we create a cell specifically for that element.
//            ncells++;
//            for(auto ele : cell) {
//                eleToCellMap[ele] = ncells;
//            }
//        }
//    }
//    // Now group all the elements according to their cell numbers and add them to resulting cell.
//    map<int, set<unsigned>> result; //The first coordinate is the cell number. The second coordinate is the set of elements in that cell
//    for(auto pair : eleToCellMap) {
//        result[pair.second].insert(pair.first);
//    }
//    set<set<unsigned >> partition;
//    for(auto pair : result) {
//        partition.insert(pair.second);
//    }
//    return partition;
//}
//
//


// TODO: TEST THIS FUNCTION 
set<set<unsigned >> mergePartitions(set<set<unsigned int>> &partition1, set<set<unsigned int>> &partition2) {
    map<unsigned, int> eleToCellMap;
    int ncells = 0;
    for(auto cell : partition1) {
        ncells++;
        for(auto ele : cell) {
            eleToCellMap[ele] = ncells;
        }
    }
    for(auto cell : partition2) {
        set<unsigned> s; // This set is initialized to empty
        for(auto ele : cell) {
	    auto it = eleToCellMap.find(ele); 
            if(it != eleToCellMap.end()) {
                s.insert(it->second) ;
            }
        }
        if(!s.empty()) {
            int minCellNumber = *(s.begin()); 
	    for (auto ele:cell){
	    	eleToCellMap.insert(make_pair(ele,minCellNumber)); 
	    }
	    // change the cell number
            for(auto pair : eleToCellMap) { // iterating through all pairs in the map and change the cell number to minCellNumber if the current one belong to s.
                if(s.find(pair.second) != s.end()) {
                    pair.second = minCellNumber;
                }
            }
        } else {
            // If the element does not occur in some cell of w1, then we create a cell specifically for that element.
            ncells++;
            for(auto ele : cell) {
                eleToCellMap[ele] = ncells;
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
    witness->size = 0; 
    //witness->partition is already empty by default
    //witness->disconnectingEdges is already empty by default
    this->insertIntoInitialWitnessSet(witness);
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_DynamicCore::intro_v_implementation(unsigned i, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found ){
        // In this case, a disconnecting set has already been found in the past.
	// Therefore, we return a singleton containing only the input witness.
        witnessSet->insert(w);
    }else if(w->processed){
        // In this case, since "processed" is true, we have already read a full connected
	// component in the past. By adding "i", the graph gets disconnected. 
        EdgeConnected_AtMost_WitnessPointer witness = createWitness();
        witness->found = true;
        witness->processed = false;
	witness->size = 0;	
	witnessSet->insert(witness); 	    
    }else{
        // In this case, we can add "i" as a new cell to the partition.
        EdgeConnected_AtMost_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        set<unsigned > cell; // new cell containing only "i"
        cell.insert(i);
        witness->partition.insert(cell);
        witnessSet->insert(witness);
        return;
    }
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found){
        // In this case, a disconnecting set has already been found in the past.
	// Therefore, we return a singleton containing only the input witness.
	witnessSet->insert(w);
    }else{
        // In this case, we have two options. Either we {i,j} to disconnectingEdges or 
	// to use {i,j} to connect vertices belonging to cells of the partition.
    	// First case. Add {i,j} to partition
	EdgeConnected_AtMost_WitnessPointer witness1 = createWitness();
        witness1->set_equal(*w);
        addEdgeToPartition(i,j,witness1->partition);
        witnessSet->insert(witness1);
        // Second case. Add {i,j} to disconnecting set.
        if(w->size < parameter){
            EdgeConnected_AtMost_WitnessPointer  witness2 = createWitness();
            witness2->set_equal(*w);
            witness2->disconnectingEdges.insert(make_pair(min(i,j),max(i,j)));
	    witness2->size++; 
            witnessSet->insert(witness2);
        }
    }
    //*****************************
    //*****************************
}

void EdgeConnected_AtMost_DynamicCore::forget_v_implementation(unsigned i, Bag &b, EdgeConnected_AtMost_WitnessPointer w, EdgeConnected_AtMost_WitnessSetPointer witnessSet){
    //*****************************
    //*****************************
    if(w->found ){
        // In this case, a disconnecting set has already been found in the past.
	// Therefore, we return a singleton containing only the input witness.
	witnessSet->insert(w);
    }else{
        // In this case, we remove all incident edges to "i" from disconnectingEdges. We also remove "i" from the partition
	set<set<int> > newpartition = w->partition; 
        pair<bool,bool> result = removeVertexFromPartition(i,newpartition);
    	EdgeConnected_AtMost_WitnessPointer witness = createWitness();
        if (result.first== true){
		witness->found = true;
        	witness->processed = false;
		witness->size = 0;	
	} else {
		witness->set_equal(*w);
	        witness->processed = result.second;
		witness->partition = newpartition; 
		witness->removeIncidentEdges(i);
	}
	witnessSet->insert(witness);
    }
    //*****************************
    //*****************************
}
void EdgeConnected_AtMost_DynamicCore::join_implementation(Bag &b, EdgeConnected_AtMost_WitnessPointer w1, EdgeConnected_AtMost_WitnessPointer w2, EdgeConnected_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    EdgeConnected_AtMost_WitnessPointer witness = createWitness();
    if(w1->found){
	witnessSet->insert(w1); 
    } else if (w2->found){
	witnessSet->insert(w2); 
    } else {  
	// First check if not more than the allowed number of edges has been 
	// added to the set of deleted edges. 
	set<pair<unsigned ,unsigned >> edgeIntersection;
 	     		set_intersection(w1->disconnectingEdges.begin(),w1->disconnectingEdges.end(),
        	        	w2->disconnectingEdges.begin(),w2->disconnectingEdges.end(), inserter(edgeIntersection,edgeIntersection.begin()));

        unsigned size  = w1->size + w2->size - edgeIntersection.size();
        if(size > parameter){
            // If size is bigger than parameter, then it is not valid
            return;
        }else{
		 if ((w1->processed and w2->processed) or 
	       		(w1->processed and !w2->partition.empty()) or 
	       		(w2->processed and !w1->partition.empty()){
	        	EdgeConnected_AtMost_WitnessPointer witness = createWitness();
			witness->found = true;
		       	witness->processed = false;
			witness->size = 0;	
			witnessSet->insert(witness); 
 		   } else{ 
    		        witness->found = false;
		        set<pair<unsigned ,unsigned >> edgeUnion;
 	     		set_union(w1->disconnectingEdges.begin(),w1->disconnectingEdges.end(),
        	        	w2->disconnectingEdges.begin(),w2->disconnectingEdges.end(), inserter(edgeUnion,edgeUnion.begin()));
            		witness->disconnectingEdges = edgeUnion;
            		witness->size = size;
            		witness->partition = mergePartitions(w1->partition,w2->partition);
			witnessSet->insert(witness); 
		}
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

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore * create() {
    return new EdgeConnected_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new EdgeConnected_AtMost_DynamicCore(param);
}
}

