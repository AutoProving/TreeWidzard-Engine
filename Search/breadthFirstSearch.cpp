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

unsigned BreadthFirstSearch::bagSetToNumber(set<unsigned> bagSet,
										  unsigned width) {
	unsigned number = 0;
	for (unsigned i = 1; i <= width + 1; i++) {
		if (bagSet.find(i) != bagSet.end()) {
			number = number + pow(2, i - 1);
		}
	}
	return number;
}
shared_ptr<CTDNodeNew> BreadthFirstSearch::extractCTDNode(
	unsigned level, State::ptr s,
	vector<vector<set<State::ptr> > > &leveledSetAllStates) {
	unsigned w = this->kernel->get_width().get_value();
	shared_ptr<CTDNodeNew> node(new CTDNodeNew());
	node->set_B(s->get_bag());
	node->set_parent(nullptr);
	if (level == 0) {
		// nodeType is already "Empty" by default
		// children is also empty by Default
		return node;
	} else {
		set<unsigned> bagElements = s->get_bag().get_elements();
		int bagNumber =
			bagSetToNumber(bagElements, kernel->get_width().get_value());
		// IntroVertex
		for (auto element = bagElements.begin(); element != bagElements.end();
			 element++) {
			set<unsigned> bagElementsPrime = bagElements;
			bagElementsPrime.erase(*element);
			unsigned bagNumberPrime = bagSetToNumber(bagElementsPrime, w);
			for (auto it =
					 leveledSetAllStates[level - 1][bagNumberPrime].begin();
				 it != leveledSetAllStates[level - 1][bagNumberPrime].end();
				 it++) {
				State::ptr sprime = *it;
				if (*s == *(kernel->intro_v(sprime, *element))) {
					// this means that s is obtained from sprime by introudcing
					// vertex *element
					shared_ptr<CTDNodeNew> child =
						extractCTDNode(level - 1, sprime, leveledSetAllStates);
					child->set_parent(node);
					vector<shared_ptr<CTDNodeNew> > children;
					children.push_back(child);
					node->set_children(children);
					node->set_nodeType("IntroVertex_" + to_string(*element));
					return node;
				}
			}
		}

		// IntroEdge
		pair<unsigned, unsigned> edge = s->get_bag().get_edge();
		if ((edge.first != 0) and (edge.second != 0)) {
			for (auto it = leveledSetAllStates[level - 1][bagNumber].begin();
				 it != leveledSetAllStates[level - 1][bagNumber].end(); it++) {
				State::ptr sprime = *it;
				if (*s == *kernel->intro_e(sprime, edge.first, edge.second)) {
					// this means that s is obained from sprime by introudcing
					// vertex *element
					shared_ptr<CTDNodeNew> child =
						extractCTDNode(level - 1, sprime, leveledSetAllStates);
					child->set_parent(node);
					vector<shared_ptr<CTDNodeNew> > children;
					children.push_back(child);
					node->set_children(children);
					node->set_nodeType("IntroEdge_" + to_string(edge.first) +
									   "_" + to_string(edge.second));
					return node;
				}
			}
		}

		// ForgetVertex
		set<unsigned> allElements;
		for (size_t x = 1; x <= kernel->get_width().get_value(); x++) {
			allElements.insert(x);
		}
		set<unsigned> remainingElements;
		set_difference(allElements.begin(), allElements.end(),
					   bagElements.begin(), bagElements.end(),
					   inserter(remainingElements, remainingElements.begin()));
		for (auto element = remainingElements.begin();
			 element != remainingElements.end(); element++) {
			set<unsigned> bagElementsPrime = bagElements;
			bagElementsPrime.insert(*element);
			unsigned bagNumberPrime = bagSetToNumber(
				bagElementsPrime, kernel->get_width().get_value());
			for (auto it =
					 leveledSetAllStates[level - 1][bagNumberPrime].begin();
				 it != leveledSetAllStates[level - 1][bagNumberPrime].end();
				 it++) {
				State::ptr sprime = *it;
				if (*s == *kernel->forget_v(sprime, *element)) {
					// this means that s is obained from sprime by introudcing
					// vertex *element
					shared_ptr<CTDNodeNew> child =
						extractCTDNode(level - 1, sprime, leveledSetAllStates);
					child->set_parent(node);
					vector<shared_ptr<CTDNodeNew> > children;
					children.push_back(child);
					node->set_children(children);
					node->set_nodeType("ForgetVertex_" + to_string(*element));
					return node;
				}
			}
		}
		// Join
		for (auto it1 = leveledSetAllStates[level - 1][bagNumber].begin();
			 it1 != leveledSetAllStates[level - 1][bagNumber].end(); it1++) {
			State::ptr sprime1 = *it1;
			for (unsigned j = level - 1; j >= 0; j--) {
				for (auto it2 = leveledSetAllStates[j][bagNumber].begin();
					 it2 != leveledSetAllStates[j][bagNumber].end(); it2++) {
					State::ptr sprime2 = *it2;
					if (*s == *kernel->join(sprime1, sprime2)) {
						// this means that s is obained from sprime by
						// introudcing vertex *element
						shared_ptr<CTDNodeNew> child1 = extractCTDNode(
							level - 1, sprime1, leveledSetAllStates);
						shared_ptr<CTDNodeNew> child2 =
							extractCTDNode(j, sprime2, leveledSetAllStates);
						child1->set_parent(node);
						child2->set_parent(node);
						vector<shared_ptr<CTDNodeNew> > children;
						children.push_back(child1);
						children.push_back(child2);
						node->set_children(children);
						node->set_nodeType("Join");
						return node;
					}
				}
			}
		}
	}
}

ConcreteTreeDecomposition BreadthFirstSearch::extractCTDDecomposition(
	unsigned level, State::ptr s,
	vector<vector<set<State::ptr> > > &leveledSetAllStates) {
	ConcreteTreeDecomposition T;
	T.root = extractCTDNode(level, s, leveledSetAllStates);
	return T;
}

shared_ptr<StateTreeNode> BreadthFirstSearch::extractStateTreeNode(
	unsigned level, State::ptr s,
	vector<vector<set<State::ptr> > > &leveledSetAllStates, bool tree_width) {
	unsigned w = this->kernel->get_width().get_value();
	shared_ptr<StateTreeNode> node(new StateTreeNode());
	node->set_S(s);
	shared_ptr<DynamicKernel> spkernel(kernel);
	node->set_kernel(spkernel);
	node->set_parent(nullptr);
	if (level == 0) {
		// nodeType is already "Empty" by default
		// children is also empty by Default
		return node;
	} else {
		set<unsigned> bagElements = s->get_bag().get_elements();
		unsigned bagNumber = bagSetToNumber(bagElements, w);
		// IntroVertex
		for (auto element = bagElements.begin(); element != bagElements.end();
			 element++) {
			set<unsigned> bagElementsPrime = bagElements;
			bagElementsPrime.erase(*element);
			unsigned bagNumberPrime = bagSetToNumber(bagElementsPrime, w);
			for (auto it =
					 leveledSetAllStates[level - 1][bagNumberPrime].begin();
				 it != leveledSetAllStates[level - 1][bagNumberPrime].end();
				 it++) {
				State::ptr sprime = *it;

				State::ptr temp = kernel->intro_v(sprime, *element);
				if (*s == *temp) {
					// this means that s is obtained from sprime by introudcing
					// vertex *element
					shared_ptr<StateTreeNode> child = extractStateTreeNode(
						level - 1, sprime, leveledSetAllStates, tree_width);
					child->set_parent(node);
					vector<shared_ptr<StateTreeNode> > children;
					children.push_back(child);
					node->set_children(children);
					node->set_nodeType("IntroVertex_" + to_string(*element));
					return node;
				}
			}
		}

		// IntroEdge
		pair<int, int> edge = s->get_bag().get_edge();
		if ((edge.first != 0) and (edge.second != 0)) {
			for (auto it = leveledSetAllStates[level - 1][bagNumber].begin();
				 it != leveledSetAllStates[level - 1][bagNumber].end(); it++) {
				State::ptr sprime = *it;
				State::ptr temp =
					kernel->intro_e(sprime, edge.first, edge.second);
				if (*s == *temp) {
					// this means that s is obained from sprime by introudcing
					// vertex *element
					shared_ptr<StateTreeNode> child = extractStateTreeNode(
						level - 1, sprime, leveledSetAllStates, tree_width);
					child->set_parent(node);
					vector<shared_ptr<StateTreeNode> > children;
					children.push_back(child);
					node->set_children(children);
					node->set_nodeType("IntroEdge_" + to_string(edge.first) +
									   "_" + to_string(edge.second));
					return node;
				}
			}
		}

		// ForgetVertex
		set<unsigned> allElements;
		for (size_t x = 1; x <= kernel->get_width().get_value(); x++) {
			allElements.insert(x);
		}
		set<unsigned> remainingElements;
		set_difference(allElements.begin(), allElements.end(),
					   bagElements.begin(), bagElements.end(),
					   inserter(remainingElements, remainingElements.begin()));
		for (auto element = remainingElements.begin();
			 element != remainingElements.end(); element++) {
			set<unsigned> bagElementsPrime = bagElements;
			bagElementsPrime.insert(*element);
			unsigned bagNumberPrime = bagSetToNumber(
				bagElementsPrime, kernel->get_width().get_value());
			for (auto it =
					 leveledSetAllStates[level - 1][bagNumberPrime].begin();
				 it != leveledSetAllStates[level - 1][bagNumberPrime].end();
				 it++) {
				State::ptr sprime = *it;
				State::ptr temp = kernel->forget_v(sprime, *element);
				if (*s == *temp) {
					// this means that s is obained from sprime by introudcing
					// vertex *element
					shared_ptr<StateTreeNode> child = extractStateTreeNode(
						level - 1, sprime, leveledSetAllStates, tree_width);
					child->set_parent(node);
					vector<shared_ptr<StateTreeNode> > children;
					children.push_back(child);
					node->set_children(children);
					node->set_nodeType("ForgetVertex_" + to_string(*element));
					return node;
				}
			}
		}

		// Join
		if (tree_width) {
			for (auto it1 = leveledSetAllStates[level - 1][bagNumber].begin();
				 it1 != leveledSetAllStates[level - 1][bagNumber].end();
				 it1++) {
				State::ptr sprime1 = *it1;
				for (unsigned j = level - 1; j >= 0; j--) {
					for (auto it2 = leveledSetAllStates[j][bagNumber].begin();
						 it2 != leveledSetAllStates[j][bagNumber].end();
						 it2++) {
						State::ptr sprime2 = *it2;
						State::ptr temp = kernel->join(sprime1, sprime2);
						if (*s == *temp) {
							// this means that s is obained from sprime by
							// introudcing vertex *element
							shared_ptr<StateTreeNode> child1 =
								extractStateTreeNode(level - 1, sprime1,
													 leveledSetAllStates,
													 tree_width);
							shared_ptr<StateTreeNode> child2 =
								extractStateTreeNode(j, sprime2,
													 leveledSetAllStates,
													 tree_width);
							child1->set_parent(node);
							child2->set_parent(node);
							vector<shared_ptr<StateTreeNode> > children;
							children.push_back(child1);
							children.push_back(child2);
							node->set_children(children);
							node->set_nodeType("Join");
							return node;
						}
					}
				}
			}
		}
	}
	cout << "In extract state tree node function no nodes found!" << endl;
	exit(20);
}

StateTree BreadthFirstSearch::extractStateTreeDecomposition(
	unsigned level, State::ptr s,
	vector<vector<set<State::ptr> > > &leveledSetAllStates, bool tree_width) {
	StateTree stateTree;
	stateTree.root =
		extractStateTreeNode(level, s, leveledSetAllStates, tree_width);
	return stateTree;
}

pair<bool, AbstractTreeDecomposition> BreadthFirstSearch::search() {
	TreeAutomaton bfsDAG; // Constructs a DAG corresponding to the BFS. 
	set<searchState> allStatesSet = InitialSearchState; //TODO define InitialSearchState
        set<searchState> intermediateStatesSet; 
	set<searchState> newStatesSet = InitialSeachState; //TODO define InitialSearchState
	vector<searchState> newStatesVector = CONVERT InitialSearchState into vector; // This will allow us to use parallel search 
	while(newStateVector.size()>0){
		//This loop is suitable for parallelization
		for(int l=0;l<newStatesVector.size();l++){
			
			//Intro Vertex
			for (int i=0; i< dynamicKernel->width;){
			
			}
			//Forget Vertex
			for (){
			
			}  
			//Complete This
	
	
	}
	
	return std::make_pair(true, T);
}
