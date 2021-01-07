// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CONCRETETREEDECOMPOSITION_H
#define CONCRETETREEDECOMPOSITION_H

#include <string.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <experimental/filesystem>
#include "../Multigraph/MultiGraph.h"
#include "Bag.h"
#include "Conjecture.h"
#include "StateTree.h"


using namespace std;
namespace concrete_fs = std::experimental::filesystem;

class CTDNodeNew : public enable_shared_from_this<CTDNodeNew> {
  protected:
	string nodeType;						 // Default: "Empty"
	Bag B;	    								 // Default: Empty bag
	shared_ptr<CTDNodeNew> parent = nullptr; // Default Value
	vector<shared_ptr<CTDNodeNew>> children; // Default: no child
  public:
	// Constructors
	CTDNodeNew();
	CTDNodeNew(string nodeType, Bag B);
	CTDNodeNew(string nodeType, Bag B, vector<shared_ptr<CTDNodeNew>> children);
	// Get Functions
	string get_nodeType();
	Bag get_B();
	shared_ptr<CTDNodeNew> get_parent();
	vector<shared_ptr<CTDNodeNew>> get_children();
	// Set Functions
	void set_nodeType(string nodeType);
	void set_B(Bag B);
	void set_parent(shared_ptr<CTDNodeNew> parent);
	void set_children(vector<shared_ptr<CTDNodeNew>> children);
	// Print Functions
	string printCTDNode();
	string printAbstract(); // Prints the Bagtype
	void printConcrete();   // Prints the Bag
	// Read Functions
	void readAbstract(); // Reads the BagType
	void readConcrete(); // Reads the Bag
	// Decomposition Functions
	CTDNodeNew introVertex(unsigned i);
	CTDNodeNew forgetVertex(unsigned i);
	CTDNodeNew introEdge(unsigned i, unsigned j);
	friend CTDNodeNew join(CTDNodeNew left, CTDNodeNew right);
};

class ConcreteTreeDecomposition {
  public:
	shared_ptr<CTDNodeNew> root= nullptr;
	MultiGraph extractMultiGraph();
	void set_root(shared_ptr<CTDNodeNew> node){root = node;};
	void traverseNode(CTDNodeNew &node, MultiGraph &G, map<unsigned, unsigned> &colorToVertexMap, unsigned &nVertices, unsigned &nEdges);
	string printAbstractRecursive( CTDNodeNew &node, unsigned &label); // This prints the tree recursively
	void printAbstract(); // This prints the tree of Bag Types
	void writeToFileAbstract(string fileName);
	string printTreeRecursive(CTDNodeNew &node, unsigned &label);
	void printTree();
	bool readToken(string::iterator &it, string token);
	unsigned readInteger(string::iterator &it); // Reads a string, if the string starts with integers it
				  // returns integers else returns -1. Ex "123abc" returns 123
	bool readAbstractVertexNumber(string::iterator &it, unsigned &v); // This function will be deleted
	bool readAbstractEdgeNumber( string::iterator &it, pair<unsigned, unsigned> &e); // This function will be deleted
	shared_ptr<CTDNodeNew> readAbstractExpressionRecursive( string::iterator &it, string::iterator end, vector<shared_ptr<CTDNodeNew>> &v); // Reads Abstract Tree Decomposition recursively
	void readAbstract(string s); // Reads Abstract Tree Decomposition
	void readConcrete(); // Reads Concrete Tree Decomposition
	// take a conjecture as an input and check that the concrete tree
	// decomposition satisfies the conjecture or not
	pair<bool, State::ptr> constructWitnesses(Conjecture &conjecture, shared_ptr<CTDNodeNew> node);
	bool conjectureCheck(Conjecture &conjecture);
	shared_ptr<StateTreeNode> generateStateTreeNode( shared_ptr<CTDNodeNew> node, shared_ptr<DynamicKernel> kernel);
	StateTree convertToStateTree(shared_ptr<DynamicKernel> kernel);
};

#endif
