// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CONCRETETREEDECOMPOSITION_H
#define CONCRETETREEDECOMPOSITION_H

#include <string.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include "../Multigraph/MultiGraph.h"
#include "Bag.h"
#include "Conjecture.h"
#include "StateTree.h"

class CTDNodeNew : public std::enable_shared_from_this<CTDNodeNew> {
  protected:
	std::string nodeType;							   // Default: "Empty"
	Bag B;											   // Default: Empty bag
	std::shared_ptr<CTDNodeNew> parent = nullptr;	   // Default Value
	std::vector<std::shared_ptr<CTDNodeNew>> children; // Default: no child
  public:
	// Constructors
	CTDNodeNew();
	CTDNodeNew(std::string nodeType, Bag B);
	CTDNodeNew(std::string nodeType, Bag B,
			   std::vector<std::shared_ptr<CTDNodeNew>> children);
	// Get Functions
	std::string get_nodeType();
	Bag get_B();
	std::shared_ptr<CTDNodeNew> get_parent();
	std::vector<std::shared_ptr<CTDNodeNew>> get_children();
	// Set Functions
	void set_nodeType(std::string nodeType);
	void set_B(Bag B);
	void set_parent(std::shared_ptr<CTDNodeNew> parent);
	void set_children(std::vector<std::shared_ptr<CTDNodeNew>> children);
	// Print Functions
	std::string printCTDNode();
	std::string printITD(); // Prints the Bagtype
	void printConcrete();	// Prints the Bag
	// Read Functions
	void readITD();		 // Reads the BagType
	void readConcrete(); // Reads the Bag
	// Decomposition Functions
	CTDNodeNew introVertex(unsigned i);
	CTDNodeNew forgetVertex(unsigned i);
	CTDNodeNew introEdge(unsigned i, unsigned j);
	friend CTDNodeNew join(CTDNodeNew left, CTDNodeNew right);
};

class ConcreteTreeDecomposition {
  public:
	std::shared_ptr<CTDNodeNew> root;
	MultiGraph extractMultiGraph();
	void traverseNode(CTDNodeNew &node, MultiGraph &G,
					  std::map<unsigned, unsigned> &colorToVertexMap,
					  unsigned &nVertices, unsigned &nEdges);
	std::string printITDRecursive(
		CTDNodeNew &node, unsigned &label); // This prints the tree recursively
	void printITD();						// This prints the tree of Bag Types
	std::string printTreeRecursive(CTDNodeNew &node, unsigned &label);
	void printTree();
	void printConcrete(); // This prints the tree of Bags
	bool readToken(std::string::iterator &it, std::string token);
	unsigned readInteger(
		std::string::iterator
			&it); // Reads a string, if the string starts with integers it
				  // returns integers else returns -1. Ex "123abc" returns 123
	bool readITDVertexNumber(std::string::iterator &it,
							 unsigned &v); // This function will be deleted
	bool readITDEdgeNumber(
		std::string::iterator &it,
		std::pair<unsigned, unsigned> &e); // This function will be deleted
	std::shared_ptr<CTDNodeNew> readITDExpressionRecursive(
		std::string::iterator &it, std::string::iterator end,
		std::vector<std::shared_ptr<CTDNodeNew>>
			&v); // Reads Instructive Tree Decomposition recursively
	void readITD(std::string s); // Reads Instructive Tree Decomposition
	void readConcrete();		 // Reads Concrete Tree Decomposition
	// take a conjecture as an input and check that the concrete tree
	// decomposition satisfies the conjecture or not
	std::pair<bool, State::ptr> constructWitnesses(
		Conjecture &conjecture, std::shared_ptr<CTDNodeNew> node);
	bool conjectureCheck(Conjecture &conjecture);
	std::shared_ptr<StateTreeNode> generateStateTreeNode(
		std::shared_ptr<CTDNodeNew> node,
		std::shared_ptr<DynamicKernel> kernel);
	StateTree &convertToStateTree(std::shared_ptr<DynamicKernel> kernel);
};

#endif
