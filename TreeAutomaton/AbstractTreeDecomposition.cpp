
#include "AbstractTreeDecomposition.h"
AbstractTreeDecompositionNodeContent::AbstractTreeDecompositionNodeContent() {

}
AbstractTreeDecompositionNodeContent::AbstractTreeDecompositionNodeContent(const string &symbol) : symbol(symbol) {}

const string &AbstractTreeDecompositionNodeContent::getSymbol() const {
    return symbol;
}

void AbstractTreeDecompositionNodeContent::setSymbol(const string &symbol) {
    AbstractTreeDecompositionNodeContent::symbol = symbol;
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

void AbstractTreeDecompositionNodeContent::print() {
    cout<<"symbol: " << symbol<<endl;
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



