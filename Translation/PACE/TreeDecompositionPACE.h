#ifndef TREEDECOMPOSITIONPACE_H
#define TREEDECOMPOSITIONPACE_H

#include <iostream>
#include <set>
#include <vector>
#include "../../Multigraph/MultiGraph.h"
#include "../../TreeAutomaton/ConcreteTreeDecomposition.h"

using namespace std;
typedef set<unsigned> vertex_t;

class RawAbstractTreeDecomposition{
    private:
    public:
        Bag bag;
        vector<shared_ptr<RawAbstractTreeDecomposition> > children;
        shared_ptr<RawAbstractTreeDecomposition> parent;
        string type;
        map<unsigned,unsigned> color_to_vertex_map;
        // constructor
        RawAbstractTreeDecomposition();
        void  print(unsigned &k, unsigned parentno);
        void printNode();
};

class TreeDecompositionPACE {
    private:
    public:
        vector<vertex_t> bags;
        set<pair<unsigned,unsigned> > edges;
        unsigned num_vertices;
        unsigned num_edges;
        unsigned num_graph_vertices;
        unsigned width;
        string width_type; // tree_width or path_width
        shared_ptr<RawAbstractTreeDecomposition> root;
        shared_ptr<MultiGraph> multigraph;
        TreeDecompositionPACE();

        void setNum_vertices(unsigned n);
        void setNum_graph_vertices(unsigned n);
        void setWidth(unsigned w);
        void setWidthType(const string &widthType);
        bool addVertexToBag(unsigned vertex, unsigned bagnumber);
        bool setABag(vertex_t s, unsigned bagnumber);
        bool addEdge(unsigned e1, unsigned e2);
        //////////////////////////////////////////////////////////////
        unsigned int getWidth() const;
        const string &getWidthType() const;
        ////////////////////////////////////////////
        void printBags();
        void printEdges();
        void print();
        void printTree();
        // Constructs the Raw abstract tree decomposition
        shared_ptr<RawAbstractTreeDecomposition> constructInnerNodes(set<unsigned> &visited_bags, unsigned neighbor);
        bool constructRaw();
        bool convertToBinary(shared_ptr<RawAbstractTreeDecomposition> node);// Converts a tree decomposition to a binary tree decomposition
        bool joinFormat(shared_ptr<RawAbstractTreeDecomposition> node);// Adds join nodes to a tree decomposition
        bool addEmptyNodes(shared_ptr<RawAbstractTreeDecomposition> node);// Adds empty nodes to a tree decomposition
        bool addIntroVertex(shared_ptr<RawAbstractTreeDecomposition> node);// Adds introVertex nodes to a tree decomposition
        bool addForgetVertex(shared_ptr<RawAbstractTreeDecomposition> node);// Adds forgetVertex nodes to a tree decomposition
        bool addIntroEdge(shared_ptr<RawAbstractTreeDecomposition> node, set<unsigned>& visited_edges);// add introEdge nodes to a tree decomposition
        bool colorNode(shared_ptr<RawAbstractTreeDecomposition> node, vector<unsigned> &color_vertex, vector<unsigned> &vertex_color);// sub function of colorTree
        bool colorTree();// Colors a tree decomposition with tree-width+1 colors
        bool updateInnerNodeTD(shared_ptr<RawAbstractTreeDecomposition> node, unsigned &number,unsigned parentno);
        bool updateTD();
        void construct();
        void createCTDNode(shared_ptr<TermNode<ConcreteNode>> cnode, shared_ptr<RawAbstractTreeDecomposition> rnode);
        shared_ptr<ConcreteTreeDecomposition> convertToConcreteTreeDecomposition();
};


#endif
