// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "RandomLeveledSetSearchPremise.h"
extern "C" {
RandomLeveledSetSearchPremise * create() {
    return new RandomLeveledSetSearchPremise();
}
RandomLeveledSetSearchPremise * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
    return new RandomLeveledSetSearchPremise(dynamicKernel,conjecture,flags);
}
}
RandomLeveledSetSearchPremise::RandomLeveledSetSearchPremise() {
    addAttribute("SearchName","RandomLeveledSetSearchPremise");
}
RandomLeveledSetSearchPremise::RandomLeveledSetSearchPremise(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags) : SearchStrategy(dynamicKernel, conjecture, flags) {
    addAttribute("SearchName","RandomLeveledSetSearchPremise");
    this->kernel = kernel;
    this->conjecture = conjecture;
    this->flags = flags;
}

unsigned RandomLeveledSetSearchPremise::bagSetToNumber(set<unsigned> bagSet,unsigned width){
    unsigned number = 0;
    for (unsigned i = 1; i<= width+1; i++){
        if (bagSet.find(i)!=bagSet.end()){
            number = number + pow(2,i-1);
        }
    }
    return number;
}
shared_ptr<CTDNodeNew> RandomLeveledSetSearchPremise::extractCTDNode( unsigned index){

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

ConcreteTreeDecomposition RandomLeveledSetSearchPremise::extractCTDDecomposition(){
    ConcreteTreeDecomposition T;
    T.root = extractCTDNode(generatedVector.size()-1);
    return T;
}


shared_ptr<StateTreeNode> RandomLeveledSetSearchPremise::extractStateTreeNode(unsigned index){

    shared_ptr<StateTreeNode> node (new StateTreeNode());
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

StateTree RandomLeveledSetSearchPremise::extractStateTreeDecomposition(){

    StateTree stateTree;
    stateTree.root = extractStateTreeNode(generatedVector.size()-1);
    return stateTree;

}

pair<bool,ConcreteTreeDecomposition> RandomLeveledSetSearchPremise::search(){
    // Add Initial state to vector
    generatedVector.push_back({kernel->initialState(),"Empty"});
    bool foundCounterexample = false;
    unsigned iterationNumber = 1;
    while (!foundCounterexample) {
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
                    State::ptr aux = kernel->intro_v(q,i);
                    if (conjecture->evaluatePremiseOnState(*aux,kernel)){
                        introducibleVertices.push_back(i);
                    }
                }
            }
        }
        // introduce an edge
        for (set<unsigned>::iterator it = bagSet.begin(); it != bagSet.end(); it++) {
            set<unsigned>::iterator tempit = it;
            tempit++;
            for (set<unsigned>::iterator itr = tempit; itr != bagSet.end(); ++itr) {
                if (q->get_bag().edge_introducible(*it, *itr)){
                    pair<unsigned, unsigned> p = make_pair(*it, *itr);
		    State::ptr aux = kernel->intro_e(q,*it,*itr);
		    if (conjecture->evaluatePremiseOnState(*aux,kernel)){
		            introducibleEdges.push_back(p);
		    }
		}
            }
        }
        // forget a vertex
        for (size_t i = 1; i < kernel->get_width().get_value() + 2; ++i) {
            if (q->get_bag().vertex_forgettable(i)) {
		State::ptr aux = kernel->forget_v(q,i);
		if (conjecture->evaluatePremiseOnState(*aux,kernel)){
	                forgettableVertices.push_back(i);
		}
            }
        }

        unsigned numberPossibilities = introducibleVertices.size() + introducibleEdges.size() + forgettableVertices.size();
        if(generatedVector.size() > 1){
            numberPossibilities++;
        }
        if(numberPossibilities==0){
            unsigned r = random() % numberPossibilities; // select one of the possibilities
            cout<<" before random number"<<endl;

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
                ConcreteTreeDecomposition *concreteDecomposition = new ConcreteTreeDecomposition;
                *concreteDecomposition = extractCTDDecomposition();
                concreteDecomposition->printAbstract();
                concreteDecomposition->extractMultiGraph().printGraph();
                cout<< "\n convert to gml " <<concreteDecomposition->extractMultiGraph().convertToGML();
                StateTree *stateTree = new StateTree();
                *stateTree = extractStateTreeDecomposition();
                stateTree->printAbstract();
                stateTree->extractMultiGraph().printGraph();
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
            if(flags->get("LoopTime")==1){
                cout<<"----------Vector Size:"<<generatedVector.size()<<"---- Iteration number:"<<iterationNumber <<endl;
            }
            iterationNumber++;
        }
    }


}
