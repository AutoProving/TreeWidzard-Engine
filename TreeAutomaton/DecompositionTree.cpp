#include "DecompositionTree.h"

DecompositionNode::DecompositionNode() {
	parent = std::shared_ptr<DecompositionNode>(nullptr);
}

std::set<unsigned> DecompositionNode::getVertices() { return this->vertices; }

void DecompositionNode::addVertex(unsigned i) { this->vertices.insert(i); }

void DecompositionNode::setVertices(std::set<unsigned> vertices) {
	this->vertices = vertices;
}

void DecompositionNode::setParent(std::shared_ptr<DecompositionNode> node) {
	this->parent = node;
}

std::shared_ptr<DecompositionNode> DecompositionNode::getParent() {
	return this->parent;
}

std::vector<std::shared_ptr<DecompositionNode>>
DecompositionNode::getChildren() {
	return this->children;
}

void DecompositionNode::addChild(std::shared_ptr<DecompositionNode> child) {
	children.push_back(child);
}

void DecompositionNode::setChildren(
	std::vector<std::shared_ptr<DecompositionNode>> children) {
	this->children = children;
}

Decomposition::Decomposition() {
	root = std::shared_ptr<DecompositionNode>(nullptr);
}

void Decomposition::setRoot(std::shared_ptr<DecompositionNode> node) {
	this->root = node;
}

void Decomposition::print() {
	std::list<std::pair<int, int>> edges;
	int label = 0;
	int parentLabel = 0;
	std::string s = printNode(root, label, parentLabel, edges);
	for (auto item : edges) {
		s += std::to_string(item.first) + " " + std::to_string(item.second) +
			 "\n";
	}
	findMaxBagSizeAndVertices(root);
	s = "s td " + std::to_string(label) + " " + std::to_string(maxBagSize) +
		" " + std::to_string(vertices.size()) + "\n" + s;
	std::cout << s;
}

std::string Decomposition::printNode(std::shared_ptr<DecompositionNode> node,
									 int &label, int parentLabel,
									 std::list<std::pair<int, int>> &edges) {
	label++;
	if (parentLabel != 0) edges.push_back(std::make_pair(parentLabel, label));
	int l = label;
	std::string str = "b " + std::to_string(label);
	std::set<unsigned> v = node->getVertices();
	for (auto item : v) {
		str += " " + std::to_string(item);
	}
	str += "\n";
	std::vector<std::shared_ptr<DecompositionNode>> children =
		node->getChildren();
	for (size_t i = 0; i < children.size(); i++) {
		str += printNode(children[i], label, l, edges);
	}
	return str;
}

std::string Decomposition::stringPrint() {
	std::list<std::pair<int, int>> edges;
	int label = 0;
	int parentLabel = 0;
	std::string s = printNode(root, label, parentLabel, edges);
	for (auto item : edges) {
		s += std::to_string(item.first) + " " + std::to_string(item.second) +
			 "\n";
	}
	findMaxBagSizeAndVertices(root);
	s = "s td " + std::to_string(label) + " " + std::to_string(maxBagSize) +
		" " + std::to_string(vertices.size()) + "\n" + s;
	return s;
}

void Decomposition::writeToFile(std::string fileName) {
	std::ofstream decFile(fileName);
	if (decFile.is_open()) {
		decFile << stringPrint();
		decFile.close();
	} else {
		std::cout << "Unable to open " << fileName << std::endl;
		exit(20);
	}
}

void Decomposition::findMaxBagSizeAndVertices(
	std::shared_ptr<DecompositionNode> node) {
	if (maxBagSize < int(node->getVertices().size())) {
		maxBagSize = int(node->getVertices().size());
	}
	std::set<unsigned> nVertices;
	std::set<unsigned> nodeVertices = node->getVertices();
	set_union(nodeVertices.begin(), nodeVertices.end(), vertices.begin(),
			  vertices.end(), inserter(nVertices, nVertices.begin()));
	vertices = nVertices;
	for (size_t i = 0; i < node->getChildren().size(); i++) {
		findMaxBagSizeAndVertices(node->getChildren()[i]);
	}
}
