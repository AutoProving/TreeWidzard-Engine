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


#include "PathEdgeCover.h"

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

void PathEdgeCover_Witness::print() {
    for (int l=0 ; l < partialPathCover.size(); l++) {
        cout<<"[";
        for (int i = 0; i < partialPathCover[l].size() ; ++i) {
            cout<<partialPathCover[l][i];
            if(i!= partialPathCover[l].size()-1){
                cout<<", ";
            }
        }
        cout<<"] ("; 
        for(int i = 0; i < edgesFound[l].size(); i++){
            cout<<edgesFound[l][i];
            if(i!= edgesFound[l].size()-1){

                cout<<",";
            }
        
        }
        cout<<")";
    }
    cout<<endl;
}

string PathEdgeCover_Witness::witnessInformation() {
    string info="";
    for (int l=0 ; l < partialPathCover.size(); l++) {
        info+="[";
        for (int i = 0; i < partialPathCover[l].size() ; ++i) {
            info+=to_string(partialPathCover[l][i]);
            if(i!= partialPathCover[l].size()-1){
                info+=", ";
            }
        }
        info+="] ("; 
        for(int i = 0; i < edgesFound[l].size(); i++){
            info+=to_string(edgesFound[l][i]);
            if(i!= edgesFound[l].size()-1){

                info+=",";
            }
        
        }
        info+=")";
    }
    return info+"\n";
}

bool PathEdgeCover_Witness::is_equal_implementation(const PathEdgeCover_WitnessPointerConst w) const {
    return this->partialPathCover == w->partialPathCover and this->edgesFound == w->edgesFound;
}
bool PathEdgeCover_Witness::is_less_implementation(const PathEdgeCover_WitnessPointerConst w) const {
    if( this->partialPathCover < w->partialPathCover) return true ;
    if( this->partialPathCover ==  w->partialPathCover) return this->edgesFound < w->edgesFound;
    return false;

}

Witness & PathEdgeCover_Witness::set_equal_implementation(PathEdgeCover_WitnessPointer w) {
    this->partialPathCover = w->partialPathCover;
    this->edgesFound = w->edgesFound;
    return *this;
}

shared_ptr<Witness> PathEdgeCover_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    PathEdgeCover_WitnessPointer w (new PathEdgeCover_Witness);
    w->width = this->width;
    w->edgesFound = this->edgesFound;
    vector<vector<unsigned >> m;
    for (auto item:partialPathCover) {
        vector<unsigned > nPart;
        for (auto cell:item){
            if(cell == 0 or cell == width+2){
                nPart.push_back(cell);
            }else if(relabelingMap.count(cell)){
                nPart.push_back(relabelingMap[cell]);
            }else{
                cout<<"Error: PathEdgeCover_Witness::relabel, "<< cell << " is not in the map"<<endl;
                cout<<"Width  "<< width<<endl;
                exit(20);
            }
        }
        m.push_back(nPart);
    }
    w->partialPathCover = m;
    return w;
}

PathEdgeCover_Witness::~PathEdgeCover_Witness() {

}

PathEdgeCover_DynamicCore::PathEdgeCover_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","PathEdgeCover");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","Equal");
}

PathEdgeCover_DynamicCore::PathEdgeCover_DynamicCore(unsigned parameter){
    if(parameter < 2){

        cout<<"PathEdgeCover parameter should be at least 2"<<endl;
        exit(20);
    }
    this->parameter = parameter;
    createInitialWitnessSet(); 
    // Initializing attributes
    addAttribute("CoreName","PathEdgeCover");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","Equal");
}

void PathEdgeCover_DynamicCore::createInitialWitnessSet_implementation() {
    PathEdgeCover_WitnessPointer w = createWitness();
    w->width = this->getWidth();
    this->insertIntoInitialWitnessSet(w);
}

void PathEdgeCover_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, PathEdgeCover_WitnessPointer w,
                                                           PathEdgeCover_WitnessSetPointer witnessSet) {
    w->width = this->getWidth();
    witnessSet->insert(w);
}

void PathEdgeCover_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                           PathEdgeCover_WitnessPointer w,
                                                           PathEdgeCover_WitnessSetPointer witnessSet) {
    // Go through each vector(edge partition) to check that it is possible to add {i,j}
    // in the vector or not. If it is possible, then we create a new witness which is w but {i,j} 
    // is added to the vector.
    w->width = this->getWidth();
    vector<vector<unsigned>> ppc = w->partialPathCover;
    vector<vector<bool> > ef = w->edgesFound;
    for(int l=0; l < ppc.size(); l++){
      for(int index=0 ; index < ppc[l].size()-1 ; index++){
        // if ppc[l][index] = 0 and pcc[index+1]=i, ppc[l][index+1]=j or ppc[l][index+1]=0,
        // then it is possible add {i,j} to ppc[l].
          if(ppc[l][index] == 0){
                if(ppc[l][index+1]==i){
                    bool checkOccurrence = true;
                    for (int k = 0; k < ppc[l].size() ; ++k) {
                        if(ppc[l][k]==j){
                            checkOccurrence = false;
                            break;
                        }
                    }
                    if(!checkOccurrence) break;
                    PathEdgeCover_WitnessPointer wPrime = createWitness();
                    wPrime->width = w->width;
                    wPrime->partialPathCover = ppc;
                    wPrime->edgesFound = ef;
                    wPrime->partialPathCover[l][index]= j;
                    wPrime->edgesFound[l][index]=true;
                    witnessSet->insert(wPrime);
                }else if (ppc[l][index+1]==j){
                    bool checkOccurrence = true;
                    for (int k = 0; k < ppc[l].size() ; ++k) {
                        if(ppc[l][k]==i){
                            checkOccurrence = false;
                            break;
                        }
                    }
                    if(!checkOccurrence) break;
                    PathEdgeCover_WitnessPointer wPrime = createWitness();
                    wPrime->width = w->width;
                    wPrime->partialPathCover = ppc;
                    wPrime->edgesFound = ef;
                    wPrime->partialPathCover[l][index] = i;
                    wPrime->edgesFound[l][index]=true;
                    witnessSet->insert(wPrime);
                }else if (ppc[l][index+1]==0){
                    bool checkOccurrence = true;
                    for (int k = 0; k < ppc[l].size() ; ++k) {
                        if(ppc[l][k]==i or ppc[l][k]==j){
                            checkOccurrence = false;
                            break;
                        }
                    }
                    if(!checkOccurrence) break;
                    PathEdgeCover_WitnessPointer wPrime1 = createWitness();
                    wPrime1->width = w->width;
                    wPrime1->partialPathCover = ppc;
                    wPrime1->edgesFound = ef;
                    wPrime1->partialPathCover[l][index] = i;
                    wPrime1->partialPathCover[l][index+1] = j;
                    wPrime1->edgesFound[l][index] = true;
                    witnessSet->insert(wPrime1);
                    PathEdgeCover_WitnessPointer wPrime2 = createWitness();
                    wPrime2->width = w->width;
                    wPrime2->partialPathCover = ppc;
                    wPrime2->edgesFound = ef;
                    wPrime2->partialPathCover[l][index] = j;
                    wPrime2->partialPathCover[l][index+1] = i;
                    wPrime2->edgesFound[l][index] = true;
                    witnessSet->insert(wPrime2);
                }
          }else if((ppc[l][index]==i or ppc[l][index]==j) and (ppc[l][index+1]==0)){
            if(ppc[l][index]==i){
                bool checkOccurrence = true;
                for (int k = 0; k < ppc[l].size() ; ++k) {
                    if(ppc[l][k]==j){
                        checkOccurrence = false;
                        break;
                    }
                }
                if(!checkOccurrence) break;
                PathEdgeCover_WitnessPointer wPrime = createWitness();
                wPrime->width = w->width;
                wPrime->partialPathCover = ppc;
                wPrime->edgesFound = ef;
                wPrime->partialPathCover[l][index+1]=j;
                wPrime->edgesFound[l][index]=true;
                witnessSet->insert(wPrime);
            }else if(ppc[l][index]==j){
                bool checkOccurrence = true;
                for (int k = 0; k < ppc[l].size() ; ++k) {
                    if(ppc[l][k]==i){
                        checkOccurrence = false;
                        break;
                    }
                }
                if(!checkOccurrence) break;
                PathEdgeCover_WitnessPointer wPrime = createWitness();
                wPrime->width = w->width;
                wPrime->partialPathCover = ppc;
                wPrime->edgesFound = ef;
                wPrime->partialPathCover[l][index+1]= i;
                wPrime->edgesFound[l][index] = true;
                witnessSet->insert(wPrime);
            }
          }else if((ppc[l][index]==i and ppc[l][index+1]==j) or (ppc[l][index]==j
                    and ppc[l][index+1]==i) and ef[l][index]==false){
                PathEdgeCover_WitnessPointer wPrime = createWitness();
                wPrime->width = w->width;
                wPrime->partialPathCover = ppc;
                wPrime->edgesFound = ef;
                wPrime->edgesFound[l][index] = true;
                witnessSet->insert(wPrime);
          }
      }
    }
    // create new parts
    for(int index = 0 ; index < this->parameter-1 ; index++){
        vector<unsigned> nPart1;
        vector<unsigned> nPart2;
        nPart1.resize(this->parameter,0);
        nPart2.resize(this->parameter,0);
        nPart1[index]=i;
        nPart1[index+1]=j;
        nPart2[index]=j;
        nPart2[index+1]=i;
        vector<bool> edgesBool;
        edgesBool.resize(this->parameter-1,false);
        edgesBool[index]= true;
        PathEdgeCover_WitnessPointer wPrime1 = createWitness();
        PathEdgeCover_WitnessPointer wPrime2 = createWitness();
        wPrime1->width = w->width;
        wPrime2->width = w->width;
        wPrime1->partialPathCover = w->partialPathCover;
        wPrime2->partialPathCover = w->partialPathCover;
        wPrime1->partialPathCover.push_back(nPart1);
        wPrime2->partialPathCover.push_back(nPart2);
        wPrime1->edgesFound = w->edgesFound;
        wPrime2->edgesFound = w->edgesFound;
        wPrime1->edgesFound.push_back(edgesBool);
        wPrime2->edgesFound.push_back(edgesBool);
        witnessSet->insert(wPrime1);
        witnessSet->insert(wPrime2);
    }
}

void PathEdgeCover_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, PathEdgeCover_WitnessPointer w,
                                                            PathEdgeCover_WitnessSetPointer witnessSet) {

    w->width = this->getWidth();
    PathEdgeCover_WitnessPointer wPrime = createWitness();
    wPrime->width = w->width;
    wPrime->partialPathCover = w->partialPathCover;
    vector<vector<unsigned>> ppc = wPrime->partialPathCover;
    bool flag = true;
    for(int l = 0; l <  ppc.size() and flag; l++){
        for(int index = 0 ; index < ppc[l].size(); index++){
            if(ppc[l][index]==i){
                if(index==0){
                    if(!w->edgesFound[l][index]){
                        flag = false;
                        break;
                    }else{
                        ppc[l][index] = this->getWidth()+2;
                    }
                }else if(index == ppc[l].size()-1 ){
                    if(!w->edgesFound[l][index-1]){
                        flag = false;
                        break;
                    }else{
                        ppc[l][index] = this->getWidth()+2;
                    }
                }else{
                    if(!w->edgesFound[l][index-1] or !w->edgesFound[l][index]){
                        flag = false;
                        break;
                    }else{
                        ppc[l][index] = this->getWidth()+2;
                    }
                }
            }
        } 
    }
    if(flag){
        wPrime->partialPathCover = ppc;
        wPrime->edgesFound = w->edgesFound;
        witnessSet->insert(wPrime);
    }
}


void PathEdgeCover_DynamicCore::join_implementation(Bag &b, PathEdgeCover_WitnessPointer w1,
                                                        PathEdgeCover_WitnessPointer w2,
                                                        PathEdgeCover_WitnessSetPointer witnessSet) {

    auto result = combinePartialPathCover(make_pair(w1->partialPathCover,w1->edgesFound), make_pair(w2->partialPathCover,w2->edgesFound),
                                          this->getWidth());
    for (int i = 0; i < result.first.size(); ++i) {
        PathEdgeCover_WitnessPointer wPrime = createWitness();
        wPrime->width = this->getWidth();
        wPrime->partialPathCover = result.first[i];
        wPrime->edgesFound = result.second[i];
        witnessSet->insert(wPrime);
    }
}

bool PathEdgeCover_DynamicCore::is_final_witness_implementation(PathEdgeCover_WitnessPointer w) {
    vector<vector<bool>> ef = w->edgesFound;
    if(ef.size()==0){
        return false;
    }else{
        for(int l = 0; l <  ef.size(); l++){
            for(int index = 0 ; index < ef[l].size(); index++){
                if(!ef[l][index]){
                    return false;
                }
            }
        }
        return true;
    }

}

shared_ptr<WitnessSet> PathEdgeCover_DynamicCore::clean_implementation(PathEdgeCover_WitnessSetPointer witnessSet) {
    return witnessSet;
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool PathEdgeCover_Witness::is_equal(const Witness &rhs)const {
    if(PathEdgeCover_Witness const *e = dynamic_cast<PathEdgeCover_Witness const *>(&rhs)) {
        shared_ptr<PathEdgeCover_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool PathEdgeCover_Witness::is_less(const  Witness &rhs)const {
    if (PathEdgeCover_Witness const *e = dynamic_cast<PathEdgeCover_Witness const *>(&rhs)) {
        shared_ptr<PathEdgeCover_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &PathEdgeCover_Witness::set_equal(Witness &witness) {
    if (PathEdgeCover_Witness  *e = dynamic_cast<PathEdgeCover_Witness *>(&witness)) {
        shared_ptr<PathEdgeCover_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

PathEdgeCover_Witness::PathEdgeCover_Witness() {
  // Empty constructor
}

shared_ptr<WitnessSet> PathEdgeCover_WitnessSet::createEmptyWitnessSet() {
    PathEdgeCover_WitnessSetPointer witnessSet(new PathEdgeCover_WitnessSet);
    return witnessSet;
}

PathEdgeCover_WitnessPointer PathEdgeCover_DynamicCore::createWitness() {
    PathEdgeCover_WitnessPointer w(new PathEdgeCover_Witness);
    return w;
}

void PathEdgeCover_DynamicCore::copyWitness(PathEdgeCover_WitnessPointer w_input,
                                                PathEdgeCover_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void PathEdgeCover_DynamicCore::createInitialWitnessSet() {
    shared_ptr<PathEdgeCover_WitnessSet> witnessSet(new PathEdgeCover_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

WitnessSetPointer PathEdgeCover_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (PathEdgeCover_Witness *e = dynamic_cast<PathEdgeCover_Witness *>(&witness)) {
       // shared_ptr<PathEdgeCover_Witness> p = make_shared<PathEdgeCover_Witness>(*e);
        PathEdgeCover_WitnessPointer w = e->shared_from_this();
        PathEdgeCover_WitnessSetPointer witnessSet(new PathEdgeCover_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer PathEdgeCover_DynamicCore::intro_e(unsigned i , unsigned j, Bag &b, Witness &witness) {
    if (PathEdgeCover_Witness *e = dynamic_cast<PathEdgeCover_Witness *>(&witness)) {
        PathEdgeCover_WitnessPointer w = e->shared_from_this();
        PathEdgeCover_WitnessSetPointer witnessSet(new PathEdgeCover_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}


WitnessSetPointer PathEdgeCover_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (PathEdgeCover_Witness *e = dynamic_cast<PathEdgeCover_Witness *>(&witness)) {
        PathEdgeCover_WitnessPointer w = e->shared_from_this();
        PathEdgeCover_WitnessSetPointer witnessSet(new PathEdgeCover_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer PathEdgeCover_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (PathEdgeCover_Witness  *e1 = dynamic_cast<PathEdgeCover_Witness  *>(&witness1)){
        if(PathEdgeCover_Witness  *e2 = dynamic_cast<PathEdgeCover_Witness  *>(&witness2)) {
            shared_ptr<PathEdgeCover_Witness> w1 = e1->shared_from_this();
            shared_ptr<PathEdgeCover_Witness> w2 = e2->shared_from_this();
            shared_ptr<PathEdgeCover_WitnessSet> witnessSet(new PathEdgeCover_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::join cast error"<<endl;
        exit(20);
    }
}


bool PathEdgeCover_DynamicCore::is_final_witness(Witness &witness) {
    if (PathEdgeCover_Witness *e = dynamic_cast<PathEdgeCover_Witness *>(&witness)) {
        PathEdgeCover_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> PathEdgeCover_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    if (PathEdgeCover_WitnessSetPointer e = dynamic_pointer_cast<PathEdgeCover_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in PathEdgeCover_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}



pair<vector<vector<vector<unsigned >>>, vector<vector<vector<bool>>>>
PathEdgeCover_DynamicCore::combinePartialPathCover(pair<vector<vector<unsigned int>>, vector<vector<bool>>> p1,
                                                   pair<vector<vector<unsigned int>>, vector<vector<bool>>> p2, unsigned width) {
    vector<vector<unsigned >> pc1 = p1.first;
    vector<vector<bool>> ef1 = p1.second;
    vector<vector<unsigned >> pc2 = p2.first;
    vector<vector<bool>> ef2 = p2.second;
    vector<vector<vector<unsigned >>> P;
    vector<vector<vector<bool>>> B;
    if(pc1.size() and pc2.size()){
        vector<unsigned > partialPathCover = *(pc1.end()-1);
        vector<bool> edgesFound = *(ef1.end()-1);
        // first
        vector<vector<unsigned >> a1 = pc1;
        vector<vector<bool>> e1 = ef1;
        a1.erase(a1.end()-1);
        e1.erase(e1.end()-1);
        pair<vector<vector<vector<unsigned >>>, vector<vector<vector<bool>>>> c1 = combinePartialPathCover(make_pair(a1,e1),p2,width);
        for (int i = 0; i < c1.first.size(); ++i) {
            c1.first[i].push_back(partialPathCover);
            c1.second[i].push_back(edgesFound);
            P.push_back(c1.first[i]);
            B.push_back(c1.second[i]);
        }
        // second
        for (int i = 0; i < pc2.size(); ++i) {
            //check
            vector<vector<unsigned >> a2 = pc2;
            vector<vector<bool>> e2 = ef2;
            a2.erase(a2.begin()+i);
            e2.erase(e2.begin()+i);
            auto result = combinePartition(make_pair(partialPathCover,edgesFound), make_pair(pc2[i],ef2[i]),width);
            if(result.first){
                vector<unsigned > combinedPartialCover = result.second.first;
                vector<bool > combinedEdgesFound = result.second.second;
                pair<vector<vector<vector<unsigned >>>, vector<vector<vector<bool>>>> c2 = combinePartialPathCover(make_pair(a1,e1),make_pair(a2,e2),width);
                for (int j = 0; j < c2.first.size(); ++j) {
                    c2.first[j].push_back(combinedPartialCover);
                    c2.second[j].push_back(combinedEdgesFound);
                    P.push_back(c2.first[j]);
                    B.push_back(c2.second[j]);
                }
            }
        }
    }else if(pc1.size() and pc2.size()==0){
        P.push_back(pc1);
        B.push_back(ef1);
    }else if(pc1.size()==0 and pc2.size()){
        P.push_back(pc2);
        B.push_back(ef2);
    }
    return make_pair(P,B);
}

pair<bool, pair<vector<unsigned >, vector<bool>> >
PathEdgeCover_DynamicCore::combinePartition(pair<vector<unsigned int>, vector<bool>> p1,
                                            pair<vector<unsigned int>, vector<bool>> p2, unsigned width) {
    vector<unsigned > combinedCover;
    combinedCover.resize(p1.first.size(),0);
    vector<bool > edgesBool;
    edgesBool.resize(p1.second.size(),false);
    bool possibility = true;
    for (int i = 0; i < p1.first.size(); ++i) {
        if( p1.first[i]==p2.first[i] and p1.first[i]!=0 and p1.first[i]!=width+2 ){
            combinedCover[i] = p1.first[i];
        }else if(p1.first[i]==0 or p2.first[i]==0){
            combinedCover[i] = max(p1.first[i],p2.first[i]);
        }else if( (p1.first[i]==width+2 and p2.first[i]==0) or (p1.first[i]==0 and p2.first[i]==width+2) ){
            combinedCover[i] = max(p1.first[i],p2.first[i]);
        }else{
            possibility = false;
            break;
        }
    }
    if(!possibility){
        combinedCover.clear();
        return make_pair(possibility,make_pair(combinedCover,edgesBool));
    }

    for (int i = 0; i < p1.second.size(); ++i) {
        if(p1.second[i] and p2.second[i]){
            possibility = false;
            break;
        }else{
            edgesBool[i] = (p1.second[i] or p2.second[i]);
        }
    }
    return make_pair(possibility,make_pair(combinedCover,edgesBool));
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new PathEdgeCover_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new PathEdgeCover_DynamicCore(param);
}
}


