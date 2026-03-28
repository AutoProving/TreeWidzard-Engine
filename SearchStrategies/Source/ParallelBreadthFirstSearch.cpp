#include "ParallelBreadthFirstSearch.h"

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>

#include "Kernel/CertificateUtils.h"
#include "Kernel/CertificateWriter.h"

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

namespace {

using CounterexampleTrace = std::unordered_map<
	State::ptr,
	Transition<State::ptr, InstructiveTreeDecompositionNodeContent>,
	State::ptr::Hash,
	State::ptr::Equal>;

auto lookup_transition(const CounterexampleTrace &trace, const State::ptr &state)
	-> const Transition<State::ptr, InstructiveTreeDecompositionNodeContent> & {
	const auto it = trace.find(state);
	if (it == trace.end()) {
		std::cerr << "Error: No transition with consequent equal to the input "
					 "state."
				  << std::endl;
		std::exit(20);
	}
	return it->second;
}

void record_transition(CounterexampleTrace &trace, const State::ptr &state,
					   const std::string &symbol,
					   const std::vector<State::ptr> &antecedents) {
	InstructiveTreeDecompositionNodeContent content(symbol);
	Transition<State::ptr, InstructiveTreeDecompositionNodeContent> candidate(
		state, content, antecedents);
	const auto [it, inserted] = trace.emplace(state, candidate);
	if (!inserted && candidate < it->second) {
		it->second = std::move(candidate);
	}
}

struct BagSignature {
	std::vector<unsigned> elements;

	bool operator==(const BagSignature &rhs) const {
		return elements == rhs.elements;
	}
};

struct BagSignatureHash {
	auto operator()(const BagSignature &signature) const -> std::size_t {
		Hasher h(0);
		h << static_cast<unsigned>(signature.elements.size());
		for (unsigned element : signature.elements) {
			h << element;
		}
		return h.get();
	}
};

auto make_bag_signature(const std::set<unsigned> &elements) -> BagSignature {
	BagSignature signature;
	signature.elements.assign(elements.begin(), elements.end());
	return signature;
}

auto make_bag_signature(const Bag &bag) -> BagSignature {
	return make_bag_signature(bag.get_elements());
}

void populate_term_node(
	const CounterexampleTrace &trace, const State::ptr &state,
	const std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> &node) {
	const auto &transition = lookup_transition(trace, state);
	node->setNodeContent(transition.getTransitionContent());
	for (const auto &antecedent : transition.getAntecedentStates()) {
		auto child =
			std::make_shared<TermNode<InstructiveTreeDecompositionNodeContent>>();
		child->setParent(node);
		node->addChild(child);
		populate_term_node(trace, antecedent, child);
	}
}

void populate_run_node(
	const CounterexampleTrace &trace, const State::ptr &state,
	const std::shared_ptr<TermNode<
		RunNodeContent<State::ptr, InstructiveTreeDecompositionNodeContent>>> &node) {
	const auto &transition = lookup_transition(trace, state);
	RunNodeContent<State::ptr, InstructiveTreeDecompositionNodeContent> runNode;
	runNode.setState(state);
	runNode.setRunNodeContent(transition.getTransitionContent());
	node->setNodeContent(runNode);

	std::vector<std::shared_ptr<TermNode<
		RunNodeContent<State::ptr, InstructiveTreeDecompositionNodeContent>>>>
		children;
	children.reserve(transition.getAntecedentStates().size());
	for (const auto &antecedent : transition.getAntecedentStates()) {
		auto child = std::make_shared<TermNode<RunNodeContent<
			State::ptr, InstructiveTreeDecompositionNodeContent>>>();
		child->setParent(node);
		populate_run_node(trace, antecedent, child);
		children.push_back(child);
	}
	node->setChildren(children);
}

void populate_state_tree_node(const CounterexampleTrace &trace,
							  const State::ptr &state,
							  const std::shared_ptr<StateTreeNode> &node) {
	const auto &transition = lookup_transition(trace, state);
	node->set_nodeType(transition.getTransitionContent().getSymbol());
	node->set_S(state);

	std::vector<std::shared_ptr<StateTreeNode>> children;
	children.reserve(transition.getAntecedentStates().size());
	for (const auto &antecedent : transition.getAntecedentStates()) {
		auto child = std::make_shared<StateTreeNode>();
		child->set_parent(node);
		child->set_kernel(node->get_kernel());
		populate_state_tree_node(trace, antecedent, child);
		children.push_back(child);
	}
	node->set_children(children);
}

} // namespace

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

InstructiveTreeDecomposition
ParallelBreadthFirstSearch::extractCounterExampleTerm(State::ptr state) {
	InstructiveTreeDecomposition atd;
	std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> rootNode(
		new TermNode<InstructiveTreeDecompositionNodeContent>);
	rootNode->setParent(nullptr);
	populate_term_node(counterexampleTrace, state, rootNode);
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
	populate_state_tree_node(counterexampleTrace, state, node);
}

template <typename T>
void update_maximum(std::atomic<T> &maximum_value, const T &value) noexcept {
	T prev_value = maximum_value;
	while (prev_value < value &&
		   !maximum_value.compare_exchange_weak(prev_value, value)) {
	}
}

void ParallelBreadthFirstSearch::search() {
	// if (flags->get("Premise")) {
	// 	std::cout << "Premise is ACTIVATED" << std::endl;
	// } else {
	// 	std::cout << "Premise is NOT ACTIVATED" << std::endl;
	// }
	bool printStateFlag = flags->get("PrintStates");
	counterexampleTrace.clear();
	State::ptr initialState = kernel->initialState();

	std::unique_ptr<TreeWidzard::Certificates::CertificateWriter> cert;
	std::unordered_map<State::ptr, int, State::ptr::Hash, State::ptr::Equal>
		cert_ids;
	std::mutex cert_lock;
	std::atomic<int> next_cert_id = 1;
	if (const char *cert_path = std::getenv("TREEWIDZARD_CERT_PATH")) {
		if (std::string(cert_path).size() > 0) {
			cert = std::make_unique<TreeWidzard::Certificates::CertificateWriter>(
				cert_path);
			const uint64_t prop_hash =
				TreeWidzard::Certificates::fnv1a64_file(getPropertyFilePath());
			const uint64_t dpcores_hash =
				TreeWidzard::Certificates::dp_cores_fingerprint();
			cert->writeHeader(kernel->get_width(), getAttributeValue("SearchName"),
							  TreeWidzard::Certificates::CanonMode::NONE,
							  static_cast<bool>(flags->get("Premise")), prop_hash,
							  getPropertyFilePath(), dpcores_hash);
			cert_ids.emplace(initialState, 0);
			cert->writeLeaf(0);
		}
	}
	{
		std::lock_guard lock(stateSetsLock);
		allStatesSet.insert(initialState);
		newStatesSet.insert(initialState);
	}
	if (!noBFSDAG) {
		record_transition(counterexampleTrace, initialState, "Leaf", {});
	}
	unsigned int width = kernel->get_width().get_value();
	std::vector<std::atomic<unsigned>> numberOfWitnesses(
		initialState->numberOfComponents());
	int iterationNumber = 0;
	std::cout << std::left << std::setw(25) << "Iteration" << std::setw(25)
			  << "ALLSTATES" << std::setw(25) << "NEWSTATES"
			  << "Max WITNESSSET SIZE" << std::endl;
	const bool enable_joins = kernel->get_width().get_name() == "tree_width";
	std::unordered_map<BagSignature, std::vector<State::ptr>, BagSignatureHash>
		joinBuckets;
	if (enable_joins) {
		joinBuckets[make_bag_signature(initialState->get_bag())].push_back(
			initialState);
	}

	std::mutex everything_lock;
	auto try_insert_new_state = [this](const State::ptr &candidate) {
		std::lock_guard lock(stateSetsLock);
		if (allStatesSet.count(candidate) || newStatesSet.count(candidate)) {
			return false;
		}
		newStatesSet.insert(candidate);
		return true;
	};

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
						if (try_insert_new_state(newStatePointer)) {
							if (cert) {
								std::lock_guard lock(cert_lock);
								if (!cert_ids.count(newStatePointer)) {
									const int parent_id =
										cert_ids.at(statePointer);
									const int new_id = next_cert_id++;
									cert_ids.emplace(newStatePointer, new_id);
									cert->writeIntroVertex(new_id, parent_id,
														   i);
								}
							}

							State::ptr consequentState = newStatePointer;

							if (!noBFSDAG) {
								{
									std::lock_guard lock(everything_lock);
									record_transition(
										counterexampleTrace, consequentState,
										"IntroVertex_" + std::to_string(i),
										{statePointer});
								}
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
					if (try_insert_new_state(newStatePointer)) {
						if (cert) {
							std::lock_guard lock(cert_lock);
							if (!cert_ids.count(newStatePointer)) {
								const int parent_id =
									cert_ids.at(statePointer);
								const int new_id = next_cert_id++;
								cert_ids.emplace(newStatePointer, new_id);
								cert->writeForgetVertex(new_id, parent_id,
														*it);
							}
						}
						State::ptr consequentState = newStatePointer;
						if (!noBFSDAG) {
							{
								std::lock_guard lock(everything_lock);
								record_transition(
									counterexampleTrace, consequentState,
									"ForgetVertex_" + std::to_string(*it),
									{statePointer});
							}
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
							if (try_insert_new_state(newStatePointer)) {
								if (cert) {
									std::lock_guard lock(cert_lock);
									if (!cert_ids.count(newStatePointer)) {
										const int parent_id =
											cert_ids.at(statePointer);
										const int new_id = next_cert_id++;
										cert_ids.emplace(newStatePointer,
														 new_id);
										cert->writeIntroEdge(
											new_id, parent_id, *it, *itPrime);
									}
								}
								State::ptr consequentState = newStatePointer;
								if (!noBFSDAG) {
									{
										std::lock_guard lock(everything_lock);
										record_transition(
											counterexampleTrace, consequentState,
											"IntroEdge_" + std::to_string(*it) +
												"_" + std::to_string(*itPrime),
											{statePointer});
									}
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
			if (enable_joins) {
				const auto bucket_it = joinBuckets.find(make_bag_signature(bagElement));
				if (bucket_it != joinBuckets.end()) {
					for (const auto &join_state : bucket_it->second) {
						State::ptr newStatePointer =
							kernel->join(statePointer, join_state);
						bool premiseFlag = flags->get("Premise");
						bool satisfiesPremise = false;
						if (premiseFlag) {
							satisfiesPremise =
								conjecture->evaluatePremiseOnState(
									*newStatePointer);
						}
						if (!premiseFlag or
							(premiseFlag and satisfiesPremise)) {
							if (try_insert_new_state(newStatePointer)) {
								if (cert) {
									std::lock_guard lock(cert_lock);
									if (!cert_ids.count(newStatePointer)) {
										const int left_id =
											cert_ids.at(statePointer);
										const int right_id =
											cert_ids.at(join_state);
										const int new_id = next_cert_id++;
										cert_ids.emplace(newStatePointer,
														 new_id);
										cert->writeJoin(new_id, left_id,
														right_id,
														std::nullopt);
									}
								}
								State::ptr consequentState = newStatePointer;
								if (!noBFSDAG) {
									{
										std::lock_guard lock(everything_lock);
										record_transition(counterexampleTrace,
														 consequentState, "Join",
														 {statePointer,
														  join_state});
									}
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
									join_state->print();
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
					const bool printStateTreeFlag = flags->get("StateTree");
					const bool writeFilesFlag = flags->get("WriteToFiles");
					const bool printDirectedBipartiteGraphFlag =
						flags->get("PrintDirectedBipartiteGraphNAUTY");
					std::cout << "\nResult: ";
					std::cout << "PROPERTY NOT SATISFIED\n" << std::endl;
					if (cert) {
					std::lock_guard lock(cert_lock);
					cert->writeResultNotSatisfied(cert_ids.at(*it));
				}
				std::cout << "Counterexample found:" << std::endl;

				if (noBFSDAG) {
					std::cerr << "Rerun without -no-bfs-dag to construct a "
								 "counter example."
							  << std::endl;
					return;
				}

				State::ptr badState = *it;
				/* printing the conjectures and values of the variables */
					std::cout << "The assignment that makes the formula false:"
							  << std::endl;
					conjecture->printValues(*badState, conjecture->getRoot());
					std::cout << std::endl;

					if (printStateTreeFlag) {
						std::cout << "Counterexample state tree:" << std::endl;
						StateTree stateTree = extractCounterExampleStateTree(badState);
						stateTree.printStateTree();
						std::cout << std::endl;
					}

					std::string file = this->getOutputsPath();
					if (flags->get("Premise")) {
						file += "_Premise";
					}
					file += "_CounterExample";

					if (writeFilesFlag || printDirectedBipartiteGraphFlag) {
						InstructiveTreeDecomposition atd =
							extractCounterExampleTerm(badState);
						ConcreteTreeDecomposition ctd =
							atd.convertToConcreteTreeDecomposition();
						RunTree<State::ptr, InstructiveTreeDecompositionNodeContent>
							runTree = extractCounterExampleRun(badState);
						MultiGraph multiGraph = ctd.extractMultiGraph();
						multiGraph.printGraph();

						if (writeFilesFlag) {
							atd.writeToFile(file + "_ITD.txt");
							ctd.writeToFile(file + "_ConcreteDecomposition.txt");
							runTree.writeToFile(file + "_RunTree.txt");
							multiGraph.printToFile(file + "_Graph.txt");
							multiGraph.convertToGML(file + "_GMLGraph.gml");
							multiGraph.printToFilePACEFormat(file + "_GraphPaceFormat.gr");
						}

						if (printDirectedBipartiteGraphFlag) {
							multiGraph.printToFileDirectedBipartiteGraphNAUTY(
								file + "_DirectedBipartiteGraphNAUTY.txt");
						}
					}

				return;
			}
		}

		allStatesSet.insert(newStatesSet.cbegin(), newStatesSet.cend());
		if (enable_joins) {
			for (const auto &state : std::as_const(newStatesSet)) {
				joinBuckets[make_bag_signature(state->get_bag())].push_back(
					state);
			}
		}

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
	std::cout << "\nResult: ";
	std::cout << "PROPERTY SATISFIED" << std::endl;
	if (cert) {
		cert->writeResultSatisfied();
	}
}
RunTree<State::ptr, InstructiveTreeDecompositionNodeContent>
ParallelBreadthFirstSearch::extractCounterExampleRun(State::ptr state) {
	std::shared_ptr<TermNode<RunNodeContent<
		State::ptr, InstructiveTreeDecompositionNodeContent>>>
		root(new TermNode<RunNodeContent<
			 State::ptr, InstructiveTreeDecompositionNodeContent>>);
	root->setParent(nullptr);
	populate_run_node(counterexampleTrace, state, root);
	RunTree<State::ptr, InstructiveTreeDecompositionNodeContent> runTree;
	runTree.setRoot(root);
	return runTree;
}
