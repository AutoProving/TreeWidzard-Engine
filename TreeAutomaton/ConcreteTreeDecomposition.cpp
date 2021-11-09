// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "ConcreteTreeDecomposition.h"
#include "AbstractTreeDecomposition.h"

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


shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> ConcreteTreeDecomposition::constructATDNode(TermNode<ConcreteNode> &node) {

    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> atdNode(new TermNode<AbstractTreeDecompositionNodeContent>);
    vector<shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>>> children;
    string symbol = node.getNodeContent().getSymbol();
    AbstractTreeDecompositionNodeContent abstract;
    abstract.setSymbol(symbol);
    for(auto &child:node.getChildren()){
        shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> atdChild = constructATDNode(*child);
        atdChild->setParent(atdNode);
        children.push_back(atdChild);
    }
    atdNode->setNodeContent(abstract);
    atdNode->setChildren(children);
    return atdNode;
}


AbstractTreeDecomposition ConcreteTreeDecomposition::convertToAbstractTreeDecomposition() {
    AbstractTreeDecomposition abstractTreeDecomposition;
    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> root  = constructATDNode(*this->getRoot());
    abstractTreeDecomposition.setRoot(root);
    return abstractTreeDecomposition;
}

void ConcreteTreeDecomposition::writeToFile(string fileName) {
    fileName = "ConcreteTreeDecomposition_"+concrete_fs::path(fileName).filename().string();
    ofstream atdFile (fileName);
    if (atdFile.is_open())
    {
        atdFile << termInformation();
        atdFile.close();
    }
    else {
        cout << "Unable to open "<< fileName << endl;
        exit(20);
    }
}

State::ptr ConcreteTreeDecomposition::constructWitnesses(Conjecture &conjecture, shared_ptr<TermNode<ConcreteNode>> node, Flags &flags) {
    // First, We check the type of the node
    if (node->getNodeContent().getSymbol() == "Leaf") {
        // if it is an empty, then it is a leaf
        State::ptr q = conjecture.getKernel()->initialState();
        if(flags.get("PrintStates")) {
            q->print();
        }
        return q;
    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "IntroVertex")) {
        State::ptr childState = constructWitnesses(conjecture, node->getChildren()[0],flags);
        // find the introduced vertex
        set<unsigned> bagSet = node->getNodeContent().getBag().get_elements();
        set<unsigned> childBagSet = childState->get_bag().get_elements();
        set<unsigned> bagSetDifference;
        set_difference( bagSet.begin(), bagSet.end(), childBagSet.begin(),
                childBagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            cout
                    << "ERROR: ConcreteTreeDecomposition::constructWitnesses "
                       "in "<<node->getNodeContent().getSymbol()<<":child's bag and node's bag are not valid"
                    << endl;
                cout<<"node's bag: ";
                node->getNodeContent().getBag().print();
                cout<<"\nchild's bag: ";
                node->getChildren()[0]->getNodeContent().getBag().print();
            exit(20);
        }
        State::ptr q = conjecture.getKernel()->intro_v(
                childState, *bagSetDifference.begin());
        if(flags.get("PrintStates")) {
            q->print();
        }
        return q;

    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "ForgetVertex")) {
        State::ptr childState =
                constructWitnesses(conjecture, node->getChildren()[0],flags);
        // find the forgotten vertex
        set<unsigned> bagSet = node->getNodeContent().getBag().get_elements();
        set<unsigned> childBagSet = childState->get_bag().get_elements();
        set<unsigned> bagSetDifference;
        set_difference(
                childBagSet.begin(), childBagSet.end(), bagSet.begin(),
                bagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            cout << "ERROR: ConcreteTreeDecomposition::constructWitnesses "
                    "in ForgetVertex child's bag and node's bag are not "
                    "valid"
                 << endl;
            exit(20);
        }
        State::ptr q = conjecture.getKernel()->forget_v(childState, *bagSetDifference.begin());
        if(flags.get("PrintStates")) {
            q->print();
        }
        return q;

    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "IntroEdge")) {
        State::ptr childState = constructWitnesses(conjecture, node->getChildren()[0],flags);

        pair<unsigned, unsigned> e =
                node->getNodeContent().getBag().get_edge();
        State::ptr q = conjecture.getKernel()->intro_e(childState,e.first, e.second);
        bool conjectureEvaluationResult =
                conjecture.evaluateConjectureOnState(*q);
        if(flags.get("PrintStates")) {
            q->print();
        }
        return q;

    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "Join")) {
        State::ptr childState1 = constructWitnesses(conjecture, node->getChildren()[0],flags);
        State::ptr childState2 = constructWitnesses(conjecture, node->getChildren()[1],flags);
        State::ptr q = conjecture.getKernel()->join(childState1,childState2);
        if(flags.get("PrintStates")) {
            q->print();
        }
        return q;
    } else {
        cout << "ERROR in constructWitnesses: The function could not recognize "
                "the type of the node"
             << endl;
        cout << "The devastated node is: " << endl;
        node->getNodeContent().print();
        exit(20);
    }
}

bool ConcreteTreeDecomposition::conjectureCheck(Conjecture &conjecture,Flags &flags) {
    State::ptr q = constructWitnesses(conjecture, getRoot(),flags);
    if (!conjecture.evaluateConjectureOnState(*q)) {
        cout << "not satisfied" << endl;
        cout << "Printing bad State: " << endl;
        q.print();
        return false;
    } else {
        cout << "satisfied"<< endl;
        return true;
    }
}
