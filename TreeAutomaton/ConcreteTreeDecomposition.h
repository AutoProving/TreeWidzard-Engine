// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
#define TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
#include <iostream>
#include <cstring>
#include "Term.h"
#include "../Kernel/Bag.h"
#include "../Multigraph/MultiGraph.h"

using namespace std;


class AbstractTreeDecompositionNodeContent;
class AbstractTreeDecomposition;

class ConcreteNode : public TermNodeContentType
{
private :
    Bag bag;
    string symbol = "Leaf";
public:
    const Bag &getBag() const;

    void setBag(const Bag &bag);

    const string &getSymbol() const;

    void setSymbol(const string &symbol);

    bool operator==(const ConcreteNode &rhs) const;

    bool operator!=(const ConcreteNode &rhs) const;

    bool operator<(const ConcreteNode &rhs) const;

    bool operator>(const ConcreteNode &rhs) const;

    bool operator<=(const ConcreteNode &rhs) const;

    bool operator>=(const ConcreteNode &rhs) const;

    void print() override;

    string nodeInformation() override;
};


class ConcreteTreeDecomposition: public Term<ConcreteNode> {
public:
    void traverseNode(TermNode<ConcreteNode> &node, MultiGraph &G, map<unsigned, unsigned> &colorToVertexMap, unsigned &nVertices, unsigned &nEdges);
    MultiGraph extractMultiGraph();
    AbstractTreeDecomposition convertToAbstractTreeDecomposition();

};


#endif //TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
