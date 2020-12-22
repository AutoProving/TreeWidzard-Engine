// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "LeveledSetSearch.h"

extern "C" {
    LeveledSetSearch * create() {
        return new LeveledSetSearch();
    }
    LeveledSetSearch * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
        return new LeveledSetSearch(dynamicKernel,conjecture,flags);
    }
}

bool check_lessThan_states_leveledSetSearch(const State::ptr &lhs,
											const State::ptr &rhs) {
	return *lhs < *rhs;
}

bool check_equality_states_leveledSetSearch(const State::ptr &lhs,
											const State::ptr &rhs) {
	return (*lhs == *rhs);
}

LeveledSetSearch::LeveledSetSearch() {
    addAttribute("SearchName","LeveledSetSearch");
}

LeveledSetSearch::LeveledSetSearch(DynamicKernel *dynamicKernel,
								   Conjecture *conjecture, Flags *flags)
	: SearchStrategy(dynamicKernel, conjecture, flags) {
	this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
    addAttribute("SearchName","LeveledSetSearch");

}

unsigned LeveledSetSearch::bagSetToNumber(set<unsigned> bagSet,
										  unsigned width) {
	unsigned number = 0;
	for (unsigned i = 1; i <= width + 1; i++) {
		if (bagSet.find(i) != bagSet.end()) {
			number = number + pow(2, i - 1);
		}
	}
	return number;
}
shared_ptr<CTDNodeNew> LeveledSetSearch::extractCTDNode(
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

ConcreteTreeDecomposition LeveledSetSearch::extractCTDDecomposition(
	unsigned level, State::ptr s,
	vector<vector<set<State::ptr> > > &leveledSetAllStates) {
	ConcreteTreeDecomposition T;
	T.root = extractCTDNode(level, s, leveledSetAllStates);
	return T;
}

shared_ptr<StateTreeNode> LeveledSetSearch::extractStateTreeNode(
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

StateTree LeveledSetSearch::extractStateTreeDecomposition(
	unsigned level, State::ptr s,
	vector<vector<set<State::ptr> > > &leveledSetAllStates, bool tree_width) {
	StateTree stateTree;
	stateTree.root =
		extractStateTreeNode(level, s, leveledSetAllStates, tree_width);
	return stateTree;
}

pair<bool, ConcreteTreeDecomposition> LeveledSetSearch::search() {
	// The number of positions of the vectors setAllStates, setNewStates and
	// setIntermediateStates is equal to the number of BagSets of with width+1
	// elements (where width = pathwidth or treewidth) that means, 2^{width+1}.
	unsigned numberBagSets = pow(2, kernel->get_width().get_value() + 1);
	setAllStates.resize(numberBagSets);
	setNewStates.resize(numberBagSets);
	setIntermediateStates.resize(numberBagSets);
	// 0 is the number corresponding to the empty bag.
	setAllStates[0].insert(kernel->initialState());
	setNewStates[0].insert(kernel->initialState());
	leveledSetAllStates.push_back(setAllStates);
    unsigned newStatesCount = 1;
	unsigned intermediateCount = 0;
	unsigned iterationNumber = 0;
	vector<unsigned> maximumStatesBag;
	maximumStatesBag.resize(numberBagSets, 0);
	while (newStatesCount) {
		// Statistics
		auto start = high_resolution_clock::now(); // Set initial time
		vector<unsigned> maxWitnessSetSize;
		//
		unsigned width = kernel->get_width().get_value();
		if (flags->get("PrintStates") == 1) {
			cout << endl << "----------------- Begin Iteration: " << iterationNumber << " ----------------------------" << endl;
		}
		// PROCESSING PHASE
		for (size_t k = 0; k < setNewStates.size(); k++) {
			for (auto iteratorNewStates = setNewStates[k].begin();
				 iteratorNewStates != setNewStates[k].end();
				 iteratorNewStates++) {
                if (flags->get("PrintStates") == 1) {
					cout << "Current State Being Processed: " << endl;
					(*iteratorNewStates)->print();
					cout << "---- **** ---" << endl;
				}
				// The next vector is for statistics purpose. It records the
				// maximum size of a witness if the flag "LoopTime" is
				// activated.
				maxWitnessSetSize.resize(
					(*iteratorNewStates)->numberOfComponents(), 0);
				// introduce a vertex
				// the +2 below comes from the fact that treewidth is size of
				// bag minus one. So the loop iterates from 1 to number of
				// elements inteh bag.
				for (unsigned i = 1; i < width + 2; ++i) {
					if ((*iteratorNewStates)->get_bag().vertex_introducible(i)) {
						State::ptr newState =
							kernel->intro_v(*iteratorNewStates, i);
                        unsigned index = bagSetToNumber(
							newState->get_bag().get_elements(), width);
						setIntermediateStates[index].insert(newState);
						// The next two ifs are for statistics
						if (flags->get("PrintStates") == 1) {
							cout << "intro_v_" << i << endl;
							newState->print();
						}
						if (flags->get("LoopTime") == 1) {
							for (size_t m = 0;
								 m < newState->numberOfComponents(); m++) {
								if (maxWitnessSetSize[m] <
									static_cast<unsigned>(
										newState->getWitnessSet(m)->size())) {
maxWitnessSetSize[m] =
										newState->getWitnessSet(m)->size();
								}
							}
						}
					}
				}
				// introduce an edge
				if ((*iteratorNewStates)->get_bag().get_elements().size() > 1) {
					set<unsigned> bagSet =
						(*iteratorNewStates)->get_bag().get_elements();
					for (auto itBagEl1 = bagSet.begin();
						 itBagEl1 != bagSet.end(); itBagEl1++) {
						auto it = itBagEl1;
						it++;
						if (it != (*iteratorNewStates)
									  ->get_bag()
									  .get_elements()
									  .end()) {
							for (auto itBagEl2 = it; itBagEl2 != bagSet.end();
								 itBagEl2++) {
								if ((*iteratorNewStates)
										->get_bag()
										.edge_introducible(*itBagEl1,
														   *itBagEl2)) {
									State::ptr newState =
										kernel->intro_e(*iteratorNewStates,
														*itBagEl1, *itBagEl2);
									unsigned index = bagSetToNumber(
										newState->get_bag().get_elements(),
										width);
									setIntermediateStates[index].insert(
										newState);
									// Statistics
									if (flags->get("PrintStates") == 1) {
										cout << "intro_e_" << *itBagEl1 << "_"
											 << *itBagEl2 << endl;
										newState->print();
									}
									if (flags->get("LoopTime") == 1) {
										for (size_t m = 0;
											 m < newState->numberOfComponents();
											 m++) {
											if (maxWitnessSetSize[m] <
												static_cast<unsigned>(
													newState->getWitnessSet(m)
														->size())) {
												maxWitnessSetSize[m] =
													newState->getWitnessSet(m)
														->size();
											}
										}
									}
								}
							}
						}
					}
				}
				// forget a vertex
				for (size_t i = 1; i < kernel->get_width().get_value() + 2;
					 ++i) {
					if ((*iteratorNewStates)->get_bag().vertex_forgettable(i)) {
						State::ptr newState =
							kernel->forget_v(*iteratorNewStates, i);
						unsigned index = bagSetToNumber(
							newState->get_bag().get_elements(), width);
						setIntermediateStates[index].insert(newState);
						// Statistics
						if (flags->get("PrintStates") == 1) {
							cout << "forget_v_" << i << endl;
							newState->print();
						}
						if (flags->get("LoopTime") == 1) {
							for (size_t m = 0;
								 m < newState->numberOfComponents(); m++) {
								if (maxWitnessSetSize[m] <
									static_cast<unsigned>(
										newState->getWitnessSet(m)->size())) {
									maxWitnessSetSize[m] =
										newState->getWitnessSet(m)->size();
								}
							}
						}
					}
				}
				if (kernel->get_width().get_name() == "tree_width") {
					// join
					for (auto it = setAllStates[k].begin();
						 it != setAllStates[k].end(); it++) {
						// Observation: The order in which the states are joined
						// here do not matter, since we are considering only
						// states. But when constructing automata, we should
						// consider two transitions, one where the new states
						// come before the old states and one in which the old
						// states come first Otherwise, we could potentially
						// miss some terms (VERIFY IF THIS IS THE CASE).
						State::ptr newState =
							kernel->join(*iteratorNewStates, *it);
						int index = bagSetToNumber(
							newState->get_bag().get_elements(), width);
						setIntermediateStates[index].insert(newState);
						// Statistics
						if (flags->get("PrintStates") == 1) {
							cout << "join" << endl;
							cout << "------picked state:" << endl;
							(*it)->print();
							cout << "------result state:" << endl;
							newState->print();
						}
						if (flags->get("LoopTime") == 1) {
							for (size_t m = 0;
								 m < newState->numberOfComponents(); m++) {
								if (maxWitnessSetSize[m] <
									static_cast<unsigned>(
										newState->getWitnessSet(m)->size())) {
									maxWitnessSetSize[m] =
										newState->getWitnessSet(m)->size();
								}
							}
						}
					}
					for (auto itrJoin = setNewStates[k].begin();
						 itrJoin != setNewStates[k].end(); itrJoin++) {
						State::ptr newState =
							kernel->join(*iteratorNewStates, *itrJoin);
						setIntermediateStates[k].insert(newState);
					}
				}
			}
		}

		intermediateCount = 0;
		for (unsigned bagSetIndex = 0; bagSetIndex < numberBagSets;
			 bagSetIndex++) {
			intermediateCount =
				intermediateCount + setIntermediateStates[bagSetIndex].size();
		}

		// Add new states to setNewStates
		for (unsigned bagSetIndex = 0; bagSetIndex < numberBagSets;
			 bagSetIndex++) {
			setNewStates[bagSetIndex].clear();
			set_difference(setIntermediateStates[bagSetIndex].begin(),
						   setIntermediateStates[bagSetIndex].end(),
						   setAllStates[bagSetIndex].begin(),
						   setAllStates[bagSetIndex].end(),
						   inserter(setNewStates[bagSetIndex],
									setNewStates[bagSetIndex].begin()),
						   check_lessThan_states_leveledSetSearch);
		}

		// Check for bad States, i.e., states that certify that the conjecture
		// is false
		for (unsigned bagSetIndex = 0; bagSetIndex < numberBagSets;
			 bagSetIndex++) {
			for (auto it = setNewStates[bagSetIndex].begin();
				 it != setNewStates[bagSetIndex].end(); it++) {
				if (!conjecture->evaluateConjectureOnState(*(*it), kernel)) {
					cout << endl
						 << "-----------------------BAD STATE "
							"FOUND---------------------"
						 << endl;
					(*it)->print();
					cout << endl
						 << "--------------------------------------------------"
							"---------"
						 << endl;
					cout << endl
						 << "Constructing Counter Example Decomposition"
						 << endl;
					bool tree_width = false;
					if (kernel->get_width().get_name() == "tree_width") {
						tree_width = true;
					}
					ConcreteTreeDecomposition *T = new ConcreteTreeDecomposition;
					*T = extractCTDDecomposition(iterationNumber + 1, *it,
												 leveledSetAllStates);
					cout<<"Abstract Concrete Tree Decomposition:"<<endl;
					T->printAbstract();
                    cout<<"Tree Concrete Tree Decomposition 1:"<<endl;
                    T->printTree();
                    StateTree* S = new StateTree;
                    *S = extractStateTreeDecomposition(iterationNumber+1,*it,leveledSetAllStates,tree_width);
					cout << "-----------------state tree "
							"printing--------------------"
						 << endl;
                    cout<<"Abstract State Tree Decomposition:"<<endl;
                    S->printAbstract();
                    cout<<"Tree State Tree Decomposition:"<<endl;
                    S->printStateTree();
					//
					cout << endl
						 << "------------------Constructing Counter Example "
							"Graph-------------------"
						 << endl;
					T->extractMultiGraph().printGraph();
					cout << "\n convert to gml "
						 << T->extractMultiGraph().convertToGML();
					// S->extractMultiGraph().printGraph();
					// S->extractMultiGraph().convertToGML();
					exit(20);
				}
			}
		}

		leveledSetAllStates.push_back(setNewStates);

		for (unsigned bagSetIndex = 0; bagSetIndex < numberBagSets;
			 bagSetIndex++) {
			set<State::ptr> temp;
			set_union(setAllStates[bagSetIndex].begin(),
					  setAllStates[bagSetIndex].end(),
					  setNewStates[bagSetIndex].begin(),
					  setNewStates[bagSetIndex].end(),
					  inserter(temp, temp.begin()),
					  check_lessThan_states_leveledSetSearch);
			setAllStates[bagSetIndex].clear();
			setAllStates[bagSetIndex] = temp;
		}

		iterationNumber++;
		// Print Statistics if "LoopTime" flag is activated
		if (flags->get("LoopTime") == 1) {
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			cout << endl;
			cout << "Iteration: " << iterationNumber << "  ";
			// cout<<"T:"<<duration.count();
			unsigned totalAllStates = 0;
			unsigned totalNewStates = 0;
			unsigned totalIntermediateStates = 0;
			for (unsigned i = 0; i < numberBagSets; i++) {
				totalAllStates = totalAllStates + setAllStates[i].size();
				totalNewStates = totalNewStates + setNewStates[i].size();
				totalIntermediateStates =
					totalIntermediateStates + setIntermediateStates[i].size();
			}
			cout << endl;
			cout << "   AllStates: " << totalAllStates;
			cout << "   NewStates: " << totalNewStates;
			cout << "   IntermediateStates s: " << totalIntermediateStates;
			// cout<<"   Average: " <<
			// (duration.count()/setIntermediateStates.size());
			cout << "   WitnessSize: ";
			for (size_t m = 0; m < maxWitnessSetSize.size(); m++) {
				cout << maxWitnessSetSize[m] << ",";
			}
			cout << endl;
			int sum = 0;
			cout << "max states bag: ";
			for (size_t m = 0;
				 m < leveledSetAllStates[leveledSetAllStates.size() - 1].size();
				 m++) {
				if (leveledSetAllStates[leveledSetAllStates.size() - 1][m]
						.size() > maximumStatesBag[m]) {
					maximumStatesBag[m] =
						leveledSetAllStates[leveledSetAllStates.size() - 1][m]
							.size();
				}
				cout << m << ":" << maximumStatesBag[m] << " ";
				sum += leveledSetAllStates[leveledSetAllStates.size() - 1][m]
						   .size();
			}
			cout << "sum:" << sum << endl;
		}
		newStatesCount = 0;
		for (unsigned i = 0; i < numberBagSets; i++) {
			newStatesCount = newStatesCount + setNewStates[i].size();
		}
		if (flags->get("LoopTime") == 1) {
			cout << endl
				 << "----------------- End Iteration: " << iterationNumber<< " ----------------------------" << endl << endl;
		}
		// Remove the states from setIntermediateStates
		for (unsigned bagSetIndex = 0; bagSetIndex < numberBagSets;
			 bagSetIndex++) {
			setIntermediateStates[bagSetIndex].clear();
		}
	}
	cout << "!!!FINISH!!!" << endl;
	ConcreteTreeDecomposition T;
	return std::make_pair(true, T);
}
