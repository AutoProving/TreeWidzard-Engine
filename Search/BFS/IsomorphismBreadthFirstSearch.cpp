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
                    ConcreteTreeDecomposition ctd = atd.convertToConcreteTreeDecomposition();
                    cout << "=======Concrete TREE=========" << endl;
                    ctd.printTree();
                    ctd.writeToFileConcreteTD(this->getPropertyFilePath());
                    shared_ptr<DynamicKernel> sharedKernel = make_shared<DynamicKernel>(*kernel);
                    StateTree stateTree = ctd.convertToStateTree(sharedKernel);
                    if (flags->get("StateTree") == 1) {
                        cout << "=======STATE TREE=========" << endl;
                        stateTree.printStateTree();
                    }
                    stateTree.writeToFile(this->getPropertyFilePath());
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

map<unsigned int, unsigned int> IsomorphismBreadthFirstSearch::initialPermutation(unsigned int k) {
    map<unsigned , unsigned > m;
    for(unsigned index = 1; index <= k ; index++){
        m.insert(make_pair(index,index));
    }
    return m;
}

bool IsomorphismBreadthFirstSearch::nextPermutation(map<unsigned int, unsigned int> &m) {
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

void IsomorphismBreadthFirstSearch::testPermutationGeneration(unsigned k) {
    map<unsigned ,unsigned > m = initialPermutation(k);
    do{
        for(auto item:m){
            cout<<item.first<<"->"<< item.second<<"  ";
        }
        cout<<endl;
    } while (nextPermutation(m));
}

State::ptr IsomorphismBreadthFirstSearch::canonicalState(State::ptr state) {
   // Assume that the bag.elements is equal to {1,...,k} for some k.
    State::ptr canonicalState = state;
    map<unsigned ,unsigned > m = initialPermutation(state->get_bag().get_elements().size());
    do{
        State::ptr relabeledState = state->relabel(m);
        if(relabeledState < canonicalState){
            canonicalState = relabeledState;
        }
    }while(nextPermutation(m));
    return canonicalState;
}
