#include "Conjecture.h"

// To Do: implement getCoreWitnessSetByVar
int Conjecture::evaluateConjectureNodeOnState(State &q, ConjectureNode* node) {
     //evaluate a node based on its type
    switch(node->getType()) {
        case CORE_VARIABLE:

            if (variablesToCoreName.count(node->getVal())) {
                string coreType = kernel->getCoreByVar(node->getVal())->getAttributeValue("CoreType");
                if (coreType == "Min" or coreType == "Max") {
                    return kernel->getCoreByVar(node->getVal())
                            ->weight(q.getWitnessSet(kernel->getIndexByVar(node->getVal())));
                } else if (coreType == "Bool") {
                    Bag bag =q.get_bag();
                    return kernel->getCoreByVar(node->getVal())
                            ->is_final_set_witness(bag,q.getWitnessSet(kernel->getIndexByVar(node->getVal())));
                } else {
                    //error
                    exit(20);
                }

            } else {
                // error
                exit(20);
            }
        case EXP_VARIABLE:
            cout << "case EXP_VARIABLE no implemented"<<endl;
            exit(20);
        case NUMBER:
            return node->getX();
        case OPERATOR:
            string op = node->getVal();
            if (op == "+"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                        + evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if (op == "-"){
                if(node->getChildren().size()==2){
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                            - evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                }else{
                    exit(20);
                }
            }else if(op == "/"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           / evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "*"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           * evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == ">"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           > evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == ">="){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           >= evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "<"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           < evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "<="){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           <= evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "=="){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           == evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "and"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           and evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "or"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           or evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "not"){
                if (node->getChildren().size() == 1) {
                    return !evaluateConjectureNodeOnState(q,node->getChildren()[0]);
                } else {
                    exit(20);
                }
            }else if(op == "implies"){
                if (node->getChildren().size() == 2) {
                    return (!evaluateConjectureNodeOnState(q,node->getChildren()[0]))
                           or evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else if(op == "iff"){
                if (node->getChildren().size() == 2) {
                    return evaluateConjectureNodeOnState(q,node->getChildren()[0])
                           == evaluateConjectureNodeOnState(q,node->getChildren()[1]);
                } else {
                    exit(20);
                }
            }else{
                exit(20);
            }
    }
}

int Conjecture::evaluateConjectureOnState(State &q) {
    int result = evaluateConjectureNodeOnState(q,root);
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
    unsigned label = 0;
    root->printWithNumbers(label);
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
            return evaluateConjectureNodeOnState(q, root);
        }
    } else {
        cout << "ERROR: could not determine the premise. The conjecture is not "
                "in the form A->B. "
             << endl;
        exit(20);
    }
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

