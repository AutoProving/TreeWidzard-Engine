// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "ChromaticNumber_Min.h"

bool ChromaticNumber_Min_Witness::is_equal_implementation(const ChromaticNumber_Min_WitnessPointerConst w) const{
    /*return this->partialColoring == w->partialColoring and
        this->numberUsedColors == w->numberUsedColors;*/
    return this->partialColoring  == w->partialColoring and this->numberUsedColors == w->numberUsedColors;
}

bool ChromaticNumber_Min_Witness::is_less_implementation(const ChromaticNumber_Min_WitnessPointerConst w) const{
    /*if(this->numberUsedColors < w->numberUsedColors) return true;
    if(this->numberUsedColors == w->numberUsedColors) return this->partialColoring < w->partialColoring;
    retrun false;*/
    if(this->numberUsedColors < w->numberUsedColors) return true;
    if (this->numberUsedColors == w->numberUsedColors) return this->partialColoring < w->partialColoring;
    return false;
}

Witness& ChromaticNumber_Min_Witness::set_equal_implementation(ChromaticNumber_Min_WitnessPointer w){
    // this->numberUsedColors = w->numberUsedColors;
    this->partialColoring = w->partialColoring;
    this->numberUsedColors = w->numberUsedColors;
    return *this;
}

void ChromaticNumber_Min_Witness::print() {
    cout<<"numberUsedColors: " << numberUsedColors<<" ";
    cout<<"partialColoring:";
    for(auto cell:partialColoring){
        cout<<"{ ";
        for(auto item:cell){

            cout<<item;
            if(item!= *cell.crbegin()){
                cout<<",";
            }
        }
        cout<<"}";
    }
    cout<<endl;
}


string ChromaticNumber_Min_Witness::witnessInformation() {
    string info;
    info = "numberUsedColors: " + to_string(numberUsedColors)+" ";
    info = "partialColoring:";
    for(auto cell:partialColoring){
        info = info + "{ ";
        for(auto item:cell){

            info+= to_string(item);
            if(item!= *cell.crbegin()){
                info+=",";
            }
        }
        info+="}";
    }
    info= info + "\n";
    return info;

}

shared_ptr<Witness> ChromaticNumber_Min_Witness::relabel(map<unsigned,unsigned> relabelingMap){
    shared_ptr<ChromaticNumber_Min_Witness> relabeledWitness(new ChromaticNumber_Min_Witness);
    relabeledWitness->numberUsedColors = this->numberUsedColors;
    for(auto cell:this->partialColoring){
        set<unsigned> relabeledCell;
        for(auto item:cell){
            if(relabelingMap.count(item)){
                relabeledCell.insert(relabelingMap[item]);
            }else{
                cout<<"Error: ChromaticNumber_Min_Witness::relabel function error"<<endl;
                exit(20);
            }
        }
        relabeledWitness->partialColoring.insert(relabeledCell);
    }
    return relabeledWitness;
}

ChromaticNumber_Min_DynamicCore::ChromaticNumber_Min_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","ChromaticNumberMin");
    addAttribute("CoreType","Min");
    addAttribute("ParameterType","ParameterLess");
    createInitialWitnessSet();
    //addAttribute("PrimaryOperator","");
}


void ChromaticNumber_Min_DynamicCore::createInitialWitnessSet_implementation(){
    shared_ptr<ChromaticNumber_Min_Witness> witness = createWitness();
    witness->numberUsedColors = 0;
    this->insertIntoInitialWitnessSet(witness);
}

void ChromaticNumber_Min_DynamicCore::intro_v_implementation(unsigned i, Bag &b,
                                                                ChromaticNumber_Min_WitnessPointer w, ChromaticNumber_Min_WitnessSetPointer witnessSet){
    // insert to one of the cells of  w->partialColoring
    for(auto cell:w->partialColoring){
        ChromaticNumber_Min_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        witness->partialColoring.erase(cell);
        cell.insert(i);
        witness->partialColoring.insert(cell);
        witnessSet->insert(witness);
    }

    // i can be added as an separated cell to the partition
    ChromaticNumber_Min_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    witness->numberUsedColors = witness->numberUsedColors +1;
    set<unsigned> iCell;
    iCell.insert(i);
    witness->partialColoring.insert(iCell);
    witnessSet->insert(witness);
}

void ChromaticNumber_Min_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                                ChromaticNumber_Min_WitnessPointer w,
                                                                ChromaticNumber_Min_WitnessSetPointer witnessSet) {
    // If i and j are in different cell then w will be returned, otherwise, it is invalid
    set<unsigned> iCell;
    set<unsigned> jCell;
    for(auto cell:w->partialColoring){
        if(cell.count(i)){
            iCell = cell;
        }
        if(cell.count(j)){
            jCell = cell;
        }
    }
    if(jCell!=iCell){
        witnessSet->insert(w);
    }
}

void ChromaticNumber_Min_DynamicCore::forget_v_implementation(unsigned i, Bag &b,
                                                                 ChromaticNumber_Min_WitnessPointer w,ChromaticNumber_Min_WitnessSetPointer witnessSet){
    for(auto cell:w->partialColoring){
        if(cell.count(i)){
            ChromaticNumber_Min_WitnessPointer witness = createWitness();
            witness->set_equal(*w);
            witness->partialColoring.erase(cell);
            cell.erase(i);
            if(!cell.empty()){
                witness->partialColoring.insert(cell);
            }
            witnessSet->insert(witness);
            break;
        }
    }
}

void ChromaticNumber_Min_DynamicCore::join_implementation(Bag &b, ChromaticNumber_Min_WitnessPointer w1,
                                                             ChromaticNumber_Min_WitnessPointer w2, ChromaticNumber_Min_WitnessSetPointer witnessSet){

    if(w1->partialColoring == w2->partialColoring){
        ChromaticNumber_Min_WitnessPointer witness = createWitness();
        witness->partialColoring = w1->partialColoring;
        witness->numberUsedColors = w1->numberUsedColors+w2->numberUsedColors-w1->partialColoring.size();
        witnessSet->insert(witness);
    }
}

bool ChromaticNumber_Min_DynamicCore::is_final_witness_implementation(ChromaticNumber_Min_WitnessPointer w){
    return true;
}

int ChromaticNumber_Min_DynamicCore::weight_implementation(ChromaticNumber_Min_WitnessPointer w){
    return w->numberUsedColors;
}

ChromaticNumber_Min_WitnessSetPointer ChromaticNumber_Min_DynamicCore::clean_implementation(
        ChromaticNumber_Min_WitnessSetPointer witnessSet){
    return witnessSet;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ChromaticNumber_Min_Witness::is_equal(const Witness &rhs) const{
    if(ChromaticNumber_Min_Witness const *e = dynamic_cast<ChromaticNumber_Min_Witness const *>(&rhs)) {
        shared_ptr<ChromaticNumber_Min_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool ChromaticNumber_Min_Witness::is_less(const Witness &rhs)const {
    if (ChromaticNumber_Min_Witness const *e = dynamic_cast<ChromaticNumber_Min_Witness const *>(&rhs)) {
        shared_ptr<ChromaticNumber_Min_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &ChromaticNumber_Min_Witness::set_equal(Witness &witness) {
    if (ChromaticNumber_Min_Witness  *e = dynamic_cast<ChromaticNumber_Min_Witness *>(&witness)) {
        shared_ptr<ChromaticNumber_Min_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::is_less cast error"<<endl;
        exit(20);
    }
}


void ChromaticNumber_Min_DynamicCore::createInitialWitnessSet() {
    ChromaticNumber_Min_WitnessSetPointer witnessSet(new ChromaticNumber_Min_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

void ChromaticNumber_Min_DynamicCore::copyWitness(ChromaticNumber_Min_WitnessPointer w_input,
                                                     ChromaticNumber_Min_WitnessPointer w_output){

    w_output->set_equal_implementation(w_input);
}

ChromaticNumber_Min_WitnessPointer ChromaticNumber_Min_DynamicCore::createWitness(){
    ChromaticNumber_Min_WitnessPointer w (new ChromaticNumber_Min_Witness);
    return w;
}

WitnessSetPointer ChromaticNumber_Min_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (ChromaticNumber_Min_Witness *e = dynamic_cast<ChromaticNumber_Min_Witness *>(&witness)) {
        // shared_ptr<ChromaticNumber_Min_Witness> p = make_shared<ChromaticNumber_Min_Witness>(*e);
        ChromaticNumber_Min_WitnessPointer w = e->shared_from_this();
        ChromaticNumber_Min_WitnessSetPointer witnessSet(new ChromaticNumber_Min_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer ChromaticNumber_Min_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (ChromaticNumber_Min_Witness *e = dynamic_cast<ChromaticNumber_Min_Witness *>(&witness)) {
        // shared_ptr<ChromaticNumber_Min_Witness> p = make_shared<ChromaticNumber_Min_Witness>(*e);
        ChromaticNumber_Min_WitnessPointer w = e->shared_from_this();
        ChromaticNumber_Min_WitnessSetPointer witnessSet(new ChromaticNumber_Min_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer ChromaticNumber_Min_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (ChromaticNumber_Min_Witness *e = dynamic_cast<ChromaticNumber_Min_Witness *>(&witness)) {
        // shared_ptr<ChromaticNumber_Min_Witness> p = make_shared<ChromaticNumber_Min_Witness>(*e);
        ChromaticNumber_Min_WitnessPointer w = e->shared_from_this();
        ChromaticNumber_Min_WitnessSetPointer witnessSet(new ChromaticNumber_Min_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer ChromaticNumber_Min_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (ChromaticNumber_Min_Witness  *e1 = dynamic_cast<ChromaticNumber_Min_Witness  *>(&witness1)){
        if(ChromaticNumber_Min_Witness  *e2 = dynamic_cast<ChromaticNumber_Min_Witness  *>(&witness2)) {
            shared_ptr<ChromaticNumber_Min_Witness> w1 = e1->shared_from_this();
            shared_ptr<ChromaticNumber_Min_Witness> w2 = e2->shared_from_this();
            shared_ptr<ChromaticNumber_Min_WitnessSet> witnessSet(new ChromaticNumber_Min_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in ChromaticNumber_Min_Witness::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::join cast error"<<endl;
        exit(20);
    }
}

bool ChromaticNumber_Min_DynamicCore::is_final_witness(Witness &witness) {
    if (ChromaticNumber_Min_Witness *e = dynamic_cast<ChromaticNumber_Min_Witness *>(&witness)) {
        ChromaticNumber_Min_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer ChromaticNumber_Min_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (ChromaticNumber_Min_WitnessSetPointer e = dynamic_pointer_cast<ChromaticNumber_Min_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

int ChromaticNumber_Min_DynamicCore::weight(Witness &witness){
    if (ChromaticNumber_Min_Witness *e = dynamic_cast<ChromaticNumber_Min_Witness *>(&witness)) {
        ChromaticNumber_Min_WitnessPointer w = e->shared_from_this();
        return weight_implementation(w);
    }else{
        cerr<<"ERROR: in ChromaticNumber_Min_Witness::weight cast error"<<endl;
        exit(20);
    }

}

shared_ptr<WitnessSet> ChromaticNumber_Min_WitnessSet::createEmptyWitnessSet() {
    ChromaticNumber_Min_WitnessSetPointer witnessSet(new ChromaticNumber_Min_WitnessSet);
    return witnessSet;
}
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new ChromaticNumber_Min_DynamicCore;
}
}
