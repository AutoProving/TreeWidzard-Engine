#ifndef WITNESSTREEPACE_H
#define WITNESSTREEPACE_H
#include <iostream>
#include "../../Kernel/DynamicKernel.h"
#include "../../Kernel/StateTree.h"

using namespace std;

class WitnessTreePACENode{
    public:
        vector<shared_ptr<Witness> > witnesses;
        vector<shared_ptr<WitnessTreePACENode> > children;
        shared_ptr<WitnessTreePACENode> parent;
        string node_type;
        Bag bag;
        void print();
};

class WitnessTreePACE{
    public:
        shared_ptr<WitnessTreePACENode> root;
        // map of color to vertices
        multimap<unsigned,unsigned> color_to_vertices;
        shared_ptr<WitnessTreePACENode>  generateNode(shared_ptr<StateTreeNode> snode, shared_ptr<WitnessTreePACENode> parnet, unsigned joinno, DynamicKernel &kernel);
        bool stateTreeToWitnessTreePACE(StateTree &stateTree, DynamicKernel &kernel);
        void print();
};

#endif
