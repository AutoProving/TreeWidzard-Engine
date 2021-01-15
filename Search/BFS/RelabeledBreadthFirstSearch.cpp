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
                        map<unsigned ,unsigned > m = initialPermutation(index);
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
                    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = convertRelabeledRunTree(*it);
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

map<unsigned int, unsigned int> RelabeledBreadthFirstSearch::initialPermutation(unsigned int k) {
    map<unsigned , unsigned > m;
    for(unsigned index = 1; index <= k ; index++){
        m.insert(make_pair(index,index));
    }
    return m;
}

bool RelabeledBreadthFirstSearch::nextPermutation(map<unsigned int, unsigned int> &m) {
    vector<unsigned > vec;
    vec.resize(m.size());
    for(auto item:m){
        vec[item.first-1] = item.second;
    }
    if(!next_permutation(vec.begin(),vec.end())){
        return false;
    }else{
        unsigned index = 1;
        for(auto& item:m){
            item.second = vec[index-1];
            index++;
        }
        return true;
    }
}

void RelabeledBreadthFirstSearch::testPermutationGeneration(unsigned k) {
    map<unsigned ,unsigned > m = initialPermutation(k);
    do{
        for(auto item:m){
            cout<<item.first<<"->"<< item.second<<"  ";
        }
        cout<<endl;
    } while (nextPermutation(m));
}

void RelabeledBreadthFirstSearch::convertRelabeledRunNode(
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> relabeledNode,
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> node, vector<unsigned > &v) {
    cout<<"enter"<<endl;
    set<unsigned > bagElements = relabeledNode->getNodeContent().getState()->get_bag().get_elements();
    vector<unsigned > nv;
    copy(v.begin(),v.end(),back_inserter(nv));
    cout<<"after copy vec"<<endl;
    if(relabeledNode->getParent()== nullptr){
        for(auto item:bagElements){
            nv.push_back(item);
        }
    }else{
        string parentSymbol = relabeledNode->getParent()->getNodeContent().getRunNodeContent().getSymbol();
        if(strstr(parentSymbol.c_str(),"IntroVertex_")){
            nv.pop_back();
        }else if(strstr(parentSymbol.c_str(),"IntroEdge_")){
           // nv = v
        }else if(strstr(parentSymbol.c_str(),"ForgetVertex_")){
            vector<int > numbers = relabeledNode->getParent()->getNodeContent().getRunNodeContent().extractIntegerWords(parentSymbol);
            if(numbers.size()==1){
                unsigned i = numbers[0];
                nv.insert(nv.begin()+i,v.size()+1);
            }else{
                cout<<"Error: RelabeledBreadthFirstSearch::convertRelabeledRunNode extracted numbers are not valid"<<endl;
                exit(20);
            }
        }else if(strstr(parentSymbol.c_str(),"Join")){
            // nv = v
        }else{
            cout<<"Error: RelabeledBreadthFirstSearch::convertRelabeledRunNode node type error!"<<endl;
            exit(20);
        }
    }

    vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent >>>> children;
    for(auto& item:relabeledNode->getChildren()){
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>> child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>> );
        child->setParent(node);
        convertRelabeledRunNode(item, child, nv);
        children.push_back(child);
    }
    ///////
    node->setChildren(children);
    string relabeledSymbol = relabeledNode->getNodeContent().getRunNodeContent().getSymbol();
    string symbol;
    vector<int > numbers = relabeledNode->getNodeContent().getRunNodeContent().extractIntegerWords(relabeledSymbol);
    if(strstr(relabeledSymbol.c_str(),"IntroVertex_")){
        if(numbers.size()==1){
            symbol = "IntroVertex_"+to_string(nv[numbers[0]-1]);
        }else{

        }
    }else if(strstr(relabeledSymbol.c_str(),"IntroEdge_")){
        if(numbers.size()==2){
            symbol = "IntroEdge_"+to_string(nv[numbers[0]-1])+"_"+to_string(nv[numbers[1]-1]);
        }else{

        }

    }else if(strstr(relabeledSymbol.c_str(),"ForgetVertex_")){
        if(numbers.size()==1){
            symbol = "ForgetVertex_"+to_string(nv[numbers[0]-1]);
        }else{

        }
    }else if(strstr(relabeledSymbol.c_str(),"Join")){
        if(numbers.size()==0){
            symbol = "Join";
        }else{

        }
    }else if(strstr(relabeledSymbol.c_str(),"Leaf")){
        if(numbers.size()==0){
            symbol = "Leaf";
        }else{

        }
    }else{

    }
    map<unsigned ,unsigned > m;
    for(unsigned indx = 0 ; indx < nv.size(); indx++){
        m.insert(make_pair(indx+1,nv[indx]));
    }
    State::ptr originalState = relabeledNode->getNodeContent().getState()->relabel(m);
    AbstractTreeDecompositionNodeContent absNode(symbol);
    RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> runNodeContent(absNode,originalState);
    node->setNodeContent(runNodeContent);
}

RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
RelabeledBreadthFirstSearch::convertRelabeledRunTree(State::ptr state) {
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = bfsDAG.retrieveRunAcyclicAutomaton(state);
    shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>> originalRoot(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
    vector<unsigned > v;
//    set<unsigned > bagElements =  root->getNodeContent().getState()->get_bag().get_elements();
//    for(auto item:bagElements){
//        v.push_back(item);
//    }
    convertRelabeledRunNode(runTree.getRoot(),originalRoot,v);
    cout<<"after coدversion"<<endl;
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> originalRunTree;
    originalRunTree.setRoot(originalRoot);
    return originalRunTree;
}

