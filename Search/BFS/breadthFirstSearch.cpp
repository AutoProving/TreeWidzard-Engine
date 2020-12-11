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

bool check_lessThan_states_BreadthFirstSearch(const State::ptr &lhs,
											const State::ptr &rhs) {
	return *lhs < *rhs;
}

bool check_equality_states_BreadthFirstSearch(const State::ptr &lhs,
											const State::ptr &rhs) {
	return (*lhs == *rhs);
}

BreadthFirstSearch::BreadthFirstSearch() {
    addAttribute("SearchName","BreadthFirstSearch");
}

BreadthFirstSearch::BreadthFirstSearch(DynamicKernel *dynamicKernel,
								   Conjecture *conjecture, Flags *flags)
	: SearchStrategy(dynamicKernel, conjecture, flags) {
	this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
    addAttribute("SearchName","BreadthFirstSearch");

}

pair<bool, AbstractTreeDecomposition> BreadthFirstSearch::search(){
	SearchState initialSearchState(0,dynamicKernel->initialState); // 0 is the number corresponding to the initial decomposition state (\empty,\empty,\empty) while dynamicKernel->initialSearchState is the initial kernel state
	TreeAutomaton bfsDAG; // Constructs a DAG corresponding to the BFS. 
	allStatesSet.insert(initialSearchState); //TODO define InitialSearchState
	newStatesSet.insert(initialSearchState);  
	unsigned int width = kernel->get_width().get_value(); 
	while(newStateVector.size()>0){
		newStatesVector.clear();
		std::copy(newStatesSet.begin(),newStatesSet.end(),newStatesVector.begin());
		newStatesSet.clear();
		//This loop is suitable for parallelization
		for(int l=0;l<newStatesVector.size();l++){
			ATDState decState(newStatesVector[l].decStateNum); 
			///////////////////////////////////////////////////////
			//////////////////// Introduce Vertex /////////////////
			///////////////////////////////////////////////////////
			// the +1 below comes from the fact that treewidth is 
			// size of the bag minus one. So the loop iterates 
			// from 1 to number of elements inteh bag.
			for (int i=1; i<=dynamicKernel.width+1; i++){
				if (decState.bagset.find(i)== decState.bagset.end()){
					ATDState decStatePrime(decState.bagset.insert(i),decState.bagedge,decState.setEdges);
				        State::ptr newStatePointer = kernel->intro_v(newStatesVector[l], i); 
					SearchState::ptr newSearchStatePointer = SearchState(decStatePrime.convertToNumber(),newStatePointer); // TODO fix this line 
					intermediateStatesSet.insert(newSearchStatePointer); // TODO define comparator
				}
			}
			///////////////////////////////////////////////////////
			//////////////////// Forget Vertex //////////////////// 
			///////////////////////////////////////////////////////
			for (auto it=decState.bagset.begin(); it!=decState.bagset.end(); it++){
				set<int> newBagSet = decState.bagset.erase(i);
				pair<int,int> newBagEdge(0,0); 
				set<pair<int,int> > newSetEdges; 
				for (auto itedge = decState.setEdges.begin(); itedge != decState.setEdges.end(); itedge++){
					if (itedge->first != *it and itedge->second != *it){
						newSetEdges.insert(*itedge);
					}		
				}
				ATDState decStatePrime(newBagSet,newBagEdge,newSetEdges);
 			        State::ptr newStatePointer = kernel->forget_v(newStatesVector[l], i); 
				SearchState::ptr newSearchStatePointer = SearchState(decStatePrime.convertToNumber(),newStatePointer); // TODO fix this line 
				intermediateStatesSet.insert(newSearchStatePointer); // TODO define comparator
			}  
			//Introduce Edge
			for (auto it=decState.bagset.begin(); it!=decState.bagset.end(); it++){
				auto itX = it; itX++; // TODO write this more elegantly 
				for (auto itPrime=itX; itPrime!=decState.bagset.end(); itPrime++){
					pair<int,int> newBagEdge(*it,*itPrime); 
					set<pair<int,int> > newSetEdges = decState.setEdges.insert(newBagEdge); 
					ATDState decStatePrime(decState.bagset,newBagEdge,newSetEdges);
 				        State::ptr newStatePointer = kernel->intro_e(newStatesVector[l], *it, *itPrime); 
					SearchState::ptr newSearchStatePointer = SearchState(decStatePrime.convertToNumber(),newStatePointer); // TODO fix this line 
					intermediateStatesSet.insert(newSearchStatePointer); // TODO define comparator
				}
			}
			//Join
			shared_ptr startIt; // first position with a search state whose decomposition state has decState.bagset as the first coordinate
			shared_ptr endIt; // last position with a search state whose decomposition state has decState.bagset as the first coordinate
			if (endIt!= newStatesSet.end()) endIt++; 
			for (auto it = startIt; it!= endIt){
				set<pair<int,int> > newSetEdges = decState.setEdges; 
				ATDState decStateAux(it.decState.convertToNumber()); 
				for (auto itedge = decStateAux.setEdges.begin(); itedge!= decStateAux.setEdges.end(); itedge++){
					newSetEdges.insert(*itedge); 
				}	
				if (newSetEdges.size() == decState.setEdges.size() + decStateAux.setEdges.size()){ 
					// in this case there are no duplicates. 
					pair<int,int> newBagEdge(0,0); 
					ATDState decStatePrime(decStatePrime.bagset,newBagEdge,newSetEdges);
 					State::ptr newStatePointer = kernel->intro_e(newStatesVector[l], *it, *itPrime); 
					SearchState::ptr newSearchStatePointer = SearchState(decStatePrime.convertToNumber(),newStatePointer); // TODO fix this line 
					intermediateStatesSet.insert(newSearchStatePointer); // TODO define comparator
				}
			}
		}
		set_difference(intermediateStatesSet.begin(),intermediateStatesSet.end(),allStatesSet.begin(),allStatesSet.end(),newStatesSet,newStatesSet.end()); //TODO shoudl this be newStatesSet.begin() or .end()	
		set_union() // Make the union of newStates with all states
	}
	return std::make_pair(true, T);
}
