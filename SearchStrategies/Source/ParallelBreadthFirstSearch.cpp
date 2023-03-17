#include "ParallelBreadthFirstSearch.h"

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>

/**
 * Does not work with WitnessTypeOne.
 * Works with WitnessTypeTwo.
 *
 * Witnesses must be hashable.
 */

extern "C" {
std::map<std::string, std::string> *metadata() {
	return new std::map(ParallelBreadthFirstSearch().getAttributes());
}
ParallelBreadthFirstSearch *create(DynamicKernel *dynamicKernel,
								   Conjecture *conjecture, Flags *flags) {
	return new ParallelBreadthFirstSearch(dynamicKernel, conjecture, flags);
}
}

ParallelBreadthFirstSearch::ParallelBreadthFirstSearch() {
	addAttribute("SearchName", "ParallelBreadthFirstSearch");
}

ParallelBreadthFirstSearch::ParallelBreadthFirstSearch(
	DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags)
	: SearchStrategy(dynamicKernel, conjecture, flags) {
	this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
	addAttribute("SearchName", "ParallelBreadthFirstSearch");
	this->noBFSDAG = flags->get("NoBFSDAG");
	this->nThreads = flags->get("NThreads");
	if (this->nThreads == -1) {
		this->nThreads = 4;
		std::cerr << "Number of threads was not set. Using default of "
				  << this->nThreads << std::endl;
	}
}

AbstractTreeDecomposition ParallelBreadthFirstSearch::extractCounterExampleTerm(
	State::ptr state) {
	AbstractTreeDecomposition atd;
	std::shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> rootNode;
	rootNode = bfsDAG.retrieveTermAcyclicAutomaton(state);
	atd.setRoot(rootNode);
	return atd;
}

StateTree ParallelBreadthFirstSearch::extractCounterExampleStateTree(
	State::ptr state) {
	StateTree stateTree;
	std::shared_ptr<StateTreeNode> root(new StateTreeNode());
	std::shared_ptr<DynamicKernel> sharedKernel =
		std::make_shared<DynamicKernel>(*kernel);
	root->set_kernel(sharedKernel);
	extractCounterExampleStateTreeNode(state, root);
	stateTree.root = root;
	return stateTree;
}

void ParallelBreadthFirstSearch::extractCounterExampleStateTreeNode(
	State::ptr state, std::shared_ptr<StateTreeNode> node) {
	// Assumes that the automaton is acyclic and that each state has a
	// transition in which the state is the consequent
	if (!bfsDAG.getTransitions().empty()) {
		AbstractTreeDecompositionNodeContent a;
		a.setSymbol(
			a.smallestContent()); // Creates a symbol of type TermNodeContent
								  // and set it to the smallest symbol
		std::vector<State::ptr> emptyAntecedents;
		Transition<State::ptr, AbstractTreeDecompositionNodeContent> t(
			state, a, emptyAntecedents); // This is the smallest transition with
										 // a consequent equal to state
		auto it = bfsDAG.getTransitions().find(t);
		if (it == bfsDAG.getTransitions().end())
			it = bfsDAG.getTransitions().begin();
		auto itAux = it;
		if (itAux->getConsequentState() != state) {
			itAux++;
			if (itAux != bfsDAG.getTransitions().end()) {
				if (itAux->getConsequentState() != state) {
					std::cout
						<< "Error: No transition with consequent equal to the "
						   "input state.";
					exit(20);
				}
			} else {
				std::cout << "Error: No transition with consequent equal to "
							 "the input state.";
				exit(20);
			}
		}
		node->set_nodeType(itAux->getTransitionContent().getSymbol());
		node->set_S(state);
		std::vector<std::shared_ptr<StateTreeNode>> children;
		for (size_t i = 0; i < itAux->getAntecedentStates().size(); i++) {
			std::shared_ptr<StateTreeNode> child(new StateTreeNode);
			child->set_parent(node);
			child->set_kernel(node->get_kernel());
			children.push_back(child);
			extractCounterExampleStateTreeNode(itAux->getAntecedentStates()[i],
											   child);
		}
		node->set_children(children);
	} else {
		std::cout << "Error: The automaton has no transitions." << std::endl;
		exit(20);
	}
}

//
template <typename T>
void update_maximum(std::atomic<T> &maximum_value, const T &value) noexcept {
	T prev_value = maximum_value;
	while (prev_value < value &&
		   !maximum_value.compare_exchange_weak(prev_value, value)) {
	}
}

void ParallelBreadthFirstSearch::search() {
	if (flags->get("Premise")) {
		std::cout << " Premise is ACTIVATED" << std::endl;
	} else {
		std::cout << "Premise is NOT ACTIVATED" << std::endl;
	}
	bool printStateFlag = flags->get("PrintStates");
	State::ptr initialState = kernel->initialState();
	allStatesSet.insert(initialState);
	newStatesSet.insert(initialState);
	if (!noBFSDAG) {
		// Initialize the DAG
		bfsDAG.addState(initialState);
		AbstractTreeDecompositionNodeContent initialTransitionContent("Leaf");
		std::vector<State::ptr> initialAntecedents; // Empty std::vector since
													// there are no children.
		Transition<State::ptr, AbstractTreeDecompositionNodeContent>
			initialTransition(initialState, initialTransitionContent,
							  initialAntecedents);
		bfsDAG.addTransition(initialTransition);
		////////////////////////////////////
	}
	unsigned int width = kernel->get_width().get_value();
	std::vector<std::atomic<unsigned>> numberOfWitnesses(
		initialState->numberOfComponents());
	int iterationNumber = 0;
	std::cout << std::left << std::setw(25) << "Iteration" << std::setw(25)
			  << "ALLSTATES" << std::setw(25) << "NEWSTATES"
			  << "Max WITNESSSET SIZE" << std::endl;

	std::mutex everything_lock;

	while (!newStatesSet.empty()) {
		iterationNumber++;
		////////////////////////////////////////////////////////////////////////////////////
		newStatesVector.clear(); // clear newStatesVector to add states in
								 // newStatesSet in it
		newStatesVector.reserve(newStatesSet.size()); //
		for (const auto &state : std::as_const(newStatesSet))
			newStatesVector.push_back(state);
		newStatesSet.clear(); // clear newStatesSet to add new states that are
							  // generated in this loop

		// part ran in parallel
		auto visit_state = [&](State::ptr statePointer) {
			Bag bag = statePointer->get_bag();
			std::set<unsigned> bagElement = bag.get_elements();

			///////////////////////////////////////////////////////
			//////////////////// Introduce Vertex /////////////////
			///////////////////////////////////////////////////////
			// the +1 below comes from the fact that treewidth is
			// size of the bag minus one. So the loop iterates
			// from 1 to number of elements in the  bag.
			for (size_t i = 1; i <= width + 1; i++) {
				if (bag.vertex_introducible(i)) {
					State::ptr newStatePointer =
						kernel->intro_v(statePointer, i);

					bool premiseFlag = flags->get("Premise");
					bool satisfiesPremise = false;
					if (premiseFlag) {
						satisfiesPremise = conjecture->evaluatePremiseOnState(
							*newStatePointer);
					}

					if (!premiseFlag or (premiseFlag and satisfiesPremise)) {
						if (!allStatesSet.count(newStatePointer) and
							!newStatesSet.count(newStatePointer)) {
							newStatesSet.insert(newStatePointer);

							State::ptr consequentState = newStatePointer;

							AbstractTreeDecompositionNodeContent
								transitionContent("IntroVertex_" +
												  std::to_string(i));
							std::vector<State::ptr> antecedentStates;
							antecedentStates.push_back(statePointer);
							Transition<State::ptr,
									   AbstractTreeDecompositionNodeContent>
								transition(consequentState, transitionContent,
										   antecedentStates);

							if (!noBFSDAG) {
								std::lock_guard lock(everything_lock);
								bfsDAG.addState(consequentState);
								bfsDAG.addTransition(transition);
							}

							if (printStateFlag) {
								std::cout << std::endl;
								std::cout << "================================="
											 "================"
											 "======================="
										  << std::endl;
								std::cout << " Introduce Vertex: " << i
										  << std::endl;
								std::cout << "================================="
											 "================"
											 "======================="
										  << std::endl;
								std::cout << " Current State:" << std::endl;
								statePointer->print();
								std::cout << " New State:" << std::endl;
								newStatePointer->print();
								std::cout << "================================="
											 "================"
											 "======================="
										  << std::endl;
								std::cout << std::endl;
							}

							// size of witnessSets
							for (size_t component = 0;
								 component < numberOfWitnesses.size();
								 ++component) {
								update_maximum(numberOfWitnesses[component],
											   (unsigned)consequentState
												   ->getWitnessSet(component)
												   ->size());
							}
						}
					}
				}
			}

			///////////////////////////////////////////////////////
			//////////////////// Forget Vertex ////////////////////
			///////////////////////////////////////////////////////
			for (auto it = bagElement.begin(); it != bagElement.end(); ++it) {
				State::ptr newStatePointer =
					kernel->forget_v(statePointer, *it);
				bool premiseFlag = flags->get("Premise");
				bool satisfiesPremise = false;
				if (premiseFlag) {
					satisfiesPremise =
						conjecture->evaluatePremiseOnState(*newStatePointer);
				}
				if (!premiseFlag or (premiseFlag and satisfiesPremise)) {
					if (!allStatesSet.count(newStatePointer) and
						!newStatesSet.count(newStatePointer)) {
						newStatesSet.insert(newStatePointer);
						State::ptr consequentState = newStatePointer;
						AbstractTreeDecompositionNodeContent transitionContent(
							"ForgetVertex_" + std::to_string(*it));
						std::vector<State::ptr> antecedentStates;
						antecedentStates.push_back(statePointer);
						Transition<State::ptr,
								   AbstractTreeDecompositionNodeContent>
							transition(consequentState, transitionContent,
									   antecedentStates);
						if (!noBFSDAG) {
							std::lock_guard lock(everything_lock);
							bfsDAG.addState(consequentState);
							bfsDAG.addTransition(transition);
						}
						if (printStateFlag) {
							std::cout << std::endl;
							std::cout << "====================================="
										 "=============="
										 "====================="
									  << std::endl;
							std::cout << " Forget Vertex: " << *it << std::endl;
							std::cout << "====================================="
										 "=============="
										 "====================="
									  << std::endl;
							std::cout << " Current State:" << std::endl;
							statePointer->print();
							std::cout << " New State:" << std::endl;
							newStatePointer->print();
							std::cout << "====================================="
										 "=============="
										 "====================="
									  << std::endl;
							std::cout << std::endl;
						}
						// size of witnessSets
						for (size_t component = 0;
							 component < numberOfWitnesses.size();
							 ++component) {
							update_maximum(numberOfWitnesses[component],
										   (unsigned)consequentState
											   ->getWitnessSet(component)
											   ->size());
						}
					}
				}
			}

			///////////////////////////////////////////////////////
			//////////////////// Introduce Edge ///////////////////
			///////////////////////////////////////////////////////
			if (bag.get_elements().size() > 1) {
				for (auto it = bagElement.begin(); it != bagElement.end();
					 ++it) {
					for (auto itPrime = std::next(it);
						 itPrime != bagElement.end(); itPrime++) {
						State::ptr newStatePointer =
							kernel->intro_e(statePointer, *it, *itPrime);
						bool premiseFlag = flags->get("Premise");
						bool satisfiesPremise = false;
						if (premiseFlag) {
							satisfiesPremise =
								conjecture->evaluatePremiseOnState(
									*newStatePointer);
						}
						if (!premiseFlag or
							(premiseFlag and satisfiesPremise)) {
							if (!allStatesSet.count(newStatePointer) and
								!newStatesSet.count(newStatePointer)) {
								newStatesSet.insert(newStatePointer);
								State::ptr consequentState = newStatePointer;
								AbstractTreeDecompositionNodeContent
									transitionContent(
										"IntroEdge_" + std::to_string(*it) +
										"_" + std::to_string(*itPrime));
								std::vector<State::ptr> antecedentStates;
								antecedentStates.push_back(statePointer);
								Transition<State::ptr,
										   AbstractTreeDecompositionNodeContent>
									transition(consequentState,
											   transitionContent,
											   antecedentStates);
								if (!noBFSDAG) {
									std::lock_guard lock(everything_lock);
									bfsDAG.addState(consequentState);
									bfsDAG.addTransition(transition);
								}
								if (printStateFlag) {
									std::cout << std::endl;
									std::cout << "============================="
												 "================"
												 "==========================="
											  << std::endl;
									std::cout << " Introduce Edge: " << *it
											  << " " << *itPrime << std::endl;
									std::cout << "============================="
												 "================"
												 "==========================="
											  << std::endl;
									std::cout << " Current State:" << std::endl;
									statePointer->print();
									std::cout << " New State:" << std::endl;
									newStatePointer->print();
									std::cout << "============================="
												 "================"
												 "==========================="
											  << std::endl;
									std::cout << std::endl;
								}
								// size of witnessSets
								for (size_t component = 0;
									 component < numberOfWitnesses.size();
									 ++component) {
									update_maximum(
										numberOfWitnesses[component],
										(unsigned)consequentState
											->getWitnessSet(component)
											->size());
								}
							}
						}
					}
				}
			}

			///////////////////////////////////////////////////////
			//////////////////////// Join /////////////////////////
			///////////////////////////////////////////////////////
			if (kernel->get_width().get_name() == "tree_width") {
				// join
				for (auto it = allStatesSet.cbegin(); it != allStatesSet.cend();
					 ++it) {
					if (statePointer->get_bag().joinable((*it)->get_bag())) {
						State::ptr newStatePointer =
							kernel->join(statePointer, *it);
						bool premiseFlag = flags->get("Premise");
						bool satisfiesPremise = false;
						if (premiseFlag) {
							satisfiesPremise =
								conjecture->evaluatePremiseOnState(
									*newStatePointer);
						}
						if (!premiseFlag or
							(premiseFlag and satisfiesPremise)) {
							if (!allStatesSet.count(newStatePointer) and
								!newStatesSet.count(newStatePointer)) {
								newStatesSet.insert(newStatePointer);
								State::ptr consequentState = newStatePointer;
								AbstractTreeDecompositionNodeContent
									transitionContent("Join");
								std::vector<State::ptr> antecedentStates;
								antecedentStates.push_back(statePointer);
								antecedentStates.push_back(*it);
								Transition<State::ptr,
										   AbstractTreeDecompositionNodeContent>
									transition(consequentState,
											   transitionContent,
											   antecedentStates);
								if (!noBFSDAG) {
									std::lock_guard lock(everything_lock);
									bfsDAG.addState(consequentState);
									bfsDAG.addTransition(transition);
								}
								if (printStateFlag) {
									std::cout << std::endl;
									std::cout << "============================="
												 "=================="
												 "========================="
											  << std::endl;
									std::cout << " Join: " << std::endl;
									std::cout << "============================="
												 "=================="
												 "========================="
											  << std::endl;
									std::cout << " State One:" << std::endl;
									statePointer->print();
									std::cout << " State Two:" << std::endl;
									(*it)->print();
									std::cout << " New State:" << std::endl;
									newStatePointer->print();
									std::cout << "============================="
												 "=================="
												 "========================="
											  << std::endl;
									std::cout << std::endl;
								}
								// size of witnessSets
								for (size_t component = 0;
									 component < numberOfWitnesses.size();
									 ++component) {
									update_maximum(
										numberOfWitnesses[component],
										(unsigned)consequentState
											->getWitnessSet(component)
											->size());
								}
							}
						}
					}
				}
				//                for (auto it = newStatesSet.begin(); it !=
				//                newStatesSet.end(); it++) {
				//                    if
				//                    (statePointer->get_bag().joinable((*it)->get_bag()))
				//                    {
				//                        State::ptr newStatePointer =
				//                        kernel->join(statePointer, *it); if
				//                        (!allStatesSet.count(newStatePointer)
				//                        and
				//                        !newStatesSet.count(newStatePointer))
				//                        {
				//                            newStatesSet.insert(newStatePointer);
				//                            State::ptr consequentState =
				//                            newStatePointer;
				//                            AbstractTreeDecompositionNodeContent
				//                            transitionContent("Join");
				//                            bfsDAG.addState(consequentState);
				//                            std::vector<State::ptr>
				//                            antecedentStates;
				//                            antecedentStates.push_back(statePointer);
				//                            antecedentStates.push_back(*it);
				//                            Transition<State::ptr,
				//                            AbstractTreeDecompositionNodeContent>
				//                            transition(consequentState,
				//                                                                                                    transitionContent,
				//                                                                                                    antecedentStates);
				//                            bfsDAG.addTransition(transition);
				//                        }
				//                    }
				//                }
			}
		};

		auto visit_range = [&](size_t begin, size_t end) {
			for (size_t l = begin; l < end; ++l)
				visit_state(newStatesVector[l]);
		};

		std::vector<std::thread> threads;
		threads.reserve(nThreads);

		size_t per_thread = (newStatesVector.size() + nThreads - 1) / nThreads;

		for (size_t l = 0; l < newStatesVector.size(); l += per_thread)
			threads.push_back(
				std::thread(visit_range, l,
							std::min(l + per_thread, newStatesVector.size())));

		for (auto &t : threads) t.join();

		for (auto it = newStatesSet.cbegin(); it != newStatesSet.cend(); it++) {
			if (!conjecture->evaluateConjectureOnState(**it)) {
				std::cout << "Conjecture: Not Satisfied" << std::endl;

				if (noBFSDAG) {
					std::cerr << "Rerun without -no-bfs-dag to construct a "
								 "counter example."
							  << std::endl;
					return;
				}

				State::ptr badState = *it;
				bfsDAG.addFinalState(badState);
				AbstractTreeDecomposition atd =
					extractCounterExampleTerm(badState);
				std::string file = this->getOutputsPath();
				if (flags->get("Premise")) {
					file += "_Premise";
				}
				file += "_CounterExample";
				ConcreteTreeDecomposition ctd =
					atd.convertToConcreteTreeDecomposition();
				RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
					runTree = extractCounterExampleRun(badState);
				MultiGraph multiGraph = ctd.extractMultiGraph();
				multiGraph.printGraph();

				atd.writeToFile(file + "_AbstractDecomposition.txt");
				ctd.writeToFile(file + "_ConcreteDecomposition.txt");
				runTree.writeToFile(file + "_RunTree.txt");
				multiGraph.printToFile(file + "_Graph.txt");
				multiGraph.convertToGML(file + "_GMLGraph.gml");
				multiGraph.printToFilePACEFormat(file + "_GraphPaceFormat.gr");

				if (flags->get("PrintDirectedBipartiteGraphNAUTY")) {
					multiGraph.printToFileDirectedBipartiteGraphNAUTY(
						file + "_DirectedBipartiteGraphNAUTY.txt");
				}

				return;
			}
		}

		allStatesSet.insert(newStatesSet.cbegin(), newStatesSet.cend());

		if (flags->get("LoopTime") == 1) {
			std::cout << std::left << std::setw(25) << iterationNumber
					  << std::setw(25) << allStatesSet.size() << std::setw(25)
					  << newStatesSet.size();
			for (size_t component = 0; component < numberOfWitnesses.size();
				 ++component) {
				std::cout << (unsigned)numberOfWitnesses[component];
				if (component != numberOfWitnesses.size() - 1) std::cout << ",";
			}
			std::cout << std::endl;
		}
	}
	std::cout << "Conjecture: Satisfied" << std::endl;
}
RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
ParallelBreadthFirstSearch::extractCounterExampleRun(State::ptr state) {
	RunTree<State::ptr, AbstractTreeDecompositionNodeContent> runTree =
		bfsDAG.retrieveRunAcyclicAutomaton(state);
	return runTree;
}
