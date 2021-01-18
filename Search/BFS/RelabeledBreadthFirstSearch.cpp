// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "RelabeledBreadthFirstSearch.h"

extern "C" {
    RelabeledBreadthFirstSearch * create() {
        return new RelabeledBreadthFirstSearch();
    }
    RelabeledBreadthFirstSearch * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
        return new RelabeledBreadthFirstSearch(dynamicKernel,conjecture,flags);
    }
}

RelabeledBreadthFirstSearch::RelabeledBreadthFirstSearch() {
    addAttribute("SearchName","RelabeledBreadthFirstSearch");
}

RelabeledBreadthFirstSearch::RelabeledBreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags): SearchStrategy(dynamicKernel, conjecture, flags) {
    this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
    addAttribute("SearchName","RelabeledBreadthFirstSearch");
}

void RelabeledBreadthFirstSearch::search(){
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
                        State::ptr relabeledNewStatePointer = newStatePointer; // The relabeling is the identity.
                        if (!allStatesSet[bagElements.size()+1].count(relabeledNewStatePointer) and
                            !newStatesSet[bagElements.size()+1].count(relabeledNewStatePointer)) {
                            newStatesSet[bagElements.size()+1].insert(relabeledNewStatePointer);
                            State::ptr consequentState = relabeledNewStatePointer;
                            bfsDAG.addState(consequentState);
                            AbstractTreeDecompositionNodeContent transitionContent("IntroVertex_" + to_string(i));
                            vector<State::ptr> antecedentStates;
                            antecedentStates.push_back(statePointer); // StatePointer is the only antecedent state.
                            Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
                                                                                                    transitionContent,
                                                                                                    antecedentStates);
                            bfsDAG.addTransition(transition);
                        }
                    }
                }

                ///////////////////////////////////////////////////////
                //////////////////// Forget Vertex ////////////////////
                ///////////////////////////////////////////////////////
                for (auto it = bagElements.begin(); it != bagElements.end(); it++) {
                    State::ptr newStatePointer = kernel->forget_v(statePointer, *it);
                    State::ptr relabeledNewStatePointer = newStatePointer->relabel(
                            relabeledMapGenerator(newStatePointer->get_bag().get_elements()));
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
                                State::ptr relabeledNewStatePointer = newStatePointer; // The relabeling is the identity.
                                if (!allStatesSet[bagElements.size()].count(relabeledNewStatePointer) and
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
                        map<unsigned ,unsigned > m = identityMap(bagElements);
                        do{
                            State::ptr permutedState = (*it)->relabel(m);
                            State::ptr newStatePointer = kernel->join(statePointer, permutedState);
                            State::ptr relabeledNewStatePointer = newStatePointer; // The identity relabeling.
                            // for each relabeling of the bag from it, you do the join between the statePointer and the relabeled version of it.
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
                    cout << "-----------------Term Print---------------------" << endl;
                    AbstractTreeDecomposition atd;
                    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> rootNode;
                    rootNode = bfsDAG.retrieveTermAcyclicAutomaton(*it);
                    atd.setRoot(rootNode);
                    cout << "=======ABSTRACT TREE=========" << endl;
                    atd.printTermNodes();
                    atd.writeToFile(this->getPropertyFilePath());
//                    ConcreteTreeDecomposition ctd = atd.convertToConcreteTreeDecomposition();
//                    cout << "=======Concrete TREE=========" << endl;
//                    ctd.printTree();
//                    ctd.writeToFileConcreteTD(this->getPropertyFilePath());
//                    shared_ptr<DynamicKernel> sharedKernel = make_shared<DynamicKernel>(*kernel);
//                    StateTree stateTree = ctd.convertToStateTree(sharedKernel);
//                    if (flags->get("StateTree") == 1) {
//                        cout << "=======STATE TREE=========" << endl;
//                        stateTree.printStateTree();
//                    }
//                    stateTree.writeToFile(this->getPropertyFilePath());
                    cout<<"===========Run Tree============"<<endl;
                    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = extractRunTree(*it);
                    runTree.writeToFile(this->getPropertyFilePath());

                    cout << "\n ------------------Constructing Counter Example Graph-------------------" << endl;
//                    MultiGraph multiGraph = ctd.extractMultiGraph();
//                    multiGraph.printGraph();
//                    multiGraph.printToFile(this->getPropertyFilePath());
//                    multiGraph.convertToGML(this->getPropertyFilePath());
//                    multiGraph.printToFilePACEFormat(this->getPropertyFilePath());
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

map<unsigned, unsigned> RelabeledBreadthFirstSearch::relabeledMapGenerator(set<unsigned int> bagElements) {
    map<unsigned int, unsigned int> map;
    unsigned i =1;
    for(auto v:bagElements){
        map.insert(make_pair(v,i));
        i++;
    }
    return map;
}

map<unsigned int, unsigned int> RelabeledBreadthFirstSearch::identityMap(unsigned int k) {
    map<unsigned , unsigned > m;
    for(unsigned index = 1; index <= k ; index++){
        m.insert(make_pair(index,index));
    }
    return m;
}
map<unsigned int, unsigned int> RelabeledBreadthFirstSearch::identityMap(set<unsigned > bagSet) {
    map<unsigned , unsigned > m;
    for(auto item:bagSet){
        m.insert(make_pair(item,item));
    }
    return m;
}


bool RelabeledBreadthFirstSearch::nextPermutation(map<unsigned int, unsigned int> &permutation) {
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

void RelabeledBreadthFirstSearch::extractRunTreeNode(
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> wrongRunNode,
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> correctedRunNode,
        map<unsigned ,unsigned > &m) {
    string wrongSymbol = wrongRunNode->getNodeContent().getRunNodeContent().getSymbol();
    cout<<wrongSymbol<<endl;
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
        for(auto item:m){
            cout<<item.first<<"->"<<item.second<<" ,";
        }
        cout<<endl;
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        AbstractTreeDecompositionNodeContent abs;
        vector<int> numbers = abs.extractIntegerWords(wrongSymbol); // Extract the integers inside the string.
        if(numbers.size()==1){
            abs.setSymbol("IntroVertex_"+to_string(m[numbers[0]]));
        }else{
            cout<<"Error: in IntroVertex numbers is not valid."<<endl;
            exit(20);
        }
        cout<<"new symbol -----> "<<abs.getSymbol()<<endl;
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        m.erase(numbers[0]);
        cout<<"modified map ==============="<<endl;
        for(auto item:m){
            cout<<item.first<<"->"<<item.second<<" ,";
        }
        cout<<endl;
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,m);


    }else if(strstr(wrongSymbol.c_str(),"ForgetVertex")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);

        for(auto item:m){
            cout<<item.first<<"->"<<item.second<<" ,";
        }
        cout<<endl;
        unsigned smallestAvailable = 1;
        set<unsigned > usedValues;
        for(auto item:m){
            usedValues.insert(item.second);
        }
        for(auto item:usedValues){
            if(item!=smallestAvailable){
                break;
            }else{
                smallestAvailable++;
            }
        }
        AbstractTreeDecompositionNodeContent abs;
        vector<int> numbers = abs.extractIntegerWords(wrongSymbol); // Extract the integers inside the string.
        auto it = m.find(numbers[0]);
        if(it!=m.end()){
           map<unsigned ,unsigned > nm;
           for(auto item:m){
               if(item.first >= numbers[0]){
                   nm.insert(make_pair(item.first+1,item.second));
               }else{
                   nm.insert(make_pair(item.first,item.second));
               }
           }
           m = nm;
        }
        m.insert(make_pair(numbers[0],smallestAvailable));
        if(numbers.size()==1){
            abs.setSymbol("ForgetVertex_"+to_string(m[numbers[0]]));
        }else{
            cout<<"Error: in IntroVertex numbers is not valid."<<endl;
            exit(20);
        }
        cout<<"new symbol -----> "<<abs.getSymbol()<<endl;
        cout<<"modified map==============="<<endl;
        for(auto item:m){
            cout<<item.first<<"->"<<item.second<<" ,";
        }
        cout<<endl;
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,m);

    }else if(strstr(wrongSymbol.c_str(),"IntroEdge")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);

        for(auto item:m){
            cout<<item.first<<"->"<<item.second<<" ,";
        }
        cout<<endl;
        AbstractTreeDecompositionNodeContent abs;
        vector<int> numbers = abs.extractIntegerWords(wrongSymbol); // Extract the integers inside the string.
        if(numbers.size()==2){
            abs.setSymbol("IntroEdge_"+to_string(m[numbers[0]])+"_"+to_string(m[numbers[1]]));
        }else{
            cout<<"Error: in IntroEdge numbers is not valid."<<endl;
            exit(20);
        }
        cout<<"new symbol -----> "<<abs.getSymbol()<<endl;
        cout<<"modified map==============="<<endl;
        for(auto item:m){
            cout<<item.first<<"->"<<item.second<<" ,";
        }
        cout<<endl;
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,m);
    }else if(strstr(wrongSymbol.c_str(),"Join")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("Join");
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        // Child 1
        map<unsigned ,unsigned > m1 = m;
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child1(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child1->setParent(correctedRunNode);
        children.push_back(child1);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child1,m1);
        // Child 2
        map<unsigned ,unsigned > m2 = identityMap(correctedState->get_bag().get_elements());
        // We need to discover a map that is compatible with the join.
        State::ptr joinState;
        bool flag = false;
        do {
            map<unsigned ,unsigned > compositionMap; // Changes the domain of the child
            for(auto item:m){
                compositionMap.insert(make_pair(item.first,m2[item.second]));
            }
            State::ptr state = wrongRunNode->getChildren()[1]->getNodeContent().getState()->relabel(compositionMap);
            joinState = kernel->join(child1->getNodeContent().getState(),state);
            if(joinState==correctedState){
                flag = true;
                break;
            }
        } while (nextPermutation(m2));
        if(flag){
            shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                    child2(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
            child2->setParent(correctedRunNode);
            children.push_back(child2);
            extractRunTreeNode(wrongRunNode->getChildren()[0],child2,m2);
        }else{
            cout<<"Error: join is not valid.";
            exit(20);
        }
        correctedRunNode->setChildren(children);
    }else{
        cout<<"Error: Node Type is not valid."<<endl;
        exit(20);
    }
}

RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
RelabeledBreadthFirstSearch::extractRunTree(State::ptr state) {
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> wrongRunTree = bfsDAG.retrieveRunAcyclicAutomaton(state);
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunTree;
    shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>>
            correctedRoot(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
    map<unsigned,unsigned> m = identityMap(state->get_bag().get_elements());
    extractRunTreeNode(wrongRunTree.getRoot(),correctedRoot,m);
    correctedRunTree.setRoot(correctedRoot);
    return correctedRunTree;
}




