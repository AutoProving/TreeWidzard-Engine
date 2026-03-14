#include "BreadthFirstSearch.h"

#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <utility>

#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/CertificateUtils.h"
#include "Kernel/CertificateWriter.h"

extern "C"
{
	std::map<std::string, std::string> *metadata()
	{
		return new std::map(BreadthFirstSearch().getAttributes());
	}
	BreadthFirstSearch *create(DynamicKernel *dynamicKernel, Conjecture *conjecture,
							   Flags *flags)
	{
		return new BreadthFirstSearch(dynamicKernel, conjecture, flags);
	}
}

BreadthFirstSearch::BreadthFirstSearch()
{
	addAttribute("SearchName", "BreadthFirstSearch");
}

BreadthFirstSearch::BreadthFirstSearch(DynamicKernel *dynamicKernel,
									   Conjecture *conjecture, Flags *flags)
	: SearchStrategy(dynamicKernel, conjecture, flags)
{
	this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
	addAttribute("SearchName", "BreadthFirstSearch");
	this->noBFSDAG = flags->get("NoBFSDAG");
}

InstructiveTreeDecomposition BreadthFirstSearch::extractCounterExampleTerm(
	State::ptr state)
{
	InstructiveTreeDecomposition atd;
	shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> rootNode;
	rootNode = bfsDAG.retrieveTermAcyclicAutomaton(state);
	atd.setRoot(rootNode);
	return atd;
}

StateTree BreadthFirstSearch::extractCounterExampleStateTree(State::ptr state)
{
	StateTree stateTree;
	shared_ptr<StateTreeNode> root(new StateTreeNode());
	shared_ptr<DynamicKernel> sharedKernel =
		make_shared<DynamicKernel>(*kernel);
	root->set_kernel(sharedKernel);
	extractCounterExampleStateTreeNode(state, root);
	stateTree.root = root;
	return stateTree;
}

void BreadthFirstSearch::extractCounterExampleStateTreeNode(
	State::ptr state, shared_ptr<StateTreeNode> node)
{
	// Assumes that the automaton is acyclic and that each state has a
	// transition in which the state is the consequent
	if (!bfsDAG.getTransitions().empty())
	{
		InstructiveTreeDecompositionNodeContent a;
		a.setSymbol(
			a.smallestContent()); // Creates a symbol of type TermNodeContent
								  // and set it to the smallest symbol
		vector<State::ptr> emptyAntecedents;
		Transition<State::ptr, InstructiveTreeDecompositionNodeContent> t(
			state, a, emptyAntecedents); // This is the smallest transition with
										 // a consequent equal to state
		auto it = bfsDAG.getTransitions().upper_bound(t);
		if (it != bfsDAG.getTransitions().begin())
		{
			it--; // This is always defined, since the transition set is
				  // non-empty
		}
		auto itAux = it;
		if (itAux->getConsequentState() != state)
		{
			itAux++;
			if (itAux != bfsDAG.getTransitions().end())
			{
				if (itAux->getConsequentState() != state)
				{
					std::cout
						<< "Error: No transition with consequent equal to the "
						   "input state.";
					exit(20);
				}
			}
			else
			{
				std::cout << "Error: No transition with consequent equal to "
							 "the input state.";
				exit(20);
			}
		}
		node->set_nodeType(itAux->getTransitionContent().getSymbol());
		node->set_S(state);
		vector<shared_ptr<StateTreeNode>> children;
		for (size_t i = 0; i < itAux->getAntecedentStates().size(); i++)
		{
			shared_ptr<StateTreeNode> child(new StateTreeNode);
			child->set_parent(node);
			child->set_kernel(node->get_kernel());
			children.push_back(child);
			extractCounterExampleStateTreeNode(itAux->getAntecedentStates()[i],
											   child);
		}
		node->set_children(children);
	}
	else
	{
		std::cout << "Error: The automaton has no transitions." << std::endl;
		exit(20);
	}
}

void BreadthFirstSearch::search()
{
	bool printStateFlag = flags->get("PrintStates");

	std::unique_ptr<TreeWidzard::Certificates::CertificateWriter> cert;
	std::unordered_map<State::ptr, int, State::ptr::Hash, State::ptr::Equal>
		cert_ids;
	int next_cert_id = 1;
	if (const char *cert_path = std::getenv("TREEWIDZARD_CERT_PATH"))
	{
		if (std::string(cert_path).size() > 0)
		{
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
		}
	}
	std::cout << left << setw(25) << "Iteration" << setw(25) << "ALLSTATES"
			  << setw(25) << "NEWSTATES"
			  << "Max WITNESSSET SIZE" << std::endl;
	auto print_transition = [&](const TreeWidzard::BreadthFirstExpansionEvent &event) {
		if (!printStateFlag)
		{
			return;
		}

		std::cout << std::endl;
		switch (event.kind)
		{
			case TreeWidzard::BreadthFirstExpansionKind::IntroVertex:
				std::cout << "================================="
							 "================"
							 "======================="
						  << std::endl;
				std::cout << " Introduce Vertex: " << event.value << std::endl;
				break;
			case TreeWidzard::BreadthFirstExpansionKind::ForgetVertex:
				std::cout << "====================================="
							 "=============="
							 "====================="
						  << std::endl;
				std::cout << " Forget Vertex: " << event.value << std::endl;
				break;
			case TreeWidzard::BreadthFirstExpansionKind::IntroEdge:
				std::cout << "==============================="
							 "=============="
							 "==========================="
						  << std::endl;
				std::cout << " Introduce Edge: " << event.value << " "
						  << event.secondary_value << std::endl;
				break;
			case TreeWidzard::BreadthFirstExpansionKind::Join:
				std::cout << "============================="
							 "=================="
							 "========================="
						  << std::endl;
				std::cout << " Join: " << std::endl;
				break;
		}
		std::cout << "================================="
					 "================"
					 "======================="
				  << std::endl;
		std::cout << " Current State:" << std::endl;
		event.first_parent.print();
		if (event.kind == TreeWidzard::BreadthFirstExpansionKind::Join &&
			event.second_parent.has_value())
		{
			std::cout << " State Two:" << std::endl;
			event.second_parent->print();
		}
		std::cout << " New State:" << std::endl;
		event.consequent_state.print();
		std::cout << "================================="
					 "================"
					 "======================="
				  << std::endl;
		std::cout << std::endl;
	};

	TreeWidzard::BreadthFirstTraversalHooks hooks;
	hooks.on_initial_state = [&](const State::ptr &initialState) {
		if (cert)
		{
			cert_ids.emplace(initialState, 0);
			cert->writeLeaf(0);
		}
		if (!noBFSDAG)
		{
			bfsDAG.addState(initialState);
			InstructiveTreeDecompositionNodeContent initialTransitionContent("Leaf");
			vector<State::ptr> initialAntecedents;
			Transition<State::ptr, InstructiveTreeDecompositionNodeContent>
				initialTransition(initialState, initialTransitionContent,
								  initialAntecedents);
			bfsDAG.addTransition(initialTransition);
		}
	};
	hooks.on_state_discovered =
		[&](const TreeWidzard::BreadthFirstExpansionEvent &event) {
			if (cert)
			{
				const int new_id = next_cert_id++;
				if (event.kind ==
					TreeWidzard::BreadthFirstExpansionKind::Join)
				{
					cert_ids.emplace(event.consequent_state, new_id);
					cert->writeJoin(new_id, cert_ids.at(event.first_parent),
									cert_ids.at(*event.second_parent),
									std::nullopt);
				}
				else if (event.kind ==
						 TreeWidzard::BreadthFirstExpansionKind::IntroVertex)
				{
					cert_ids.emplace(event.consequent_state, new_id);
					cert->writeIntroVertex(new_id, cert_ids.at(event.first_parent),
										   event.value);
				}
				else if (event.kind ==
						 TreeWidzard::BreadthFirstExpansionKind::ForgetVertex)
				{
					cert_ids.emplace(event.consequent_state, new_id);
					cert->writeForgetVertex(new_id, cert_ids.at(event.first_parent),
											event.value);
				}
				else if (event.kind ==
						 TreeWidzard::BreadthFirstExpansionKind::IntroEdge)
				{
					cert_ids.emplace(event.consequent_state, new_id);
					cert->writeIntroEdge(new_id, cert_ids.at(event.first_parent),
										 event.value, event.secondary_value);
				}
			}

			if (!noBFSDAG)
			{
				bfsDAG.addState(event.consequent_state);
				vector<State::ptr> antecedentStates;
				antecedentStates.push_back(event.first_parent);
				InstructiveTreeDecompositionNodeContent transitionContent("Leaf");
				switch (event.kind)
				{
					case TreeWidzard::BreadthFirstExpansionKind::IntroVertex:
						transitionContent.setSymbol("IntroVertex_" +
													to_string(event.value));
						break;
					case TreeWidzard::BreadthFirstExpansionKind::ForgetVertex:
						transitionContent.setSymbol("ForgetVertex_" +
													to_string(event.value));
						break;
					case TreeWidzard::BreadthFirstExpansionKind::IntroEdge:
						transitionContent.setSymbol("IntroEdge_" +
													to_string(event.value) + "_" +
													to_string(event.secondary_value));
						break;
					case TreeWidzard::BreadthFirstExpansionKind::Join:
						transitionContent.setSymbol("Join");
						antecedentStates.push_back(*event.second_parent);
						break;
				}
				Transition<State::ptr, InstructiveTreeDecompositionNodeContent>
					transition(event.consequent_state, transitionContent,
							   antecedentStates);
				bfsDAG.addTransition(transition);
			}

			print_transition(event);
		};
	hooks.on_iteration_complete =
		[&](int iteration,
			size_t allStatesSize,
			size_t newStatesSize,
			const vector<unsigned> &numberOfWitnesses) {
			if (flags->get("LoopTime") != 1)
			{
				return;
			}

			std::cout << left << setw(25) << iteration << setw(25)
					  << allStatesSize << setw(25) << newStatesSize;
			for (size_t component = 0; component < numberOfWitnesses.size();
				 ++component)
			{
				std::cout << numberOfWitnesses[component];
				if (component != numberOfWitnesses.size() - 1)
				{
					std::cout << ",";
				}
			}
			std::cout << std::endl;
		};

	TreeWidzard::BreadthFirstTraversalResult traversal_result =
		TreeWidzard::runBreadthFirstTraversal(*kernel, *conjecture, *flags, hooks);

	if (!traversal_result.property_satisfied)
	{
		State::ptr badState = *traversal_result.counterexample_state;
		std::cout << "\nResult: ";
		std::cout << "PROPERTY NOT SATISFIED\n"
				  << std::endl;
		if (cert)
		{
			cert->writeResultNotSatisfied(cert_ids.at(badState));
		}
		std::cout << "Counterexample found:" << std::endl;
		if (noBFSDAG)
		{
			std::cerr << "Rerun without -no-bfs-dag to construct a "
						 "counter example."
					  << std::endl;
			return;
		}

		std::cout << "The assignment that makes the formula false:"
				  << std::endl;
		conjecture->printValues(*badState, conjecture->getRoot());
		std::cout << std::endl;

		bfsDAG.addFinalState(badState);
		InstructiveTreeDecomposition atd =
			extractCounterExampleTerm(badState);
		string file = this->getOutputsPath();
		if (flags->get("Premise"))
		{
			file += "_Premise";
		}
		file += "_CounterExample";
		ConcreteTreeDecomposition ctd =
			atd.convertToConcreteTreeDecomposition();
		RunTree<State::ptr, InstructiveTreeDecompositionNodeContent>
			runTree = extractCounterExampleRun(badState);
		MultiGraph multiGraph = ctd.extractMultiGraph();
		multiGraph.printGraph();
		atd.writeToFile(file + "_ITD.txt");
		ctd.writeToFile(file + "_ConcreteDecomposition.txt");
		runTree.writeToFile(file + "_RunTree.txt");
		multiGraph.printToFile(file + "_Graph.txt");
		multiGraph.convertToGML(file + "_GMLGraph.gml");
		multiGraph.printToFilePACEFormat(file + "_GraphPaceFormat.gr");

		if (flags->get("PrintDirectedBipartiteGraphNAUTY"))
		{
			multiGraph.printToFileDirectedBipartiteGraphNAUTY(
				file + "_DirectedBipartiteGraphNAUTY.txt");
		}

		return;
	}

	std::cout << "\nResult: ";
	std::cout << "PROPERTY SATISFIED" << std::endl;
	if (cert)
	{
		cert->writeResultSatisfied();
	}
}
RunTree<State::ptr, InstructiveTreeDecompositionNodeContent>
BreadthFirstSearch::extractCounterExampleRun(State::ptr state)
{
	RunTree<State::ptr, InstructiveTreeDecompositionNodeContent> runTree =
		bfsDAG.retrieveRunAcyclicAutomaton(state);
	return runTree;
}
