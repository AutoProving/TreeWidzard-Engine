#include "ConcreteTreeDecomposition.h"
#include "AbstractTreeDecomposition.h"
const Bag &ConcreteNode::getBag() const {
    return bag;
}
void ConcreteNode::setBag(const Bag &bag) {
    ConcreteNode::bag = bag;
}
const std::string &ConcreteNode::getSymbol() const {
    return symbol;
}
void ConcreteNode::setSymbol(const std::string &symbol) {
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
    std::cout<<symbol<<" ";
    bag.print();
}
std::string ConcreteNode::nodeInformation() {
    std::string info = symbol + bag.bagInformation();
    return info;
}
MultiGraph ConcreteTreeDecomposition::extractMultiGraph() {
    MultiGraph G;
    std::map<unsigned, unsigned> colorToVertexMap;
    unsigned nVertices = 0;
    unsigned nEdges = 0;
    std::set<unsigned> elements = this->getRoot()->getNodeContent().getBag().get_elements();
    std::set<unsigned>::iterator it = elements.begin();
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
        std::map<unsigned int, unsigned int> &colorToVertexMap, unsigned int &nVertices,
        unsigned int &nEdges) {
    std::map<unsigned, unsigned> colorToVertexMapCopy = colorToVertexMap;
    if (node.getNodeContent().getSymbol() == "Leaf") {
        // do nothing
    } else if (strstr(node.getNodeContent().getSymbol().c_str(), "IntroVertex")) {
        /////////////// Finding the introduced vertex ///////////
        std::set<unsigned> bagSet = node.getNodeContent().getBag().get_elements();
        std::set<unsigned> childBagSet =
            node.getChildren()[0]->getNodeContent().getBag().get_elements();
        std::set<unsigned> bagSetDifference;
        set_difference(
                bagSet.begin(), bagSet.end(), childBagSet.begin(),
                childBagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            std::cout << "ERROR: ConcreteTreeDecomposition::traverseNode in "
                "IntroVertex child's bag and node's bag are not valid"
                << std::endl;
            exit(20);
        }
        //////////// End of Finding the introduced vertex ///////////
        colorToVertexMapCopy.erase(*(bagSetDifference.begin()));
        traverseNode(*(node.getChildren()[0]), G, colorToVertexMapCopy,
                nVertices,
                nEdges); // Nothing happens. Just process the next bag.
    } else if (strstr(node.getNodeContent().getSymbol().c_str(), "ForgetVertex")) {
        /////////////// Finding the Forgotten vertex ///////////
        std::set<unsigned> bagSet = node.getNodeContent().getBag().get_elements();
        std::set<unsigned> childBagSet =
            node.getChildren()[0]->getNodeContent().getBag().get_elements();
        std::set<unsigned> bagSetDifference;
        set_difference(
                childBagSet.begin(), childBagSet.end(), bagSet.begin(),
                bagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            std::cout << "ERROR: ConcreteTreeDecomposition::traverseNode in "
                "ForgetVertex child's bag and node's bag are not valid"
                << std::endl;
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
        std::pair<unsigned, unsigned> e = node.getNodeContent().getBag().get_edge();
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


Decomposition ConcreteTreeDecomposition::extractDecomposition(){
    Decomposition decomposition;
    std::map<unsigned, unsigned> colorToVertexMap;
    unsigned nVertices = 0;
    unsigned nEdges = 0;
    std::set<unsigned> elements = this->getRoot()->getNodeContent().getBag().get_elements();
    std::set<unsigned>::iterator it = elements.begin();
    std::shared_ptr<DecompositionNode> root( new DecompositionNode());

    for (size_t i = 1; i <= this->getRoot()->getNodeContent().getBag().get_elements().size(); ++i) {
        colorToVertexMap[*it] = i; // creates one vertex for each color.
        nVertices = i;
        root->addVertex(nVertices);
        ++it;
    }
    buildDecompositionBags(root,*(this->getRoot()), colorToVertexMap, nVertices);
    decomposition.setRoot(root);
    return decomposition;
}

void ConcreteTreeDecomposition::buildDecompositionBags(std::shared_ptr<DecompositionNode> node, TermNode<ConcreteNode> &cNode, std::map<unsigned, unsigned> &colorToVertexMap,
        unsigned &nVertices){
    std::map<unsigned, unsigned> colorToVertexMapCopy = colorToVertexMap;
    if (cNode.getNodeContent().getSymbol() == "Leaf") {
        // do nothing
    } else if (strstr(cNode.getNodeContent().getSymbol().c_str(), "IntroVertex")) {
        /////////////// Finding the introduced vertex ///////////
        std::set<unsigned> bagSet = cNode.getNodeContent().getBag().get_elements();
        std::set<unsigned> childBagSet =
            cNode.getChildren()[0]->getNodeContent().getBag().get_elements();
        std::set<unsigned> bagSetDifference;
        set_difference(
                bagSet.begin(), bagSet.end(), childBagSet.begin(),
                childBagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            std::cout << "ERROR: ConcreteTreeDecomposition::buildDecompositionBags in "
                "IntroVertex child's bag and node's bag are not valid"
                << std::endl;
            exit(20);
        }
        //////////// End of Finding the introduced vertex ///////////

        //std::shared_ptr<DecompositionNode> nNode (new DecompositionNode());
        //set<unsigned> vertices = node->getVertices();
        //vertices.erase(*(bagSetDifference.begin()));
        //nNode->setVertices(vertices);
        //nNode->setParent(node);
        //node->addChild(nNode);

        colorToVertexMapCopy.erase(*(bagSetDifference.begin()));

        buildDecompositionBags(node,*(cNode.getChildren()[0]), colorToVertexMapCopy,
                nVertices); // Nothing happens. Just process the next bag.
    } else if (strstr(cNode.getNodeContent().getSymbol().c_str(), "ForgetVertex")) {
        /////////////// Finding the Forgotten vertex ///////////
        std::set<unsigned> bagSet = cNode.getNodeContent().getBag().get_elements();
        std::set<unsigned> childBagSet =
            cNode.getChildren()[0]->getNodeContent().getBag().get_elements();
        std::set<unsigned> bagSetDifference;
        set_difference(
                childBagSet.begin(), childBagSet.end(), bagSet.begin(),
                bagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            std::cout << "ERROR: ConcreteTreeDecomposition::buildDecompositionBags in "
                "ForgetVertex child's bag and node's bag are not valid"
                << std::endl;
            exit(20);
        }
        //////////// End of Finding the Forgotten vertex ///////////
        nVertices = nVertices + 1;
        colorToVertexMapCopy[*(bagSetDifference.begin())] = nVertices;
        std::shared_ptr<DecompositionNode> nNode (new DecompositionNode());
        for(auto item:colorToVertexMapCopy) nNode->addVertex(item.second);
       // nNode->setVertices(node->getVertices());
      //  nNode->addVertex(nVertices);
        nNode->setParent(node);
        node->addChild(nNode);
        buildDecompositionBags(nNode,*(cNode.getChildren()[0]), colorToVertexMapCopy,
                nVertices); // Nothing happens. Just process the next bag.
    } else if (strstr(cNode.getNodeContent().getSymbol().c_str(), "IntroEdge")) {
        buildDecompositionBags(node,*(cNode.getChildren()[0]), colorToVertexMapCopy,
                nVertices);
    } else if (cNode.getNodeContent().getSymbol() == "Join") {
        buildDecompositionBags(node,*(cNode.getChildren()[0]), colorToVertexMapCopy,
                nVertices);
        buildDecompositionBags(node,*(cNode.getChildren()[1]), colorToVertexMapCopy,
                nVertices);
    }
}

std::shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> ConcreteTreeDecomposition::constructATDNode(TermNode<ConcreteNode> &node) {
    std::shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> atdNode(new TermNode<AbstractTreeDecompositionNodeContent>);
    std::vector<std::shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>>> children;
    std::string symbol = node.getNodeContent().getSymbol();
    AbstractTreeDecompositionNodeContent abstract;
    abstract.setSymbol(symbol);
    for(auto &child:node.getChildren()){
        std::shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> atdChild = constructATDNode(*child);
        atdChild->setParent(atdNode);
        children.push_back(atdChild);
    }
    atdNode->setNodeContent(abstract);
    atdNode->setChildren(children);
    return atdNode;
}

AbstractTreeDecomposition ConcreteTreeDecomposition::convertToAbstractTreeDecomposition() {
    AbstractTreeDecomposition abstractTreeDecomposition;
    std::shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> root  = constructATDNode(*this->getRoot());
    abstractTreeDecomposition.setRoot(root);
    return abstractTreeDecomposition;
}
void ConcreteTreeDecomposition::writeToFile(std::string fileName) {
    //fileName = "CounterExample_ConcreteTreeDecomposition_"+concrete_fs::path(fileName).filename().string();
    std::ofstream atdFile (fileName);
    if (atdFile.is_open())
    {
        atdFile << termInformation();
        atdFile.close();
    }
    else {
        std::cout << "Unable to open "<< fileName << std::endl;
        exit(20);
    }
}

std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> ConcreteTreeDecomposition::constructWitnesses(Conjecture &conjecture, std::shared_ptr<TermNode<ConcreteNode>> node, Flags &flags, std::string &str) {
    // First, We check the type of the node
    if (node->getNodeContent().getSymbol() == "Leaf") {
        // if it is an empty, then it is a leaf
        State::ptr q = conjecture.getKernel()->initialState();
        if(flags.get("PrintStates")) {
            q->print();
        }
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNode(new TermNode<RunNodeContent<State::ptr,ConcreteNode>>);
        std::shared_ptr<RunNodeContent<State::ptr,ConcreteNode>> runNodeContent (new RunNodeContent<State::ptr,ConcreteNode>);
        runNodeContent->setState(q);
        runNodeContent->setRunNodeContent(node->getNodeContent());
        runNode->setNodeContent(*runNodeContent);
        return runNode;
    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "IntroVertex")) {
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNodeChild = constructWitnesses(conjecture, node->getChildren()[0],flags, str);
        State::ptr childState =runNodeChild->getNodeContent().getState();
        // find the introduced vertex
        std::set<unsigned> bagSet = node->getNodeContent().getBag().get_elements();
        std::set<unsigned> childBagSet = childState->get_bag().get_elements();
        std::set<unsigned> bagSetDifference;
        set_difference( bagSet.begin(), bagSet.end(), childBagSet.begin(),
                childBagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            std::cout
                << "ERROR: ConcreteTreeDecomposition::constructWitnesses "
                "in "<<node->getNodeContent().getSymbol()<<":child's bag and node's bag are not valid"
                << std::endl;
            std::cout<<"node's bag: ";
            node->getNodeContent().getBag().print();
            std::cout<<"\nchild's bag: ";
            node->getChildren()[0]->getNodeContent().getBag().print();
            exit(20);
        }
        State::ptr q = conjecture.getKernel()->intro_v(
                childState, *bagSetDifference.begin());
        if(flags.get("PrintStates")) {
            q->print();
        }
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNode(new TermNode<RunNodeContent<State::ptr,ConcreteNode>>);
        std::shared_ptr<RunNodeContent<State::ptr,ConcreteNode>> runNodeContent (new RunNodeContent<State::ptr,ConcreteNode>);
        runNodeContent->setState(q);
        runNodeContent->setRunNodeContent(node->getNodeContent());
        runNode->setNodeContent(*runNodeContent);
        str += node->getNodeContent().getSymbol() +"\n"+q->stateInformation();
        runNode->addChild(runNodeChild);
        runNodeChild->setParent(runNode);
        return runNode;
    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "ForgetVertex")) {
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNodeChild = constructWitnesses(conjecture, node->getChildren()[0],flags, str);
        State::ptr childState =runNodeChild->getNodeContent().getState();
        // find the forgotten vertex
        std::set<unsigned> bagSet = node->getNodeContent().getBag().get_elements();
        std::set<unsigned> childBagSet = childState->get_bag().get_elements();
        std::set<unsigned> bagSetDifference;
        set_difference(
                childBagSet.begin(), childBagSet.end(), bagSet.begin(),
                bagSet.end(),
                std::inserter(bagSetDifference, bagSetDifference.begin()));
        if (bagSetDifference.size() != 1) {
            std::cout << "ERROR: ConcreteTreeDecomposition::constructWitnesses "
                "in ForgetVertex child's bag and node's bag are not "
                "valid"
                << std::endl;
            exit(20);
        }
        State::ptr q = conjecture.getKernel()->forget_v(childState, *bagSetDifference.begin());
        if(flags.get("PrintStates")) {
            q->print();
        }
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNode(new TermNode<RunNodeContent<State::ptr,ConcreteNode>>);
        std::shared_ptr<RunNodeContent<State::ptr,ConcreteNode>> runNodeContent (new RunNodeContent<State::ptr,ConcreteNode>);
        runNodeContent->setState(q);
        runNodeContent->setRunNodeContent(node->getNodeContent());
        runNode->setNodeContent(*runNodeContent);
        str += node->getNodeContent().getSymbol() +"\n"+q->stateInformation();
        runNode->addChild(runNodeChild);
        runNodeChild->setParent(runNode);
        return runNode;
    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "IntroEdge")) {
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNodeChild = constructWitnesses(conjecture, node->getChildren()[0],flags, str);
        State::ptr childState =runNodeChild->getNodeContent().getState();
        std::pair<unsigned, unsigned> e =
            node->getNodeContent().getBag().get_edge();
        State::ptr q = conjecture.getKernel()->intro_e(childState,e.first, e.second);
        bool conjectureEvaluationResult =
            conjecture.evaluateConjectureOnState(*q);
        if(flags.get("PrintStates")) {
            q->print();
        }
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNode(new TermNode<RunNodeContent<State::ptr,ConcreteNode>>);
        std::shared_ptr<RunNodeContent<State::ptr,ConcreteNode>> runNodeContent (new RunNodeContent<State::ptr,ConcreteNode>);
        runNodeContent->setState(q);
        runNodeContent->setRunNodeContent(node->getNodeContent());
        runNode->setNodeContent(*runNodeContent);
        str += node->getNodeContent().getSymbol() +"\n"+q->stateInformation();
        runNode->addChild(runNodeChild);
        runNodeChild->setParent(runNode);
        return runNode;
    } else if (strstr(node->getNodeContent().getSymbol().c_str(), "Join")) {
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNodeChild1 = constructWitnesses(conjecture, node->getChildren()[0],flags, str);
        State::ptr childState1 =runNodeChild1->getNodeContent().getState();
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNodeChild2 = constructWitnesses(conjecture, node->getChildren()[1],flags, str);
        State::ptr childState2 =runNodeChild2->getNodeContent().getState();
        State::ptr q = conjecture.getKernel()->join(childState1,childState2);
        if(flags.get("PrintStates")) {
            q->print();
        }
        std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNode(new TermNode<RunNodeContent<State::ptr,ConcreteNode>>);
        std::shared_ptr<RunNodeContent<State::ptr,ConcreteNode>> runNodeContent (new RunNodeContent<State::ptr,ConcreteNode>);
        runNodeContent->setState(q);
        runNodeContent->setRunNodeContent(node->getNodeContent());
        runNode->setNodeContent(*runNodeContent);
        str += node->getNodeContent().getSymbol() +"\n"+q->stateInformation();
        runNode->addChild(runNodeChild1);
        runNodeChild1->setParent(runNode);
        runNode->addChild(runNodeChild2);
        runNodeChild2->setParent(runNode);
        return runNode;
    } else {
        std::cout << "ERROR in constructWitnesses: The function could not recognize "
            "the type of the node"
            << std::endl;
        std::cout << "The devastated node is: " << std::endl;
        node->getNodeContent().print();
        exit(20);
    }
}
bool ConcreteTreeDecomposition::conjectureCheck(Conjecture &conjecture,Flags &flags, std::string name) {
    std::string str = "";
    RunTree<State::ptr,ConcreteNode> runTree;
    std::shared_ptr<TermNode<RunNodeContent<State::ptr,ConcreteNode>>> runNode = constructWitnesses(conjecture, getRoot(),flags,str);
    runTree.setRoot(runNode);
    name += "_RunTree.txt";
    std::cout << "Conjecture Value: " ;
    std::cout << conjecture.evaluateConjectureOnState(*runNode->getNodeContent().getState()) << std::endl;
    std::cout << "Assignment: ";
    conjecture.printValues(*runNode->getNodeContent().getState(),conjecture.getRoot());
    return true;
//    if (!conjecture.evaluateConjectureOnState(*runNode->getNodeContent().getState())) {
//        if(flags.get("WriteToFiles"))
//            runTree.writeToFile(name);
//        std::cout << "CONJECTURE NOT SATISFIED" << std::endl;
//        return false;
//    } else {
//        std::cout << "CONJECTURE SATISFIED"<< std::endl;
//        if(flags.get("WriteToFiles"))
//            runTree.writeToFile(name);
//        return true;
//    }
}
