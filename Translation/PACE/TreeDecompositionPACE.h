#ifndef TREEDECOMPOSITIONPACE_H
#define TREEDECOMPOSITIONPACE_H

#include <iostream>
#include <set>
#include <vector>
#include "../../Multigraph/MultiGraph.h"
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"

// TODO: vertex_t -> bag_set
typedef std::set<unsigned> vertex_t;

class RawAbstractTreeDecomposition{
    private:
    public:
        Bag bag;
        std::vector<std::shared_ptr<RawAbstractTreeDecomposition> > children;
        std::shared_ptr<RawAbstractTreeDecomposition> parent;
        std::string type;
        std::map<unsigned,unsigned> color_to_vertex_map;
        // constructor
        RawAbstractTreeDecomposition();
        void  print(unsigned &k, unsigned parentno);
        void printNode();
};

class TreeDecompositionPACE {
    private:
    public:
        std::vector<vertex_t> bags;
        std::set<std::pair<unsigned,unsigned> > edges;
        unsigned num_vertices;
        unsigned num_edges;
        unsigned num_graph_vertices;
        unsigned width;
        std::string width_type; // tree_width or path_width
        std::shared_ptr<RawAbstractTreeDecomposition> root;
        std::shared_ptr<MultiGraph> multigraph;
        TreeDecompositionPACE();
        void setNum_vertices(unsigned n);
        void setNum_graph_vertices(unsigned n);
        void setWidth(unsigned w);
        void setWidthType(const std::string &widthType);
        bool addVertexToBag(unsigned vertex, unsigned bagnumber);
        bool setABag(vertex_t s, unsigned bagnumber);
        bool addEdge(unsigned e1, unsigned e2);
        //////////////////////////////////////////////////////////////
        unsigned int getWidth() const;
        const std::string &getWidthType() const;
        ////////////////////////////////////////////
        void printBags();
        void printEdges();
        void print();
        void printTree();
        // Constructs the Raw abstract tree decomposition
        std::shared_ptr<RawAbstractTreeDecomposition> constructInnerNodes(std::set<unsigned> &visited_bags, unsigned neighbor);
        bool constructRaw();
        bool convertToBinary(std::shared_ptr<RawAbstractTreeDecomposition> node);// Converts a tree decomposition to a binary tree decomposition
        bool eliminateDuplicate(std::shared_ptr<RawAbstractTreeDecomposition> node); // If a node has one child and the bag is identical, we merge these two nodes
        bool joinFormat(std::shared_ptr<RawAbstractTreeDecomposition> node);// Adds join nodes to a tree decomposition
        bool addEmptyNodes(std::shared_ptr<RawAbstractTreeDecomposition> node);// Adds empty nodes to a tree decomposition
        bool addIntroVertex(std::shared_ptr<RawAbstractTreeDecomposition> node);// Adds introVertex nodes to a tree decomposition
        bool addForgetVertex(std::shared_ptr<RawAbstractTreeDecomposition> node);// Adds forgetVertex nodes to a tree decomposition
        bool addIntroEdge(std::shared_ptr<RawAbstractTreeDecomposition> node, std::set<unsigned>& visited_edges);// add introEdge nodes to a tree decomposition
        bool colorNode(std::shared_ptr<RawAbstractTreeDecomposition> node, std::vector<unsigned> &color_vertex, std::vector<unsigned> &vertex_color);// sub function of colorTree
        bool colorTree();// Colors a tree decomposition with tree-width+1 colors
        bool updateInnerNodeTD(std::shared_ptr<RawAbstractTreeDecomposition> node, unsigned &number,unsigned parentno);
        bool updateTD();
        void construct();
        bool validateTree(std::shared_ptr<RawAbstractTreeDecomposition> node);
        void createCTDNode(std::shared_ptr<TermNode<ConcreteNode>> cnode, std::shared_ptr<RawAbstractTreeDecomposition> rnode);
        std::shared_ptr<ConcreteTreeDecomposition> convertToConcreteTreeDecomposition();
};


#endif
