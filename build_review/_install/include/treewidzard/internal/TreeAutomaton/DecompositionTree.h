#ifndef TREEWIDZARD_DECOMPOSITIONTREE_H
#define TREEWIDZARD_DECOMPOSITIONTREE_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>  // std::pair, std::make_pair
#include <vector>

class DecompositionNode {

    private:
        std::set<unsigned> vertices;
        std::shared_ptr<DecompositionNode> parent;
        std::vector<std::shared_ptr<DecompositionNode>> children;
    public:
        DecompositionNode();
        std::set<unsigned> getVertices();
        void addVertex(unsigned i);
        void setVertices(std::set<unsigned> vertices);
        void setParent(std::shared_ptr<DecompositionNode> node);
        std::shared_ptr<DecompositionNode> getParent();
        std::vector<std::shared_ptr<DecompositionNode>>  getChildren();
        void addChild(std::shared_ptr<DecompositionNode> child);
        void setChildren(std::vector<std::shared_ptr<DecompositionNode>> children);
};

class Decomposition{
    private:
        std::shared_ptr<DecompositionNode> root;
        int maxBagSize = 0;
        std::set<unsigned> vertices; // vertices of the orginal graphs
    public:
        Decomposition();
        void setRoot(std::shared_ptr<DecompositionNode> node);
        void print();
        std::string stringPrint();
        std::string printNode(std::shared_ptr<DecompositionNode> node,int &labeli,int parentLabel,std::list<std::pair<int,int>> &edges);
        void writeToFile(std::string fileName);
        void findMaxBagSizeAndVertices(std::shared_ptr<DecompositionNode> node);
};

#endif
