// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "ConcreteTreeDecomposition.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Constructors
//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Default constructor for CTDNodeNEw. Default: empty node

CTDNodeNew::CTDNodeNew() {
	// Empty noded is the default.
	this->nodeType = "Empty";
	// this->B is already the empty bag by default
	// this->parent is NULL by default
	// this->children is the empty vector by default.
}

// Constructors that takes a node type and bag as input. Used when retrieving a
// tree decomposition for a counter example.
CTDNodeNew::CTDNodeNew(string nodeType, Bag B) {
	this->set_nodeType(nodeType);
	this->set_B(B);
	// this->parent is NULL by default
	// this->children is the empty vector by default.
}

// Constructors that takes a node type, a bag and a vector of children as input.
// Used when constructing a tree decomposition in a bottom-up fashion.
CTDNodeNew::CTDNodeNew(string nodeType, Bag B,
					   vector<shared_ptr<CTDNodeNew>> children) {
	this->set_nodeType(nodeType);
	this->set_B(B);
	this->set_children(children);
	// this->parent is NULL by default
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Get Functions
//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Bag CTDNodeNew::get_B() { return this->B; }

string CTDNodeNew::get_nodeType() { return this->nodeType; }

shared_ptr<CTDNodeNew> CTDNodeNew::get_parent() { return this->parent; }

vector<shared_ptr<CTDNodeNew>> CTDNodeNew::get_children() {
	return this->children;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Set Functions
//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTDNodeNew::set_B(Bag B) { this->B = B; }

void CTDNodeNew::set_nodeType(string nodeType) { this->nodeType = nodeType; }

void CTDNodeNew::set_parent(shared_ptr<CTDNodeNew> parent) {
	this->parent = parent;
}

void CTDNodeNew::set_children(vector<shared_ptr<CTDNodeNew>> children) {
	this->children = children;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Print Functions
///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

string CTDNodeNew::printCTDNode() {
	set<unsigned> bagElements = B.get_elements();
	string s = "{";
	for (auto element : bagElements) {
		s = s + to_string(element);
        if(element!=*(--bagElements.end()))
            s = s + ",";
	}
	s = s + "} [" + to_string(this->get_B().get_edge().first) + "," +
		to_string(this->get_B().get_edge().second) + "]";
	return s;
}

string CTDNodeNew::printAbstract() { return this->get_nodeType(); }

void CTDNodeNew::printConcrete() { this->get_B().print(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Read Functions
////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTDNodeNew::readAbstract() {}

void CTDNodeNew::readConcrete() {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Decomposition Functions
////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTDNodeNew CTDNodeNew::introVertex(unsigned i) {
	if (this->B.vertex_introducible(i)) {
		Bag Bprime = this->B;
		Bprime.intro_v(i);
		vector<shared_ptr<CTDNodeNew>> children;
		children.push_back(this->shared_from_this());
		CTDNodeNew node("IntroVertex_" + to_string(i), Bprime, children);
		return node;
	} else {
		cout << "ERROR in function CTDNodeNew::introVertex. Number " << i
			 << " is not introducible at current bag." << endl;
		exit(20);
	}
}

CTDNodeNew CTDNodeNew::forgetVertex(unsigned i) {
	if (this->B.vertex_forgettable(i)) {
		Bag Bprime = this->B;
		Bprime.forget_v(i);
		vector<shared_ptr<CTDNodeNew>> children;
		children.push_back(this->shared_from_this());
		CTDNodeNew node("ForgetVertex_" + to_string(i), Bprime, children);
		return node;
	} else {
		cout << "ERROR in function CTDNodeNew::ForgetVertex. Number " << i
			 << " is not Forgettable at current bag." << endl;
		exit(20);
	}
}

CTDNodeNew CTDNodeNew::introEdge(unsigned i, unsigned j) {
	// checks if j<i and if yes swap them
	if (j < i) {
		unsigned z = j;
		j = i;
		i = z;
	}
	if (this->B.edge_introducible(i, j)) {
		Bag Bprime = this->B;
		Bprime.intro_e(i, j);
		vector<shared_ptr<CTDNodeNew>> children;
		children.push_back(this->shared_from_this());
		CTDNodeNew node("IntroEdge_" + to_string(i) + "_" + to_string(j),
						Bprime, children);
		return node;
	} else {
		cout << "ERROR in function CTDNodeNew::IntroEdge. Number " << i << ","
			 << j << " is not introducible at current bag." << endl;
		exit(20);
	}
}

CTDNodeNew join(CTDNodeNew left, CTDNodeNew right) {
	if (left.get_B().joinable(right.get_B())) {
		Bag Bprime;
		Bprime.set_elements(left.get_B().get_elements());
		vector<shared_ptr<CTDNodeNew>> children;
		children.push_back(left.shared_from_this());
		children.push_back(right.shared_from_this());
		CTDNodeNew node("Join", Bprime, children);
		return node;
	} else {
		cout << "ERROR in function friend CTDNodeNew::Join. Two nodes are not "
				"joinable."
			 << endl;
		exit(20);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Concrete Tree Decomposition
/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConcreteTreeDecomposition::traverseNode(
	CTDNodeNew &node, MultiGraph &G, map<unsigned, unsigned> &colorToVertexMap,
	unsigned &nVertices, unsigned &nEdges) {
	map<unsigned, unsigned> colorToVertexMapCopy = colorToVertexMap;
	if (node.get_nodeType() == "Empty") {
		// do nothing
	} else if (strstr(node.get_nodeType().c_str(), "IntroVertex")) {
		/////////////// Finding the introduced vertex ///////////
		set<unsigned> bagSet = node.get_B().get_elements();
		set<unsigned> childBagSet =
			node.get_children()[0]->get_B().get_elements();
		set<unsigned> bagSetDifference;
		set_difference(
			bagSet.begin(), bagSet.end(), childBagSet.begin(),
			childBagSet.end(),
			std::inserter(bagSetDifference, bagSetDifference.begin()));

		if (bagSetDifference.size() != 1) {
			cout << "ERROR: ConcreteTreeDecomposition::traverseNode in "
					"IntroVertex child's bag and node's bag are not valid"
				 << endl;

			exit(20);
		}
		//////////// End of Finding the introduced vertex ///////////
		colorToVertexMapCopy.erase(*(bagSetDifference.begin()));
		traverseNode(*(node.get_children()[0]), G, colorToVertexMapCopy,
					 nVertices,
					 nEdges); // Nothing happens. Just process the next bag.
	} else if (strstr(node.get_nodeType().c_str(), "ForgetVertex")) {
		/////////////// Finding the Forgotten vertex ///////////
		set<unsigned> bagSet = node.get_B().get_elements();
		set<unsigned> childBagSet =
			node.get_children()[0]->get_B().get_elements();
		set<unsigned> bagSetDifference;
		set_difference(
			childBagSet.begin(), childBagSet.end(), bagSet.begin(),
			bagSet.end(),
			std::inserter(bagSetDifference, bagSetDifference.begin()));

		if (bagSetDifference.size() != 1) {
			cout << "ERROR: ConcreteTreeDecomposition::traverseNode in "
					"ForgetVertex child's bag and node's bag are not valid"
				 << endl;
			exit(20);
		}
		//////////// End of Finding the Forgotten vertex ///////////

		nVertices = nVertices + 1;
		G.addVertex(nVertices);
		colorToVertexMapCopy[*(bagSetDifference.begin())] = nVertices;
		traverseNode(*(node.get_children()[0]), G, colorToVertexMapCopy,
					 nVertices,
					 nEdges); // Nothing happens. Just process the next bag.
	} else if (strstr(node.get_nodeType().c_str(), "IntroEdge")) {
		nEdges = nEdges + 1;
		pair<unsigned, unsigned> e = node.get_B().get_edge();
		G.addEdgeLabel(nEdges);
		G.addIncidence(nEdges, colorToVertexMap.find(e.first)->second);
		G.addIncidence(nEdges, colorToVertexMap.find(e.second)->second);
		traverseNode(*(node.get_children()[0]), G, colorToVertexMapCopy,
					 nVertices, nEdges);
	} else if (node.get_nodeType() == "Join") {
		traverseNode(*(node.get_children()[0]), G, colorToVertexMapCopy,
					 nVertices, nEdges);
		traverseNode(*(node.get_children()[1]), G, colorToVertexMapCopy,
					 nVertices, nEdges);
	}
}

MultiGraph ConcreteTreeDecomposition::extractMultiGraph() {
	MultiGraph G;
	map<unsigned, unsigned> colorToVertexMap;
	unsigned nVertices = 0;
	unsigned nEdges = 0;

	set<unsigned> elements = root->get_B().get_elements();
	set<unsigned>::iterator it = elements.begin();

	for (size_t i = 1; i <= this->root->get_B().get_elements().size(); ++i) {
		colorToVertexMap[*it] = i; // creates one vertex for each color.
		nVertices = i;
		G.addVertex(nVertices);
		++it;
	}

	traverseNode(*(this->root), G, colorToVertexMap, nVertices, nEdges);
	return G;
}

string ConcreteTreeDecomposition::printTreeRecursive(CTDNodeNew &node,
													 unsigned &label) {
	if (node.get_nodeType() == "Join") {
		string s1 = printTreeRecursive(*node.get_children()[0], label);
		unsigned label1 = label;
		string s2 = printTreeRecursive(*node.get_children()[1], label);
		unsigned label2 = label;
		label++;
		return s1 + s2 + to_string(label) + " " + node.get_nodeType() + "(" +
			   to_string(label1) + "," + to_string(label2) + ") " +
			   node.printCTDNode() + "\n";
	} else if (node.get_nodeType() == "Empty") {
		label++;
		return to_string(label) + " Empty\n";
	} else {
		string s = printTreeRecursive(*node.get_children()[0], label);
		label++;
		return s + to_string(label) + " " + node.get_nodeType() + "(" +
			   to_string(label - 1) + ") " + node.printCTDNode() + "\n";
	}
}
void ConcreteTreeDecomposition::printTree() {
	unsigned label = 0;

	cout << printTreeRecursive(*root, label);
}

string ConcreteTreeDecomposition::printAbstractRecursive(CTDNodeNew &node,
														 unsigned &label) {
	if (node.get_nodeType() == "Join") {
		string s1 = printAbstractRecursive(*node.get_children()[0], label);
		unsigned label1 = label;
		string s2 = printAbstractRecursive(*node.get_children()[1], label);
		unsigned label2 = label;
		label++;
		return s1 + s2 + to_string(label) + " " + node.printAbstract() + "(" +
			   to_string(label1) + "," + to_string(label2) + ")\n";
	} else if (node.get_nodeType() == "Empty") {
		label++;
		return to_string(label) + " " + node.printAbstract() + "\n";
	} else {
		string s = printAbstractRecursive(*node.get_children()[0], label);
		label++;
		return s + to_string(label) + " " + (node.printAbstract()) + "(" +
			   to_string(label - 1) +
			   ")"
			   "\n";
	}
}
void ConcreteTreeDecomposition::printAbstract() {
	unsigned label = 0;

	cout << printAbstractRecursive(*root, label);
}

bool ConcreteTreeDecomposition::readToken(string::iterator &it, string token) {
	string::iterator tempIt = it;
	for (auto c : token) {
		if (!(*tempIt == c)) {
			return false;
		} else {
			tempIt++;
		}
	}
	it = tempIt;
	return true;
}

bool ConcreteTreeDecomposition::readAbstractVertexNumber(string::iterator &it,
														 unsigned &v) {
	string num;
	string::iterator tempIt = it;
	if (isdigit(*tempIt)) {
		num = *tempIt;
		tempIt++;
		while (1) {
			if (isdigit(*tempIt)) {
				num = num + *tempIt;
				tempIt++;
			} else if (*tempIt == '(') {
				break;
			} else {
				return false;
			}
		}
		it = tempIt;
		v = stoi(num);
		return true;
	} else {
		return false;
	}
}

bool ConcreteTreeDecomposition::readAbstractEdgeNumber(
	string::iterator &it, pair<unsigned, unsigned> &e) {
	string num;
	unsigned v1 = -1;
	unsigned v2 = -1;
	string::iterator tempIt = it;
	if (isdigit(*tempIt)) {
		num = *tempIt;
		tempIt++;
		while (1) {
			if (isdigit(*tempIt)) {
				num = num + *tempIt;
				tempIt++;
			} else if (*tempIt == '(') {
				v2 = stoi(num);
				if (v2 == -1 or v1 == -1) {
					return false;
				} else {
					break;
				}
			} else if (*tempIt == '_') {
				v1 = stoi(num);
				num = "";
				tempIt++;
			} else {
				return false;
			}
		}
		it = tempIt;
		e = make_pair(v1, v2);
		return true;
	} else {
		return false;
	}
}
unsigned ConcreteTreeDecomposition::readInteger(string::iterator &it) {
	string::iterator tempIt = it;
	int integer = -1;
	string digits;
	while (isdigit(*tempIt)) {
		digits = digits + *tempIt;
		tempIt++;
	}
	it = tempIt;
	integer = stoi(digits);
	return integer;
}

shared_ptr<CTDNodeNew>
ConcreteTreeDecomposition::readAbstractExpressionRecursive(
	string::iterator &it, string::iterator end,
	vector<shared_ptr<CTDNodeNew>> &v) {
	unsigned label = 0;

	if (isdigit(*it)) {
		label = readInteger(it);

		if (readToken(it, "IntroVertex_")) {
			unsigned introducedVertex = readInteger(it);
			shared_ptr<CTDNodeNew> ctdNode(new CTDNodeNew);
			ctdNode->set_nodeType("IntroVertex_" + to_string(introducedVertex));
			unsigned childLabel = 0;
			if (readToken(it, "(")) {
				childLabel = readInteger(it);
				if (readToken(it, ")")) {
				} else {
					// To Do : print an error message here
				}
			} else {
				// To Do : print an error message here
			}
			vector<shared_ptr<CTDNodeNew>> children;
			children.push_back(v[childLabel]);
			ctdNode->set_children(children);
			v[childLabel]->set_parent(ctdNode);
			Bag b = v[childLabel]->get_B();
			if (!b.vertex_introducible(introducedVertex)) {
				cout << "ERROR: it is not " << introducedVertex
					 << "vertex introducible" << endl;
				exit(20);
			} else {
				b.intro_v(introducedVertex);
				ctdNode->set_B(b);

				if (v.size() < label + 1) {
					v.resize(label + 1);
				}
				v[label] = ctdNode;
				if (it != end) {
					readAbstractExpressionRecursive(it, end, v);
				}
				return ctdNode;
			}
		} else if (readToken(it, "IntroEdge_")) {
			unsigned introducedEdge1 = readInteger(it);
			unsigned introducedEdge2 = 0;
			shared_ptr<CTDNodeNew> ctdNode(new CTDNodeNew);
			if (readToken(it, "_")) {
				introducedEdge2 = readInteger(it);
			} else {
				// TO DO: print an error message
			}
			ctdNode->set_nodeType("IntroEdge_" + to_string(introducedEdge1) +
								  "_" + to_string(introducedEdge2));
			unsigned childLabel = 0;
			if (readToken(it, "(")) {
				childLabel = readInteger(it);
				if (readToken(it, ")")) {
				} else {
					// To Do : print an error message here
				}
			} else {
				// To Do : print an error message here
			}
			Bag b = v[childLabel]->get_B();
			if (!b.edge_introducible(introducedEdge1, introducedEdge2)) {
				cout << "ERROR:" << introducedEdge1 << " " << introducedEdge2
					 << " are not edge introducible" << endl;
				exit(20);
			} else {
				b.intro_e(introducedEdge1, introducedEdge2);
				ctdNode->set_B(b);
				vector<shared_ptr<CTDNodeNew>> children;
				children.push_back(v[childLabel]);
				ctdNode->set_children(children);
				v[childLabel]->set_parent(ctdNode);
				if (v.size() < label + 1) {
					v.resize(label + 1);
				}
				v[label] = ctdNode;
				if (it != end) {
					readAbstractExpressionRecursive(it, end, v);
				}
				return ctdNode;
			}
		} else if (readToken(it, "ForgetVertex_")) {
			unsigned forgottenVertex = readInteger(it);
			shared_ptr<CTDNodeNew> ctdNode(new CTDNodeNew);
			ctdNode->set_nodeType("ForgetVertex_" + to_string(forgottenVertex));
			unsigned childLabel = 0;
			if (readToken(it, "(")) {
				childLabel = readInteger(it);
				if (readToken(it, ")")) {
				} else {
					// To Do : print an error message here
				}
			} else {
				// To Do : print an error message here
			}
			Bag b = v[childLabel]->get_B();
			if (!b.vertex_forgettable(forgottenVertex)) {
				cout << "ERROR:" << forgottenVertex
					 << "is not vertex forgetable" << endl;
			} else {
				b.forget_v(forgottenVertex);
				ctdNode->set_B(b);
				vector<shared_ptr<CTDNodeNew>> children;
				children.push_back(v[childLabel]);
				ctdNode->set_children(children);
				v[childLabel]->set_parent(ctdNode);
				if (v.size() < label + 1) {
					v.resize(label + 1);
				}
				v[label] = ctdNode;
				if (it != end) {
					readAbstractExpressionRecursive(it, end, v);
				}
				return ctdNode;
			}

		} else if (readToken(it, "Join")) {
			shared_ptr<CTDNodeNew> ctdNode(new CTDNodeNew);
			ctdNode->set_nodeType("Join");
			unsigned childLabel1 = 0;
			unsigned childLabel2 = 0;
			if (readToken(it, "(")) {
				childLabel1 = readInteger(it);
				if (readToken(it, ",")) {
					childLabel2 = readInteger(it);
				} else {
					// TO DO: print an error message
				}
				if (readToken(it, ")")) {
				} else {
					// To Do : print an error message here
				}

			} else {
				// To Do : print an error message here
			}
			Bag b = v[childLabel1]->get_B();
			if (!b.joinable(v[childLabel1]->get_B())) {
				cout << "ERROR: two bags are not joinable" << endl;
				exit(20);
			} else {
				ctdNode->set_B(b);
				vector<shared_ptr<CTDNodeNew>> children;

				cout << childLabel1 << childLabel2 << endl;
				children.push_back(v[childLabel1]);
				children.push_back(v[childLabel2]);
				ctdNode->set_children(children);
				v[childLabel1]->set_parent(ctdNode);
				v[childLabel2]->set_parent(ctdNode);

				if (v.size() < label + 1) {
					v.resize(label + 1);
				}
				v[label] = ctdNode;
				if (it != end) {
					readAbstractExpressionRecursive(it, end, v);
				}
				return ctdNode;
			}
		} else if (readToken(it, "Empty")) {
			shared_ptr<CTDNodeNew> ctdNode(new CTDNodeNew);
			ctdNode->set_nodeType("Empty");

			if (v.size() < label + 1) {
				v.resize(label + 1);
			}
			v[label] = ctdNode;
			if (it != end) {
				readAbstractExpressionRecursive(it, end, v);
			}
			return ctdNode;

		} else {
			cout
				<< "ERROR: "
				   "ConcreteTreeDecomposition::readAbstractExpressionRecursive "
				   "Token is not matched with defined tokens"
				<< endl;
			exit(20);
		}
	}
}

void ConcreteTreeDecomposition::readAbstract(string s) {
	s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
	string::iterator it = s.begin();
	vector<shared_ptr<CTDNodeNew>> v;
	readAbstractExpressionRecursive(it, s.end(), v);
	this->root = v[v.size() - 1];
}

void ConcreteTreeDecomposition::printConcrete() {}

pair<bool, State::ptr> ConcreteTreeDecomposition::constructWitnesses(
	Conjecture &conjecture, shared_ptr<CTDNodeNew> node) {
	// First, We check the type of the node
	if (node->get_nodeType() == "Empty") {
		// if it is an empty, then it is a leaf
		State::ptr q = conjecture.kernel->initialState();
		return make_pair(
			conjecture.evaluateConjectureOnState(*q, conjecture.kernel), q);
	} else if (strstr(node->get_nodeType().c_str(), "IntroVertex")) {
		pair<bool, State::ptr> childProcess =
			constructWitnesses(conjecture, node->get_children()[0]);
		if (!childProcess.first) {
			return childProcess;
		} else {
			// find the introduced vertex
			set<unsigned> bagSet = node->get_B().get_elements();
			set<unsigned> childBagSet =
				childProcess.second->get_bag().get_elements();
			set<unsigned> bagSetDifference;
			set_difference(
				bagSet.begin(), bagSet.end(), childBagSet.begin(),
				childBagSet.end(),
				std::inserter(bagSetDifference, bagSetDifference.begin()));
			if (bagSetDifference.size() != 1) {
				cout
					<< "ERROR: ConcreteTreeDecomposition::constructWitnesses "
					   "in IntroVertex child's bag and node's bag are not valid"
					<< endl;
				exit(20);
			}
			State::ptr q = conjecture.kernel->intro_v(
				childProcess.second, *bagSetDifference.begin());
			bool conjectureEvaluationResult =
				conjecture.evaluateConjectureOnState(*q, conjecture.kernel);
			return make_pair(conjectureEvaluationResult, q);
		}
	} else if (strstr(node->get_nodeType().c_str(), "ForgetVertex")) {
		pair<bool, State::ptr> childProcess =
			constructWitnesses(conjecture, node->get_children()[0]);
		if (!childProcess.first) {
			return childProcess;
		} else {
			// find the forgotten vertex
			set<unsigned> bagSet = node->get_B().get_elements();
			set<unsigned> childBagSet =
				childProcess.second->get_bag().get_elements();
			set<unsigned> bagSetDifference;
			set_difference(
				childBagSet.begin(), childBagSet.end(), bagSet.begin(),
				bagSet.end(),
				std::inserter(bagSetDifference, bagSetDifference.begin()));
			if (bagSetDifference.size() != 1) {
				cout << "ERROR: ConcreteTreeDecomposition::constructWitnesses "
						"in ForgetVertex child's bag and node's bag are not "
						"valid"
					 << endl;
				exit(20);
			}
			State::ptr q = conjecture.kernel->forget_v(
				childProcess.second, *bagSetDifference.begin());
			bool conjectureEvaluationResult =
				conjecture.evaluateConjectureOnState(*q, conjecture.kernel);
			return make_pair(conjectureEvaluationResult, q);
		}
	} else if (strstr(node->get_nodeType().c_str(), "IntroEdge")) {
		pair<bool, State::ptr> childProcess =
			constructWitnesses(conjecture, node->get_children()[0]);
		if (!childProcess.first) {
			return childProcess;
		} else {
			pair<unsigned, unsigned> e =
				node->get_B().get_edge();
			State::ptr q = conjecture.kernel->intro_e(childProcess.second,
													  e.first, e.second);
			bool conjectureEvaluationResult =
				conjecture.evaluateConjectureOnState(*q, conjecture.kernel);
			return make_pair(conjectureEvaluationResult, q);
		}
	} else if (strstr(node->get_nodeType().c_str(), "Join")) {
		pair<bool, State::ptr> childProcess1 =
			constructWitnesses(conjecture, node->get_children()[0]);
		pair<bool, State::ptr> childProcess2 =
			constructWitnesses(conjecture, node->get_children()[1]);
		if (!childProcess1.first) {
			return childProcess1;
		} else if (!childProcess2.first) {
			return childProcess2;
		} else {
			State::ptr q = conjecture.kernel->join(childProcess1.second,
												   childProcess2.second);
			bool conjectureEvaluationResult =
				conjecture.evaluateConjectureOnState(*q, conjecture.kernel);
			return make_pair(conjectureEvaluationResult, q);
		}
	} else {
		cout << "ERROR in constructWitnesses: The function could not recognize "
				"the type of the node"
			 << endl;
		cout << "The devastated node is: " << endl;
		node->printCTDNode();
		exit(20);
	}
}

bool ConcreteTreeDecomposition::conjectureCheck(Conjecture &conjecture) {
	pair<bool, State::ptr> concreteEvaluation =
		constructWitnesses(conjecture, root);
	if (!concreteEvaluation.first) {
		cout << " Concrete Tree Decomposition does not satisfy the conjecture"
			 << endl;
		cout << " Printing bad State: " << endl;
		concreteEvaluation.second->print();
		return false;
	} else {
		cout << " Concrete Tree Decomposition satisfies the conjecture" << endl;
		return true;
	}
}

shared_ptr<StateTreeNode> ConcreteTreeDecomposition::generateStateTreeNode(
	shared_ptr<CTDNodeNew> node, shared_ptr<DynamicKernel> kernel) {
	// First, We check the type of the node
	if (node->get_nodeType() == "Empty") {
		shared_ptr<StateTreeNode> stateTreeNode(new StateTreeNode);
		// if it is an empty, then it is a leaf
		State::ptr q = kernel->initialState();
		stateTreeNode->set_S(q);
		stateTreeNode->set_kernel(kernel);
		stateTreeNode->set_nodeType(node->get_nodeType());
		return stateTreeNode;
	} else if (strstr(node->get_nodeType().c_str(), "IntroVertex")) {
		shared_ptr<StateTreeNode> stateTreeNode(new StateTreeNode);
		// generate the child node
		shared_ptr<StateTreeNode> childNode =
			generateStateTreeNode(node->get_children()[0], kernel);
		// find the introduced vertex
		set<unsigned> bagSet = node->get_B().get_elements();
		set<unsigned> childBagSet =
			node->get_children()[0]->get_B().get_elements();
		set<unsigned> bagSetDifference;
		set_difference(
			bagSet.begin(), bagSet.end(), childBagSet.begin(),
			childBagSet.end(),
			std::inserter(bagSetDifference, bagSetDifference.begin()));
		if (bagSetDifference.size() != 1) {
			cout << "ERROR: ConcreteTreeDecomposition::generateStateTreeNode "
					"in IntroVertex child's bag and node's bag are not valid"
				 << endl;
			exit(20);
		}
		State::ptr q =
			kernel->intro_v(childNode->get_S(), *bagSetDifference.begin());
		// set the class members of the state tree node
		stateTreeNode->set_nodeType(node->get_nodeType());
		stateTreeNode->set_S(q);
		// create a vector to push the childNode in it
		vector<shared_ptr<StateTreeNode>> childVector;
		childVector.push_back(childNode);
		stateTreeNode->set_children(childVector);
		stateTreeNode->set_kernel(kernel);
		// set the parent node of the childNode
		childNode->set_parent(stateTreeNode);
		return stateTreeNode;

	} else if (strstr(node->get_nodeType().c_str(), "ForgetVertex")) {
		shared_ptr<StateTreeNode> stateTreeNode(new StateTreeNode);
		// generate the child node
		shared_ptr<StateTreeNode> childNode =
			generateStateTreeNode(node->get_children()[0], kernel);
		// find the forgotten vertex
		set<unsigned> bagSet = node->get_B().get_elements();
		set<unsigned> childBagSet =
			node->get_children()[0]->get_B().get_elements();
		set<unsigned> bagSetDifference;
		set_difference(
			childBagSet.begin(), childBagSet.end(), bagSet.begin(),
			bagSet.end(),
			std::inserter(bagSetDifference, bagSetDifference.begin()));
		if (bagSetDifference.size() != 1) {
			cout << "ERROR: ConcreteTreeDecomposition::generateStateTreeNode "
					"in ForgetVertex child's bag and node's bag are not valid"
				 << endl;
			exit(20);
		}
		State::ptr q =
			kernel->forget_v(childNode->get_S(), *bagSetDifference.begin());
		// set the class members of the state tree node
		stateTreeNode->set_nodeType(node->get_nodeType());
		stateTreeNode->set_S(q);
		// create a vector to push the childNode in it
		vector<shared_ptr<StateTreeNode>> childVector;
		childVector.push_back(childNode);
		stateTreeNode->set_children(childVector);
		stateTreeNode->set_kernel(kernel);
		// set the parent node of the childNode
		childNode->set_parent(stateTreeNode);
		return stateTreeNode;
	} else if (strstr(node->get_nodeType().c_str(), "IntroEdge")) {
		shared_ptr<StateTreeNode> stateTreeNode(new StateTreeNode);
		// generate the child node
		shared_ptr<StateTreeNode> childNode =
			generateStateTreeNode(node->get_children()[0], kernel);
		pair<unsigned, unsigned> introducedEdge = node->get_B().get_edge();
		if (introducedEdge.first == 0 or introducedEdge.second == 0) {
			cout << "ERROR: ConcreteTreeDecomposition::generateStateTreeNode "
					"in IntroEdge the edge isn't valid"
				 << endl;
			exit(20);
		}
		State::ptr q = kernel->intro_e(childNode->get_S(), introducedEdge.first,
									   introducedEdge.second);
		// set the class members of the state tree node
		stateTreeNode->set_nodeType(node->get_nodeType());
		stateTreeNode->set_S(q);
		// create a vector to push the childNode in it
		vector<shared_ptr<StateTreeNode>> childVector;
		childVector.push_back(childNode);
		stateTreeNode->set_children(childVector);
		stateTreeNode->set_kernel(kernel);
		// set the parent node of the childNode
		childNode->set_parent(stateTreeNode);
		return stateTreeNode;
	} else if (strstr(node->get_nodeType().c_str(), "Join")) {
		shared_ptr<StateTreeNode> stateTreeNode(new StateTreeNode);
		// generate children nodes
		shared_ptr<StateTreeNode> child1 =
			generateStateTreeNode(node->get_children()[0], kernel);
		shared_ptr<StateTreeNode> child2 =
			generateStateTreeNode(node->get_children()[1], kernel);
		State::ptr q = kernel->join(child1->get_S(), child2->get_S());
		// set the class members of the state tree node
		stateTreeNode->set_nodeType(node->get_nodeType());
		stateTreeNode->set_S(q);
		// create a vector of the children
		vector<shared_ptr<StateTreeNode>> childrenVector;
		childrenVector.push_back(child1);
		childrenVector.push_back(child2);
		stateTreeNode->set_children(childrenVector);
		stateTreeNode->set_kernel(kernel);
		// set the parent node of the children
		child1->set_parent(stateTreeNode);
		child2->set_parent(stateTreeNode);
		return stateTreeNode;

	} else {
		cout << "ERROR in generateStateTreeNode The function could not  "
				"recognize the type of the node"
			 << endl;
		cout << "The devastated node is: " << endl;
		node->printCTDNode();
		exit(20);
	}
}
StateTree ConcreteTreeDecomposition::convertToStateTree(shared_ptr<DynamicKernel> kernel) {
    StateTree stateTree;
    stateTree.root = generateStateTreeNode(root, kernel);
    return stateTree;
}