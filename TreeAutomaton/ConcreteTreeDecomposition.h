// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
#define TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
#include <iostream>
#include <cstring>
#include "Term.h"
#include "../Kernel/Bag.h"
#include "../Multigraph/MultiGraph.h"
#include "../Kernel/Conjecture.h"

using namespace std;

namespace concrete_fs = std::experimental::filesystem;

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
    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> constructATDNode(TermNode<ConcreteNode> &node);
    AbstractTreeDecomposition convertToAbstractTreeDecomposition();
    void writeToFile(string fileName);
    State::ptr constructWitnesses(Conjecture &conjecture, shared_ptr<TermNode<ConcreteNode>> node);
    bool conjectureCheck(Conjecture &conjecture);


};


#endif //TREEWIDZARD_CONCRETETREEDECOMPOSITION_H
