// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef STATETREE_H
#define STATETREE_H

#include <string.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include "../Multigraph/MultiGraph.h"
#include "Bag.h"
#include "DynamicKernel.h"
#include "State.h"

namespace state_fs = std::experimental::filesystem;

class StateTreeNode : public std::enable_shared_from_this<StateTreeNode> {
  protected:
	std::string nodeType;							// Default: "Empty"
	State::ptr S;								// Default: Empty State
	std::shared_ptr<StateTreeNode> parent = nullptr; // Default Value
	std::vector<std::shared_ptr<StateTreeNode>> children; // Default: no child
	std::shared_ptr<DynamicKernel> kernel;

  public:
	// Constructors
	StateTreeNode();
	StateTreeNode(std::string nodeType, State::ptr s);
	StateTreeNode(std::string nodeType, State::ptr s, std::vector<std::shared_ptr<StateTreeNode>> children);
	StateTreeNode(std::string nodeType, State::ptr s, std::vector<std::shared_ptr<StateTreeNode>> children, std::shared_ptr<DynamicKernel>);

	// Get Functions
	std::string get_nodeType();
	State::ptr get_S();
	std::shared_ptr<StateTreeNode> get_parent();
	std::vector<std::shared_ptr<StateTreeNode>> get_children();
	std::shared_ptr<DynamicKernel> get_kernel();
	// Set Functions
	void set_nodeType(std::string nodeType);
	void set_S(State::ptr S);
	void set_parent(std::shared_ptr<StateTreeNode> parent);
	void set_children(std::vector<std::shared_ptr<StateTreeNode>> children);
	void set_kernel(std::shared_ptr<DynamicKernel> kernel);
	// Print Functions
	std::string printStateTreeNode(); // prints the State
	std::string printAbstract();	// Prints the State type
	// Decomposition Functions
	StateTreeNode introVertex(unsigned i);
	StateTreeNode forgetVertex(unsigned i);
	StateTreeNode introEdge(unsigned i, unsigned j);
	friend StateTreeNode join(StateTreeNode &left, StateTreeNode &right);
};

typedef std::shared_ptr<StateTreeNode> StateTreeNodePointer;

class StateTree {
  public:
	StateTreeNodePointer root;
	void traverseNode(StateTreeNode &node, MultiGraph &G, std::map<unsigned, unsigned> &colorToVertexMap,
					  unsigned &nVertices, unsigned &nEdges);
	MultiGraph extractMultiGraph();
	///////
	std::string printAbstractRecursive(StateTreeNode &node, unsigned &label); // This prints the tree recursively
	void printAbstract(); // This prints the tree of State Types
	std::string printTreeRecursive(StateTreeNode &node, unsigned &label);
	void printStateTree();
	void writeToFile(std::string fileName);

//	bool readToken(string::iterator &it, string token);
//	unsigned readInteger( string::iterator &it); // Reads a string, if the string starts with integers then it
//				  // returns integers else return -1. Ex "123v" returns 123
//	StateTreeNodePointer readStateTreeExpressionRecursive(
//		string::iterator &it, string::iterator end,
//		vector<StateTreeNodePointer> &v,
//		DynamicKernel &kernel); // Reads Abstract Tree Decomposition recursively
//	void readStateTree(
//		string s,
//		DynamicKernel &kernel); // Reads State Tree Decomposition from File
//	void readConcrete();		// Reads Concrete Tree Decomposition
};

#endif
