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

shared_ptr<TermNode<ConcreteNode> > RandomLeveledSetSearchPremise::extractCTDNode( unsigned index){
    shared_ptr<TermNode <ConcreteNode > >  node (new TermNode<ConcreteNode>);
    ConcreteNode concrete;
    concrete.setBag(generatedVector[index].first->get_bag());
    node->setParent(nullptr);
    concrete.setSymbol(generatedVector[index].second);
    node->setNodeContent(concrete);
    if(index > 0){
        shared_ptr<TermNode<ConcreteNode> > child = extractCTDNode(index-1);
        child->setParent(node);
        node->addChild(child);
    }
    return node;
}
ConcreteTreeDecomposition RandomLeveledSetSearchPremise::extractCTDDecomposition(){
    ConcreteTreeDecomposition T;
    shared_ptr<TermNode<ConcreteNode>> root = extractCTDNode(generatedVector.size()-1);
    T.setRoot(root);
    return T;
}

//shared_ptr<StateTreeNode> RandomLeveledSetSearchPremise::extractStateTreeNode(unsigned index){
//
//    shared_ptr<StateTreeNode> node (new StateTreeNode());
//    node->set_S(generatedVector[index].first);
//    node->set_parent(nullptr);
//    shared_ptr<DynamicKernel> sharedKernel(kernel);
//    node->set_kernel(sharedKernel);
//    node->set_nodeType(generatedVector[index].second);
//    if(index>0)
//    {
//        shared_ptr<StateTreeNode> child = extractStateTreeNode(index-1);
//        child->set_parent(node);
//        vector<shared_ptr<StateTreeNode>> children;
//        children.push_back(child);
//        node->set_children(children);
//    }
//    return node;
//
//}
//
//StateTree RandomLeveledSetSearchPremise::extractStateTreeDecomposition(){
//
//    StateTree stateTree;
//    stateTree.root = extractStateTreeNode(generatedVector.size()-1);
//    return stateTree;
//
//}

void RandomLeveledSetSearchPremise::search(){
    // Add Initial state to vector
    generatedVector.push_back({kernel->initialState(),"Leaf"});
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
            cout<<"There is no generated state!"<<endl;
            exit(20);
        }
        if(numberPossibilities!=0){
            unsigned r = random() % numberPossibilities; // select one of the possibilities
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
                AbstractTreeDecomposition abstractTreeDecomposition = T.convertToAbstractTreeDecomposition();
                abstractTreeDecomposition.printTermNodes();
                abstractTreeDecomposition.writeToFile(this->getPropertyFilePath());
                cout<<"=======Concrete TREE========="<<endl;
                T.printTermNodes();

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
            if(flags->get("LoopTime")==1  and counter == 10){
                counter=0;
                cout<<"----------Vector Size:"<<generatedVector.size()<<"---- Iteration number:"<<iterationNumber <<endl;
            }
            iterationNumber++;
        }
    }
}
