#include "Conjecture.h"

// To Do: implement getCoreWitnessSetByVar
double Conjecture::evaluateConjectureNodeOnState(State &q, ConjectureNode* node) {
     //evaluate a node based on its type
    switch(node->getType()) {
        case CORE_VARIABLE:
            if (variablesToCoreName.count(node->getVal())) {
                string coreType = kernel->getCoreByVar(node->getVal())->getAttributeValue("CoreType");
                if (coreType == "Min" or coreType == "Max") {
                    return kernel->getCoreByVar(node->getVal())
                            ->weight(q.getWitnessSet(kernel->getIndexByVar(node->getVal())));
                } else if (coreType == "Bool") {
                    Bag bag = q.get_bag();
                    return kernel->getCoreByVar(node->getVal())
                            ->is_final_set_witness(bag,q.getWitnessSet(kernel->getIndexByVar(node->getVal())));
                } else {
                    //error
                    cout<<"Error in Conjecture::evaluateConjectureNodeOnState: coreType " << coreType << " is not defined.";
                    exit(20);
                }

            } else {
                // error
                cout<<"Error in Conjecture::evaluateConjectureNodeOnState.";
                exit(20);
            }
        case EXP_VARIABLE:
            cout << "case EXP_VARIABLE no implemented"<<endl;
            exit(20);
        case NUMBER:
            return node->getX();
        case OPERATOR: {
            string op = node->getVal();
            if (op == "+") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           + evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "-") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           - evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "/") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           / evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "*") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           * evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == ">") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           > evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == ">=") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           >= evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "<") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           < evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "<=") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           <= evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "==") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           == evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "and") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           and evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "or") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           or evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "not") {
                if (node->getChildren().size() == 1) {
                    return !evaluateConjectureNodeOnState(q, node->getChildren()[0]);
                } else {
                    exit(20);
                }
            } else if (op == "implies") {
                if (node->getChildren().size() == 2) {
                    return (!evaluateConjectureNodeOnState(q, node->getChildren()[0]))
                           or evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else if (op == "iff") {
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q, node->getChildren()[0])
                           == evaluateConjectureNodeOnState(q, node->getChildren()[1]);
                } else {
                    exit(20);
                }
            } else {
                exit(20);
            }
        }
        case FUNCTION_BINARY:
            if(node->getChildren().size()!=2){
             cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " has invalid number of children"<<endl;
             exit(20);
            }else{
                if(functions_binary.count(node->getVal())){
                    Function_Binary f = functions_binary[node->getVal()];
                    return f(evaluateConjectureNodeOnState(q,node->getChildren()[0]), evaluateConjectureNodeOnState(q,node->getChildren()[1]));
                }else{
                    cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " hasn't defined"<<endl;
                }
            }
        case FUNCTION_UNARY:
            if(node->getChildren().size()!=1){
                cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " has invalid number of children"<<endl;
                exit(20);
            }else{
                if(functions_unary  .count(node->getVal())){
                    Function_Unary f = functions_unary[node->getVal()];
                    return f(evaluateConjectureNodeOnState(q,node->getChildren()[0]));
                }else{
                    cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " hasn't defined"<<endl;
                }
            }

        default:
        {
            cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " hasn't defined"<<endl;
            exit(20);
        }
    }
}

double Conjecture::evaluateConjectureOnState(State &q) {
    double result = evaluateConjectureNodeOnState(q,root);
    //cout<<__FUNCTION__  << " " <<__PRETTY_FUNCTION__ << " " << result << endl;
    return result;
}

ConjectureNode *Conjecture::getRoot() const {
    return root;
}

void Conjecture::setRoot(ConjectureNode *root) {
    Conjecture::root = root;
}

DynamicKernel *Conjecture::getKernel() const {
    return kernel;
}

void Conjecture::setKernel(DynamicKernel *kernel) {
    Conjecture::kernel = kernel;
}

void Conjecture::print() {
//    unsigned label = 0;
//    root->printWithNumbers(label);
    root->printInfix();

}

bool Conjecture::checkConjectureStructure(ConjectureNode *node) {
    switch (node->getType()) {
        case NUMBER:
            if(node->getChildren().size()!=0){
                cout<< node->getVal() << " is number and has children " << endl;
                return false;
            }
        case OPERATOR:
        {string op = node->getVal();
            if(op == "and" or op == "or" or op == "implies" or op == "iff" or op == ">" or op == "<"
                or op == ">" or op == ">=" or op == "<="){
                if(node->getChildren().size()!=2){
                    cout<< node->getVal() << "has invalid children"<<endl;
                    return false;
                }
            }else if (op == "not"){
                if(node->getChildren().size()!=1){
                    cout<< node->getVal() << "has invalid children"<<endl;
                    return false;
                }
            }
        }
//        case CORE_VARIABLE:
//            return true;
//        case EXP_VARIABLE:
//            return true;
//        case FUNC:
//            return true;
    }
    for(auto child:node->getChildren()){
        if(!checkConjectureStructure(child)){
            return false;
        }
    }
    return true;
}

const map<string, string> &Conjecture::getVariablesToCoreName() const {
    return variablesToCoreName;
}

void Conjecture::setVariablesToCoreName(const map<string, string> &variablesToCoreName) {
    Conjecture::variablesToCoreName = variablesToCoreName;
}

int Conjecture::evaluatePremiseOnState(State &q) {
    if (root->getType() == OPERATOR and root->getVal()=="implies") {
        if (root->getChildren().size() != 2) {
            cerr << "ERROR: In ConjectureNode::evaluateState, IMPLIES operator "
                    "does not have 2 children";
            exit(20);
        } else {
            bool result = evaluateConjectureNodeOnState(q, root->getChildren()[0]);
            // cout<< result << " ";
            // root->getChildren()[0]->printInfix();
            // cout<<endl;
            return result;
        }
    } else {
        cout << "ERROR: could not determine the premise. The conjecture is not "
                "in the form A->B. "
             << endl;
        exit(20);
    }
}

double min_specialized(double x1, double x2){
    return min(x1,x2);
}
double max_specialized(double x1, double x2){
    return max(x1,x2);
}

Conjecture::Conjecture() {
    functions_binary.insert(make_pair("pow", (Function_Binary)pow) );
    functions_binary.insert(make_pair("min", min_specialized));
    functions_binary.insert(make_pair("max",max_specialized));

    // unary functions
    functions_unary.insert(make_pair("abs", (Function_Unary)fabs));
    functions_unary.insert(make_pair("acos", (Function_Unary)acos));
    functions_unary.insert(make_pair("asin", (Function_Unary)asin));
    functions_unary.insert(make_pair("atan", (Function_Unary)atan));
    functions_unary.insert(make_pair("cos", (Function_Unary)cos));
    functions_unary.insert(make_pair("exp", (Function_Unary)exp));
    functions_unary.insert(make_pair("floor", (Function_Unary)floor));
    functions_unary.insert(make_pair("ln", (Function_Unary)log));
    functions_unary.insert(make_pair("log", (Function_Unary)log10));
    functions_unary.insert(make_pair("sin", (Function_Unary)sin));
    functions_unary.insert(make_pair("sqrt", (Function_Unary)sqrt));
    functions_unary.insert(make_pair("tan", (Function_Unary)tan));
}


ConjectureNode::ConjectureNode(TokenType type, const string &val, double x) : type(type), val(val), x(x) {}

TokenType ConjectureNode::getType() const {
    return type;
}

void ConjectureNode::setType(TokenType type) {
    ConjectureNode::type = type;
}

const string &ConjectureNode::getVal() const {
    return val;
}

void ConjectureNode::setVal(const string &val) {
    ConjectureNode::val = val;
}

double ConjectureNode::getX() const {
    return x;
}

void ConjectureNode::setX(double x) {
    ConjectureNode::x = x;
}

const vector<ConjectureNode *> &ConjectureNode::getChildren() const {
    return children;
}

void ConjectureNode::setChildren(const vector<ConjectureNode *> &children) {
    ConjectureNode::children = children;
}

ConjectureNode *ConjectureNode::getParent() const {
    return parent;
}

void ConjectureNode::setParent(ConjectureNode *parent) {
    ConjectureNode::parent = parent;
}

ConjectureNode::ConjectureNode(TokenType type, const string &val) : type(type), val(val) {}

ConjectureNode::ConjectureNode(TokenType type, double x) : type(type), x(x) {}

void ConjectureNode::printWithNumbers(unsigned &label) {
    set<unsigned > childrenLabel;
    for(auto child : children){
        child->printWithNumbers(label);
        childrenLabel.insert(label);
    }
    label++;
    cout<<label <<"  ";
    print() ;
    if(childrenLabel.size()){
        cout<<"(";
        for(auto childNo:childrenLabel){
            cout<<childNo;
            if(childNo!=*(--childrenLabel.end())){
                cout<<",";
            }
        }
        cout<<")";
    }
    cout<<endl;
}

void ConjectureNode::print() {
    switch (type) {
        case NUMBER:
            cout << x;
        default:
            cout << val;
    }
//        cout<< "(" << type << "," << val << "," << x<< ") " ;
}

void ConjectureNode::printInfix() {
    switch (this->type) {
        case NUMBER:
            if(this->getChildren().size()!=0){
                cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: NUMBER, node: " << x <<endl;
                exit(20);
            }
            cout<<  this->x;
            return;

        case FUNCTION_BINARY:

            if(this->getChildren().size()!=2){
                cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: FUNCTION_BINARY, node: " << val <<" , " << x<<endl;
                exit(20);
            }
            cout <<" " << val << "(";
            children[0]->printInfix();
            cout << ",";
            children[1]->printInfix();
            cout<< ")";
            return;

        case FUNCTION_UNARY:

            if(this->getChildren().size()!=1){
                cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: FUNCTION_UNARY, node: " << val <<endl;
                exit(20);
            }
            cout <<" "<<val << "(";
            children[0]->printInfix();
            cout << ")";
            return;
        case CORE_VARIABLE:

            if(this->getChildren().size()!=0){
                cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: CORE_VARIABLE, node: " << val <<endl;
                exit(20);
            }
            cout << val ;
            return;
        case OPERATOR:

            if(val == "not"){
                if(children.size()!=1){
                    cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: OPERATOR, node: " << val <<endl;
                    exit(20);
                }
                cout<< " " << val << " ";
                children[0]->printInfix();
            }else{
                if(children.size()!=2){
                    cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: OPERATOR, node: " << val <<endl;
                    exit(20);
                }
                cout<< "(";
                children[0]->printInfix();
                cout<< " " << val << " ";
                children[1]->printInfix();
                cout<< ")";
            }
            return;
        default:

            cout<<"ERROR:  " << __PRETTY_FUNCTION__  << "node " << val << " not defined " <<endl;
            exit(20);

    }
}
