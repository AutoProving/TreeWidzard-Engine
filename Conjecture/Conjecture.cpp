// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "Conjecture.h"

#include <memory>
#include <utility>

namespace {

auto is_binary_operator(const std::string &op) -> bool {
	return op == "+" || op == "-" || op == "/" || op == "*" || op == ">" ||
		   op == ">=" || op == "<" || op == "<=" || op == "==" ||
		   op == "and" || op == "or" || op == "implies" || op == "iff";
}

auto is_unary_operator(const std::string &op) -> bool {
	return op == "not";
}

void clone_property_assignments(
	const std::map<std::string, PropertyAssignment *> &source,
	std::vector<std::unique_ptr<PropertyAssignment>> &owned,
	std::map<std::string, PropertyAssignment *> &mapped) {
	owned.clear();
	mapped.clear();

	owned.reserve(source.size());
	for (const auto &[name, property] : source) {
		if (property == nullptr) {
			mapped[name] = nullptr;
			continue;
		}
		owned.push_back(std::make_unique<PropertyAssignment>(*property));
		mapped[name] = owned.back().get();
	}
}

} // namespace

// To Do: implement getCoreWitnessSetByVar
double Conjecture::evaluateConjectureNodeOnState(const State &q, const ConjectureNode *node) {
	double result;
	// evaluate a node based on its type
	// TODO:
	// should only compute invariant and is_final_set at most once
	// double -> int (because double - bool conversions are scary)
	// if you give ownership -> shared_ptr or unique_ptr
	// if you let someone borrow -> reference

	// f(const Witness &w)
	//
	// f(const shared_ptr<const Witness> w)
	//

		switch (node->getType()) {
			case CORE_VARIABLE:
				if (variablesToCoreName.count(node->getVal())) {
					auto *core = kernel->getCoreByVar(node->getVal());
					const int index = kernel->getIndexByVar(node->getVal());
					const Bag &bag = q.get_bag();
					result = core->is_final_witness_set(bag, q.getWitnessSet(index));
					break;
				} else {
				// error
				std::cout
					<< "Error in Conjecture::evaluateConjectureNodeOnState.";
				exit(20);
			}
			case INV:
				if (node->getChildren().size() == 1) {
					if (node->getChildren()[0]->getType() == CORE_VARIABLE) {
						const std::string &variable =
							node->getChildren()[0]->getVal();
						auto *core = kernel->getCoreByVar(variable);
						const int index = kernel->getIndexByVar(variable);
						const Bag &bag = q.get_bag();
						result = core->inv(bag, q.getWitnessSet(index));
						break;
					} else {
					std::cout << "Error in "
								 "Conjecture::evaluateConjectureNodeOnState: "
								 "INV is not in a valid form. A core variable "
								 "should be given to inv.";
					exit(20);
				}
			} else {
				std::cout
					<< "Error in Conjecture::evaluateConjectureNodeOnState: "
					   "INV is not in a valid form.";
				exit(20);
			}
		case EXP_VARIABLE:
			std::cout << "case EXP_VARIABLE no implemented" << std::endl;
			exit(20);
		case NUMBER:
			result = node->getX();
			break;
		case OPERATOR: {
			std::string op = node->getVal();
			if (op == "+") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) +
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "-") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) -
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "/") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) /
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "*") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) *
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == ">") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) >
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == ">=") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) >=
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "<") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) <
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "<=") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) <=
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "==") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) ==
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "and") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) and
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "or") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) or
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "not") {
				if (node->getChildren().size() == 1) {
					result = !evaluateConjectureNodeOnState(
						q, node->getChildren()[0]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "implies") {
				if (node->getChildren().size() == 2) {
					result = (!evaluateConjectureNodeOnState(
								 q, node->getChildren()[0])) or
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else if (op == "iff") {
				if (node->getChildren().size() == 2) {
					result = evaluateConjectureNodeOnState(
								 q, node->getChildren()[0]) ==
							 evaluateConjectureNodeOnState(
								 q, node->getChildren()[1]);
					break;
				} else {
					exit(20);
				}
			} else {
				exit(20);
			}
		}
		case FUNCTION_BINARY:
			if (node->getChildren().size() != 2) {
				std::cout << " ERROR " << __PRETTY_FUNCTION__ << " :  "
						  << node->getVal() << " has invalid number of children"
						  << std::endl;
				exit(20);
			} else {
				if (functions_binary.count(node->getVal())) {
					Function_Binary f = functions_binary[node->getVal()];
					result = f(evaluateConjectureNodeOnState(
								   q, node->getChildren()[0]),
							   evaluateConjectureNodeOnState(
								   q, node->getChildren()[1]));
					break;
				} else {
					std::cout << " ERROR " << __PRETTY_FUNCTION__ << " :  "
							  << node->getVal() << " hasn't defined"
							  << std::endl;
					exit(20);
				}
			}
		case FUNCTION_UNARY:
			if (node->getChildren().size() != 1) {
				std::cout << " ERROR " << __PRETTY_FUNCTION__ << " :  "
						  << node->getVal() << " has invalid number of children"
						  << std::endl;
				exit(20);
			} else {
				if (functions_unary.count(node->getVal())) {
					Function_Unary f = functions_unary[node->getVal()];
					result = f(evaluateConjectureNodeOnState(
						q, node->getChildren()[0]));
					break;
				} else {
					std::cout << " ERROR " << __PRETTY_FUNCTION__ << " :  "
							  << node->getVal() << " hasn't defined"
							  << std::endl;
					exit(20);
				}
			}

		default: {
			std::cout << " ERROR " << __PRETTY_FUNCTION__ << " :  "
					  << node->getVal() << " hasn't defined" << std::endl;
			exit(20);
		}
	}
	return result;
}

double Conjecture::evaluateConjectureOnState(const State &q) {
	double result = evaluateConjectureNodeOnState(q, root);
	return result;
}

ConjectureNode *Conjecture::getRoot() const { return root; }

void Conjecture::setRoot(ConjectureNode *root_) {
	if (root == root_) {
		return;
	}
	if (ownsRoot) {
		ConjectureNode::deleteSubtree(root);
	}
	root = root_;
	ownsRoot = false;
}

void Conjecture::setOwnedRoot(ConjectureNode *root_) {
	if (root == root_ && ownsRoot) {
		return;
	}
	if (ownsRoot) {
		ConjectureNode::deleteSubtree(root);
	}
	root = root_;
	ownsRoot = true;
}

DynamicKernel *Conjecture::getKernel() const { return kernel; }

void Conjecture::setKernel(DynamicKernel *kernel_) { kernel = kernel_; }

void Conjecture::print() {
	//    unsigned label = 0;
	//    root->printWithNumbers(label);
	root->printInfix();
}

bool Conjecture::checkConjectureStructure(ConjectureNode *node) {
	if (node == nullptr) {
		std::cout << "Conjecture root is null" << std::endl;
		return false;
	}

	switch (node->getType()) {
		case NUMBER:
		case CORE_VARIABLE:
		case EXP_VARIABLE:
			if (!node->getChildren().empty()) {
				std::cout << node->getVal() << " is number and has children "
						  << std::endl;
				return false;
			}
			break;
		case OPERATOR: {
			const std::string &op = node->getVal();
			if (is_binary_operator(op)) {
				if (node->getChildren().size() != 2) {
					std::cout << node->getVal() << " has invalid children"
							  << std::endl;
					return false;
				}
			} else if (is_unary_operator(op)) {
				if (node->getChildren().size() != 1) {
					std::cout << node->getVal() << " has invalid children"
							  << std::endl;
					return false;
				}
			} else {
				std::cout << node->getVal() << " is not a supported operator"
						  << std::endl;
				return false;
			}
			break;
		}
		case FUNCTION_BINARY:
			if (node->getChildren().size() != 2) {
				std::cout << node->getVal() << " has invalid children"
						  << std::endl;
				return false;
			}
			if (!functions_binary.count(node->getVal())) {
				std::cout << node->getVal() << " is not a supported binary function"
						  << std::endl;
				return false;
			}
			break;
		case FUNCTION_UNARY:
			if (node->getChildren().size() != 1) {
				std::cout << node->getVal() << " has invalid children"
						  << std::endl;
				return false;
			}
			if (!functions_unary.count(node->getVal())) {
				std::cout << node->getVal() << " is not a supported unary function"
						  << std::endl;
				return false;
			}
			break;
		case INV:
			if (node->getChildren().size() != 1) {
				std::cout << node->getVal() << " has invalid children"
						  << std::endl;
				return false;
			}
			if (node->getChildren()[0] == nullptr ||
				node->getChildren()[0]->getType() != CORE_VARIABLE) {
				std::cout << node->getVal()
						  << " requires a single core variable child"
						  << std::endl;
				return false;
			}
			break;
		default:
			std::cout << node->getVal() << " has unsupported node type"
					  << std::endl;
			return false;
	}
	for (auto child : node->getChildren()) {
		if (!checkConjectureStructure(child)) {
			return false;
		}
	}
	return true;
}

const std::map<std::string, std::string> &Conjecture::getVariablesToCoreName()
	const {
	return variablesToCoreName;
}

void Conjecture::setVariablesToCoreName(
	const std::map<std::string, std::string> &variablesToCoreName_) {
	variablesToCoreName = variablesToCoreName_;
}

int Conjecture::evaluatePremiseOnState(const State &q) {
	if (root->getVal() == "implies") {
		return evaluateConjectureNodeOnState(q, root->getChildren()[0]);
	} else {
		std::cout
			<< "ERROR: could not determine the premise. The conjecture is not "
			   "in the form A->B. "
			<< std::endl;
		exit(20);
	}
}

double min_specialized(double x1, double x2) { return std::min(x1, x2); }
double max_specialized(double x1, double x2) { return std::max(x1, x2); }

Conjecture::Conjecture() {
	functions_binary.insert(std::make_pair("pow", (Function_Binary)pow));
	functions_binary.insert(std::make_pair("min", min_specialized));
	functions_binary.insert(std::make_pair("max", max_specialized));

	// unary functions
	functions_unary.insert(std::make_pair("abs", (Function_Unary)fabs));
	functions_unary.insert(std::make_pair("acos", (Function_Unary)acos));
	functions_unary.insert(std::make_pair("asin", (Function_Unary)asin));
	functions_unary.insert(std::make_pair("atan", (Function_Unary)atan));
	functions_unary.insert(std::make_pair("cos", (Function_Unary)cos));
	functions_unary.insert(std::make_pair("exp", (Function_Unary)exp));
	functions_unary.insert(std::make_pair("floor", (Function_Unary)floor));
	functions_unary.insert(std::make_pair("ln", (Function_Unary)log));
	functions_unary.insert(std::make_pair("log", (Function_Unary)log10));
	functions_unary.insert(std::make_pair("sin", (Function_Unary)sin));
	functions_unary.insert(std::make_pair("sqrt", (Function_Unary)sqrt));
	functions_unary.insert(std::make_pair("tan", (Function_Unary)tan));
}

Conjecture::~Conjecture() {
	if (ownsRoot) {
		ConjectureNode::deleteSubtree(root);
	}
}

Conjecture::Conjecture(const Conjecture &other)
	: variablesToCoreName(other.variablesToCoreName),
	  functions_binary(other.functions_binary),
	  functions_unary(other.functions_unary),
	  root(other.root != nullptr ? other.root->cloneSubtree() : nullptr),
	  ownsRoot(other.root != nullptr),
	  kernel(other.kernel) {
	clone_property_assignments(other.varToProperty, ownedPropertyAssignments,
							   varToProperty);
}

Conjecture &Conjecture::operator=(const Conjecture &other) {
	if (this == &other) {
		return *this;
	}

	ConjectureNode *new_root =
		other.root != nullptr ? other.root->cloneSubtree() : nullptr;
	if (ownsRoot) {
		ConjectureNode::deleteSubtree(root);
	}
	variablesToCoreName = other.variablesToCoreName;
	functions_binary = other.functions_binary;
	functions_unary = other.functions_unary;
	root = new_root;
	ownsRoot = other.root != nullptr;
	kernel = other.kernel;
	clone_property_assignments(other.varToProperty, ownedPropertyAssignments,
							   varToProperty);
	return *this;
}

Conjecture::Conjecture(Conjecture &&other) noexcept
	: variablesToCoreName(std::move(other.variablesToCoreName)),
	  functions_binary(std::move(other.functions_binary)),
	  functions_unary(std::move(other.functions_unary)),
	  root(other.root),
	  ownsRoot(other.ownsRoot),
	  kernel(other.kernel),
	  ownedPropertyAssignments(std::move(other.ownedPropertyAssignments)),
	  varToProperty(std::move(other.varToProperty)) {
	other.root = nullptr;
	other.ownsRoot = false;
	other.kernel = nullptr;
	other.varToProperty.clear();
}

Conjecture &Conjecture::operator=(Conjecture &&other) noexcept {
	if (this == &other) {
		return *this;
	}

	if (ownsRoot) {
		ConjectureNode::deleteSubtree(root);
	}
	variablesToCoreName = std::move(other.variablesToCoreName);
	functions_binary = std::move(other.functions_binary);
	functions_unary = std::move(other.functions_unary);
	root = other.root;
	ownsRoot = other.ownsRoot;
	kernel = other.kernel;
	ownedPropertyAssignments = std::move(other.ownedPropertyAssignments);
	varToProperty = std::move(other.varToProperty);
	other.root = nullptr;
	other.ownsRoot = false;
	other.kernel = nullptr;
	other.varToProperty.clear();
	return *this;
}

ConjectureNode::ConjectureNode(TokenType type, const std::string &val, double x)
	: type(type), val(val), x(x) {}

ConjectureNode::~ConjectureNode() = default;

TokenType ConjectureNode::getType() const { return type; }

void ConjectureNode::setType(TokenType type_) { type = type_; }

const std::string &ConjectureNode::getVal() const { return val; }

void ConjectureNode::setVal(const std::string &val_) { val = val_; }

double ConjectureNode::getX() const { return x; }

void ConjectureNode::setX(double x_) { x = x_; }

const std::vector<ConjectureNode *> &ConjectureNode::getChildren() const {
	return children;
}

void ConjectureNode::setChildren(
	const std::vector<ConjectureNode *> &children_) {
	children = children_;
}

ConjectureNode *ConjectureNode::getParent() const { return parent; }

void ConjectureNode::setParent(ConjectureNode *parent_) { parent = parent_; }

ConjectureNode::ConjectureNode(TokenType type, const std::string &val)
	: type(type), val(val) {}

ConjectureNode::ConjectureNode(TokenType type, double x) : type(type), x(x) {}

void ConjectureNode::printWithNumbers(unsigned &label) {
	std::set<unsigned> childrenLabel;
	for (auto child : children) {
		child->printWithNumbers(label);
		childrenLabel.insert(label);
	}
	label++;
	std::cout << label << "  ";
	print();
	if (childrenLabel.size()) {
		std::cout << "(";
		for (auto childNo : childrenLabel) {
			std::cout << childNo;
			if (childNo != *(--childrenLabel.end())) {
				std::cout << ",";
			}
		}
		std::cout << ")";
	}
	std::cout << std::endl;
}

void ConjectureNode::print() {
	switch (type) {
		case NUMBER:
			std::cout << x;
			break;
		default:
			std::cout << val;
			break;
	}
}

ConjectureNode *ConjectureNode::cloneSubtree() const {
	auto *copy = new ConjectureNode(type, val, x);
	std::vector<ConjectureNode *> cloned_children;
	cloned_children.reserve(children.size());
	for (const ConjectureNode *child : children) {
		ConjectureNode *cloned_child = child->cloneSubtree();
		cloned_child->setParent(copy);
		cloned_children.push_back(cloned_child);
	}
	copy->setChildren(cloned_children);
	return copy;
}

void ConjectureNode::deleteSubtree(ConjectureNode *node) {
	if (node == nullptr) {
		return;
	}

	std::vector<ConjectureNode *> children = node->getChildren();
	for (ConjectureNode *child : children) {
		deleteSubtree(child);
	}
	delete node;
}

void ConjectureNode::printInfix() {
	switch (this->type) {
		case NUMBER:
			if (this->getChildren().size() != 0) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: NUMBER, node: " << x << std::endl;
				exit(20);
			}
			std::cout << this->x;
			return;

		case FUNCTION_BINARY:

			if (this->getChildren().size() != 2) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: FUNCTION_BINARY, node: " << val << " , "
						  << x << std::endl;
				exit(20);
			}
			std::cout << " " << val << "(";
			children[0]->printInfix();
			std::cout << ",";
			children[1]->printInfix();
			std::cout << ")";
			return;

		case FUNCTION_UNARY:

			if (this->getChildren().size() != 1) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: FUNCTION_UNARY, node: " << val
						  << std::endl;
				exit(20);
			}
			std::cout << " " << val << "(";
			children[0]->printInfix();
			std::cout << ")";
			return;
		case CORE_VARIABLE:

			if (this->getChildren().size() != 0) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: CORE_VARIABLE, node: " << val << std::endl;
				exit(20);
			}
			std::cout << val;
			return;

		case OPERATOR:

			if (val == "not") {
				if (children.size() != 1) {
					std::cout << "ERROR:  " << __PRETTY_FUNCTION__
							  << " type: OPERATOR, node: " << val << std::endl;
					exit(20);
				}
				std::cout << " " << val << " ";
				children[0]->printInfix();
			} else {
				if (children.size() != 2) {
					std::cout << "ERROR:  " << __PRETTY_FUNCTION__
							  << " type: OPERATOR, node: " << val << std::endl;
					exit(20);
				}
				std::cout << "(";
				children[0]->printInfix();
				std::cout << " " << val << " ";
				children[1]->printInfix();
				std::cout << ")";
			}
			return;
		case INV:
			if (this->getChildren().size() != 1) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: INV, node: " << val << std::endl;
				exit(20);
			}
			std::cout << " " << val << "(";
			children[0]->printInfix();
			std::cout << ")";
			return;
		default:
			std::cout << "ERROR:  " << __PRETTY_FUNCTION__ << " node " << val
					  << " not defined " << std::endl;
			exit(20);
	}
}

void Conjecture::printValues(const State &q, const ConjectureNode *node) {
	switch (node->getType()) {
		case NUMBER:
			if (node->getChildren().size() != 0) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: NUMBER, node: " << node->getX()
						  << std::endl;
				exit(20);
			}
			std::cout << node->getX();
			return;

		case FUNCTION_BINARY:

			if (node->getChildren().size() != 2) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: FUNCTION_BINARY, node: " << node->getVal()
						  << " , " << node->getX() << std::endl;
				exit(20);
			}
			std::cout << " " << node->getVal() << "(";
			printValues(q, node->getChildren()[0]);
			std::cout << ",";
			printValues(q, node->getChildren()[1]);
			std::cout << ")";
			return;

		case FUNCTION_UNARY:
			if (node->getChildren().size() != 1) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: FUNCTION_UNARY, node: " << node->getVal()
						  << std::endl;
				exit(20);
			}
			std::cout << " " << node->getVal() << "(";
			printValues(q, node->getChildren()[0]);
			std::cout << ")";
			return;
		case CORE_VARIABLE:
			if (node->getChildren().size() != 0) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: CORE_VARIABLE, node: " << node->getVal()
						  << std::endl;
				exit(20);
			}
			std::cout << node->getVal() << "="
					  << evaluateConjectureNodeOnState(q, node);
			return;

		case OPERATOR:
			if (node->getVal() == "not") {
				if (node->getChildren().size() != 1) {
					std::cout << "ERROR:  " << __PRETTY_FUNCTION__
							  << " type: OPERATOR, node: " << node->getVal()
							  << std::endl;
					exit(20);
				}
				std::cout << node->getVal() << "(";
				printValues(q, node->getChildren()[0]);
				std::cout << ")";
			} else {
				if (node->getChildren().size() != 2) {
					std::cout << "ERROR:  " << __PRETTY_FUNCTION__
							  << " type: OPERATOR, node: " << node->getVal()
							  << std::endl;
					exit(20);
				}
				std::cout << "(";
				printValues(q, node->getChildren()[0]);

				std::cout << " " << node->getVal() << " ";
				printValues(q, node->getChildren()[1]);

				std::cout << ")";
			}
			return;
		case INV:
			if (node->getChildren().size() != 1) {
				std::cout << "ERROR:  " << __PRETTY_FUNCTION__
						  << " type: INV, node: " << node->getVal()
						  << std::endl;
				exit(20);
			}
			std::cout << node->getVal() << "("
					  << node->getChildren()[0]->getVal();
			std::cout << ")=";
			std::cout << evaluateConjectureNodeOnState(q, node);
			return;
		default:
			std::cout << "ERROR:  " << __PRETTY_FUNCTION__ << " node "
					  << node->getVal() << " not defined " << std::endl;
			exit(20);
	}
}

void Conjecture::setvarToProperty(
	const std::map<std::string, PropertyAssignment *> &varToProperty_) {
	clone_property_assignments(varToProperty_, ownedPropertyAssignments,
							   varToProperty);
}
