// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef CURRENT_BREADTHFIRSTSEARCH_H
#define CURRENT_BREADTHFIRSTSEARCH_H
#include <math.h>
#include "../Kernel/ConcreteTreeDecomposition.h"
#include "../Kernel/SearchStrategy.h"
#include "../Kernel/StateTree.h"
#include "../TreeAutomaton/TreeAutomaton.h"

//These are states used to define what is a 
//valid decomposition
class DecompositionState{

	//TODO: DEFINE ALL COMPARATORS, AND PRINT FUNCTION AS VIRTUAL. ATD implements these menthods

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
	DecompositionState decState;
	State kernelState;
        DecompositionState getDecompositionState();
	void setDecompositionState();
	State getKernelState(); 
	void setKernelState(); 
	void setSearchState(); 
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





using namespace std;
using namespace std::chrono;

class BreadthFirstSearch : public SearchStrategy {
  private:
	TreeAutomaton bfsDAG; // Constructs a DAG corresponding to the BFS. 
	set<searchState> allStatesSet;
        set<searchState> intermediateStatesSet; 
	set<searchState> newStatesSet; 
	vector<searchState> newStatesVector; // This will allow us to use parallel search 
  public:
    	BreadthFirstSearch();
	BreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags);
	~BreadthFirstSearch(){};
	Term<AbstractTreeDecompositionSymbol> extractCounterExampleTerm(); 	
	//Term<pair<AbstractTreeDecompositionSymbol,SearchState>> extractCounterExampleStateTree(); fix this part. Create a class for these pairs.
};

#endif // CURRENT_BREADTHFIRSTSEARCH_H
