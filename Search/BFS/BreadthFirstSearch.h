// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CURRENT_BREADTHFIRSTSEARCH_H
#define CURRENT_BREADTHFIRSTSEARCH_H
#include <math.h>
#include "../../Kernel/ConcreteTreeDecomposition.h"
#include "../../Kernel/SearchStrategy.h"
#include "../../Kernel/StateTree.h"
#include "../../TreeAutomaton/TreeAutomaton.h"
#include "../../TreeAutomaton/AbstractTreeDecomposition.h"

/*
//These are states used to define what is a 
//valid decomposition
class DecompositionState{
	//TODO: DEFINE ALL COMPARATORS, AND PRINT FUNCTION AS VIRTUAL. ATD implements these menthods
	unsigned int convertToNumber(); //converts this DecompsitionState into an integer
	DecompositionState convertFromNumber(unsigned int decompositionStateNumber); // converts a unsigned int representing a DecompositionState into this DecompositionState
};


//Abstract Tree Decomposition States
class ATDState: public DecompositionState{
	set<int> bagset; // Current bag
	pair<int,int> bagedge; // Edge at current bag
	set<pair<int,int>> pastEdges; // All edges that have been added to the bag (including bagedge)
	void getBagSet();
	void setBagSet(set<int> bagset);
	void getBagEdge();
	void setBagEdge(set<int> bagedge);
	void getPastEdges();
	void setPastEdges(set<pair<int,int> > pastEdges);
	void addPastEdge(int i, int j); 

	void printDecompositionState(); 
	bool operator<(const ATDState& rhs) const;
	bool operator==(const ATDState& rhs) const;
	class ptr {
	  private:
		shared_ptr<ADTState> pointer;
	  public:
		ptr() { pointer = make_shared<ADTState>(); }
		ptr(shared_ptr<ADTState> pointer_) : pointer(pointer_) {}
		ADTState &operator*() const { return *pointer; }
		ADTState *operator->() const { return &*pointer; }
		bool operator<(const ptr &rhs) const { return **this < *rhs; }
	};
	ptr get_ptr() { return ptr(this->shared_from_this()); }
};

//SearchState
class SearchState: public DecompositionState{
	int decStateNum; // numerical representation of a decomposition state
	State::ptr kernelState;
	void setDecStateNum(int i);
	void setKernelState(shared_ptr<State>)
	int getDecStateNum()
	shared_ptr<State> getKernelState(); 
	void setDecStateNum(); 
	void setSearchState(); 
	
        DecompositionState getDecompositionState();
	
	bool operator<(const SearchState& rhs) const;
	bool operator==(const SearchState& rhs) const;
	class ptr {
	  private:
		shared_ptr<SearchState> pointer;
	  public:
		ptr() { pointer = make_shared<SearchState>(); }
		ptr(shared_ptr<SearchState> pointer_) : pointer(pointer_) {}
		SearchState &operator*() const { return *pointer; }
		SearchState *operator->() const { return &*pointer; }
		bool operator<(const ptr &rhs) const { return **this < *rhs; }
	};
	ptr get_ptr() { return ptr(this->shared_from_this()); }
};
*/




using namespace std;
using namespace std::chrono;

class BreadthFirstSearch : public SearchStrategy {
  private:
	TreeAutomaton<State,AbstractTreeDecompositionNodeContent> bfsDAG; // Constructs a DAG corresponding to the BFS.
	set<State::ptr> allStatesSet;
	set<State::ptr> intermediateStatesSet;
	set<State::ptr>  newStatesSet; 
	vector<State::ptr> newStatesVector; // This will make it easier to do parallel search 
  public:
    BreadthFirstSearch();
	BreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
	~BreadthFirstSearch(){};
AbstractTreeDecomposition extractCounterExampleTerm(State::ptr state, TreeAutomaton<State::ptr,AbstractTreeDecompositionNodeContent>)
{
	//Convert a run into a AbstractTreeDecomposition (forget the states)

}
Term<RunNodeContent<State,AbstractTreeDecompositionNodeContent> > extractCounterExampleRun(State::ptr state, TreeAutomaton<State::ptr,AbstractTreeDecompositionNodeContent>);
    void search();
};

#endif // CURRENT_BREADTHFIRSTSEARCH_H
