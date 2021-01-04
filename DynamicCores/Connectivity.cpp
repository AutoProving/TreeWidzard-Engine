// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Connectivity.h"
extern "C" {
DynamicCore * create() {
    return new Connectivity_DynamicCore;
}
}

//////////////////////Implementation/////////////////////////////
////////////////////////////////////////////////////////////////
bool Connectivity_Witness::is_equal_implementation(const Connectivity_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->partition == w->partition;
    //*****************************
    //*****************************
}
bool Connectivity_Witness::is_less_implementation(const Connectivity_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->partition < w->partition;
    //*****************************
    //*****************************
}
Witness & Connectivity_Witness::set_equal_implementation(Connectivity_WitnessPointer w) {
    //*****************************
    //*****************************
    this->partition = w->partition;
    return *this;
    //*****************************
    //*****************************
}

shared_ptr<Witness> Connectivity_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    Connectivity_WitnessPointer w(new Connectivity_Witness);
    set<set<unsigned >> newPartition;
    for(auto cell:this->partition){
        set<unsigned > newCell;
        for(auto v:cell){
            if(relabelingMap.count(v)){
                newCell.insert(relabelingMap[v]);
            }else{
                cout<<"Error: Connectivity_Witness::relabel "<< v << " is not in the map"<<endl;
                exit(20);
            }
        }
        newPartition.insert(newCell);
    }
    w->partition = newPartition;
    return w;
}

void Connectivity_Witness::print() {
    //*****************************
    //*****************************
    auto witness = this->partition;
    cout<<"Witness:"<<endl;
    for(auto part : witness) {
        for(auto it=part.begin(); it != part.end(); it++)
            cout<<*it<<" ";
        cout<<endl;
    }
    //*****************************
    //*****************************
}

Connectivity_Witness::~Connectivity_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

Connectivity_DynamicCore::Connectivity_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","Connectivity");
    addAttribute("ParameterType","NoParameter");
    addAttribute("PrimaryOperator","---");
    createInitialWitnessSet();
}

Connectivity_DynamicCore::Connectivity_DynamicCore(unsigned parameter){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","Connectivity");
    addAttribute("ParameterType","NoParameter");
    addAttribute("PrimaryOperator","---");
    this->parameter = parameter;
    createInitialWitnessSet();
}
void Connectivity_DynamicCore::createInitialWitnessSet_implementation() {
    Connectivity_WitnessPointer witness = createWitness();
    //*****************************
    //*****************************
    //*****************************
    //*****************************
    this->insertIntoInitialWitnessSet(witness);
}
void Connectivity_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, Connectivity_WitnessPointer w,
                                                      Connectivity_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    Connectivity_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    set<unsigned> s = { i };
    w1->partition.insert(s);
    witnessSet->insert(w1);
    //*****************************
    //*****************************
}
void Connectivity_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                      Connectivity_WitnessPointer w,
                                                      Connectivity_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    Connectivity_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    set<unsigned> s1, s2;
    //CHECK: changed "part" with "cell"
    for(auto cell : w1->partition) {
        if(cell.count(i) > 0) {
            s1 = cell;
        }
        if(cell.count(j) > 0) {
            s2 = cell;
        }
    }
    if(s1 != s2) {
        set<unsigned> intermediate;
        set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
                  inserter(intermediate, intermediate.end()));
        w1->partition.erase(s1);
        w1->partition.erase(s2);
        w1->partition.insert(intermediate);
    }
    witnessSet->insert(w1);
    //*****************************
    //*****************************
}
void Connectivity_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, Connectivity_WitnessPointer w,
                                                       Connectivity_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    Connectivity_WitnessPointer w1 = createWitness();
    //CHECK: added the line below to copy *p into w1. Otherwise w1 would be empty
    w1->set_equal(*w);
    if(w1->partition.size() == 1) {
        auto part = w1->partition;
        //CHECK: renamed "it" to "cell"
        auto cell = *(part.begin());
        if(cell.size() == 1) {
            // remove the entire cell if it only contains one element
            w1->partition.erase(cell);
        } else {
            // remove the single element from the cell
            auto s1 = cell;
            w1->partition.erase(s1);
            s1.erase(i);
            w1->partition.insert(s1);
        }
        //CHECK: added the line below, otherwise the witness would not be added to the returned set
        witnessSet->insert(w1);
    } else {
        auto part = w1->partition;
        for(auto cell : part) {
            auto s1 = cell;
            // Do nothing if i is alone in the cell
            // since in this case the witness is not compatible
            // with the forget_v(i) operation
            if(s1.count(i) > 0 && s1.size() > 1) {
                w1->partition.erase(s1);
                s1.erase(i);
                w1->partition.insert(s1);
                witnessSet->insert(w1);
                break;
            }
        }
    }

    //*****************************
    //*****************************
}
void Connectivity_DynamicCore::join_implementation(Bag &b, Connectivity_WitnessPointer w1,
                                                   Connectivity_WitnessPointer w2,
                                                   Connectivity_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    Connectivity_WitnessPointer wPrime1 = createWitness();
    Connectivity_WitnessPointer wPrime2 = createWitness();
    wPrime1->set_equal(*w1);
    wPrime2->set_equal(*w2);
    map<unsigned, int> eleToCellMap;
    int cells = 1;
    // Create a map (eleToCellMap) which contains the mapping of each element of w1 with its cell number.
    for(auto cell : wPrime1->partition) {
        for(auto ele : cell) {
            eleToCellMap[ele] = cells;
        }
        cells++;
    }
    // Iterate through the cells of the second witness and check if an element
    // of a cell is already a part of the map or not.
    // If yes, change the cell number of all the elements in that cell. Also change
    // the cell number of the elements who belong to other cells and are a part of the current cell.
    // If not, add all the elements of the current cell to a new cell number.
    for(auto cell : wPrime2->partition) {
        set<unsigned> s; // This set is initialized to empty
        for(auto ele : cell) {
            if(eleToCellMap.find(ele) != eleToCellMap.end()) {
                s = cell;
                break;
            }
        }
        if(s.empty() == false) { // This means that no element in the cell was present in the domain of the map
            set<unsigned> otherCellNumbers; // Two elements in s may have distinct cell numbers in w1. This variable
            // is used to record all such cell numbers (these will be collapsed later).
            unsigned parent = 1e9; // the final parent after merging the intersecting cells. It is initialized to a very large number
            // at the start.
            // This loop will get cell numbers of all elements in the cell
            for(auto i : s) {
                if(eleToCellMap.find(i) != eleToCellMap.end()) {
                    otherCellNumbers.insert(eleToCellMap[i]);
                    parent = min(parent, (unsigned) eleToCellMap[i]);
                } else {
                    eleToCellMap[i] = -1; // this means that element i does not occur in a cell of w1 yet.
                }
            }
            // change the cell number
            for(auto pair : eleToCellMap) { // iterating through all pairs in the map.
                if(otherCellNumbers.find(pair.second) != otherCellNumbers.end() || pair.second == -1) {
                    // If the condition is not satisfied, then the elemnt of the map is not related
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
    Connectivity_WitnessPointer wPrime3 = createWitness();
    for(auto pair : result) {
        wPrime3->partition.insert(pair.second);
    }
    witnessSet->insert(wPrime3);
    //*****************************
    //*****************************
}
WitnessSetPointer Connectivity_DynamicCore::clean(WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // In most cases, you will not need to change this function.
    //*****************************
    //*****************************
    return witnessSet;
}

bool Connectivity_DynamicCore::is_final_witness_implementation(Connectivity_WitnessPointer w) {
    //*****************************
    //*****************************
    Connectivity_WitnessPointer w1 = createWitness();
    w1->set_equal(*w);
    if(w1->partition.size() == 1) {
        for(auto cell: w1->partition) {
            if(cell.empty() == true) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
    //*****************************
    //*****************************
}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

bool Connectivity_Witness::is_equal(const Witness &rhs) const{
    if (Connectivity_Witness const *e = dynamic_cast<Connectivity_Witness const *>(&rhs)) {
        Connectivity_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in Connectivity_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}


bool Connectivity_Witness::is_less(const Witness &rhs)const {
    if (Connectivity_Witness const *e = dynamic_cast<Connectivity_Witness const *>(&rhs)) {
        Connectivity_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in Connectivity_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &Connectivity_Witness::set_equal(Witness &witness) {
    if (Connectivity_Witness *e = dynamic_cast<Connectivity_Witness *>(&witness)) {
        Connectivity_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in Connectivity_Witness::operator= cast error"<<endl;
        exit(20);
    }
}


void Connectivity_DynamicCore::createInitialWitnessSet() {
    Connectivity_WitnessSetPointer witnessSet(new Connectivity_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}


WitnessSetPointer Connectivity_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (Connectivity_Witness *e = dynamic_cast<Connectivity_Witness *>(&witness)){
        Connectivity_WitnessPointer w = e->shared_from_this();
        Connectivity_WitnessSetPointer witnessSet (new Connectivity_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in Connectivity_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer Connectivity_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (Connectivity_Witness *e = dynamic_cast<Connectivity_Witness *>(&witness)){
        Connectivity_WitnessPointer w = e->shared_from_this();
        Connectivity_WitnessSetPointer witnessSet (new Connectivity_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in Connectivity_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}


WitnessSetPointer Connectivity_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (Connectivity_Witness *e = dynamic_cast<Connectivity_Witness *>(&witness)){
        Connectivity_WitnessPointer w = e->shared_from_this();
        Connectivity_WitnessSetPointer witnessSet (new Connectivity_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in Connectivity_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer Connectivity_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(Connectivity_Witness *e1 = dynamic_cast<Connectivity_Witness *>(&witness1)){
        if(Connectivity_Witness *e2 = dynamic_cast<Connectivity_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            Connectivity_WitnessPointer w1 = e1->shared_from_this();
            Connectivity_WitnessPointer w2 = e2->shared_from_this();
            Connectivity_WitnessSetPointer witnessSet (new Connectivity_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in Connectivity_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in Connectivity_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

bool Connectivity_DynamicCore::is_final_witness(Witness &witness) {
    if (Connectivity_Witness *e = dynamic_cast<Connectivity_Witness *>(&witness)) {
        Connectivity_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in Connectivity_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}


Connectivity_WitnessPointer Connectivity_DynamicCore::createWitness() {
    Connectivity_WitnessPointer w(new Connectivity_Witness);
    return w;
}

void Connectivity_DynamicCore::copyWitness(Connectivity_WitnessPointer w_input,
                                           Connectivity_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}