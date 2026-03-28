#include "BreadthFirstSearch.h"

#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <utility>

#include "Kernel/BreadthFirstTraversal.h"
#include "Kernel/CertificateUtils.h"
#include "Kernel/CertificateWriter.h"

namespace {

using CounterexampleTrace = std::unordered_map<
	State::ptr,
	Transition<State::ptr, InstructiveTreeDecompositionNodeContent>,
	State::ptr::Hash,
	State::ptr::Equal>;

auto lookup_transition(const CounterexampleTrace &trace, const State::ptr &state)
	-> const Transition<State::ptr, InstructiveTreeDecompositionNodeContent> & {
	const auto it = trace.find(state);
	if (it == trace.end())
	{
		std::cerr << "Error: No transition with consequent equal to the input "
					 "state."
				  << std::endl;
		std::exit(20);
	}
	return it->second;
}

void record_transition(CounterexampleTrace &trace, const State::ptr &state,
					   const std::string &symbol,
					   const std::vector<State::ptr> &antecedents)
{
	InstructiveTreeDecompositionNodeContent content(symbol);
	Transition<State::ptr, InstructiveTreeDecompositionNodeContent> candidate(
		state, content, antecedents);
	const auto [it, inserted] = trace.emplace(state, candidate);
	if (!inserted && candidate < it->second)
	{
		it->second = std::move(candidate);
	}
}

void populate_term_node(
	const CounterexampleTrace &trace, const State::ptr &state,
	const shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> &node)
{
	const auto &transition = lookup_transition(trace, state);
	node->setNodeContent(transition.getTransitionContent());
	for (const auto &antecedent : transition.getAntecedentStates())
	{
		shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> child(
			new TermNode<InstructiveTreeDecompositionNodeContent>);
		child->setParent(node);
		node->addChild(child);
		populate_term_node(trace, antecedent, child);
	}
}

void populate_run_node(
	const CounterexampleTrace &trace, const State::ptr &state,
	const shared_ptr<TermNode<
		RunNodeContent<State::ptr, InstructiveTreeDecompositionNodeContent>>> &node)
{
	const auto &transition = lookup_transition(trace, state);
	RunNodeContent<State::ptr, InstructiveTreeDecompositionNodeContent> runNode;
	runNode.setState(state);
	runNode.setRunNodeContent(transition.getTransitionContent());
	node->setNodeContent(runNode);

	std::vector<shared_ptr<TermNode<
		RunNodeContent<State::ptr, InstructiveTreeDecompositionNodeContent>>>>
		children;
	children.reserve(transition.getAntecedentStates().size());
	for (const auto &antecedent : transition.getAntecedentStates())
	{
		shared_ptr<TermNode<RunNodeContent<
			State::ptr, InstructiveTreeDecompositionNodeContent>>>
			child(new TermNode<RunNodeContent<
				  State::ptr, InstructiveTreeDecompositionNodeContent>>);
		child->setParent(node);
		populate_run_node(trace, antecedent, child);
		children.push_back(child);
	}
	node->setChildren(children);
}

void populate_state_tree_node(const CounterexampleTrace &trace,
							  const State::ptr &state,
							  const shared_ptr<StateTreeNode> &node)
{
	const auto &transition = lookup_transition(trace, state);
	node->set_nodeType(transition.getTransitionContent().getSymbol());
	node->set_S(state);

	vector<shared_ptr<StateTreeNode>> children;
	children.reserve(transition.getAntecedentStates().size());
	for (const auto &antecedent : transition.getAntecedentStates())
	{
		shared_ptr<StateTreeNode> child(new StateTreeNode);
		child->set_parent(node);
		child->set_kernel(node->get_kernel());
		populate_state_tree_node(trace, antecedent, child);
		children.push_back(child);
	}
	node->set_children(children);
}

} // namespace

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
	shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> rootNode(
		new TermNode<InstructiveTreeDecompositionNodeContent>);
	rootNode->setParent(nullptr);
	populate_term_node(counterexampleTrace, state, rootNode);
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
	populate_state_tree_node(counterexampleTrace, state, node);
}

void BreadthFirstSearch::search()
{
	bool printStateFlag = flags->get("PrintStates");
	counterexampleTrace.clear();

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
			record_transition(counterexampleTrace, initialState, "Leaf", {});
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
				vector<State::ptr> antecedentStates;
				antecedentStates.push_back(event.first_parent);
				std::string symbol = "Leaf";
				switch (event.kind)
				{
					case TreeWidzard::BreadthFirstExpansionKind::IntroVertex:
						symbol = "IntroVertex_" + to_string(event.value);
						break;
					case TreeWidzard::BreadthFirstExpansionKind::ForgetVertex:
						symbol = "ForgetVertex_" + to_string(event.value);
						break;
					case TreeWidzard::BreadthFirstExpansionKind::IntroEdge:
						symbol = "IntroEdge_" + to_string(event.value) + "_" +
								 to_string(event.secondary_value);
						break;
					case TreeWidzard::BreadthFirstExpansionKind::Join:
						symbol = "Join";
						antecedentStates.push_back(*event.second_parent);
						break;
				}
				record_transition(counterexampleTrace, event.consequent_state,
								 symbol, antecedentStates);
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
		const bool printStateTreeFlag = flags->get("StateTree");
		const bool writeFilesFlag = flags->get("WriteToFiles");
		const bool printDirectedBipartiteGraphFlag =
			flags->get("PrintDirectedBipartiteGraphNAUTY");
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

		if (printStateTreeFlag)
		{
			std::cout << "Counterexample state tree:" << std::endl;
			StateTree stateTree = extractCounterExampleStateTree(badState);
			stateTree.printStateTree();
			std::cout << std::endl;
		}

		string file = this->getOutputsPath();
		if (flags->get("Premise"))
		{
			file += "_Premise";
		}
		file += "_CounterExample";

		if (writeFilesFlag || printDirectedBipartiteGraphFlag)
		{
			InstructiveTreeDecomposition atd =
				extractCounterExampleTerm(badState);
			ConcreteTreeDecomposition ctd =
				atd.convertToConcreteTreeDecomposition();
			RunTree<State::ptr, InstructiveTreeDecompositionNodeContent>
				runTree = extractCounterExampleRun(badState);
			MultiGraph multiGraph = ctd.extractMultiGraph();
			multiGraph.printGraph();
			if (writeFilesFlag)
			{
				atd.writeToFile(file + "_ITD.txt");
				ctd.writeToFile(file + "_ConcreteDecomposition.txt");
				runTree.writeToFile(file + "_RunTree.txt");
				multiGraph.printToFile(file + "_Graph.txt");
				multiGraph.convertToGML(file + "_GMLGraph.gml");
				multiGraph.printToFilePACEFormat(file + "_GraphPaceFormat.gr");
			}

			if (printDirectedBipartiteGraphFlag)
			{
				multiGraph.printToFileDirectedBipartiteGraphNAUTY(
					file + "_DirectedBipartiteGraphNAUTY.txt");
			}
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
	shared_ptr<TermNode<RunNodeContent<
		State::ptr, InstructiveTreeDecompositionNodeContent>>>
		root(new TermNode<RunNodeContent<
			 State::ptr, InstructiveTreeDecompositionNodeContent>>);
	root->setParent(nullptr);
	populate_run_node(counterexampleTrace, state, root);
	RunTree<State::ptr, InstructiveTreeDecompositionNodeContent> runTree;
	runTree.setRoot(root);
	return runTree;
}
