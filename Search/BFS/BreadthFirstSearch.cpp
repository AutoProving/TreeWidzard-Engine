// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "BreadthFirstSearch.h"

extern "C" {
    BreadthFirstSearch * create() {
        return new BreadthFirstSearch();
    }
    BreadthFirstSearch * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
        return new BreadthFirstSearch(dynamicKernel,conjecture,flags);
    }
}

BreadthFirstSearch::BreadthFirstSearch() {
    addAttribute("SearchName","BreadthFirstSearch");
}

BreadthFirstSearch::BreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags): SearchStrategy(dynamicKernel, conjecture, flags) {
    this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
    addAttribute("SearchName","BreadthFirstSearch");
}

void BreadthFirstSearch::search(){
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
                    if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer) ){
                        newStatesSet.insert(newStatePointer);
                        State::ptr consequentState = newStatePointer;
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
                if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer)){
                    newStatesSet.insert(newStatePointer);
                    State::ptr consequentState = newStatePointer;
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
                            if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer)){
                                newStatesSet.insert(newStatePointer);
                                State::ptr consequentState = newStatePointer;
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
		}
		for(auto it = newStatesSet.begin(); it!=newStatesSet.end(); it++){
		    if(!conjecture->evaluateConjectureOnState(**it,kernel)){
		        bfsDAG.addFinalState(*it);
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
