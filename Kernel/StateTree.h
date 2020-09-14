// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef STATETREE_H
#define STATETREE_H

#include <iostream>
#include "../Multigraph/MultiGraph.h"
#include "Bag.h"
#include <algorithm>
#include <vector>
#include <string.h>
#include "State.h"
#include "DynamicKernel.h"
using namespace std;

class StateTreeNode :public std::enable_shared_from_this<StateTreeNode> {
protected:
    string nodeType; // Default: "Empty"
    shared_ptr<State> S; // Default: Empty State
    shared_ptr<StateTreeNode> parent = nullptr;  // Default Value
    vector<shared_ptr<StateTreeNode>> children; // Default: no child
    shared_ptr<DynamicKernel> kernel;
public:
    //Constructors 
    StateTreeNode();
    StateTreeNode(string nodeType, shared_ptr<State> s);
    StateTreeNode(string nodeType, shared_ptr<State> s, vector<shared_ptr<StateTreeNode> > children);
    StateTreeNode(string nodeType, shared_ptr<State> s, vector<shared_ptr<StateTreeNode> > children, shared_ptr<DynamicKernel> );

    //Get Functions
    string get_nodeType();
    shared_ptr<State> get_S();
    shared_ptr<StateTreeNode> get_parent();
    vector<shared_ptr<StateTreeNode> > get_children();
    shared_ptr<DynamicKernel> get_kernel();
    //Set Functions
    void set_nodeType(string nodeType);
    void set_S(shared_ptr<State> S);
    void set_parent(shared_ptr<StateTreeNode> parent);
    void set_children(vector<shared_ptr<StateTreeNode> > children);
    void set_kernel(shared_ptr<DynamicKernel> kernel);
    //Print Functions
    void printStateTreeNode(); // prints the State
    string printAbstract(); // Prints the State type
    //Read Functions
    void readAbstract(); // Reads the BagType
    void readConcrete(); // Reads the Bag
    //Decomposition Functions
    StateTreeNode introVertex(unsigned i);
    StateTreeNode forgetVertex(unsigned i);
    StateTreeNode introEdge(unsigned i,unsigned j);
    friend StateTreeNode join(StateTreeNode &left, StateTreeNode &right);
};



class StateTree{
public:
    shared_ptr<StateTreeNode> root;
    void traverseNode(StateTreeNode &node, MultiGraph &G, map<unsigned,unsigned> &colorToVertexMap, unsigned &nVertices, unsigned &nEdges);
    MultiGraph extractMultiGraph();
    string printAbstractRecursive(StateTreeNode &node, unsigned  &label); // This prints the tree recursively
    void printAbstract(); // This prints the tree of State Types
    void printTreeRecursive(StateTreeNode& node, unsigned &label);
    void printStateTree();
    void printConcrete(); // This prints the tree of State
    bool readToken(string::iterator &it, string token);
    unsigned readInteger(string::iterator &it);// Reads a string, if the string starts with integers then it returns integers else return -1. Ex "123v" returns 123
    shared_ptr<StateTreeNode> readStateTreeExpressionRecursive(string::iterator &it, string::iterator end, vector<shared_ptr<StateTreeNode> > &v,DynamicKernel &kernel); // Reads Abstract Tree Decomposition recursively
    void readStateTree(string s, DynamicKernel &kernel) ;// Reads State Tree Decomposition from File
    void readConcrete(); // Reads Concrete Tree Decomposition
};

#endif
