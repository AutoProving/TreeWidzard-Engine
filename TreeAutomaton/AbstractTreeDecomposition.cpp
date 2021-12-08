
#include "AbstractTreeDecomposition.h"
AbstractTreeDecompositionNodeContent::AbstractTreeDecompositionNodeContent() {

}
AbstractTreeDecompositionNodeContent::AbstractTreeDecompositionNodeContent(const string &symbol) : symbol(symbol) {}

const string &AbstractTreeDecompositionNodeContent::getSymbol() const {
    return symbol;
}

void AbstractTreeDecompositionNodeContent::setSymbol(const string &symbol){
    this->symbol = symbol;
}

AbstractTreeDecompositionNodeContent &
AbstractTreeDecompositionNodeContent::operator=(const AbstractTreeDecompositionNodeContent &other) {
    setSymbol(other.getSymbol());
    return *this;
}

bool AbstractTreeDecompositionNodeContent::operator==(const AbstractTreeDecompositionNodeContent &rhs) const {
    return symbol == rhs.symbol;
}

bool AbstractTreeDecompositionNodeContent::operator!=(const AbstractTreeDecompositionNodeContent &rhs) const {
    return !(rhs == *this);
}

bool AbstractTreeDecompositionNodeContent::operator<(const AbstractTreeDecompositionNodeContent &rhs) const {
        int typeSymbol1 = symbolType(this->symbol);
        int typeSymbol2 = symbolType(rhs.symbol);
        if (typeSymbol1<typeSymbol2) return true;
        if (typeSymbol1>typeSymbol2) return false;
        vector<int> numbersSymbol1 = symbolNumbers(this->symbol);
        vector<int> numbersSymbol2 = symbolNumbers(rhs.symbol);
        return numbersSymbol1 < numbersSymbol2;
}

bool AbstractTreeDecompositionNodeContent::operator>(const AbstractTreeDecompositionNodeContent &rhs) const {
    return rhs < *this;
}

bool AbstractTreeDecompositionNodeContent::operator<=(const AbstractTreeDecompositionNodeContent &rhs) const {
    return !(rhs < *this);
}

bool AbstractTreeDecompositionNodeContent::operator>=(const AbstractTreeDecompositionNodeContent &rhs) const {
    return !(*this < rhs);
}

int AbstractTreeDecompositionNodeContent::symbolType(string s) const {
    if(s.find("Leaf") != string::npos){
        return 0;
    }else if(s.find("IntroVertex") != string::npos){
        return 1;
    } else if(s.find("ForgetVertex") != string::npos) {
        return 2;
    }else     if(s.find("IntroEdge") != string::npos) {
        return 3;
    }else if(s.find("Join") != string::npos) {
        return 4;
    }else{
        cout<<"Error: AbstractTreeDecompositionNodeContent::symbolType, string is not in formal format."<<endl;
        exit(20);
    }
}

vector<int> AbstractTreeDecompositionNodeContent::symbolNumbers(string s) const {
    if(s.find("Leaf") != string::npos){

    }else if(s.find("IntroVertex") != string::npos){

    } else if(s.find("ForgetVertex") != string::npos) {

    }else if(s.find("IntroEdge") != string::npos) {

    }else if(s.find("Join") != string::npos) {

    }else{
        cout<<"Error: AbstractTreeDecompositionNodeContent::symbolNumbers, string is not in formal format."<<endl;
        exit(20);
    }
    return extractIntegerWords(s);
}

string AbstractTreeDecompositionNodeContent::smallestContent(){
        return "Leaf";
}

void AbstractTreeDecompositionNodeContent::print() {
    cout<< symbol;
}

vector<int> AbstractTreeDecompositionNodeContent::extractIntegerWords(string s) const{

    std::replace( s.begin(), s.end(), '_', '\t');
    stringstream ss;
    /* Storing the whole string into string stream */
    ss << s;
    vector<int> numberVec;
    /* Running loop till the end of the stream */
    string temp;
    int found;
    while (!ss.eof()) {

        /* extracting word by word from stream */
        ss >> temp;

        /* Checking the given word is integer or not */
        if (stringstream(temp) >> found){
            numberVec.push_back(found);
        }

        /* To save from space at the end of string */
        temp = "";
    }
    return numberVec;
}

AbstractTreeDecompositionNodeContent::AbstractTreeDecompositionNodeContent(
        const AbstractTreeDecompositionNodeContent &abstractTreeDecompositionNodeContent) {
    this->symbol = abstractTreeDecompositionNodeContent.getSymbol();
}

string AbstractTreeDecompositionNodeContent::nodeInformation() {
    return symbol;
}

shared_ptr<TermNode<ConcreteNode>> AbstractTreeDecomposition::constructCTDNode(TermNode<AbstractTreeDecompositionNodeContent> &node) {

    shared_ptr<TermNode<ConcreteNode>> ctdNode(new TermNode<ConcreteNode>);
    vector<shared_ptr<TermNode<ConcreteNode>>> children;
    string symbol = node.getNodeContent().getSymbol();
    ConcreteNode concrete;
    concrete.setSymbol(symbol);
    //ctdNode->setNodeContent(concrete);
    for(auto &child:node.getChildren()){
        shared_ptr<TermNode<ConcreteNode>> ctdChild = constructCTDNode(*child);
        ctdChild->setParent(ctdNode);
        children.push_back(ctdChild);
    }
    Bag b;
    if(symbol == "Leaf"){
        concrete.setBag(b);
        ctdNode->setNodeContent(concrete);
    }else {
        vector<int> numbersInString = node.getNodeContent().extractIntegerWords(symbol);
        if(strstr(symbol.c_str(),"IntroVertex")){
            if(numbersInString.size()!=1){
                cout<<"Error: ConcreteTreeDecomposition::constructCTDNode children numbers not valid"<<endl;
                exit(20);
            }else{
                b = children[0]->getNodeContent().getBag();
                b.intro_v(numbersInString[0]);
                concrete.setBag(b);
                ctdNode->setNodeContent(concrete);
            }
        }else if(strstr(symbol.c_str(),"ForgetVertex")){
            if(numbersInString.size()!=1){
                cout<<"Error: ConcreteTreeDecomposition::constructCTDNode children numbers not valid"<<endl;
                exit(20);
            }else{
                b = children[0]->getNodeContent().getBag();
                b.forget_v(numbersInString[0]);
                concrete.setBag(b);
                ctdNode->setNodeContent(concrete);
            }
        }else if(strstr(symbol.c_str(),"IntroEdge")){
            if(numbersInString.size()!=2){
                cout<<"Error: ConcreteTreeDecomposition::constructCTDNode children numbers not valid"<<endl;
                exit(20);
            }else{
                b = children[0]->getNodeContent().getBag();
                b.intro_e(numbersInString[0],numbersInString[1]);
                concrete.setBag(b);
                ctdNode->setNodeContent(concrete);
            }
        }else if(symbol == "Join"){
            Bag b;
            b.set_elements(children[0]->getNodeContent().getBag().get_elements());
            concrete.setBag(b);
            ctdNode->setNodeContent(concrete);
        }
    }
    ctdNode->setChildren(children);
    return ctdNode;
}


ConcreteTreeDecomposition AbstractTreeDecomposition::convertToConcreteTreeDecomposition() {
    ConcreteTreeDecomposition concreteTreeDecomposition;
    shared_ptr<TermNode<ConcreteNode>> root = constructCTDNode(*this->getRoot());
    concreteTreeDecomposition.setRoot(root);
    return concreteTreeDecomposition;
}


void AbstractTreeDecomposition::writeToFile(string fileName) {
    //fileName = "CounterExample_AbstractTreeDecomposition_"+abstract_fs::path(fileName).filename().string();
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
