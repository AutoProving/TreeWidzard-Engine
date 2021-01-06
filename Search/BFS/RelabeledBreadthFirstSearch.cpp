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
	TreeAutomaton<State::ptr,AbstractTreeDecompositionNodeContent> bfsDAG; // Constructs a DAG corresponding to the BFS.
	State::ptr initialState = kernel->initialState();
	allStatesSet.insert(initialState); //TODO define InitialSearchState
	newStatesSet.insert(initialState);
    bfsDAG.addState(initialState);
    AbstractTreeDecompositionNodeContent initialTransitionContent("Leaf");
    vector<State::ptr> initialAntecedents; // Empty vector since there are no children.
    Transition<State::ptr,AbstractTreeDecompositionNodeContent> initialTransition(initialState,initialTransitionContent,initialAntecedents);
	bfsDAG.addTransition(initialTransition);
    unsigned int width = kernel->get_width().get_value();
	int iterationNumber = 0;
	while(!newStatesSet.empty()){
	    iterationNumber++;
		newStatesVector.clear();
		newStatesVector.resize(newStatesSet.size());
		std::copy(newStatesSet.begin(),newStatesSet.end(),newStatesVector.begin());
		newStatesSet.clear();
		//This loop is suitable for parallelization
		for(int l=0;l<newStatesVector.size();l++){
			State::ptr statePointer = newStatesVector[l];
			Bag bag = statePointer->get_bag();
			set<unsigned > bagElement  = bag.get_elements();
			///////////////////////////////////////////////////////
			//////////////////// Introduce Vertex /////////////////
			///////////////////////////////////////////////////////
			// the +1 below comes from the fact that treewidth is 
			// size of the bag minus one. So the loop iterates 
			// from 1 to number of elements inteh bag.
			for (int i=1; i<= width+1; i++){
                if(bag.vertex_introducible(i)){
                    State::ptr newStatePointer = kernel->intro_v(statePointer, i);
                    State::ptr relabeledNewStatePointer= newStatePointer->relabel(relabeledMapGenerator(newStatePointer->get_bag().get_elements()));
                    if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer) ){
                        newStatesSet.insert(relabeledNewStatePointer);
                        State::ptr consequentState = relabeledNewStatePointer;
                        bfsDAG.addState(consequentState);
                        AbstractTreeDecompositionNodeContent transitionContent("IntroVertex_"+ to_string(i));
                        vector<State::ptr> antecedentStates;
                        antecedentStates.push_back(statePointer);
                        Transition<State::ptr,AbstractTreeDecompositionNodeContent> transition(consequentState,transitionContent,antecedentStates);
                        bfsDAG.addTransition(transition);
                    }
                }
			}
			///////////////////////////////////////////////////////
			//////////////////// Forget Vertex //////////////////// 
			///////////////////////////////////////////////////////
			for (auto it= bagElement.begin(); it!=bagElement.end(); it++){
                State::ptr newStatePointer = kernel->forget_v(statePointer, *it);
                State::ptr relabeledNewStatePointer= newStatePointer->relabel(relabeledMapGenerator(newStatePointer->get_bag().get_elements()));
                if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer)){
                    newStatesSet.insert(relabeledNewStatePointer);
                    State::ptr consequentState = relabeledNewStatePointer;
                    AbstractTreeDecompositionNodeContent transitionContent("ForgetVertex_"+ to_string(*it));
                    bfsDAG.addState(consequentState);
                    vector<State::ptr> antecedentStates;
                    antecedentStates.push_back(statePointer);
                    Transition<State::ptr,AbstractTreeDecompositionNodeContent> transition(consequentState,transitionContent,antecedentStates);
                    bfsDAG.addTransition(transition);
                }
			}
            //Introduce Edge
			if(bag.get_elements().size()>1){
                for (auto it=bagElement.begin(); it!=bagElement.end(); it++){
                    auto itX = it;
                    itX++; // TODO write this more elegantly
                    if(itX!=bagElement.end()){
                        for (auto itPrime = itX ; itPrime != bagElement.end(); itPrime++){
                            State::ptr newStatePointer = kernel->intro_e(statePointer, *it, *itPrime);
                            State::ptr relabeledNewStatePointer= newStatePointer->relabel(relabeledMapGenerator(newStatePointer->get_bag().get_elements()));
                            if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer)){
                                newStatesSet.insert(relabeledNewStatePointer);
                                State::ptr consequentState = relabeledNewStatePointer;
                                AbstractTreeDecompositionNodeContent transitionContent("IntroEdge_"+ to_string(*it)+"_"+to_string(*itPrime));
                                bfsDAG.addState(consequentState);
                                vector<State::ptr> antecedentStates;
                                antecedentStates.push_back(statePointer);
                                Transition<State::ptr,AbstractTreeDecompositionNodeContent> transition(consequentState,transitionContent,antecedentStates);
                                bfsDAG.addTransition(transition);
                            }
                        }
                    }
                }
			}

            // join
            if (kernel->get_width().get_name() == "tree_width") {
                // join
                for (auto it = allStatesSet.begin(); it != allStatesSet.end(); it++) {
                    if (statePointer->get_bag().joinable((*it)->get_bag())) {
                        State::ptr newStatePointer = kernel->join(statePointer, *it);
                        State::ptr relabeledNewStatePointer= newStatePointer->relabel(relabeledMapGenerator(newStatePointer->get_bag().get_elements()));
                        if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer)) {
                            newStatesSet.insert(relabeledNewStatePointer);
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
                    }
                }
                for (auto it = newStatesSet.begin(); it != newStatesSet.end(); it++) {
                    if (statePointer->get_bag().joinable((*it)->get_bag())) {
                        State::ptr newStatePointer = kernel->join(statePointer, *it);
                        State::ptr relabeledNewStatePointer= newStatePointer->relabel(relabeledMapGenerator(newStatePointer->get_bag().get_elements()));
                        if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer)) {
                            newStatesSet.insert(relabeledNewStatePointer);
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
                    }
                }
            }
		}
		for(auto it = newStatesSet.begin(); it!=newStatesSet.end(); it++){
		    if(!conjecture->evaluateConjectureOnState(**it,kernel)){
                cout<<"BAD STATE:"<<endl;
		        (**it).print();
		        bfsDAG.addFinalState(*it);
		        cout<<"-----------------Term Print---------------------"<<endl;
		        AbstractTreeDecomposition atd;
		        shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> rootNode;
		        rootNode = bfsDAG.retrieveTermAcyclicAutomaton(*it);
		        atd.setRoot(rootNode);
		        atd.printTermNodes();
		        ConcreteTreeDecomposition ctd = atd.convertToConcreteTreeDecomposition();
                shared_ptr<DynamicKernel> sharedKernel = make_shared<DynamicKernel>(*kernel);
                ctd.convertToStateTree(sharedKernel).printStateTree();
		        ctd.extractMultiGraph().printGraph();
		        exit(20);
		    }
		}
        set<State::ptr> setUnion;
		set_union(allStatesSet.begin(),allStatesSet.end(),newStatesSet.begin(),newStatesSet.end(),inserter(setUnion,setUnion.begin()));
		allStatesSet = setUnion;
		setUnion.clear();
        if(flags->get("LoopTime") == 1){
            cout<<"AllState:"<<allStatesSet.size()<<" new State: "<<newStatesSet.size()<<endl;
            cout << endl << "----------------- End Iteration: " << iterationNumber << " ----------------------------" << endl << endl;
        }
	}
    cout<<"Finish"<<endl;
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