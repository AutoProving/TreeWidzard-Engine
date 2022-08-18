// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef TREEWIDZARD_DECOMPOSITIONTREE_H
#define TREEWIDZARD_DECOMPOSITIONTREE_H
#include <iostream>
#include <memory>
#include<set>
#include<vector>
#include<list>
#include <utility>      // std::pair, std::make_pair
#include <string>
#include<fstream>
#include<algorithm>
using namespace std;

class DecompositionNode {

    private:
        set<unsigned> vertices;
        shared_ptr<DecompositionNode> parent;
        vector<shared_ptr<DecompositionNode>> children;
    public:
        DecompositionNode();
        set<unsigned> getVertices();
        void addVertex(unsigned i);
        void setVertices(set<unsigned> vertices);
        void setParent(shared_ptr<DecompositionNode> node);
        shared_ptr<DecompositionNode> getParent();
        vector<shared_ptr<DecompositionNode>>  getChildren();
        void addChild(shared_ptr<DecompositionNode> child);
        void setChildren(vector<shared_ptr<DecompositionNode>> children);
};

class Decomposition{
    private: 
        shared_ptr<DecompositionNode> root;
        int maxBagSize = 0;
        set<unsigned> vertices; // vertices of the orginal graphs
    public:
        Decomposition();
        void setRoot(shared_ptr<DecompositionNode> node);
        void print();
        string stringPrint();
        string printNode(shared_ptr<DecompositionNode> node,int &labeli,int parentLabel,list<pair<int,int>> &edges);
        void writeToFile(string fileName);
        void findMaxBagSizeAndVertices(shared_ptr<DecompositionNode> node);
};

#endif
