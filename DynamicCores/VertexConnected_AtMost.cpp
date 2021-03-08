// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "VertexConnected_AtMost.h"

bool VertexConnected_AtMost_Witness::is_equal_implementation(const VertexConnected_AtMost_WitnessPointerConst rhs) const {
   
        // return this->partition == w->partition && this->disconnectingSet == w->disconnectingSet;
        found == rhs->found &&
        sizeOfDisconnectedSet == rhs->sizeOfDisconnectedSet &&
        disconnectingSet == rhs->disconnectingSet &&
        partitionProcessed == rhs->partitionProcessed &&
        partition == rhs->partition;}

bool VertexConnected_AtMost_Witness::is_less_implementation(const VertexConnected_AtMost_WitnessPointerConst rhs) const {

    if (found > rhs->found)
        return true;
    if (rhs->found > found)
        return false;
    if (sizeOfDisconnectedSet < rhs->sizeOfDisconnectedSet)
        return true;
    if (rhs->sizeOfDisconnectedSet < sizeOfDisconnectedSet)
        return false;
    if (disconnectingSet < rhs->disconnectingSet)
        return true;
    if (rhs->disconnectingSet < disconnectingSet)
        return false;
    if (partitionProcessed < rhs->partitionProcessed)
        return true;
    if (rhs->partitionProcessed < partitionProcessed)
        return false;
    return partition < rhs->partition;
}

Witness & VertexConnected_AtMost_Witness::set_equal_implementation(VertexConnected_AtMost_WitnessPointer w) {
    this->found = w->found;
    this->partitionProcessed = w->partitionProcessed;
    this->partition = w->partition;
    this->disconnectingSet = w->disconnectingSet;
    this->sizeOfDisconnectedSet = w->sizeOfDisconnectedSet;
    return *this;
}


shared_ptr<Witness> VertexConnected_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    VertexConnected_AtMost_WitnessPointer witness (new VertexConnected_AtMost_Witness);
    witness->found = this->found;
    witness->partitionProcessed = this->partitionProcessed;
    witness->sizeOfDisconnectedSet = this->sizeOfDisconnectedSet;
    for(auto cell : this->partition){
        set<unsigned > relabeledCell;
        for(auto item:cell){
            if(relabelingMap.count(item)){
                relabeledCell.insert(relabelingMap[item]);
            }else{
                cout<<"Error 1: "<< item << " is not in relabelingMap"<<endl;
                exit(20);
            }
        }
        witness->partition.insert(relabeledCell);
    }
    for(auto v:this->disconnectingSet){
        if(relabelingMap.count(v)){
            witness->disconnectingSet.insert(relabelingMap[v]);
        }else{
            cout<<"Error 2: "<< v << " is not in relabelingMap"<<endl;
            exit(20);
        }
    }
    return witness;
}

void VertexConnected_AtMost_Witness::print() {
    //*****************************
    //*****************************
    cout<<"found: "<<found<<" sizeOfDisconnectedSet: "<<sizeOfDisconnectedSet <<" partitionProcessed: "<< partitionProcessed <<endl;
    cout<<"partition: "<<endl;
    for(auto cell : this->partition) {
        cout<<"{";
        for(auto ele : cell) {
            cout<<ele;
            if(ele!=*cell.crbegin()){
                cout<<",";
            }
        }
        cout<<"}";
    }
    cout<<"\n{";
    for(auto ele : this->disconnectingSet) {
        cout<<ele<<" ";
        if(ele!=*disconnectingSet.crbegin()){
            cout<<",";
        }
    }
    cout<<"}\n";
    //*****************************
    //*****************************
}

string VertexConnected_AtMost_Witness::witnessInformation(){

    //*****************************
    //*****************************
    string info;
    info = "found: "+to_string(found)+" sizeOfDisconnectedSet: "+to_string(sizeOfDisconnectedSet) +
            " partitionProcessed: " + to_string(partitionProcessed)+"\n";
    info+="partition: \n";
    for(auto cell : this->partition) {
        info+="{";
        for(auto ele : cell) {
            info+=to_string(ele);
            if(ele!=*cell.crbegin()){
                info+=",";
            }
        }
        info+="}";
    }
    info+="\n{";
    for(auto ele : this->disconnectingSet) {
        info+=to_string(ele)+" ";
        if(ele!=*disconnectingSet.crbegin()){
            info+=",";
        }
    }
    info+="}\n";
    //*****************************
    //*****************************
}

VertexConnected_AtMost_Witness::~VertexConnected_AtMost_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

VertexConnected_AtMost_DynamicCore::VertexConnected_AtMost_DynamicCore(){
    // Initializing attributes
    addAttribute("CoreName","VertexConnected");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

VertexConnected_AtMost_DynamicCore::VertexConnected_AtMost_DynamicCore(unsigned parameter){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    this->parameter = parameter;
    createInitialWitnessSet();
     // Initializing attributes
    addAttribute("CoreName","VertexConnected");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

void VertexConnected_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    VertexConnected_AtMost_WitnessPointer w = createWitness();
    w->sizeOfDisconnectedSet = 0;
    w->found = false;
    w->partitionProcessed = false;
    this->insertIntoInitialWitnessSet(w);
}

void VertexConnected_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, VertexConnected_AtMost_WitnessPointer w,
                                                           VertexConnected_AtMost_WitnessSetPointer witnessSet) {
        if(w->found){
            witnessSet->insert(w);
        }else{
            // Add to partition
            if(!w->partitionProcessed){
                shared_ptr<VertexConnected_AtMost_Witness> w1(new VertexConnected_AtMost_Witness);
                w1->set_equal(*w);
                w1->partition.insert({ i });
                witnessSet->insert(w1);
            }else{
                // The witness will be true witness
                shared_ptr<VertexConnected_AtMost_Witness> w1(new VertexConnected_AtMost_Witness);
                w1->found = true;
                witnessSet->insert(w1);
            }
            // Add to disconnectedSet
            if(w->sizeOfDisconnectedSet < this->parameter){
                shared_ptr<VertexConnected_AtMost_Witness> w2(new VertexConnected_AtMost_Witness);
                w2->set_equal(*w);
                w2->disconnectingSet.insert(i);
                w2->sizeOfDisconnectedSet = w->sizeOfDisconnectedSet+1;
                witnessSet->insert(w2);
            }
        }
}

void VertexConnected_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                           VertexConnected_AtMost_WitnessPointer w,
                                                           VertexConnected_AtMost_WitnessSetPointer witnessSet) {
    if(w->found){
        witnessSet->insert(w);
    }else if(w->disconnectingSet.count(i) > 0 || w->disconnectingSet.count(j) > 0) {
            witnessSet->insert(w);
    }else{
        set<unsigned> s1, s2;
        for(auto cell : w->partition) {
            if(cell.count(i) > 0) {
                s1 = cell;
            }
            if(cell.count(j) > 0) {
                s2 = cell;
            }
        }
        if(s1 != s2) {
            VertexConnected_AtMost_WitnessPointer w1 = createWitness();
            w1->set_equal(*w);
            set<unsigned> intermediate;
            set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),inserter(intermediate, intermediate.end()));
            w1->partition.erase(s1);
            w1->partition.erase(s2);
            w1->partition.insert(intermediate);
            witnessSet->insert(w1);
        }else{
            witnessSet->insert(w);
        }
    }
}

void VertexConnected_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, VertexConnected_AtMost_WitnessPointer w,
                                                            VertexConnected_AtMost_WitnessSetPointer witnessSet) {

    if(w->found){
        witnessSet->insert(w);
    }else {
        shared_ptr<VertexConnected_AtMost_Witness> w1(new VertexConnected_AtMost_Witness);
        w1->set_equal(*w);
        if (w1->disconnectingSet.count(i) > 0) {
            // If i is a part of the disconnectingSet, remove it and return the witnessSet.
            w1->disconnectingSet.erase(i);
            witnessSet->insert(w1);
        } else {
            // If i is not a part of disconnectingSet, it must be a part of the partition.
            // Remove it from the partition.
            for (auto cell:w->partition) {
                if (cell.count(i)) {
                    if (cell.size() == 1) {
                        if (w->partition.size() == 1) {
                            // partition will be empty, therefore, we cannot add anymore a vertex to the partition
                            w1->partition.clear();
                            w1->partitionProcessed = true;
                            witnessSet->insert(w1);
                            break;
                        } else {
                            // partition will be disconnected
                            w1->found = true;
                            w1->partition.clear();
                            w1->disconnectingSet.clear();
                            witnessSet->insert(w1);
                            break;
                        }
                    } else {
                        cell.erase(i);
                        w1->partition.erase(cell);
                        witnessSet->insert(w1);
                        break;
                    }
                }
            }
        }
    }
}

void VertexConnected_AtMost_DynamicCore::join_implementation(Bag &b, VertexConnected_AtMost_WitnessPointer w1,
                                                        VertexConnected_AtMost_WitnessPointer w2,
                                                        VertexConnected_AtMost_WitnessSetPointer witnessSet) {
    if(w1->found){
        witnessSet->insert(w1);
        return;
    }else if (w2->found){
        witnessSet->insert(w2);
        return;
    }else {

        shared_ptr<VertexConnected_AtMost_Witness> wPrime1 = createWitness();
        shared_ptr<VertexConnected_AtMost_Witness> wPrime2 = createWitness();
        wPrime1->set_equal(*w1);
        wPrime2->set_equal(*w2);
        if (has_common_vertices(wPrime1->disconnectingSet, wPrime2->partition) ||
            has_common_vertices(wPrime2->disconnectingSet, wPrime1->partition)) {
            // return empty witness set.
            return;
        }
        // -------- Join operation starts here --------
        map<unsigned, int> eleToCellMap;
        int cells = 1;
        // Create a map (eleToCellMap) which contains the mapping of each element of wPrime1 with its cell number.
        for (auto cell : wPrime1->partition) {
            for (auto ele : cell) {
                eleToCellMap[ele] = cells;
            }
            cells++;
        }
        // Iterate through the cells of the second witness and check if an element
        // of a cell is already a part of the map or not.
        // If yes, change the cell number of all the elements in that cell. Also change
        // the cell number of the elements who belong to other cells and are a part of the current cell.
        // If not, add all the elements of the current cell to a new cell number.
        for (auto cell : wPrime2->partition) {
            set<unsigned> s; // This set is initialized to empty
            for (auto ele : cell) {
                if (eleToCellMap.find(ele) != eleToCellMap.end()) {
                    s = cell;
                    break;
                }
            }
            if (s.empty() == false) { // This means that no element in the cell was present in the domain of the map
                set<unsigned> otherCellNumbers; // Two elements in s may have distinct cell numbers in wPrime1. This variable
                // is used to record all such cell numbers (these will be collapsed later).
                unsigned parent = 1e9; // the final parent after merging the intersecting cells. It is initialized to a very large number
                // at the start.
                // This loop will get cell numbers of all elements in the cell
                for (auto i : s) {
                    if (eleToCellMap.find(i) != eleToCellMap.end()) {
                        otherCellNumbers.insert(eleToCellMap[i]);
                        parent = min(parent, (unsigned) eleToCellMap[i]);
                    } else {
                        eleToCellMap[i] = -1; // this means that element i does not occur in a cell of wPrime1 yet.
                    }
                }
                // change the cell number
                for (auto pair : eleToCellMap) { // iterating through all pairs in the map.
                    if (otherCellNumbers.find(pair.second) != otherCellNumbers.end() || pair.second == -1) {
                        // If the condition is not satisfied, then the elemnt of the map is not related
                        // to the cell we are analysing at the moment.
                        pair.second = parent;
                    }
                }
                cells = cells - otherCellNumbers.size() + 1;
            } else {
                // If the element does not occur in some cell of wPrime1, then we create a cell specifically for that element.
                cells++;
                unsigned parent = cells;
                for (auto ele : cell) {
                    eleToCellMap[ele] = parent;
                }
            }
        }
        // Now group all the elements according to their cell numbers and add them to resulting cell.
        map<int, set<unsigned>> result; //The first coordinate is the cell number. The second coordinate is the set of elements in that cell
        for (auto pair : eleToCellMap) {
            result[pair.second].insert(pair.first);
        }
        shared_ptr<VertexConnected_AtMost_Witness> wPrime3 = createWitness();
        for (auto pair : result) {
            wPrime3->partition.insert(pair.second);
        }
        witnessSet->insert(wPrime3);
    }
}

shared_ptr<WitnessSet> VertexConnected_AtMost_DynamicCore::clean_implementation(VertexConnected_AtMost_WitnessSetPointer witnessSet) {
    return witnessSet;
}

bool VertexConnected_AtMost_DynamicCore::is_final_witness_implementation(VertexConnected_AtMost_WitnessPointer w) {
    if(w->partition.size() > 1 or w->found) {
        return true;
    }
    return false;
}

bool VertexConnected_AtMost_DynamicCore::has_common_vertices(set<unsigned> disconnectingSet, set<set<unsigned>> partition) {
    for(auto ele : disconnectingSet) {
        for(auto cell : partition) {
            if(cell.count(ele) > 0) {
                return true;
            }
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

bool VertexConnected_AtMost_Witness::is_equal(const Witness &rhs) const{
    if(VertexConnected_AtMost_Witness const *e = dynamic_cast<VertexConnected_AtMost_Witness const *>(&rhs)) {
        shared_ptr<VertexConnected_AtMost_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool VertexConnected_AtMost_Witness::is_less(const Witness &rhs)const {
    if (VertexConnected_AtMost_Witness const *e = dynamic_cast<VertexConnected_AtMost_Witness const *>(&rhs)) {
        shared_ptr<VertexConnected_AtMost_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &VertexConnected_AtMost_Witness::set_equal(Witness &witness) {
   if (VertexConnected_AtMost_Witness  *e = dynamic_cast<VertexConnected_AtMost_Witness *>(&witness)) {
        shared_ptr<VertexConnected_AtMost_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::set_equal cast error"<<endl;
        exit(20);
    }
}

VertexConnected_AtMost_Witness::VertexConnected_AtMost_Witness() {

}

shared_ptr<WitnessSet> VertexConnected_AtMost_WitnessSet::createEmptyWitnessSet() {
    VertexConnected_AtMost_WitnessSetPointer witnessSet(new VertexConnected_AtMost_WitnessSet);
    return witnessSet;
}

VertexConnected_AtMost_WitnessPointer VertexConnected_AtMost_DynamicCore::createWitness() {
    VertexConnected_AtMost_WitnessPointer w(new VertexConnected_AtMost_Witness);
    return w;
}

void VertexConnected_AtMost_DynamicCore::copyWitness(VertexConnected_AtMost_WitnessPointer w_input,
                                                VertexConnected_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void VertexConnected_AtMost_DynamicCore::createInitialWitnessSet() {
    VertexConnected_AtMost_WitnessSetPointer  witnessSet (new VertexConnected_AtMost_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

shared_ptr<WitnessSet> VertexConnected_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (VertexConnected_AtMost_Witness *e = dynamic_cast<VertexConnected_AtMost_Witness *>(&witness)) {
       // shared_ptr<VertexConnected_AtMost_Witness> p = make_shared<VertexConnected_AtMost_Witness>(*e);
        VertexConnected_AtMost_WitnessPointer w = e->shared_from_this();
        VertexConnected_AtMost_WitnessSetPointer witnessSet(new VertexConnected_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> VertexConnected_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
       
   if (VertexConnected_AtMost_Witness *e = dynamic_cast<VertexConnected_AtMost_Witness *>(&witness)) {
        VertexConnected_AtMost_WitnessPointer w = e->shared_from_this();
        VertexConnected_AtMost_WitnessSetPointer witnessSet(new VertexConnected_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> VertexConnected_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {

    if (VertexConnected_AtMost_Witness *e = dynamic_cast<VertexConnected_AtMost_Witness *>(&witness)) {
        VertexConnected_AtMost_WitnessPointer w = e->shared_from_this();
        VertexConnected_AtMost_WitnessSetPointer witnessSet(new VertexConnected_AtMost_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> VertexConnected_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (VertexConnected_AtMost_Witness  *e1 = dynamic_cast<VertexConnected_AtMost_Witness  *>(&witness1)){
        if(VertexConnected_AtMost_Witness  *e2 = dynamic_cast<VertexConnected_AtMost_Witness  *>(&witness2)) {
            shared_ptr<VertexConnected_AtMost_Witness> w1 = e1->shared_from_this();
            shared_ptr<VertexConnected_AtMost_Witness> w2 = e2->shared_from_this();
            shared_ptr<VertexConnected_AtMost_WitnessSet> witnessSet(new VertexConnected_AtMost_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in VertexConnected_AtMost_Witness::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::join cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> VertexConnected_AtMost_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    if (VertexConnected_AtMost_WitnessSetPointer e = dynamic_pointer_cast<VertexConnected_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool VertexConnected_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (VertexConnected_AtMost_Witness *e = dynamic_cast<VertexConnected_AtMost_Witness *>(&witness)) {
        VertexConnected_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in VertexConnected_AtMost_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore * create() {
    return new VertexConnected_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new VertexConnected_AtMost_DynamicCore(param);
}
}
