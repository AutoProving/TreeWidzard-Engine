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
   for(auto item:partialPathCover){
       cout<<"{";
       for (int i = 0; i < item.first.size(); ++i) {
           cout<< item.first[i] ;
           if( i != item.first.size()-1){
               cout<<",";
           }
       }
       cout<<" [";
       for (int i = 0; i < item.second.size(); ++i) {
           cout<< item.second[i];
           if(i!= item.second.size()-1){
               cout<<",";
           }
       }
       cout << "]}" ;
   }
   cout<<endl;
}

string PathEdgeCover_Witness::witnessInformation() {
    string info="";
    for(auto item:partialPathCover){
        info+="{";
        for (int i = 0; i < item.first.size(); ++i) {
            info+= to_string(item.first[i]) ;
            if( i != item.first.size()-1){
                info+= ",";
            }
        }
        info+= " [";
        for (int i = 0; i < item.second.size(); ++i) {
            info+= to_string(item.second[i]);
            if(i!= item.second.size()-1){
                info+= ",";
            }
        }
        info+= "]}" ;
    }
    return info+"\n";
}

bool PathEdgeCover_Witness::is_equal_implementation(const PathEdgeCover_WitnessPointerConst w) const {
    return this->partialPathCover == w->partialPathCover;
}
bool PathEdgeCover_Witness::is_less_implementation(const PathEdgeCover_WitnessPointerConst w) const {
    return  this->partialPathCover < w->partialPathCover;
}

Witness & PathEdgeCover_Witness::set_equal_implementation(PathEdgeCover_WitnessPointer w) {
    this->partialPathCover = w->partialPathCover;
    return *this;
}

shared_ptr<Witness> PathEdgeCover_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    PathEdgeCover_WitnessPointer w (new PathEdgeCover_Witness);
    set<pair<vector<int >,vector<bool> > > m;
    for (auto item:partialPathCover) {
        vector<int > nPart;
        for (auto cell:item.first){
            if(cell == 0 or cell == -1){
                nPart.push_back(cell);
            }else if(relabelingMap.count(cell)){
                nPart.push_back(relabelingMap[cell]);
            }else{
                cout<<"Error: PathEdgeCover_Witness::relabel, "<< cell << " is not in the map"<<endl;
                print();
                exit(20);
            }
        }
        m.insert(make_pair(nPart,item.second));
    }
    w->partialPathCover = m;
    return w;
}

PathEdgeCover_Witness::~PathEdgeCover_Witness() {

}

PathEdgeCover_DynamicCore::PathEdgeCover_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","PathEdgeCover");
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","Equal");
}

PathEdgeCover_DynamicCore::PathEdgeCover_DynamicCore(unsigned parameter){
    if(parameter < 1){
        cout<<"PathEdgeCover parameter should be at least 1"<<endl;
        exit(20);
    }
    this->parameter = parameter;
    createInitialWitnessSet(); 
    // Initializing attributes
    addAttribute("CoreName","PathEdgeCover");
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","Equal");
}

void PathEdgeCover_DynamicCore::createInitialWitnessSet_implementation() {
    PathEdgeCover_WitnessPointer w = createWitness();
    this->insertIntoInitialWitnessSet(w);
}

void PathEdgeCover_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, PathEdgeCover_WitnessPointer w,
                                                           PathEdgeCover_WitnessSetPointer witnessSet) {
    witnessSet->insert(w);
}

pair<bool, pair<vector<int>, vector<bool>>> PathEdgeCover_DynamicCore::addEdgeToPath(int i, int j,
                                                                                     pair<vector<int>, vector<bool>> p, int position) {
    int mapped_i = -1;
    int mapped_j = -1;
    for (int index = 0; index < p.first.size() ; ++index) {
        if(p.first[index] == i){
            mapped_i = index;
        }else if(p.first[index]==j){
            mapped_j = index;
        }
    }
    if(!p.second[position]){
        // In this case, the edge is available
        if(mapped_i == -1 and mapped_j==-1){
            p.first[position] = i;
            p.first[position+1] = j;
            p.second[position]= true;
            return make_pair(true,p);
        }else if(mapped_i==-1 and mapped_j==position+1){
            p.first[position]=i;
            p.second[position]=true;
            return make_pair(true,p);
        }else if(mapped_i == position and mapped_j == -1){
            p.first[position+1] =j;
            p.second[position] =true;
            return make_pair(true,p);
        }else if(mapped_i == position and mapped_j == position+1){
            p.second[position]=true;
            return make_pair(true,p);
        }else{
            p.first.clear();
            p.second.clear();
            return make_pair(false,p);
        }
    }else{
        p.first.clear();
        p.second.clear();
        return make_pair(false,p);
    }
}
bool PathEdgeCover_DynamicCore::alreadyCovered(int i, int j, set<pair<vector<int>, vector<bool>>> &pathCover) {
    for(auto item : pathCover){
        for (int k = 0; k < item.first.size()-1; ++k) {
            if(item.second[k]){
                if( (item.first[k]==i and item.first[k+1] == j) or (item.first[k]==j and item.first[k+1] == i) ){
                    return true;
                }
            }
        }
    }
    return false;
}
void PathEdgeCover_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                           PathEdgeCover_WitnessPointer w,
                                                           PathEdgeCover_WitnessSetPointer witnessSet) {
    auto partialPathCover = w->partialPathCover;
    if(!alreadyCovered(i,j,partialPathCover)){
        for(auto item:partialPathCover) {
            for (int position = 0; position < parameter; ++position) {
                // Observation: addEdgePath is not symmetric with respect to i and j
                pair<bool, pair<vector<int>, vector<bool>>> newPath = addEdgeToPath(i, j, item,position);
                if (newPath.first) {
                    auto auxPathCover = w->partialPathCover;
                    auxPathCover.erase(item);
                    auxPathCover.insert(newPath.second);
                    PathEdgeCover_WitnessPointer wPrime = createWitness();
                    wPrime->partialPathCover = auxPathCover;
                    witnessSet->insert(wPrime);
                }
                // Observation: addEdgePath is not symmetric with respect to i and j
                newPath = addEdgeToPath(j,i, item,position);
                if (newPath.first) {
                    auto auxPathCover = w->partialPathCover;
                    auxPathCover.erase(item);
                    auxPathCover.insert(newPath.second);
                    PathEdgeCover_WitnessPointer wPrime = createWitness();
                    wPrime->partialPathCover = auxPathCover;
                    witnessSet->insert(wPrime);
                }
            }
        }
        // We create new path containing a single edge
        for (int position = 0; position < this->parameter ; ++position) {
            vector<int> v;
            v.resize(this->parameter + 1, 0); // parameter is the number of the edges in the path
            vector<bool> b;
            b.resize(parameter,false);
            auto emptyPair = make_pair(v,b);
            pair<bool, pair<vector<int>, vector<bool>>> newPath = addEdgeToPath(i, j, emptyPair,position);
            if (newPath.first) {
                auto auxPathCover = w->partialPathCover;
                auxPathCover.insert(newPath.second);
                PathEdgeCover_WitnessPointer wPrime = createWitness();
                wPrime->partialPathCover = auxPathCover;
                witnessSet->insert(wPrime);
            }
            newPath = addEdgeToPath(j,i, emptyPair,position);
            if (newPath.first) {
                auto auxPathCover = w->partialPathCover;
                auxPathCover.insert(newPath.second);
                PathEdgeCover_WitnessPointer wPrime = createWitness();
                wPrime->partialPathCover = auxPathCover;
                witnessSet->insert(wPrime);
            }
        }
    }else{
        // This core is for simple graphs
        // If {i,j} has been covered then it already exists as an edge and therefore, the graph is a multigraph
    }
}
pair<bool, pair<vector<int>, vector<bool>>> PathEdgeCover_DynamicCore::deleteVertexFromPath(int i,
                                                                                            pair<vector<int>, vector<bool>> p) {

    int mapped_i = -1;
    for (int index = 0; index < p.first.size() ; ++index) {
        if(p.first[index] == i){
            mapped_i = index;
            break;
        }
    }
    if(mapped_i == -1){
        return make_pair(true, p);
    }else{
        if(mapped_i==0){
            if(p.second[mapped_i]){
                p.first[mapped_i] = -1;
                return make_pair(true, p);
            }else{
                p.first.clear();
                p.second.clear();
                return make_pair(false, p);
            }
        }else if(mapped_i == p.first.size()-1){
            if(p.second[mapped_i-1]){
                p.first[mapped_i] = -1;
                return make_pair(true, p);
            }else{
                p.first.clear();
                p.second.clear();
                return make_pair(false, p);
            }
        }else{
            if(p.second[mapped_i-1] and p.second[mapped_i]){
                p.first[mapped_i] = -1;
                return make_pair(true, p);
            }else{
                p.first.clear();
                p.second.clear();
                return make_pair(false, p);
            }
        }
    }
}

void PathEdgeCover_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, PathEdgeCover_WitnessPointer w,
                                                            PathEdgeCover_WitnessSetPointer witnessSet) {

    auto partialPathCover = w->partialPathCover;
    set<pair<vector<int>, vector<bool>>> newCover;
    bool legalVertexDeletion = true;
    for(auto item:partialPathCover){
        pair<bool, pair<vector<int>, vector<bool>>> newPath = deleteVertexFromPath(i, item);
        if(!newPath.first){
            legalVertexDeletion = false;
            break;
        }else{
            bool processed = true;
            for (int index = 0; index < newPath.second.first.size() ; ++index) {
                if(newPath.second.first[index]!= -1){
                    processed = false;
                    break;
                }
            }
            if(!processed){
                newCover.insert(newPath.second);
            }
        }
    }
    if(legalVertexDeletion){
        PathEdgeCover_WitnessPointer wPrime = createWitness();
        wPrime->partialPathCover = newCover;
        witnessSet->insert(wPrime);
    }
}

pair<bool, pair<vector<int>, vector<bool>>> PathEdgeCover_DynamicCore::combinePaths(
        pair<vector<int>, vector<bool>> p1, pair<vector<int>, vector<bool>> p2) {
    vector<int > combinedVertexPath;
    combinedVertexPath.resize(p1.first.size(),0);
    vector<bool > combinedBoolPath;
    combinedBoolPath.resize(p1.second.size(),false);
    bool possibilityOfCombine = true;
    for (int i = 0; i < p1.first.size(); ++i) {
        if( p1.first[i]==p2.first[i] and p1.first[i]!=0 and p1.first[i]!=-1 ){
            combinedVertexPath[i] = p1.first[i];
        }else if(p1.first[i]==0 or p2.first[i]==0){
            combinedVertexPath[i] = max(p1.first[i],p2.first[i]);
        }else if( (p1.first[i]==-1 and p2.first[i]==0) or (p1.first[i]==0 and p2.first[i]==-1) ){
            combinedVertexPath[i] = max(p1.first[i],p2.first[i]);
        }else{
            possibilityOfCombine = false;
            break;
        }
    }
    if(!possibilityOfCombine){
        combinedVertexPath.clear();
        return make_pair(possibilityOfCombine,make_pair(combinedVertexPath,combinedBoolPath));
    }

    for (int i = 0; i < p1.second.size(); ++i) {
        if(p1.second[i] and p2.second[i]){
            possibilityOfCombine = false;
            combinedVertexPath.clear();
            break;
        }else{
            combinedBoolPath[i] = (p1.second[i] or p2.second[i]);
        }
    }
    return make_pair(possibilityOfCombine,make_pair(combinedVertexPath,combinedBoolPath));

}

set<set<pair<vector<int>, vector<bool>>>> PathEdgeCover_DynamicCore::combinePartialPathCover(
        set<pair<vector<int>, vector<bool>>> p1, set<pair<vector<int>, vector<bool>>> p2) {

    set<set<pair<vector<int>, vector<bool>>>> returnDecompositions;
    if(p1.size()>0 and p2.size()>0){
        auto auxP1 = p1;
        auto path = *(auxP1.begin());
        auxP1.erase(path);
        auto pathDecompositions = combinePartialPathCover(auxP1,p2);
        for(auto pathDecomposition:pathDecompositions){
            pathDecomposition.insert(path);
            returnDecompositions.insert(pathDecomposition);
        }
        for (auto p:p2) {
            auto combinedPathAndP = combinePaths(path,p);
            if(combinedPathAndP.first){
                auto auxP2 = p2;
                auxP2.erase(p);
                auto subPathDecompositions = combinePartialPathCover(auxP1,auxP2);
                for(auto pathDecomposition : subPathDecompositions){
                    pathDecomposition.insert(combinedPathAndP.second);
                    returnDecompositions.insert(pathDecomposition);
                }
            }
        }
    }else if(p1.size() > 0 and p2.size()==0){
        returnDecompositions.insert(p1);
    }else if(p1.size() == 0 and p2.size()>0){
        returnDecompositions.insert(p2);
    }
    return returnDecompositions;
}


void PathEdgeCover_DynamicCore::join_implementation(Bag &b, PathEdgeCover_WitnessPointer w1,
                                                        PathEdgeCover_WitnessPointer w2,PathEdgeCover_WitnessSetPointer witnessSet) {
    auto decompositions = combinePartialPathCover(w1->partialPathCover,w2->partialPathCover);
    for (auto decomposition : decompositions) {
        PathEdgeCover_WitnessPointer wPrime = createWitness();
        wPrime->partialPathCover = decomposition;
        witnessSet->insert(wPrime);
    }
}

bool PathEdgeCover_DynamicCore::is_final_witness_implementation(PathEdgeCover_WitnessPointer w) {
    auto partialPathCover = w->partialPathCover;
    for(auto item: partialPathCover){
        for (int index = 0; index < item.second.size() ; ++index) {
            if(!item.second[index]){
                return false;
            }
        }
    }
    return true;
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


//
//pair<vector<vector<vector<unsigned >>>, vector<vector<vector<bool>>>>
//PathEdgeCover_DynamicCore::combinePartialPathCover(pair<vector<vector<unsigned int>>, vector<vector<bool>>> p1,
//                                                   pair<vector<vector<unsigned int>>, vector<vector<bool>>> p2, unsigned width) {
//    vector<vector<unsigned >> pc1 = p1.first;
//    vector<vector<bool>> ef1 = p1.second;
//    vector<vector<unsigned >> pc2 = p2.first;
//    vector<vector<bool>> ef2 = p2.second;
//    vector<vector<vector<unsigned >>> P;
//    vector<vector<vector<bool>>> B;
//    if(pc1.size() and pc2.size()){
//        vector<unsigned > partialPathCover = *(pc1.end()-1);
//        vector<bool> edgesFound = *(ef1.end()-1);
//        // first
//        vector<vector<unsigned >> a1 = pc1;
//        vector<vector<bool>> e1 = ef1;
//        a1.erase(a1.end()-1);
//        e1.erase(e1.end()-1);
//        pair<vector<vector<vector<unsigned >>>, vector<vector<vector<bool>>>> c1 = combinePartialPathCover(make_pair(a1,e1),p2,width);
//        for (int i = 0; i < c1.first.size(); ++i) {
//            c1.first[i].push_back(partialPathCover);
//            c1.second[i].push_back(edgesFound);
//            P.push_back(c1.first[i]);
//            B.push_back(c1.second[i]);
//        }
//        // second
//        for (int i = 0; i < pc2.size(); ++i) {
//            //check
//            vector<vector<unsigned >> a2 = pc2;
//            vector<vector<bool>> e2 = ef2;
//            a2.erase(a2.begin()+i);
//            e2.erase(e2.begin()+i);
//            auto result = combinePartition(make_pair(partialPathCover,edgesFound), make_pair(pc2[i],ef2[i]),width);
//            if(result.first){
//                vector<unsigned > combinedPartialCover = result.second.first;
//                vector<bool > combinedEdgesFound = result.second.second;
//                pair<vector<vector<vector<unsigned >>>, vector<vector<vector<bool>>>> c2 = combinePartialPathCover(make_pair(a1,e1),make_pair(a2,e2),width);
//                for (int j = 0; j < c2.first.size(); ++j) {
//                    c2.first[j].push_back(combinedPartialCover);
//                    c2.second[j].push_back(combinedEdgesFound);
//                    P.push_back(c2.first[j]);
//                    B.push_back(c2.second[j]);
//                }
//            }
//        }
//    }else if(pc1.size() and pc2.size()==0){
//        P.push_back(pc1);
//        B.push_back(ef1);
//    }else if(pc1.size()==0 and pc2.size()){
//        P.push_back(pc2);
//        B.push_back(ef2);
//    }
//    return make_pair(P,B);
//}
//
//pair<bool, pair<vector<unsigned >, vector<bool>> >
//PathEdgeCover_DynamicCore::combinePartition(pair<vector<unsigned int>, vector<bool>> p1,
//                                            pair<vector<unsigned int>, vector<bool>> p2, unsigned width) {
//    vector<unsigned > combinedCover;
//    combinedCover.resize(p1.first.size(),0);
//    vector<bool > edgesBool;
//    edgesBool.resize(p1.second.size(),false);
//    bool possibility = true;
//    for (int i = 0; i < p1.first.size(); ++i) {
//        if( p1.first[i]==p2.first[i] and p1.first[i]!=0 and p1.first[i]!=width+2 ){
//            combinedCover[i] = p1.first[i];
//        }else if(p1.first[i]==0 or p2.first[i]==0){
//            combinedCover[i] = max(p1.first[i],p2.first[i]);
//        }else if( (p1.first[i]==width+2 and p2.first[i]==0) or (p1.first[i]==0 and p2.first[i]==width+2) ){
//            combinedCover[i] = max(p1.first[i],p2.first[i]);
//        }else{
//            possibility = false;
//            break;
//        }
//    }
//    if(!possibility){
//        combinedCover.clear();
//        return make_pair(possibility,make_pair(combinedCover,edgesBool));
//    }
//
//    for (int i = 0; i < p1.second.size(); ++i) {
//        if(p1.second[i] and p2.second[i]){
//            possibility = false;
//            break;
//        }else{
//            edgesBool[i] = (p1.second[i] or p2.second[i]);
//        }
//    }
//    return make_pair(possibility,make_pair(combinedCover,edgesBool));
//}


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


