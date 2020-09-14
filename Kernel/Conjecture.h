// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CONJECTURE_H
#define CONJECTURE_H
#include "DynamicKernel.h"

class ConjectureNode{
    public:
        string logicalOperator;
        int propertyIndex;
        vector<ConjectureNode*> children;
        virtual bool evaluateState(State &q, DynamicKernel *kernel);
        void print();
};

class Conjecture{
	public:
	    ConjectureNode *root;
	    DynamicKernel *kernel;
	    Conjecture();
            ConjectureNode *get_root();
            bool evaluateConjectureOnState(State &q, DynamicKernel* kernel);
  	    //For conjectures of the form A->B, the next function evaluates the premise A
  	    bool evaluatePremiseOnState(State &q, DynamicKernel* kernel);
	    //For conjectures of the form A->B, the next function evaluates the consequent B
            bool evaluateConsequentOnState(State &q, DynamicKernel* kernel);

            void print();
};
#endif
