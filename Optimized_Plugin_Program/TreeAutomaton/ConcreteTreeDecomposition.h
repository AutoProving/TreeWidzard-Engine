// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
#define TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
#include <iostream>
#include <cstring>
#include "Term.h"
#include "../Kernel/Bag.h"
#include "../Multigraph/MultiGraph.h"
#include "../Conjecture/Conjecture.h"
#include "../Kernel/Flags.h"
#include "RunTree.h"
#include "DecompositionTree.h"

namespace concrete_fs = std::experimental::filesystem;
class AbstractTreeDecompositionNodeContent;
class AbstractTreeDecomposition;
class ConcreteNode : public TermNodeContentType
{
    private :
        Bag bag;
        std::string symbol = "Leaf";
    public:
        const Bag &getBag() const;
        void setBag(const Bag &bag);
        const std::string &getSymbol() const;
        void setSymbol(const std::string &symbol);
        bool operator==(const ConcreteNode &rhs) const;
        bool operator!=(const ConcreteNode &rhs) const;
        bool operator<(const ConcreteNode &rhs) const;
        bool operator>(const ConcreteNode &rhs) const;
        bool operator<=(const ConcreteNode &rhs) const;
        bool operator>=(const ConcreteNode &rhs) const;
        void print() override;
        std::string nodeInformation() override;
};
class ConcreteTreeDecomposition: public Term<ConcreteNode> {
    public:
        void traverseNode(TermNode<ConcreteNode> &node, MultiGraph &G, std::map<unsigned, unsigned> &colorToVertexMap, unsigned &nVertices, unsigned &nEdges);
        MultiGraph extractMultiGraph(); // extract the graph
        void buildDecompositionBags(std::shared_ptr<DecompositionNode> node,TermNode<ConcreteNode> &cNode, std::map<unsigned, unsigned> &colorToVertexMap, unsigned &nVertices);
        Decomposition extractDecomposition(); // extract decomposition tree
        // note: extractMultiGraph() and extractDecomposition() should have the same algorithm because of the labels.
        std::shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> constructATDNode(TermNode<ConcreteNode> &node);
        AbstractTreeDecomposition convertToAbstractTreeDecomposition();
        void writeToFile(std::string fileName);
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> constructWitnesses(Conjecture &conjecture, std::shared_ptr<TermNode<ConcreteNode>> node, Flags &flags, std::string &str);
        bool conjectureCheck(Conjecture &conjecture, Flags &flags, std::string path);
};
#endif //TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
