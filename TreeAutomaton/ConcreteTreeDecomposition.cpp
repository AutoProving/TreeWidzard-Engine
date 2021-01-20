//
// Created by farhad on 20.01.2021.
//

#include "ConcreteTreeDecomposition.h"

const Bag &ConcreteNode::getBag() const {
    return bag;
}

void ConcreteNode::setBag(const Bag &bag) {
    ConcreteNode::bag = bag;
}

const string &ConcreteNode::getSymbol() const {
    return symbol;
}

void ConcreteNode::setSymbol(const string &symbol) {
    ConcreteNode::symbol = symbol;
}

bool ConcreteNode::operator==(const ConcreteNode &rhs) const {
           return bag == rhs.bag &&
           symbol == rhs.symbol;
}

bool ConcreteNode::operator!=(const ConcreteNode &rhs) const {
    return !(rhs == *this);
}

bool ConcreteNode::operator<(const ConcreteNode &rhs) const {
    if (bag < rhs.bag)
        return true;
    if (rhs.bag < bag)
        return false;
    return symbol < rhs.symbol;
}

bool ConcreteNode::operator>(const ConcreteNode &rhs) const {
    return rhs < *this;
}

bool ConcreteNode::operator<=(const ConcreteNode &rhs) const {
    return !(rhs < *this);
}

bool ConcreteNode::operator>=(const ConcreteNode &rhs) const {
    return !(*this < rhs);
}

void ConcreteNode::print() {
    cout<<symbol<<" ";
    bag.print();
}

string ConcreteNode::nodeInformation() {
    string info = symbol + bag.bagInformation();
    return info;
}

MultiGraph ConcreteTreeDecomposition::extractMultiGraph() {
    MultiGraph G;
    map<unsigned, unsigned> colorToVertexMap;
    unsigned nVertices = 0;
    unsigned nEdges = 0;

    set<unsigned> elements = this->getRoot()->getNodeContent().getBag().get_elements();
    set<unsigned>::iterator it = elements.begin();
    for (size_t i = 1; i <= this->getRoot()->getNodeContent().getBag().get_elements().size(); ++i) {
        colorToVertexMap[*it] = i; // creates one vertex for each color.
        nVertices = i;
        G.addVertex(nVertices);
        ++it;
    }
    traverseNode(*(this->getRoot()), G, colorToVertexMap, nVertices, nEdges);
    return G;
}

void ConcreteTreeDecomposition::traverseNode(TermNode<ConcreteNode> &node, MultiGraph &G,
                                             map<unsigned int, unsigned int> &colorToVertexMap, unsigned int &nVertices,
                                             unsigned int &nEdges) {
    map<unsigned, unsigned> colorToVertexMapCopy = colorToVertexMap;
    if (node.getNodeContent().getSymbol() == "Leaf") {
        // do nothing
    } else if (strstr(node.getNodeContent().getSymbol().c_str(), "IntroVertex")) {
        /////////////// Finding the introduced vertex ///////////
        set<unsigned> bagSet = node.getNodeContent().getBag().get_elements();
        set<unsigned> childBagSet =
                node.getChildren()[0]->getNodeContent().getBag().get_elements();
        set<unsigned> bagSetDifference;
        set_difference(
                bagSet.begin(), bagSet.end(), childBagSet.begin(),
                childBagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));

        if (bagSetDifference.size() != 1) {
            cout << "ERROR: ConcreteTreeDecomposition::traverseNode in "
                    "IntroVertex child's bag and node's bag are not valid"
                 << endl;

            exit(20);
        }
        //////////// End of Finding the introduced vertex ///////////
        colorToVertexMapCopy.erase(*(bagSetDifference.begin()));
        traverseNode(*(node.getChildren()[0]), G, colorToVertexMapCopy,
                     nVertices,
                     nEdges); // Nothing happens. Just process the next bag.
    } else if (strstr(node.getNodeContent().getSymbol().c_str(), "ForgetVertex")) {
        /////////////// Finding the Forgotten vertex ///////////
        set<unsigned> bagSet = node.getNodeContent().getBag().get_elements();
        set<unsigned> childBagSet =
                node.getChildren()[0]->getNodeContent().getBag().get_elements();
        set<unsigned> bagSetDifference;
        set_difference(
                childBagSet.begin(), childBagSet.end(), bagSet.begin(),
                bagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));

        if (bagSetDifference.size() != 1) {
            cout << "ERROR: ConcreteTreeDecomposition::traverseNode in "
                    "ForgetVertex child's bag and node's bag are not valid"
                 << endl;
            exit(20);
        }
        //////////// End of Finding the Forgotten vertex ///////////

        nVertices = nVertices + 1;
        G.addVertex(nVertices);
        colorToVertexMapCopy[*(bagSetDifference.begin())] = nVertices;
        traverseNode(*(node.getChildren()[0]), G, colorToVertexMapCopy,
                     nVertices,
                     nEdges); // Nothing happens. Just process the next bag.
    } else if (strstr(node.getNodeContent().getSymbol().c_str(), "IntroEdge")) {
        nEdges = nEdges + 1;
        pair<unsigned, unsigned> e = node.getNodeContent().getBag().get_edge();
        G.addEdgeLabel(nEdges);
        G.addIncidence(nEdges, colorToVertexMap.find(e.first)->second);
        G.addIncidence(nEdges, colorToVertexMap.find(e.second)->second);
        traverseNode(*(node.getChildren()[0]), G, colorToVertexMapCopy,
                     nVertices, nEdges);
    } else if (node.getNodeContent().getSymbol() == "Join") {
        traverseNode(*(node.getChildren()[0]), G, colorToVertexMapCopy,
                     nVertices, nEdges);
        traverseNode(*(node.getChildren()[1]), G, colorToVertexMapCopy,
                     nVertices, nEdges);
    }

}
