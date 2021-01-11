// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "RandomLeveledSetSearch.h"
extern "C" {
RandomLeveledSetSearch * create() {
    return new RandomLeveledSetSearch();
}
RandomLeveledSetSearch * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
        return new RandomLeveledSetSearch(dynamicKernel,conjecture,flags);
    }
}
RandomLeveledSetSearch::RandomLeveledSetSearch() {
    addAttribute("SearchName","RandomLeveledSetSearch");
}
RandomLeveledSetSearch::RandomLeveledSetSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags) : SearchStrategy(dynamicKernel, conjecture, flags) {
    addAttribute("SearchName","RandomLeveledSetSearch");
    this->kernel = kernel;
    this->conjecture = conjecture;
    this->flags = flags;
}
unsigned RandomLeveledSetSearch::bagSetToNumber(set<unsigned> bagSet,unsigned width){
    unsigned number = 0;
    for (unsigned i = 1; i<= width+1; i++){
        if (bagSet.find(i)!=bagSet.end()){
            number = number + pow(2,i-1);
        }
    }
    return number;
}
shared_ptr<CTDNodeNew> RandomLeveledSetSearch::extractCTDNode( unsigned index){
    shared_ptr<CTDNodeNew>  node (new CTDNodeNew());
    node->set_B(generatedVector[index].first->get_bag());
    node->set_parent(nullptr);
    node->set_nodeType(generatedVector[index].second);
    if(index > 0){
        shared_ptr<CTDNodeNew> child = extractCTDNode(index-1);
        child->set_parent(node);
        vector<shared_ptr<CTDNodeNew> > children;
        children.push_back(child);
        node->set_children(children);
    }
    return node;
}
ConcreteTreeDecomposition RandomLeveledSetSearch::extractCTDDecomposition(){
    ConcreteTreeDecomposition T;
    T.root = extractCTDNode(generatedVector.size()-1);
    return T;
}

shared_ptr<StateTreeNode> RandomLeveledSetSearch::extractStateTreeNode(unsigned index){
    shared_ptr<StateTreeNode> node (new StateTreeNode);
    node->set_S(generatedVector[index].first);
    node->set_parent(nullptr);
    shared_ptr<DynamicKernel> sharedKernel(kernel);
    node->set_kernel(sharedKernel);
    node->set_nodeType(generatedVector[index].second);
    if(index>0)
    {
        shared_ptr<StateTreeNode> child = extractStateTreeNode(index-1);
        child->set_parent(node);
        vector<shared_ptr<StateTreeNode>> children;
        children.push_back(child);
        node->set_children(children);
    }
    return node;
}

StateTree RandomLeveledSetSearch::extractStateTreeDecomposition(){

    StateTree stateTree;
    stateTree.root = extractStateTreeNode(generatedVector.size()-1);
    return stateTree;

}

void RandomLeveledSetSearch::search(){

    // Add Initial state to vector
    generatedVector.push_back(make_pair(kernel->initialState(),"Empty"));
    bool foundCounterexample = false;
    unsigned iterationNumber = 1;
    int counter = 0;
    while (!foundCounterexample) {
        counter++;
        string typeState;
        State::ptr q = generatedVector[generatedVector.size()-1].first;
        if(flags->get("PrintStates")==1){
            cout<<"-----Picked State------"<<endl;
            q->print();
        }
        State::ptr s;
        Bag bag = q->get_bag();
        set<unsigned> bagSet = q->get_bag().get_elements();
        vector<unsigned> introducibleVertices;
        vector<pair<unsigned, unsigned>> introducibleEdges;
        vector<unsigned> forgettableVertices;
        // introduce vertices
        if (bagSet.size() < kernel->get_width().get_value() + 1) {
            for (size_t i = 1; i < kernel->get_width().get_value() + 2; ++i) {
                if (bag.vertex_introducible(i)) {
                    introducibleVertices.push_back(i);
                }
            }
        }
        // introduce an edge
        for (set<unsigned>::iterator it = bagSet.begin(); it != bagSet.end(); it++) {
             set<unsigned>::iterator tempit = it;
             tempit++;
             for (set<unsigned>::iterator itr = tempit; itr != bagSet.end(); ++itr) {
                 if (q->get_bag().edge_introducible(*it, *itr)) {
                     pair<unsigned, unsigned> p = make_pair(*it, *itr);
                     introducibleEdges.push_back(p);
                 }
             }
        }
        // forget a vertex
        for (size_t i = 1; i < kernel->get_width().get_value() + 2; ++i) {
            if (q->get_bag().vertex_forgettable(i)) {
                forgettableVertices.push_back(i);
            }
        }
        unsigned numberPossibilities = introducibleVertices.size() + introducibleEdges.size() + forgettableVertices.size();
        if(generatedVector.size() > 1){
            numberPossibilities++;
        }
        unsigned r = random() % numberPossibilities; // select one of the possibiitlies
        //cout<<"introducibleVertices.size(): "<< introducibleVertices.size()<< " introducibleEdges.size(): "<<introducibleEdges.size()<<" forgettableVertices.size(): "<<forgettableVertices.size()<<" r:"<<r<<endl;
        if (r < introducibleVertices.size()) {
            s = kernel->intro_v(q, introducibleVertices[r]);
            typeState = "IntroVertex_"+to_string(introducibleVertices[r]);
        } else {
            r = r - introducibleVertices.size();
            if (r < introducibleEdges.size()) {
                s = kernel->intro_e(q, introducibleEdges[r].first, introducibleEdges[r].second);
                typeState = "IntroEdge_"+to_string(introducibleEdges[r].first)+"_"+to_string(introducibleEdges[r].second);
            } else {
                r = r - introducibleEdges.size();
                if (r < forgettableVertices.size()) {
                    s = kernel->forget_v(q, forgettableVertices[r]);
                    typeState = "ForgetVertex_"+to_string(forgettableVertices[r]);
                }else{
                    pair<State::ptr,string>  deletedState = generatedVector[generatedVector.size()-1];
                    mapState.erase(deletedState.first);
                    generatedVector.erase(generatedVector.begin()+generatedVector.size()-1, generatedVector.end());
                    s = generatedVector[generatedVector.size()-1].first;
                    typeState = generatedVector[generatedVector.size()-1].second;
                }
            }
        }

        if(flags->get("PrintStates")==1){
            cout<<"-----Generated State------"<<endl;
            s->print();
        }
        auto it = mapState.find(s);
        if(it != mapState.end()){
            if(flags->get("PrintStates")==1){
                cout<<"Duplicate State"<<endl;
            }
            unsigned vecIndex = it->second;
            for (unsigned p = vecIndex+1; p < generatedVector.size() ; ++p) {
                mapState.erase(generatedVector[p].first);
            }
            if(vecIndex < generatedVector.size()){
                generatedVector.erase(generatedVector.begin()+vecIndex+1,generatedVector.end());
            }
        }else{
            generatedVector.push_back({s,typeState});
            mapState.insert({s,generatedVector.size()-1});
        }
        foundCounterexample = !(conjecture->evaluateConjectureOnState(*s,kernel));
        if(foundCounterexample){
            cout<<"COUNTER EXAMPLE FOUND"<<endl;
            s->print();
            ConcreteTreeDecomposition T = extractCTDDecomposition();
            cout<<"=======ABSTRACT TREE========="<<endl;
            T.printAbstract();
            cout<<"=======Concrete TREE========="<<endl;
            T.printTree();
            T.writeToFileAbstractTD(this->getPropertyFilePath());
            T.writeToFileConcreteTD(this->getPropertyFilePath());
            shared_ptr<DynamicKernel> sharedKernel = make_shared<DynamicKernel>(*kernel);
            StateTree stateTree = T.convertToStateTree(sharedKernel);
            if(flags->get("StateTree")==1){
                cout<<"=======STATE TREE========="<<endl;
                stateTree.printStateTree();
            }
            stateTree.writeToFile(this->getPropertyFilePath());
            //
            cout << "\n ------------------Constructing Counter Example Graph-------------------"<< endl;
            MultiGraph multiGraph = T.extractMultiGraph();
            multiGraph.printGraph();
            multiGraph.printToFile(this->getPropertyFilePath());
            multiGraph.printToFilePACEFormat(this->getPropertyFilePath());
            multiGraph.convertToGML(this->getPropertyFilePath());
            exit(20);
        }
        if(flags->get("PrintVectors")==1){
            cout<<"*******generated vector print"<<endl;
            for (unsigned i = 1; i < generatedVector.size(); ++i) {
                auto tempMapItr = mapState.find(generatedVector[i].first);
                cout<<"target state"<<endl;
                generatedVector[i].first->print();
                cout<<"First:"<<endl;
                tempMapItr->first->print();
                cout<<"Second:"<<endl;
                (generatedVector[tempMapItr->second]).first->print();
            }
            cout<<"*******"<<endl;
        }
        if(flags->get("LoopTime")==1 and counter == 300000){
            counter=0; // Reset the counter
            cout<<"----------Vector Size:"<<generatedVector.size()<<"---- Iteration number:"<<iterationNumber <<endl;
        }
        iterationNumber++;
    }
}
