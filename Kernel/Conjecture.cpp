// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Conjecture.h"
bool ConjectureNode::evaluateState(State &q, DynamicKernel* kernel) {
    if(children.empty() and logicalOperator != "TRUE" and logicalOperator != "FALSE" ){
        Bag b = q.get_bag();
        shared_ptr<WitnessSet> witnessSet = q.get_witnessSetVector()[propertyIndex-1];
        return kernel->pointerToCoreNumber(propertyIndex-1)->is_final_set_witness(b,*witnessSet);
    } else {
        if (logicalOperator == "AND") {
            if (children.size() != 2) {
                cerr << "ERROR: In ConjectureNode::evaluateState, AND operator does not have 2 children";
                exit(20);
            } else {
                return (children[0]->evaluateState(q,kernel) and children[1]->evaluateState(q,kernel));
            }

        } else if (logicalOperator == "OR") {
            if (children.size() != 2) {
                cerr << "ERROR: In ConjectureNode::evaluateState, OR operator does not have 2 children";
                exit(20);
            } else {
                return (children[0]->evaluateState(q,kernel) or children[1]->evaluateState(q,kernel));
            }

        } else if (logicalOperator == "IMPLIES") {

            if (children.size() != 2) {
                cerr << "ERROR: In ConjectureNode::evaluateState, IMPLIES operator does not have 2 children";
                exit(20);
            } else {
                return (!(children[0]->evaluateState(q,kernel)) or children[1]->evaluateState(q,kernel));
            }
        } else if (logicalOperator == "IMPLIEDBY") {

            if (children.size() != 2) {
                cerr << "ERROR: In ConjectureNode::evaluateState, IMPLIEDBY operator does not have 2 children";
                exit(20);
            } else{
                return (children[0]->evaluateState(q,kernel) or (!children[1]->evaluateState(q,kernel)));
            }
        } else if (logicalOperator == "IFF") {
            if (children.size() != 2) {
                cerr << "ERROR: In ConjectureNode::evaluateState, IFF operator does not have 2 children";
                exit(20);
            } else {
                return (children[0]->evaluateState(q,kernel) == children[1]->evaluateState(q,kernel));
            }
        } else if (logicalOperator == "NOT") {
            if (children.size() != 1) {
                cerr << "ERROR: In ConjectureNode::evaluateState, NOT operator does not have 1 children";
                exit(20);
            } else {
                return (!children[0]->evaluateState(q,kernel));
            }
        } else if(logicalOperator == "TRUE"){
            return true;
        } else if(logicalOperator == "FALSE"){
            return false;
        }else{
            cerr << "ERROR: In ConjectureNode::evaluateState, Logical operator have not defined, logicalOperator: "<< logicalOperator<< endl;
            exit(20);
        }

    }
}

void ConjectureNode::print(){


    if(children.size() == 0 ) {

        cout<< propertyIndex;   
    }else{

        cout<<logicalOperator<<" (";
        if(children.size()==2){
            children[0]->print();
            cout<<" , ";
            children[1]->print();
        }else{

            children[0]->print();
            
        }
        cout<<" )";
    }
    
}

bool Conjecture::evaluateConjectureOnState(State &q, DynamicKernel *kernel) {
    return root->evaluateState(q, kernel);
}

bool Conjecture::evaluatePremiseOnState(State &q, DynamicKernel *kernel) {
	if (root->logicalOperator == "IMPLIES") {
            if (root->children.size() != 2) {
                cerr << "ERROR: In ConjectureNode::evaluateState, IMPLIES operator does not have 2 children";
                exit(20);
            } else {
                return (root->children[0]->evaluateState(q,kernel));
            }
	}else{

            cout<<"ERROR: could not determine the premise. The conjecture is not in the form A->B. "<<endl;
            exit(20);
        }
}

bool Conjecture::evaluateConsequentOnState(State &q, DynamicKernel *kernel) {
	if (root->logicalOperator == "IMPLIES") {
            if (root->children.size() != 2) {
                cerr << "ERROR: In ConjectureNode::evaluateState, IMPLIES operator does not have 2 children";
                exit(20);
            } else {
                return (root->children[1]->evaluateState(q,kernel));
            }
	}else{

            cout<<"ERROR: could not determine the consequent. The conjecture is not in the form A->B. "<<endl;
            exit(20);
        }
}




Conjecture::Conjecture() {

    kernel = new DynamicKernel;
    root = new ConjectureNode;
}

ConjectureNode *Conjecture::get_root(){
    return root;
}



void Conjecture::print(){
    root->print();
}
