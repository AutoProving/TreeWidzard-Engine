// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "TreeDecompositionPACE.h"

namespace {

[[noreturn]] void report_tree_decomposition_error(const std::string &message) {
	std::cout << "ERROR in TreeDecompositionPACE: " << message << std::endl;
	exit(20);
}

void require_vertex_label(unsigned vertex, size_t max_vertex_label,
						  const char *context) {
	if (vertex == 0 || vertex > max_vertex_label) {
		report_tree_decomposition_error(std::string(context) +
										" references vertex " +
										std::to_string(vertex) +
										" outside the graph range 1.." +
										std::to_string(max_vertex_label));
	}
}

} // namespace

RawInstructiveTreeDecomposition::RawInstructiveTreeDecomposition() {
	// constructor
}
void RawInstructiveTreeDecomposition::print(unsigned &k, unsigned parentno) {
	k++;
	std::cout << k << " " << type << "(" << parentno << ") ";
	bag.print();
	for (auto it = color_to_vertex_map.begin(); it != color_to_vertex_map.end();
		 it++) {
		std::cout << it->first << "->" << it->second << " ";
	}
	std::cout << std::endl;
	unsigned num = k;
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->print(k, num);
	}
}

void RawInstructiveTreeDecomposition::printNode() {
	bag.print();
	for (auto it = color_to_vertex_map.begin(); it != color_to_vertex_map.end();
		 it++) {
		std::cout << it->first << "->" << it->second << " ";
	}
}

TreeDecompositionPACE::TreeDecompositionPACE() {
	num_vertices = 0;
	num_graph_vertices = 0;
	num_edges = 0;
	width = 0;
}

void TreeDecompositionPACE::setNum_vertices(unsigned n) {
	bags.resize(n);
	adjacency.clear();
	adjacency.resize(n + 1);
	num_vertices = n;
}

void TreeDecompositionPACE::setNum_graph_vertices(unsigned n) {
	num_graph_vertices = n;
}

void TreeDecompositionPACE::setWidth(unsigned w) { width = w; }

bool TreeDecompositionPACE::addVertexToBag(unsigned vertex,
										   unsigned bagnumber) {
	if (bagnumber > 0) {
		bags[bagnumber - 1].insert(vertex);
	} else {
		return false;
	}
	return true;
}

bool TreeDecompositionPACE::setABag(vertex_t s, unsigned bagnumber) {
	if (bagnumber > 0 and bagnumber <= bags.size()) {
		bags[bagnumber - 1] = s;
		return true;
	} else {
		return false;
	}
}

bool TreeDecompositionPACE::addEdge(unsigned e1, unsigned e2) {
	if (e1 <= bags.size() and e2 <= bags.size()) {
		if (e1 > e2) {
			edges.insert(std::make_pair(e2, e1));
			adjacency[e2].push_back(e1);
			adjacency[e1].push_back(e2);
			return true;
		} else if (e1 < e2) {
			edges.insert(std::make_pair(e1, e2));
			adjacency[e1].push_back(e2);
			adjacency[e2].push_back(e1);
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

void TreeDecompositionPACE::printBags() {
	for (size_t i = 0; i < bags.size(); i++) {
		std::cout << "b " << i + 1 << " ";
		for (auto it = bags[i].begin(); it != bags[i].end(); it++) {
			std::cout << *it;
			if (next(it) != bags[i].end()) {
				std::cout << " ";
			}
		}
		std::cout << "" << std::endl;
	}
}

void TreeDecompositionPACE::printEdges() {
	for (auto it = edges.begin(); it != edges.end(); it++) {
		std::cout << it->first << "  " << it->second << std::endl;
	}
}

void TreeDecompositionPACE::print() {
	std::cout << "s td " << num_vertices << "  " << width + 1 << "  "
			  << num_graph_vertices << std::endl;
	printBags();
	printEdges();
}

void TreeDecompositionPACE::printTree() {
	unsigned k = 0;
	if (root == nullptr) {
		std::cout << "\033[1;31mError in TreeDecompositionPACE::printTree : "
					 "the root is not set!\033[0m"
				  << std::endl;
	} else {
		root->print(k, 0);
	}
}

void TreeDecompositionPACE::constructInnerNodes(
	std::set<unsigned> &visited_bags, unsigned current,
	std::shared_ptr<RawInstructiveTreeDecomposition> parent) {
	std::vector<std::pair<unsigned, std::shared_ptr<RawInstructiveTreeDecomposition>>> stack;
	stack.emplace_back(current, parent);

	while (!stack.empty()) {
		auto [bag_index, attach_parent] = stack.back();
		stack.pop_back();

		if (visited_bags.count(bag_index)) {
			continue;
		}

		const auto &bag_elements = bags[bag_index - 1];
		if (bag_elements == attach_parent->bag.get_elements()) {
			visited_bags.insert(bag_index);
			for (unsigned neighbor : adjacency[bag_index]) {
				if (!visited_bags.count(neighbor)) {
					stack.emplace_back(neighbor, attach_parent);
				}
			}
			continue;
		}

		auto node = std::make_shared<RawInstructiveTreeDecomposition>();
		node->bag.set_elements(bag_elements);
		node->parent = attach_parent;
		attach_parent->children.push_back(node);
		visited_bags.insert(bag_index);

		for (unsigned neighbor : adjacency[bag_index]) {
			if (!visited_bags.count(neighbor)) {
				stack.emplace_back(neighbor, node);
			}
		}
	}
}

bool TreeDecompositionPACE::constructRaw() {
	std::shared_ptr<RawInstructiveTreeDecomposition> root_node(new RawInstructiveTreeDecomposition);
	root = root_node;

	// std::cerr << "edges: ";
	// for (auto [u, v] : edges) std::cerr << '(' << u << ',' << v << ") ";
	// std::cerr << '\n';
	// std::cerr << "root:\n";
	// root->printNode();
	
	if (bags.size() > 0) {
		root->bag.set_elements(bags[0]);
		std::set<unsigned> visited_bags;
		visited_bags.insert(1);
		// create nodes for neighbors using adjacency information
		for (unsigned neighbor : adjacency[1]) {
			if (!visited_bags.count(neighbor)) {
				constructInnerNodes(visited_bags, neighbor, root);
			}
		}
		if (visited_bags.size() != bags.size()) {
			std::cout << "ERROR in TreeDecompositionPACE::constructRaw tree "
						 "decomposition is disconnected" << std::endl;
			return false;
		}
		return true;
	} else {
		std::cout
			<< "ERROR in TreeDecompositionPACE::constructRaw bags size is zero"
			<< std::endl;
		return false;
	}
}

bool TreeDecompositionPACE::convertToBinary(
	std::shared_ptr<RawInstructiveTreeDecomposition> node) {
	if (!node) {
		return true;
	}
	std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>> stack;
	stack.push_back(node);

	while (!stack.empty()) {
		auto current = stack.back();
		stack.pop_back();
		if (!current) {
			continue;
		}

		while (current->children.size() > 2) {
			auto new_node = std::make_shared<RawInstructiveTreeDecomposition>();
			new_node->bag = current->bag;
			for (size_t i = 1; i < current->children.size(); ++i) {
				new_node->children.push_back(current->children[i]);
				current->children[i]->parent = new_node;
			}
			current->children.resize(1);
			current->children.push_back(new_node);
			new_node->parent = current;
		}

		for (auto &child : current->children) {
			stack.push_back(child);
		}
	}
	return true;
}

bool TreeDecompositionPACE::joinFormat(
	std::shared_ptr<RawInstructiveTreeDecomposition> node) {
	if (!node) {
		return true;
	}
	std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>> stack;
	stack.push_back(node);
	while (!stack.empty()) {
		auto current = stack.back();
		stack.pop_back();
		if (!current) {
			continue;
		}
		if (current->children.size() > 2) {
			std::cout << "ERROR in TreeDecompositionPACE::joinFormat, the Raw "
						 "instructive tree decomposition is not in binary form"
					  << std::endl;
			exit(20);
		}
		if (current->children.size() == 2) {
			current->type = "Join";
			for (size_t idx = 0; idx < 2; ++idx) {
				auto child = current->children[idx];
				if (!(current->bag == child->bag)) {
					auto new_node =
						std::make_shared<RawInstructiveTreeDecomposition>();
					new_node->bag = current->bag;
					new_node->children.push_back(child);
					child->parent = new_node;
					current->children[idx] = new_node;
					new_node->parent = current;
					child = new_node->children[0];
				}
			}
			stack.push_back(current->children[0]);
			stack.push_back(current->children[1]);
		} else if (current->children.size() == 1) {
			stack.push_back(current->children[0]);
		}
	}
	return true;
}

bool TreeDecompositionPACE::addEmptyNodes(
	std::shared_ptr<RawInstructiveTreeDecomposition> node) {
	if (!node) {
		return true;
	}
	std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>> stack;
	stack.push_back(node);
	while (!stack.empty()) {
		auto current = stack.back();
		stack.pop_back();
		if (!current) continue;
		if (current->children.empty()) {
			if (!current->bag.get_elements().empty()) {
				auto empty_node = std::make_shared<RawInstructiveTreeDecomposition>();
				empty_node->type = "Leaf";
				empty_node->parent = current;
				current->children.push_back(empty_node);
			} else {
				current->type = "Leaf";
			}
		} else {
			for (auto &child : current->children) {
				stack.push_back(child);
			}
		}
	}
	return true;
}

bool TreeDecompositionPACE::addIntroVertex(
	std::shared_ptr<RawInstructiveTreeDecomposition> node) {
	if (!node)
		return true;
	std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>> stack;
	stack.push_back(node);

	while (!stack.empty()) {
		auto current = stack.back();
		stack.pop_back();
		if (!current)
			continue;

		const size_t child_count = current->children.size();
		if (child_count == 1) {
			auto child = current->children[0];
			std::set<unsigned> set_diff;
			const auto &elements_node = current->bag.get_elements();
			const auto &child_elements = child->bag.get_elements();
			set_difference(elements_node.begin(), elements_node.end(),
						   child_elements.begin(), child_elements.end(),
						   inserter(set_diff, set_diff.begin()));
			if (set_diff.size() > 1) {
				auto new_node =
					std::make_shared<RawInstructiveTreeDecomposition>();
				new_node->children.push_back(child);
				child->parent = new_node;
				current->children[0] = new_node;
				new_node->parent = current;
				auto elements_node_new = elements_node;
				elements_node_new.erase(*set_diff.begin());
				new_node->bag.set_elements(elements_node_new);
				current->type =
					"IntroVertex_" + std::to_string(*set_diff.begin());
				stack.push_back(new_node);
				continue;
			} else if (set_diff.size() == 1) {
				current->type =
					"IntroVertex_" + std::to_string(*set_diff.begin());
				stack.push_back(child);
			} else if (elements_node == child_elements) {
				std::cerr << "duplicated bags" << std::endl;
				exit(20);
			} else {
				stack.push_back(child);
			}
		} else if (child_count == 2) {
			stack.push_back(current->children[0]);
			stack.push_back(current->children[1]);
		} else if (child_count > 2) {
			std::cout << "ERROR in TreeDecompositionPACE::addIntroVertex, node has "
						 "more than two children. Number of Children = "
					  << child_count << std::endl;
			exit(20);
		}
	}
	return true;
}

bool TreeDecompositionPACE::addForgetVertex(
	std::shared_ptr<RawInstructiveTreeDecomposition> node) {
	if (!node)
		return true;
	std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>> stack;
	stack.push_back(node);

	while (!stack.empty()) {
		auto current = stack.back();
		stack.pop_back();
		if (!current) continue;

		if (current->children.size() == 1) {
			auto child = current->children[0];
			std::set<unsigned> set_diff;
			const auto &elements_node = current->bag.get_elements();
			const auto &elements_node_child = child->bag.get_elements();
			set_difference(elements_node_child.begin(),
						   elements_node_child.end(), elements_node.begin(),
						   elements_node.end(),
						   inserter(set_diff, set_diff.begin()));
			if (set_diff.size() > 1) {
				if (current->type == "") {
					auto new_node =
						std::make_shared<RawInstructiveTreeDecomposition>();
					new_node->children.push_back(child);
					child->parent = new_node;
					current->children[0] = new_node;
					new_node->parent = current;
					auto elements_node_new = elements_node;
					elements_node_new.insert(*set_diff.rbegin());
					new_node->bag.set_elements(elements_node_new);
					current->type =
						"ForgetVertex_" + std::to_string(*set_diff.rbegin());
					stack.push_back(new_node);
					continue;
				} else {
					set_diff.clear();
					set_difference(elements_node.begin(), elements_node.end(),
								   elements_node_child.begin(),
								   elements_node_child.end(),
								   inserter(set_diff, set_diff.begin()));
					if (set_diff.size() != 1) {
						std::cout << "Error in TreeDecompositionPACE::addForgetVertex, "
									 "set_diff size is not 1.";
						exit(20);
					}
					auto new_node =
						std::make_shared<RawInstructiveTreeDecomposition>();
					new_node->children.push_back(child);
					child->parent = new_node;
					current->children[0] = new_node;
					new_node->parent = current;
					auto elements_node_new = elements_node;
					elements_node_new.erase(*set_diff.begin());
					new_node->bag.set_elements(elements_node_new);
					stack.push_back(new_node);
					continue;
				}
			} else if (set_diff.size() == 1) {
				if (current->type == "") {
					current->type =
						"ForgetVertex_" + std::to_string(*set_diff.begin());
					stack.push_back(child);
				} else {
					set_diff.clear();
					set_difference(elements_node.begin(), elements_node.end(),
								   elements_node_child.begin(),
								   elements_node_child.end(),
								   inserter(set_diff, set_diff.begin()));
					if (set_diff.size() != 1) {
						std::cout << "Error in TreeDecompositionPACE::addForgetVertex, "
									 "set_diff size is not 1.";
						exit(20);
					}
					auto new_node =
						std::make_shared<RawInstructiveTreeDecomposition>();
					new_node->children.push_back(child);
					child->parent = new_node;
					current->children[0] = new_node;
					new_node->parent = current;
					auto elements_node_new = elements_node;
					elements_node_new.erase(*set_diff.begin());
					new_node->bag.set_elements(elements_node_new);
					stack.push_back(new_node);
				}
			} else {
				stack.push_back(child);
			}
		} else if (current->children.size() == 2) {
			stack.push_back(current->children[0]);
			stack.push_back(current->children[1]);
		}
		for (auto &child : current->children) {
			if (current->type == child->type && current->type != "Join") {
				std::cout << "ERROR in TreeDecompositionPACE::addIntroVertex, has "
							 "same ForgetVertex type"
						  << std::endl;
				exit(20);
			}
		}
	}
	return true;
}

bool TreeDecompositionPACE::addIntroEdge(
	std::shared_ptr<RawInstructiveTreeDecomposition> node,
	std::set<unsigned> &visited_edges) {
	if (!node)
		return true;
	std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>> stack;
	stack.push_back(node);

	while (!stack.empty()) {
		auto current = stack.back();
		stack.pop_back();
		if (!current)
			continue;

		if (strstr(current->type.c_str(), "IntroVertex")) {
			std::set<unsigned> set_diff;
			const auto &elements_node = current->bag.get_elements();
			const auto &elements_node_child =
				current->children[0]->bag.get_elements();
			set_difference(elements_node.begin(), elements_node.end(),
						   elements_node_child.begin(),
						   elements_node_child.end(),
						   inserter(set_diff, set_diff.begin()));
			if (set_diff.size() != 1) {
				std::cout
					<< "ERROR in TreeDecompositionPACE::addIntroEdgeRecursion, "
					   "set_diff is not verified"
					<< std::endl;
				current->bag.print();
				std::cout << "\n" << current->type << std::endl;
				current->children[0]->bag.print();
				std::cout << "\n" << current->children[0]->type << std::endl;
				exit(20);
			}
			unsigned introducedVertex = static_cast<unsigned>(*set_diff.begin());
			std::multimap<unsigned, unsigned> incidence =
				multigraph->getIncidenceMap();
			std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>>
				generated_nodes;
			for (auto it = incidence.begin(); it != incidence.end(); ++it) {
				if (it->second != introducedVertex ||
					visited_edges.count(it->first))
					continue;
				for (auto itr = incidence.begin(); itr != incidence.end();
					 ++itr) {
					if (itr->second == it->second || itr->first != it->first)
						continue;
					if (current->bag.get_elements().count(itr->second) == 0)
						continue;
					visited_edges.insert(it->first);
					auto new_node =
						std::make_shared<RawInstructiveTreeDecomposition>();
					new_node->bag = current->bag;
					if (it->second < itr->second) {
						new_node->type =
							"IntroEdge_" + std::to_string(it->second) + "_" +
							std::to_string(itr->second);
						new_node->bag.set_edge(it->second, itr->second);
					} else {
						new_node->type =
							"IntroEdge_" + std::to_string(itr->second) + "_" +
							std::to_string(it->second);
						new_node->bag.set_edge(itr->second, it->second);
					}
					generated_nodes.push_back(new_node);
					break;
				}
			}
			for (size_t i = 0; i < generated_nodes.size(); ++i) {
				if (i == 0) {
					if (!current->parent) {
						root = generated_nodes[0];
					} else {
						generated_nodes[0]->parent = current->parent;
						for (auto &child : current->parent->children) {
							if (child == current) {
								child = generated_nodes[0];
								break;
							}
						}
					}
				} else {
					generated_nodes[i]->parent = generated_nodes[i - 1];
					generated_nodes[i - 1]->children.push_back(
						generated_nodes[i]);
				}
			}
			if (!generated_nodes.empty()) {
				generated_nodes.back()->children.push_back(current);
				current->parent = generated_nodes.back();
			}
			stack.push_back(current->children[0]);
		} else if (strstr(current->type.c_str(), "Join")) {
			stack.push_back(current->children[0]);
			stack.push_back(current->children[1]);
		} else if (strstr(current->type.c_str(), "ForgetVertex")) {
			stack.push_back(current->children[0]);
		} else if (strstr(current->type.c_str(), "Leaf")) {
			continue;
		} else {
			std::cout << "ERROR in TreeDecompositionPACE::addIntroEdgeRecursion, "
						 "node type is not satisfied"
					  << std::endl;
			exit(20);
		}
	}
	return true;
}
bool TreeDecompositionPACE::colorNode(
	std::shared_ptr<RawInstructiveTreeDecomposition> node,
	std::vector<unsigned> color_vertex, std::vector<unsigned> vertex_color) {
	struct ColoringFrame {
		std::shared_ptr<RawInstructiveTreeDecomposition> node;
		std::vector<unsigned> color_vertex;
		std::vector<unsigned> vertex_color;

		ColoringFrame(std::shared_ptr<RawInstructiveTreeDecomposition> n,
					  std::vector<unsigned> cv,
					  std::vector<unsigned> vv)
			: node(std::move(n)),
			  color_vertex(std::move(cv)),
			  vertex_color(std::move(vv)) {}
	};

	std::vector<ColoringFrame> stack;
	stack.emplace_back(std::move(node), std::move(color_vertex),
					   std::move(vertex_color));

	while (!stack.empty()) {
		auto frame = std::move(stack.back());
		stack.pop_back();

		auto current_node = frame.node;
		auto &colors = frame.color_vertex;
		auto &vertex_colors = frame.vertex_color;

		if (!current_node->parent) {
			unsigned color = 1;
			const auto &elements = current_node->bag.get_elements();
			for (unsigned v : elements) {
				require_vertex_label(v, vertex_colors.size(),
									 "colorNode(root)");
				if (color > colors.size()) {
					report_tree_decomposition_error(
						"root bag requires more than width + 1 colors");
				}
				vertex_colors[v - 1] = color;
				colors[color - 1] = v;
				current_node->color_to_vertex_map.insert({color, v});
				++color;
			}
		} else {
			std::set<unsigned> set_diff;
			const auto &elements_node = current_node->bag.get_elements();
			const auto &elements_parent =
				current_node->parent->bag.get_elements();
			set_difference(elements_parent.begin(), elements_parent.end(),
						   elements_node.begin(), elements_node.end(),
						   inserter(set_diff, set_diff.begin()));
			current_node->color_to_vertex_map =
				current_node->parent->color_to_vertex_map;
			for (unsigned v : set_diff) {
				require_vertex_label(v, vertex_colors.size(),
									 "colorNode(forget)");
				unsigned color_id = vertex_colors[v - 1];
				if (color_id == 0 || color_id > colors.size()) {
					std::cout << "ERROR in TreeDecompositionPACE::colorNode "
								 "invalid color index.\n";
					exit(20);
				}
				colors[color_id - 1] = 0;
				current_node->color_to_vertex_map.erase(color_id);
			}
			set_diff.clear();
			set_difference(elements_node.begin(), elements_node.end(),
						   elements_parent.begin(), elements_parent.end(),
						   inserter(set_diff, set_diff.begin()));
			for (unsigned v : set_diff) {
				require_vertex_label(v, vertex_colors.size(),
									 "colorNode(introduce)");
				auto slot =
					std::find(colors.begin(), colors.end(), static_cast<unsigned>(0));
				if (slot == colors.end()) {
					std::cout << "ERROR in TreeDecompositionPACE::colorNode there "
								 "is no an available color!"
							  << std::endl;
					exit(20);
				}
				unsigned idx = static_cast<unsigned>(slot - colors.begin());
				colors[idx] = v;
				vertex_colors[v - 1] = idx + 1;
				current_node->color_to_vertex_map.insert({idx + 1, v});
			}
		}

		if (current_node->children.size() == 2) {
			// First child receives a copy, second child reuses moved vectors.
			stack.emplace_back(current_node->children[0], colors, vertex_colors);
			stack.emplace_back(current_node->children[1], std::move(colors),
							   std::move(vertex_colors));
		} else if (current_node->children.size() == 1) {
			stack.emplace_back(current_node->children[0], std::move(colors),
							   std::move(vertex_colors));
		}
	}

	return true;
}
bool TreeDecompositionPACE::colorTree() {
	std::vector<unsigned> color_vertex;
	std::vector<unsigned> vertex_color;
	color_vertex.resize(width + 1, 0);
	vertex_color.resize(
		std::max<size_t>(num_graph_vertices, multigraph->getVertices().size()),
		0);
	return colorNode(root, std::move(color_vertex), std::move(vertex_color));
}
bool TreeDecompositionPACE::updateInnerNodeTD(
	std::shared_ptr<RawInstructiveTreeDecomposition> node, unsigned &number,
	unsigned parentno) {
	struct Frame {
		std::shared_ptr<RawInstructiveTreeDecomposition> node;
		unsigned parent_index;
	};

	unsigned next_index = number;
	std::vector<Frame> stack;
	stack.push_back({std::move(node), parentno});

	while (!stack.empty()) {
		auto frame = stack.back();
		stack.pop_back();
		auto current_node = frame.node;
		unsigned parent_index = frame.parent_index;

		if (!current_node) {
			continue;
		}

		next_index++;
		unsigned current_index = next_index;
		bags.push_back(current_node->bag.get_elements());
		num_vertices++;
		num_edges += current_node->children.size();

		if (parent_index != 0) {
			edges.insert(std::make_pair(parent_index, current_index));
		}

		for (auto it = current_node->children.rbegin();
			 it != current_node->children.rend(); ++it) {
			stack.push_back({*it, current_index});
		}
	}

	number = next_index;
	return true;
}
bool TreeDecompositionPACE::updateTD() {
	bags.clear();
	edges.clear();
	num_edges = 0;
	num_vertices = 0;
	unsigned number = 0;
	unsigned parentno = 0;
	updateInnerNodeTD(root, number, parentno);
	return true;
}

void TreeDecompositionPACE::construct() {
	if (!validateParsedInput()) {
		report_tree_decomposition_error("parsed input is not a valid tree decomposition");
	}
	if (!constructRaw()) {
		report_tree_decomposition_error("failed to construct a connected raw tree decomposition");
	}
	convertToBinary(root);
	joinFormat(root);
	addEmptyNodes(root);
	addIntroVertex(root);
	addForgetVertex(root);
	std::set<unsigned> visited_edges;
	addIntroEdge(root, visited_edges);
	if (multigraph && visited_edges.size() != multigraph->getEdges().size()) {
		report_tree_decomposition_error(
			"tree decomposition does not cover every graph edge");
	}
	colorTree();
	updateTD();
	validateTree(root);
	// std::cout << "Construction finished" << std::endl;
}

void TreeDecompositionPACE::createCTDNode(
	std::shared_ptr<TermNode<ConcreteNode>> cnode,
	std::shared_ptr<RawInstructiveTreeDecomposition> rnode) {
	if (!rnode)
		return;
	struct Frame {
		std::shared_ptr<TermNode<ConcreteNode>> cnode;
		std::shared_ptr<RawInstructiveTreeDecomposition> rnode;
		bool processed;
	};
	std::vector<Frame> stack;
	stack.push_back({cnode, rnode, false});

	while (!stack.empty()) {
		auto frame = stack.back();
		stack.pop_back();
		auto current_cnode = frame.cnode;
		auto current_rnode = frame.rnode;
		if (!current_cnode || !current_rnode)
			continue;

		if (!frame.processed) {
			std::vector<std::shared_ptr<TermNode<ConcreteNode>>> children;
			children.reserve(current_rnode->children.size());
			for (const auto &child_rnode : current_rnode->children) {
				auto child_cnode = std::make_shared<TermNode<ConcreteNode>>();
				child_cnode->setParent(current_cnode);
				children.push_back(child_cnode);
				stack.push_back({child_cnode, child_rnode, false});
			}
			current_cnode->setChildren(children);
			stack.push_back({current_cnode, current_rnode, true});
			continue;
		}

		ConcreteNode concrete;
		concrete.setSymbol(current_rnode->type);
		Bag bag;
		std::set<unsigned> bag_elements;
		std::pair<unsigned, unsigned> e_new = std::make_pair(0, 0);
		auto e = current_rnode->bag.get_edge();
		for (const auto &kv : current_rnode->color_to_vertex_map) {
			bag_elements.insert(kv.first);
			if (e.first == kv.second) {
				e_new.first = kv.first;
			} else if (e.second == kv.second) {
				e_new.second = kv.first;
			}
		}
		if (e_new.first > e_new.second) {
			std::swap(e_new.first, e_new.second);
		}
		bag.set_elements(bag_elements);
		bag.set_edge(e_new.first, e_new.second);
		concrete.setBag(bag);

		if (strstr(current_rnode->type.c_str(), "IntroEdge_")) {
			std::string type = "IntroEdge_" + std::to_string(e_new.first) + "_" +
							   std::to_string(e_new.second);
			concrete.setSymbol(type);
		}
		current_cnode->setNodeContent(concrete);
	}
}

std::shared_ptr<ConcreteTreeDecomposition>
TreeDecompositionPACE::convertToConcreteTreeDecomposition() {
	std::shared_ptr<ConcreteTreeDecomposition> concreteTreeDecomposition(
		new ConcreteTreeDecomposition);
	std::shared_ptr<TermNode<ConcreteNode>> ctd_root(
		new TermNode<ConcreteNode>);
	concreteTreeDecomposition->setRoot(ctd_root);
	createCTDNode(ctd_root, root);
	return concreteTreeDecomposition;
}

void TreeDecompositionPACE::setWidthType(const std::string &widthType) {
	width_type = widthType;
}

unsigned int TreeDecompositionPACE::getWidth() const { return width; }

const std::string &TreeDecompositionPACE::getWidthType() const {
	return width_type;
}

bool TreeDecompositionPACE::validateParsedInput() const {
	if (!multigraph) {
		std::cout << "ERROR in TreeDecompositionPACE::validateParsedInput "
					 "multigraph is not set"
				  << std::endl;
		return false;
	}
	if (bags.empty()) {
		std::cout << "ERROR in TreeDecompositionPACE::validateParsedInput bags "
					 "are empty"
				  << std::endl;
		return false;
	}
	if (num_vertices != bags.size()) {
		std::cout << "ERROR in TreeDecompositionPACE::validateParsedInput number "
					 "of bags does not match the parsed header"
				  << std::endl;
		return false;
	}
	if (num_vertices > 0 && edges.size() + 1 != num_vertices) {
		std::cout << "ERROR in TreeDecompositionPACE::validateParsedInput tree "
					 "decomposition edges do not form a tree"
				  << std::endl;
		return false;
	}

	const auto &graph_vertices = multigraph->getVertices();
	const size_t max_vertex_label =
		std::max<size_t>(num_graph_vertices, graph_vertices.size());
	for (const auto &bag : bags) {
		if (bag.size() > width + 1) {
			std::cout << "ERROR in TreeDecompositionPACE::validateParsedInput bag "
						 "size exceeds width + 1"
					  << std::endl;
			return false;
		}
		for (unsigned vertex : bag) {
			if (vertex == 0 || vertex > max_vertex_label ||
				graph_vertices.count(vertex) == 0) {
				std::cout << "ERROR in TreeDecompositionPACE::validateParsedInput "
							 "bag references invalid graph vertex "
						  << vertex << std::endl;
				return false;
			}
		}
	}
	return true;
}

bool TreeDecompositionPACE::validateTree(
	std::shared_ptr<RawInstructiveTreeDecomposition> node) {
	if (!node) {
		return true;
	}

	std::vector<std::shared_ptr<RawInstructiveTreeDecomposition>> stack;
	stack.push_back(std::move(node));

	while (!stack.empty()) {
		auto current = stack.back();
		stack.pop_back();
		if (!current) {
			continue;
		}

		const auto child_count = current->children.size();
		if (child_count == 2) {
			if (current->type != "Join") {
				std::cout << "Error in TreeDecompositionPACE::validateTree, "
							 "join type"
						  << std::endl;
				exit(20);
			}
			if (current->children[0]->bag.get_elements() !=
				current->children[1]->bag.get_elements()) {
				std::cout
					<< "  Error in TreeDecompositionPACE::validateTree, "
					   "children of a join node do not have a same bagSet"
					<< std::endl;
				exit(20);
			}
			stack.push_back(current->children[0]);
			stack.push_back(current->children[1]);
		} else if (child_count == 1) {
			auto child = current->children[0];
			if (strstr(current->type.c_str(), "IntroVertex")) {
				std::set<unsigned> set_diff;
				const auto &elements = current->bag.get_elements();
				const auto &child_elements = child->bag.get_elements();
				set_difference(elements.begin(), elements.end(),
							   child_elements.begin(), child_elements.end(),
							   inserter(set_diff, set_diff.begin()));
				if (set_diff.size() != 1) {
					std::cout
						<< "Error in TreeDecompositionPACE::validateTree, "
						   "IntroVertex, set_diff is invalid"
						<< std::endl;
					exit(20);
				}
			} else if (strstr(current->type.c_str(), "ForgetVertex")) {
				std::set<unsigned> set_diff;
				const auto &elements = current->bag.get_elements();
				const auto &child_elements = child->bag.get_elements();
				set_difference(child_elements.begin(), child_elements.end(),
							   elements.begin(), elements.end(),
							   inserter(set_diff, set_diff.begin()));
				if (set_diff.size() != 1) {
					std::cout
						<< "Error in TreeDecompositionPACE::validateTree, "
						   "ForgetVertex, set_diff is invalid"
						<< std::endl;
					exit(20);
				}
			} else if (strstr(current->type.c_str(), "IntroEdge")) {
				std::pair<unsigned, unsigned> empty_edge = {0, 0};
				auto node_edge = current->bag.get_edge();
				if (node_edge == empty_edge) {
					std::cout
						<< "Error in TreeDecompositionPACE::validateTree, "
						   "IntroEdge, nodeEdge is invalid"
						<< std::endl;
					exit(20);
				}
			} else {
				std::cout << "Error in TreeDecompositionPACE::validateTree, "
							 "node type is invalid ("
						  << current->type << ")" << std::endl;
				exit(20);
			}
			stack.push_back(child);
		} else if (child_count == 0) {
			if (strstr(current->type.c_str(), "Leaf")) {
				if (!current->bag.get_elements().empty()) {
					std::cout
						<< "Error in TreeDecompositionPACE::validateTree, "
						   "Leaf is invalid"
						<< std::endl;
					exit(20);
				}
			} else {
				std::cout << "Error in TreeDecompositionPACE::validateTree, "
							 "node type is invalid ("
						  << current->type << ")" << std::endl;
				exit(20);
			}
		} else {
			std::cout << "Error in TreeDecompositionPACE::validateTree, node "
						 "children is invalid"
					  << std::endl;
			exit(20);
		}
	}
	return true;
}
