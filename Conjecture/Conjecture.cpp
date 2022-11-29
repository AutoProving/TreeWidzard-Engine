#include "Conjecture.h"

// To Do: implement getCoreWitnessSetByVar
double Conjecture::evaluateConjectureNodeOnState(const State &q, ConjectureNode* node) {
     //evaluate a node based on its type
    switch(node->getType()) {
        case CORE_VARIABLE:
            if (variablesToCoreName.count(node->getVal())) {
                std::string coreType = kernel->getCoreByVar(node->getVal())->getAttributeValue("CoreType");
                if (coreType == "Min" or coreType == "Max") {
                    return kernel->getCoreByVar(node->getVal())
                            ->weight(q.getWitnessSet(kernel->getIndexByVar(node->getVal())));
                } else if (coreType == "Bool") {
                    Bag bag = q.get_bag();
                    return kernel->getCoreByVar(node->getVal())
                            ->is_final_set_witness(bag,q.getWitnessSet(kernel->getIndexByVar(node->getVal())));
                } else {
                    //error
                    std::cout<<"Error in Conjecture::evaluateConjectureNodeOnState: coreType " << coreType << " is not defined.";
                    exit(20);
                }
            } else {
                // error
                std::cout<<"Error in Conjecture::evaluateConjectureNodeOnState.";
                exit(20);
            }
        case INV:
            if(node->getChildren().size() == 1){
                if(node->getChildren()[0]->getType() == CORE_VARIABLE){
                    Bag bag = q.get_bag();
                    return kernel->getCoreByVar(node->getChildren()[0]->getVal())->inv(bag, q.getWitnessSet(kernel->getIndexByVar(node->getChildren()[0]->getVal())));
                }else{
                    std::cout << "Error in Conjecture::evaluateConjectureNodeOnState: INV is not in a valid form. A core variable should be given to inv.";
                    exit(20);
                }
            }else{
                std::cout << "Error in Conjecture::evaluateConjectureNodeOnState: INV is not in a valid form.";
                exit(20);
            }
        case EXP_VARIABLE:
            std::cout << "case EXP_VARIABLE no implemented"<< std::endl;
            exit(20);
        case NUMBER:
            return node->getX();
        case OPERATOR: {
            std::string op = node->getVal();
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
             std::cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " has invalid number of children"<<std::endl;
             exit(20);
            }else{
                if(functions_binary.count(node->getVal())){
                    Function_Binary f = functions_binary[node->getVal()];
                    return f(evaluateConjectureNodeOnState(q,node->getChildren()[0]), evaluateConjectureNodeOnState(q,node->getChildren()[1]));
                }else{
                    std::cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " hasn't defined"<<std::endl;
                }
            }
        case FUNCTION_UNARY:
            if(node->getChildren().size()!=1){
                std::cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " has invalid number of children"<<std::endl;
                exit(20);
            }else{
                if(functions_unary  .count(node->getVal())){
                    Function_Unary f = functions_unary[node->getVal()];
                    return f(evaluateConjectureNodeOnState(q,node->getChildren()[0]));
                }else{
                    std::cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " hasn't defined"<<std::endl;
                }
            }

        default:
        {
            std::cout<< " ERROR "<< __PRETTY_FUNCTION__  << " :  " << node->getVal() << " hasn't defined"<<std::endl;
            exit(20);
        }
    }
}

double Conjecture::evaluateConjectureOnState(const State &q) {
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
                std::cout<< node->getVal() << " is number and has children " << std::endl;
                return false;
            }
        case OPERATOR:
        {std::string op = node->getVal();
            if(op == "and" or op == "or" or op == "implies" or op == "iff" or op == ">" or op == "<"
                or op == ">" or op == ">=" or op == "<="){
                if(node->getChildren().size()!=2){
                    std::cout<< node->getVal() << "has invalid children"<<std::endl;
                    return false;
                }
            }else if (op == "not"){
                if(node->getChildren().size()!=1){
                    std::cout<< node->getVal() << "has invalid children"<<std::endl;
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

const std::map<std::string, std::string> &Conjecture::getVariablesToCoreName() const {
    return variablesToCoreName;
}

void Conjecture::setVariablesToCoreName(const std::map<std::string, std::string> &variablesToCoreName) {
    Conjecture::variablesToCoreName = variablesToCoreName;
}

int Conjecture::evaluatePremiseOnState(const State &q) {
    if (root->getType() == OPERATOR and root->getVal()=="implies") {
        if (root->getChildren().size() != 2) {
            std::cerr << "ERROR: In ConjectureNode::evaluateState, IMPLIES operator "
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
        std::cout << "ERROR: could not determine the premise. The conjecture is not "
                "in the form A->B. "
             << std::endl;
        exit(20);
    }
}

double min_specialized(double x1, double x2){
    return std::min(x1, x2);
}
double max_specialized(double x1, double x2){
    return std::max(x1, x2);
}

Conjecture::Conjecture() {
    functions_binary.insert(std::make_pair("pow", (Function_Binary)pow) );
    functions_binary.insert(std::make_pair("min", min_specialized));
    functions_binary.insert(std::make_pair("max",max_specialized));

    // unary functions
    functions_unary.insert(std::make_pair("abs", (Function_Unary)fabs));
    functions_unary.insert(std::make_pair("acos", (Function_Unary)acos));
    functions_unary.insert(std::make_pair("asin", (Function_Unary)asin));
    functions_unary.insert(std::make_pair("atan", (Function_Unary)atan));
    functions_unary.insert(std::make_pair("cos", (Function_Unary)cos));
    functions_unary.insert(std::make_pair("exp", (Function_Unary)exp));
    functions_unary.insert(std::make_pair("floor", (Function_Unary)floor));
    functions_unary.insert(std::make_pair("ln", (Function_Unary)log));
    functions_unary.insert(std::make_pair("log", (Function_Unary)log10));
    functions_unary.insert(std::make_pair("sin", (Function_Unary)sin));
    functions_unary.insert(std::make_pair("sqrt", (Function_Unary)sqrt));
    functions_unary.insert(std::make_pair("tan", (Function_Unary)tan));
}


ConjectureNode::ConjectureNode(TokenType type, const std::string &val, double x) : type(type), val(val), x(x) {}

TokenType ConjectureNode::getType() const {
    return type;
}

void ConjectureNode::setType(TokenType type) {
    ConjectureNode::type = type;
}

const std::string &ConjectureNode::getVal() const {
    return val;
}

void ConjectureNode::setVal(const std::string &val) {
    ConjectureNode::val = val;
}

double ConjectureNode::getX() const {
    return x;
}

void ConjectureNode::setX(double x) {
    ConjectureNode::x = x;
}

const std::vector<ConjectureNode *> &ConjectureNode::getChildren() const {
    return children;
}

void ConjectureNode::setChildren(const std::vector<ConjectureNode *> &children) {
    ConjectureNode::children = children;
}

ConjectureNode *ConjectureNode::getParent() const {
    return parent;
}

void ConjectureNode::setParent(ConjectureNode *parent) {
    ConjectureNode::parent = parent;
}

ConjectureNode::ConjectureNode(TokenType type, const std::string &val) : type(type), val(val) {}

ConjectureNode::ConjectureNode(TokenType type, double x) : type(type), x(x) {}

void ConjectureNode::printWithNumbers(unsigned &label) {
    std::set<unsigned> childrenLabel;
    for(auto child : children){
        child->printWithNumbers(label);
        childrenLabel.insert(label);
    }
    label++;
    std::cout<<label <<"  ";
    print() ;
    if(childrenLabel.size()){
        std::cout<<"(";
        for(auto childNo:childrenLabel){
            std::cout<<childNo;
            if(childNo!=*(--childrenLabel.end())){
                std::cout<<",";
            }
        }
        std::cout<<")";
    }
    std::cout << std::endl;
}

void ConjectureNode::print() {
    switch (type) {
        case NUMBER:
            std::cout << x;
        default:
            std::cout << val;
    }
}

void ConjectureNode::printInfix() {
    switch (this->type) {
        case NUMBER:
            if(this->getChildren().size()!=0){
                std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: NUMBER, node: " << x << std::endl;
                exit(20);
            }
            std::cout<<  this->x;
            return;

        case FUNCTION_BINARY:

            if(this->getChildren().size()!=2){
                std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: FUNCTION_BINARY, node: " << val <<" , " << x<< std::endl;
                exit(20);
            }
            std::cout <<" " << val << "(";
            children[0]->printInfix();
            std::cout << ",";
            children[1]->printInfix();
            std::cout<< ")";
            return;

        case FUNCTION_UNARY:

            if(this->getChildren().size()!=1){
                std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: FUNCTION_UNARY, node: " << val << std::endl;
                exit(20);
            }
            std::cout <<" "<<val << "(";
            children[0]->printInfix();
            std::cout << ")";
            return;
        case CORE_VARIABLE:

            if(this->getChildren().size()!=0){
                std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: CORE_VARIABLE, node: " << val << std::endl;
                exit(20);
            }
            std::cout << val ;
            return;
                    
        case OPERATOR:

            if(val == "not"){
                if(children.size()!=1){
                    std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: OPERATOR, node: " << val << std::endl;
                    exit(20);
                }
                std::cout<< " " << val << " ";
                children[0]->printInfix();
            }else{
                if(children.size()!=2){
                    std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: OPERATOR, node: " << val << std::endl;
                    exit(20);
                }
                std::cout<< "(";
                children[0]->printInfix();
                std::cout<< " " << val << " ";
                children[1]->printInfix();
                std::cout<< ")";
            }
            return;
        case INV:
            if(this->getChildren().size()!=1){
                std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " type: INV, node: " << val << std::endl;
                exit(20);
            }
            std::cout <<" "<< val << "(";
            children[0]->printInfix();
            std::cout << ")";
            return;
        default:

            std::cout<<"ERROR:  " << __PRETTY_FUNCTION__  << " node " << val << " not defined " << std::endl;
            exit(20);

    }
}
