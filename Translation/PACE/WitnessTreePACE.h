#ifndef WITNESSTREEPACE_H
#define WITNESSTREEPACE_H
#include <iostream>
#include "../../Kernel/DynamicKernel.h"
#include "../../Kernel/StateTree.h"

class WitnessTreePACENode{
    public:
        std::vector<std::shared_ptr<Witness> > witnesses;
        std::vector<std::shared_ptr<WitnessTreePACENode> > children;
        std::shared_ptr<WitnessTreePACENode> parent;
        std::string node_type;
        Bag bag;
        void print();
};
class WitnessTreePACE{
    public:
        std::shared_ptr<WitnessTreePACENode> root;
        // map of color to vertices
        std::multimap<unsigned,unsigned> color_to_vertices;
        std::shared_ptr<WitnessTreePACENode>  generateNode(std::shared_ptr<StateTreeNode> snode, std::shared_ptr<WitnessTreePACENode> parnet, unsigned joinno, DynamicKernel &kernel);
        bool stateTreeToWitnessTreePACE(StateTree &stateTree, DynamicKernel &kernel);
        void print();
};

#endif
