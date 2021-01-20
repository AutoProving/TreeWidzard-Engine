// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "IsomorphismBreadthFirstSearch.h"

extern "C" {
    IsomorphismBreadthFirstSearch * create() {
        return new IsomorphismBreadthFirstSearch();
    }
    IsomorphismBreadthFirstSearch * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
        return new IsomorphismBreadthFirstSearch(dynamicKernel,conjecture,flags);
    }
}

IsomorphismBreadthFirstSearch::IsomorphismBreadthFirstSearch() {
    addAttribute("SearchName","IsomorphismBreadthFirstSearch");
}

IsomorphismBreadthFirstSearch::IsomorphismBreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags): SearchStrategy(dynamicKernel, conjecture, flags) {
    this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
    addAttribute("SearchName","IsomorphismBreadthFirstSearch");
}

void IsomorphismBreadthFirstSearch::search(){
    unsigned int width = kernel->get_width().get_value();
    State::ptr initialState = kernel->initialState();
    // Initialization of the acyclic tree automaton bfsDag.
    bfsDAG.addState(initialState);
    AbstractTreeDecompositionNodeContent initialTransitionContent("Leaf");
    vector<State::ptr> initialAntecedents; // Empty vector since there are no children.
    Transition<State::ptr,AbstractTreeDecompositionNodeContent> initialTransition(initialState,initialTransitionContent,initialAntecedents);
    bfsDAG.addTransition(initialTransition);
    // Initialization of the search containers.
    allStatesSet.resize(width+2);
    newStatesSet.resize(width+2);
    newStatesVector.resize(width+2);
	allStatesSet[0].insert(initialState);
	newStatesSet[0].insert(initialState);
	// Search Algorithm
	int iterationNumber = 0;
    unsigned allStatesSize = 1;
    unsigned newStatesSize = 1;
    while(newStatesSize) {
        iterationNumber++;
        // Initializing newStatesVector.
        for(int index = 0; index < width + 2; index++){
            newStatesVector[index].clear();
            newStatesVector[index].resize(newStatesSet[index].size());
            std::copy(newStatesSet[index].begin(), newStatesSet[index].end(), newStatesVector[index].begin());
            newStatesSet[index].clear();
        }
        for (int index = 0; index < width + 2; index++) {
            //This loop is suitable for parallelization
            for (int l = 0; l < newStatesVector[index].size(); l++) {
                State::ptr statePointer = newStatesVector[index][l];
                if (flags->get("PrintStates") == 1) {
                    cout<<"Base State "<<endl;
                    statePointer.print();
                }
                Bag bag = statePointer->get_bag();
                set<unsigned> bagElements = bag.get_elements();
                ///////////////////////////////////////////////////////
                //////////////////// Introduce Vertex /////////////////
                ///////////////////////////////////////////////////////
                // the +1 below comes from the fact that treewidth is
                // size of the bag minus one. So the loop iterates
                // from 1 to number of elements in the bag.
                if (bagElements.size() < width + 1) {
                    unsigned i = bagElements.size() + 1;
                    if (bag.vertex_introducible(i)) {
                        State::ptr newStatePointer = kernel->intro_v(statePointer, i);
                        State::ptr relabeledNewStatePointer = canonicalState(newStatePointer); // Computes the canonical state derived from newStatePointer.
                        if (!allStatesSet[bagElements.size()+1].count(relabeledNewStatePointer) and
                            !newStatesSet[bagElements.size()+1].count(relabeledNewStatePointer)) {
                            newStatesSet[bagElements.size() + 1].insert(relabeledNewStatePointer);
                            State::ptr consequentState = relabeledNewStatePointer;
                            bfsDAG.addState(consequentState);
                            AbstractTreeDecompositionNodeContent transitionContent("IntroVertex_" + to_string(i));
                            vector<State::ptr> antecedentStates;
                            antecedentStates.push_back(statePointer); // StatePointer is the only antecedent state.
                            Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
                                                                                                    transitionContent,
                                                                                                    antecedentStates);
                            bfsDAG.addTransition(transition);
                            if (flags->get("PrintStates") == 1) {
                                cout << "Intro Vertex " << i << endl;
                                consequentState.print();
                            }
                        }
                    }
                }

                ///////////////////////////////////////////////////////
                //////////////////// Forget Vertex ////////////////////
                ///////////////////////////////////////////////////////
                for (auto it = bagElements.begin(); it != bagElements.end(); it++) {
                    State::ptr newStatePointer = kernel->forget_v(statePointer, *it);
                    // We need to normalized the bag so that the bag.elements is equal to {1,...,k} for some k.
                    State::ptr intermediateStatePointer = newStatePointer->relabel(relabeledMapGenerator(newStatePointer->get_bag().get_elements()));
                    State::ptr relabeledNewStatePointer = canonicalState(intermediateStatePointer); // Computes the canonical state derived from newStatePointer.
                    if (!allStatesSet[bagElements.size()-1].count(relabeledNewStatePointer) and !newStatesSet[bagElements.size()-1].count(relabeledNewStatePointer)) {
                        newStatesSet[bagElements.size()-1].insert(relabeledNewStatePointer);
                        State::ptr consequentState = relabeledNewStatePointer;
                        AbstractTreeDecompositionNodeContent transitionContent("ForgetVertex_" + to_string(*it));
                        bfsDAG.addState(consequentState);
                        vector<State::ptr> antecedentStates;
                        antecedentStates.push_back(statePointer);
                        Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
                                                                                                transitionContent,
                                                                                                antecedentStates);
                        bfsDAG.addTransition(transition);
                        if (flags->get("PrintStates") == 1) {
                            cout<<"Forget Vertex "<< *it <<endl;
                            consequentState.print();
                        }
                    }
                }
                //Introduce Edge
                if (bag.get_elements().size() > 1) {
                    for (auto it = bagElements.begin(); it != bagElements.end(); it++) {
                        auto itX = it;
                        itX++; // TODO write this more elegantly
                        if (itX != bagElements.end()) {
                            for (auto itPrime = itX; itPrime != bagElements.end(); itPrime++) {
                                State::ptr newStatePointer = kernel->intro_e(statePointer, *it, *itPrime);
                                State::ptr relabeledNewStatePointer = canonicalState(newStatePointer); // Computes the canonical state derived from newStatePointer.                                if (!allStatesSet[bagElements.size()].count(relabeledNewStatePointer) and
                                if (!allStatesSet[index].count(relabeledNewStatePointer) and
                                        !newStatesSet[bagElements.size()].count(relabeledNewStatePointer)) {
                                    newStatesSet[bagElements.size()].insert(relabeledNewStatePointer);
                                    State::ptr consequentState = relabeledNewStatePointer;
                                    AbstractTreeDecompositionNodeContent transitionContent(
                                            "IntroEdge_" + to_string(*it) + "_" + to_string(*itPrime));
                                    bfsDAG.addState(consequentState);
                                    vector<State::ptr> antecedentStates;
                                    antecedentStates.push_back(statePointer);
                                    Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
                                                                                                            transitionContent,
                                                                                                            antecedentStates);
                                    bfsDAG.addTransition(transition);
                                    if (flags->get("PrintStates") == 1) {
                                        cout<<"Intro Edge "<< *it<< " " << *itPrime<<endl;
                                        consequentState.print();
                                    }
                                }
                            }
                        }
                    }
                }

                // join
                if (kernel->get_width().get_name() == "tree_width") {
                    // join new states with past states.
                    for (auto it = allStatesSet[index].begin(); it != allStatesSet[index].end(); it++) {
                        // Testing possibility of joining with each permuted version of "it".
                        map<unsigned ,unsigned > m = identityMap(statePointer->get_bag().get_elements());
                        do{
                            State::ptr permutedState = (*it)->relabel(m);
                            State::ptr newStatePointer = kernel->join(statePointer, permutedState);
                            State::ptr relabeledNewStatePointer = canonicalState(newStatePointer); // Computes the canonical state derived from newStatePointer.                            // for each relabeling of the bag from it, you do the join between the statePointer and the relabeled version of it.
                            if (!allStatesSet[index].count(relabeledNewStatePointer) and
                                !newStatesSet[index].count(relabeledNewStatePointer)) {
                                newStatesSet[index].insert(relabeledNewStatePointer);
                                State::ptr consequentState = relabeledNewStatePointer;
                                AbstractTreeDecompositionNodeContent transitionContent("Join");
                                bfsDAG.addState(consequentState);
                                vector<State::ptr> antecedentStates;
                                antecedentStates.push_back(statePointer);
                                antecedentStates.push_back(*it);
                                Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
                                                                                                        transitionContent,
                                                                                                        antecedentStates);
                                bfsDAG.addTransition(transition);
                                if (flags->get("PrintStates") == 1) {
                                    cout<<"Join "<<endl;
                                    consequentState.print();
                                }
                            }
                        }while(nextPermutation(m));
                    }
                }
            }
        }
        // Counters for new states and all states
        allStatesSize = 0;
        newStatesSize = 0;
        // Verifying the conjecture with new states
        for(int index = 0 ; index < width+2 ; index++){
            for (auto it = newStatesSet[index].begin(); it != newStatesSet[index].end(); it++) {
                if (!conjecture->evaluateConjectureOnState(**it, kernel)) {
                    cout << "BAD STATE:" << endl;
                    (**it).print();
                    bfsDAG.addFinalState(*it);
                    cout<<"===========Run Tree============"<<endl;
                    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = extractRunTree(*it);
                    runTree.writeToFile(this->getPropertyFilePath());
                    cout << "=======ABSTRACT TREE=========" << endl;
                    Term<AbstractTreeDecompositionNodeContent>* term = new AbstractTreeDecomposition;
                    *term = runTree.convertRunToTerm(runTree);
                    AbstractTreeDecomposition* atd = static_cast<AbstractTreeDecomposition *>(term);
                    atd->printTermNodes();
                    atd->writeToFile(this->getPropertyFilePath());
                    ConcreteTreeDecomposition ctd = atd->convertToConcreteTreeDecomposition();
                    cout << "=======Concrete TREE=========" << endl;
                    ctd.printTermNodes();
                   // ctd.writeToFileConcreteTD(this->getPropertyFilePath());
                    cout << "\n ------------------Constructing Counter Example Graph-------------------" << endl;
                    MultiGraph multiGraph = ctd.extractMultiGraph();
                    multiGraph.printGraph();
                    multiGraph.printToFile(this->getPropertyFilePath());
                    multiGraph.convertToGML(this->getPropertyFilePath());
                    multiGraph.printToFilePACEFormat(this->getPropertyFilePath());
                    exit(20);
                }
            }
            set<State::ptr> setUnion;
            set_union(allStatesSet[index].begin(),allStatesSet[index].end(),newStatesSet[index].begin(),newStatesSet[index].end(),inserter(setUnion,setUnion.begin()));
            allStatesSet[index] = setUnion;
            setUnion.clear();
            allStatesSize = allStatesSize + allStatesSet[index].size();
            newStatesSize = newStatesSize + newStatesSet[index].size();
        }

        if(flags->get("LoopTime") == 1){
            cout<<"AllState:"<<allStatesSize<<" new State: "<<newStatesSize<<endl;
            cout << endl << "----------------- End Iteration: " << iterationNumber << " ----------------------------" << endl << endl;
        }
	}
    cout<<"Finished :)"<<endl;
}

map<unsigned, unsigned> IsomorphismBreadthFirstSearch::relabeledMapGenerator(set<unsigned int> bagElements) {
    map<unsigned int, unsigned int> map;
    unsigned i =1;
    for(auto v:bagElements){
        map.insert(make_pair(v,i));
        i++;
    }
    return map;
}

map<unsigned int, unsigned int> IsomorphismBreadthFirstSearch::identityMap(unsigned int k) {
    map<unsigned , unsigned > m;
    for(unsigned index = 1; index <= k ; index++){
        m.insert(make_pair(index,index));
    }
    return m;
}

map<unsigned int, unsigned int> IsomorphismBreadthFirstSearch::identityMap(set<unsigned int> bagSet) {
    map<unsigned , unsigned > m;
    for(auto item:bagSet){
        m.insert(make_pair(item,item));
    }
    return m;
}

bool IsomorphismBreadthFirstSearch::nextPermutation(map<unsigned int, unsigned int> &permutation) {
    vector<unsigned > indexToValue;
    indexToValue.resize(permutation.size());
    unsigned counter = 0;
    for(auto item:permutation){
        indexToValue[counter] = item.second;
        counter++;
    }
    if(!next_permutation(indexToValue.begin(),indexToValue.end())){
        return false;
    }else{
        unsigned index= 0;
        for(auto& item:permutation){
            item.second = indexToValue[index];
            index++;
        }
        return true;
    }
}


State::ptr IsomorphismBreadthFirstSearch::canonicalState(State::ptr state) {
   // Assume that the bag.elements is equal to {1,...,k} for some k.
    State::ptr canonicalState = state;
    map<unsigned ,unsigned > m = identityMap(state->get_bag().get_elements());
    do{
        State::ptr relabeledState = state->relabel(m);
        if(relabeledState < canonicalState){
            canonicalState = relabeledState;
        }
    }while(nextPermutation(m));
    return canonicalState;
}

void IsomorphismBreadthFirstSearch::extractRunTreeNode(
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> wrongRunNode,
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> correctedRunNode,
        map<unsigned int, unsigned int> &m) {
    string wrongSymbol = wrongRunNode->getNodeContent().getRunNodeContent().getSymbol();
    ///////////////////////////////////////////////////////
    cout<<wrongSymbol<<endl;
    cout<<"Input Map"<<endl;
    for(auto item:m){
        cout<<item.first<<"->"<<item.second<<" ";
    }
    cout<<endl;
    cout<<"Wrong node"<<endl;
    wrongRunNode->getNodeContent().getState().print();
    cout<<endl;
    //////////////////////////////////////////////////////
    if(strstr(wrongSymbol.c_str(),"Leaf")){
        if(m.empty()){
            State::ptr correctedState = wrongRunNode->getNodeContent().getState();
            AbstractTreeDecompositionNodeContent abs("Leaf");
            RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
            correctedRunNode->setNodeContent(correctedRunNodeContent);

        }else{
            cout<<"Error: In Empty node, map is not valid. "<<endl;
            exit(20);
        }
    }else if(strstr(wrongSymbol.c_str(),"IntroVertex")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        /////////////////////////////////////////////////////////////////////
        cout<<"corrected state"<<endl;
        correctedState.print();
        cout<<endl;
        /////////////////////////////////////////////////////////////////////
        cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
        // discovers IntroducedVertex and child's map;
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        unsigned introducedVertex = 0;
        map<unsigned ,unsigned > childMap;
        bool breakFor = false; // If true then vertex and map have been found
        for(auto v:setElements){
            set<unsigned > childDomain = setElements;
            childDomain.erase(v);
//            map<unsigned ,unsigned > initialChildMap = m;
//            for(auto item : initialChildMap){
//                if(item.second == v){
//                    initialChildMap.erase(item.first);
//                    break;
//                }
//            }

            map<unsigned ,unsigned > initialChildMap;
            bool afterV = false;
            for(auto item:m){
                if(item.second != v and afterV){
                    initialChildMap.insert(make_pair(item.first-1,item.second));
                }else if(item.second == v){
                    afterV = true;
                }else{
                    initialChildMap.insert(make_pair(item.first,item.second));
                }
            }
            map<unsigned ,unsigned > testMap  = identityMap(childDomain);
            do {
                map<unsigned,unsigned > compositionMap;
                for(auto item:initialChildMap){
                    compositionMap.insert(make_pair(item.first,testMap[item.second]));
                }
                State::ptr childState = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap);
                State::ptr testState = kernel->intro_v(childState,v);
                if(testState == correctedState){
                    cout<<"child state"<<endl;
                    childState.print();
                    cout<<endl;
                    cout<<"test state"<<endl;
                    testState.print();
                    cout<<endl;
                    cout<<"THE MAP"<<endl;
                    for(auto item:compositionMap){
                        cout<<item.first<<"->"<<item.second<<" ";
                    }
                    cout<<"#####################################"<<endl;
                    ///////////////////////////////////////////////////////
                    introducedVertex = v;
                    childMap = compositionMap;
                    breakFor = true;
                    break;
                }
            }while(nextPermutation(testMap));
            if(breakFor){
                break;
            }
        }
        // Discover the child's label and which vertex was introduced.
        if(introducedVertex==0){
            cout<<"Error: child state is not consistent with the current state"<<endl;
            exit(20);
        }
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("IntroVertex_"+to_string(introducedVertex));

        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);

        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,childMap);

    }else if(strstr(wrongSymbol.c_str(),"ForgetVertex")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        /////////////////////////////////////////////////////////////////////
        cout<<"corrected state"<<endl;
        correctedState.print();
        cout<<endl;
        /////////////////////////////////////////////////////////////////////
        cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
        // discovers ForgottenVertex and child's map;
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        unsigned forgottenVertex = 0;
        map<unsigned ,unsigned > childMap;
        bool breakFor = false; // If true then vertex and map have been found
        set<unsigned > allVertices;
        for(unsigned i = 1; i <= kernel->get_width().get_value()+1; i++){
            allVertices.insert(i);
        }
        set<unsigned > availableVertices;
        set_difference(allVertices.begin(),allVertices.end(),setElements.begin(),setElements.end(),inserter(availableVertices,availableVertices.begin()));

        for(auto v:availableVertices){
            set<unsigned > childDomain = setElements;
            childDomain.insert(v);
            map<unsigned ,unsigned > initialChildMap = m;
            // add v to initialChildMap
            initialChildMap.insert(make_pair(m.size()+1,v));

            map<unsigned ,unsigned > testMap  = identityMap(childDomain);
            do {
                map<unsigned,unsigned > compositionMap;
                for(auto item:initialChildMap){
                    compositionMap.insert(make_pair(item.first,testMap[item.second]));
                }
                State::ptr childState = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap);
                State::ptr testState = kernel->forget_v(childState,v);
                if(testState == correctedState){
                    cout<<"child state"<<endl;
                    childState.print();
                    cout<<endl;
                    cout<<"test state"<<endl;
                    testState.print();
                    cout<<endl;
                    cout<<"THE MAP"<<endl;
                    for(auto item:compositionMap){
                        cout<<item.first<<"->"<<item.second<<" ";
                    }
                    cout<<"#####################################"<<endl;
                    ///////////////////////////////////////////////////////
                    forgottenVertex = v;
                    childMap = compositionMap;
                    breakFor = true;
                    break;
                }
            }while(nextPermutation(testMap));
            if(breakFor){
                break;
            }
        }
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("ForgetVertex_"+to_string(forgottenVertex));
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,childMap);
    }else if(strstr(wrongSymbol.c_str(),"IntroEdge")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        // discovers IntroducedEdge and child's map;
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        unsigned e_i = 0;
        unsigned e_j = 0;
        map<unsigned ,unsigned > childMap;
        bool breakFor = false; // If true then vertex and map have been found
        for(auto it = setElements.begin(); it!= setElements.end(); it++){
            auto itr = it;
            itr++;
            while(itr!=setElements.end()){
                set<unsigned > childDomain = setElements;
                map<unsigned ,unsigned > initialChildMap = m;
                map<unsigned ,unsigned > testMap  = identityMap(childDomain);
                do {
                    map<unsigned,unsigned > compositionMap;
                    for(auto item:initialChildMap){
                        compositionMap.insert(make_pair(item.first,testMap[item.second]));
                    }
                    State::ptr childState = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap);
                    State::ptr testState = kernel->intro_e(childState,*it,*itr);
                    if(testState == correctedState){
                        e_i=*it;
                        e_j=*itr;
                        childMap = compositionMap;
                        breakFor = true;
                        break;
                    }
                }while(nextPermutation(testMap));
                if(breakFor){
                    break;
                }
                itr++;
            }
            if(breakFor){
                break;
            }
        }
        if(!breakFor){
            cout<<"Error: in IntroEdge, there is not a consistent state"<<endl;
            exit(20);
        }
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("IntroEdge_"+to_string(e_i)+"_"+to_string(e_j));
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,childMap);

    }else if(strstr(wrongSymbol.c_str(),"Join")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("Join");
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        // Child 1
        map<unsigned ,unsigned > initialChildMap1 = m;
        set<unsigned > childDomain1 = setElements;
        map<unsigned ,unsigned > testMap1  = identityMap(childDomain1);
        map<unsigned ,unsigned > childMap1;
        map<unsigned ,unsigned > childMap2;
        bool checkContinue = false;
        do {
            map<unsigned,unsigned > compositionMap1;
            for(auto item:initialChildMap1){
                compositionMap1.insert(make_pair(item.first,testMap1[item.second]));
            }
            State::ptr childState1 = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap1);
            // Child 2
            map<unsigned ,unsigned > initialChildMap2 = m;
            set<unsigned > childDomain2 = setElements;
            map<unsigned ,unsigned > testMap2  = identityMap(childDomain2);
            do {
                map<unsigned,unsigned > compositionMap2;
                for(auto item:initialChildMap2){
                    compositionMap2.insert(make_pair(item.first,testMap2[item.second]));
                }
                State::ptr childState2 = wrongRunNode->getChildren()[1]->getNodeContent().getState()->relabel(compositionMap2);
                State::ptr testState = kernel->join(childState1,childState2);
                if(correctedState == testState){
                    childMap1 = compositionMap1;
                    childMap2 = compositionMap2;
                    checkContinue = true;
                    break;
                }
            } while (nextPermutation(testMap2));
            if(checkContinue){
                break;
            }
        }while(nextPermutation(testMap1));
        // child 1
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child1(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child1->setParent(correctedRunNode);
        children.push_back(child1);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child1,childMap1);
        // child 2
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child2(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child2->setParent(correctedRunNode);
        children.push_back(child2);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child2,childMap2);
        correctedRunNode->setChildren(children);
    }else{
        cout<<"Error: Node Type is not valid."<<endl;
        exit(20);
    }

}

RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
IsomorphismBreadthFirstSearch::extractRunTree(State::ptr state) {
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> wrongRunTree = bfsDAG.retrieveRunAcyclicAutomaton(state);
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunTree;
    shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>>
            correctedRoot(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
    map<unsigned,unsigned> m = identityMap(state->get_bag().get_elements());
    extractRunTreeNode(wrongRunTree.getRoot(),correctedRoot,m);
    correctedRunTree.setRoot(correctedRoot);
    return correctedRunTree;
}
