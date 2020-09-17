// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "StateTree.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Constructors ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Default constructor for StateTreeNode. Default: empty node

StateTreeNode::StateTreeNode(){
    // Empty node is the default.
    this->nodeType = "Empty";
    shared_ptr<State> state(new State);
    this->S = state;
    // this->parent is NULL by default
    // this->children is the empty vector by default.
}

// Constructors that takes a node type and state as input. Used when retrieving a state tree decomposition for a counter example.
StateTreeNode::StateTreeNode(string nodeType,shared_ptr<State> s){
    this->set_nodeType(nodeType);
    this->set_S(s);
    // this->parent is NULL by default
    // this->children is the empty vector by default.
}

// Constructors that takes a node type, a bag and a vector of children as input. Used when constructing a state tree decomposition in a bottom-up fashion.
StateTreeNode::StateTreeNode(string nodeType, shared_ptr<State> s, vector<shared_ptr<StateTreeNode> > children){
    this->set_nodeType(nodeType);
    this->set_S(s);
    this->set_children(children);
    // this->parent is NULL by default
}
// Constructors that takes a node type, a bag, vector of children and kernel as input. Used when constructing a state tree decomposition in a bottom-up fashion.
StateTreeNode::StateTreeNode(string nodeType, shared_ptr<State> s, vector<shared_ptr<StateTreeNode> > children, shared_ptr<DynamicKernel> kernel){
    this->set_nodeType(nodeType);
    this->set_S(s);
    this->set_children(children);
    this->set_kernel(kernel);
    // this->parent is NULL by default
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Get Functions ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


shared_ptr<State> StateTreeNode::get_S() {
    return this->S;
}

string StateTreeNode::get_nodeType() {
    return this->nodeType;
}

shared_ptr<StateTreeNode> StateTreeNode::get_parent() {
    return this->parent;
}

vector<shared_ptr<StateTreeNode> > StateTreeNode::get_children(){
    return this->children;
}

shared_ptr<DynamicKernel> StateTreeNode::get_kernel() {
    return kernel;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Set Functions ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateTreeNode::set_S(shared_ptr<State> s) {
    this->S = s;
}

void StateTreeNode::set_nodeType(string nodeType) {
    this->nodeType = nodeType;
}

void StateTreeNode::set_parent(shared_ptr<StateTreeNode> parent) {
    this->parent = parent;
}

void StateTreeNode::set_children(vector<shared_ptr<StateTreeNode> > children){
    this->children = children;
}

void StateTreeNode::set_kernel(shared_ptr<DynamicKernel> kernel) {
    this->kernel = kernel;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Print Functions ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateTreeNode::printStateTreeNode() {
    
    this->get_S()->print();

}

string StateTreeNode::printAbstract() {
    return this->get_nodeType();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Read Functions /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Decomposition Functions /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// parent and children are not set here
StateTreeNode StateTreeNode::introVertex(unsigned i){
    if(this->S->get_bag().vertex_introducible(i)){
        shared_ptr<State> auxState(new State);
        Bag b = this->S->get_bag();
        for(size_t r = 0; r < this->S->numberOfComponents(); r++){
            auxState->addWitnessSet(this->kernel->pointerToCoreNumber(r)->intro_v(i,b,*(this->S->getWitnessSet(r))));
        }
        auxState->set_bag(b.intro_v(i));
        StateTreeNode stateTreeNode("IntroVertex_"+to_string(i), auxState);
        stateTreeNode.set_kernel(this->kernel);
        return stateTreeNode;
    }else{
        cout << "ERROR in function StateTreeNode::introVertex. Number " <<  i << " is not introducible at current bag."<<endl;
        exit(20);
    }

}

StateTreeNode StateTreeNode::forgetVertex(unsigned i){

    if(this->S->get_bag().vertex_forgettable(i)){
        shared_ptr<State> auxState(new State);
        Bag b = this->S->get_bag();
        for(size_t r = 0; r < this->S->numberOfComponents(); r++){
            auxState->addWitnessSet(this->kernel->pointerToCoreNumber(r)->forget_v(i,b,*(this->S->getWitnessSet(r))));
        }
        auxState->set_bag(b.forget_v(i));
        StateTreeNode stateTreeNode("ForgetVertex_"+to_string(i), auxState);
        stateTreeNode.set_kernel(this->kernel);
        return stateTreeNode;
    }else{
        cout << "ERROR in function StateTreeNode::forgetVertex. Number " <<  i << " is not forgettable at current bag."<<endl;
        exit(20);
    }
}



StateTreeNode StateTreeNode::introEdge(unsigned i, unsigned j){
    if(this->S->get_bag().edge_introducible(i,j)){
        shared_ptr<State> auxState(new State);
        Bag b = this->S->get_bag();
        for(size_t r = 0; r < this->S->numberOfComponents(); r++){
            auxState->addWitnessSet(this->kernel->pointerToCoreNumber(r)->intro_e(i,j,b,*(this->S->getWitnessSet(r))));
        }
        auxState->set_bag(b.intro_e(i,j));
        StateTreeNode stateTreeNode("IntroEdge_"+to_string(i)+"_"+to_string(j), auxState);
        stateTreeNode.set_kernel(this->kernel);
        return stateTreeNode;
    }else{
        cout << "ERROR in function StateTreeNode::introEdge. Number " <<  i <<" and "<< j << " is not introducible at current bag."<<endl;
        exit(20);
    }

}


StateTreeNode join(StateTreeNode &left, StateTreeNode &right){
    if(left.get_S()->get_bag().joinable(right.get_S()->get_bag())){
        shared_ptr<State> auxState(new State);
        set<unsigned> elements = left.get_S()->get_bag().get_elements();
        Bag b;
        b.set_elements(elements);
        for(size_t r=0; r < left.get_S()->numberOfComponents(); r++){
            auxState->addWitnessSet(left.get_kernel()->pointerToCoreNumber(r)->join(b, *(left.get_S()->getWitnessSet(r)),*(right.get_S()->getWitnessSet(r))));
        }
        auxState->set_bag(b);
        StateTreeNode stateTreeNode("Join", auxState);
        stateTreeNode.set_kernel(left.get_kernel());
        return stateTreeNode;
    }else{
        cout << "ERROR in function friend StateTreeNode::Join. Two nodes are not joinable."<<endl;
        exit(20);
    }
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Concrete Tree Decomposition //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void StateTree::traverseNode(StateTreeNode &node, MultiGraph &G, map<unsigned, unsigned> &colorToVertexMap, unsigned &nVertices,unsigned  &nEdges) {
    map<unsigned,unsigned> colorToVertexMapCopy = colorToVertexMap;
    if (node.get_nodeType() == "Empty"){
        // do nothing
    } else if (strstr(node.get_nodeType().c_str(), "IntroVertex") ){
        // strstr is a c++ funciton which gets two strings and check that the second string is a substring of the first string or not. 
        // We use strstr here because node type is string+number (example IntorVertex_1) so we concern to check the string (IntroVertex) not the number.
        /////////////// Finding the introduced vertex ///////////
        set<unsigned> bagSet = node.get_S()->get_bag().get_elements();
        set<unsigned> childBagSet = node.get_children()[0]->get_S()->get_bag().get_elements();
        set<unsigned> bagSetDifference;
        set_difference(bagSet.begin(),bagSet.end(), childBagSet.begin(), childBagSet.end(), std::inserter(bagSetDifference,bagSetDifference.begin()) );
        if(bagSetDifference.size()!=1){
            cout<<"ERROR: StateTree::traverseNode in IntroVertex child's bag and node's bag are not valid"<<endl;
            exit(20);
        }
        //////////// End of Finding the introduced vertex ///////////
        colorToVertexMapCopy.erase( *(bagSetDifference.begin()) );
        traverseNode(*(node.get_children()[0]), G, colorToVertexMapCopy, nVertices,nEdges); // Nothing happens. Just process the next bag.
    } else if (strstr(node.get_nodeType().c_str(), "ForgetVertex")){
        /////////////// Finding the Forgotten vertex ///////////
        set<unsigned> bagSet = node.get_S()->get_bag().get_elements();
        set<unsigned> childBagSet = node.get_children()[0]->get_S()->get_bag().get_elements();
        set<unsigned> bagSetDifference;
        set_difference(childBagSet.begin(), childBagSet.end(), bagSet.begin(),bagSet.end(), std::inserter(bagSetDifference,bagSetDifference.begin()) );
        if(bagSetDifference.size()!=1){
            cout<<"ERROR: StateTree::traverseNode in ForgetVertex child's bag and node's bag are not valid"<<endl;
            exit(20);
        }
        //////////// End of Finding the Forgotten vertex ///////////
        nVertices = nVertices + 1;
        G.addVertex(nVertices);
        colorToVertexMapCopy[ *(bagSetDifference.begin()) ]= nVertices;
        traverseNode(*(node.get_children()[0]),G,colorToVertexMapCopy,nVertices,nEdges); // Nothing happens. Just process the next bag.
    } else if ( strstr(node.get_nodeType().c_str(), "IntroEdge") ){
        nEdges = nEdges + 1;
        pair<unsigned,unsigned> e = node.get_S()->get_bag().get_edge();
        G.addEdgeLabel(nEdges);
        G.addIncidence(nEdges,colorToVertexMap.find(e.first)->second);
        G.addIncidence(nEdges,colorToVertexMap.find(e.second)->second);
        traverseNode(*(node.get_children()[0]),G,colorToVertexMapCopy,nVertices,nEdges);
    } else if (node.get_nodeType() == "Join"){
        traverseNode(*(node.get_children()[0]),G,colorToVertexMapCopy,nVertices,nEdges);
        traverseNode(*(node.get_children()[1]),G,colorToVertexMapCopy,nVertices,nEdges);
    }
}

MultiGraph StateTree::extractMultiGraph() {
    MultiGraph G;
    map<unsigned,unsigned> colorToVertexMap;
    unsigned nVertices = 0;
    unsigned nEdges = 0;
    set<unsigned> elements = root->get_S()->get_bag().get_elements();
    set<unsigned>::iterator it = elements.begin();
    for (size_t i = 0; i < root->get_S()->get_bag().get_elements().size(); ++i){
        colorToVertexMap[*it] = i;// creates one vertex for each color.
        nVertices = i+1;
        G.addVertex(nVertices);
        ++it;
    }
    traverseNode(*(this->root),G,colorToVertexMap,nVertices, nEdges);
    return G;
}

void StateTree::printTreeRecursive(StateTreeNode &node, unsigned &label){
     if(node.get_nodeType() == "Join"){
        printTreeRecursive(*node.get_children()[0],label);
        unsigned label1 = label;
        printTreeRecursive(*node.get_children()[1],label);
        unsigned label2 = label;
        label++;
        cout<< to_string(label)+" ";
        cout<<node.get_nodeType() +"("+to_string(label1)+","+to_string(label2)+")\n";
        node.printStateTreeNode();
      }else if( node.get_nodeType() == "Empty"){
        label++;
        cout<< to_string(label)+" "+node.get_nodeType()+"\n";
        node.printStateTreeNode();
    }else {
         printTreeRecursive(*node.get_children()[0],label);
        label++;
        cout<< to_string(label)+" "+node.get_nodeType()+"("+to_string(label-1)+")\n";

        node.printStateTreeNode();
        }
}

void StateTree::printStateTree() {
    unsigned label = 0;
    printTreeRecursive(*root,label);
}

string StateTree::printAbstractRecursive(StateTreeNode &node, unsigned &label){
    
    if(node.get_nodeType() == "Join"){
        string s1 =  printAbstractRecursive(*node.get_children()[0],label);
        unsigned label1 = label;
        string s2 =  printAbstractRecursive(*node.get_children()[1],label);
        unsigned label2 = label;
        label++;
        return s1+s2+to_string(label)+" "+(node.printAbstract())+"("+to_string(label1)+","+to_string(label2)+")\n";
    }else if( node.get_nodeType() == "Empty"){
        label++;
        return to_string(label)+" "+node.printAbstract()+"\n";
    }else {
        string s = printAbstractRecursive(*node.get_children()[0],label);
        label++;
        return s +to_string(label)+" "+node.printAbstract()+"("+to_string(label-1)+")""\n";
    }



}

void StateTree::printAbstract() {
    unsigned label = 0;
    cout<< printAbstractRecursive(*root, label);
}

bool StateTree::readToken(string::iterator &it, string token){
    string::iterator tempIt = it;
    for(string::iterator itr = token.begin() ; itr != token.end() ; ++itr){
        if(!(*tempIt== *itr)){
            return false;
        }else{
            tempIt++;
        }
    }
    it = tempIt;
    return true;

} 

unsigned StateTree::readInteger(string::iterator &it){
    string::iterator tempIt = it;
    int integer = -1;
    string digits; 
    while(isdigit(*tempIt)){
        digits =  digits + *tempIt;
        tempIt++;
    }
    it = tempIt;
    integer =  stoi(digits);
    return integer;
}

shared_ptr<StateTreeNode> StateTree::readStateTreeExpressionRecursive(string::iterator &it, string::iterator end, vector<shared_ptr<StateTreeNode> > &v, DynamicKernel &kernel){
    int label  = 0; 
    if(isdigit(*it)){
        label = readInteger(it);
        if(readToken(it, "IntroVertex_")){
            int introducedVertex = readInteger(it);
            int childLabel = 0 ;
            if(readToken(it,"(")){
                childLabel = readInteger(it);
                if(readToken(it,")")){

                }else{
                    // To Do : print an error message here
                }
            }else{
                // To Do : print an error message here
            }
            StateTreeNode stateObj = v[childLabel]->introVertex(introducedVertex); 
            vector<shared_ptr<StateTreeNode> > children;
            children.push_back(v[childLabel]);
            stateObj.set_children(children);
            v[childLabel]->set_parent(stateObj.shared_from_this());
            if(v.size() < label+1){
                 v.resize(label+1);
             }
            v[label] = stateObj.shared_from_this();
            if(it!=end){
                 readStateTreeExpressionRecursive(it,end,v,kernel);
            }
            return stateObj.shared_from_this();
         }else if(readToken(it,"IntroEdge_")){
            int introducedEdge1 = readInteger(it);
            int introducedEdge2 = 0;
            if(readToken(it, "_")){
                introducedEdge2 = readInteger(it);
            }else{
                // TO DO: print an error message
            }
            int childLabel = 0 ;
            if(readToken(it,"(")){
                childLabel = readInteger(it);
                if(readToken(it,")")){

                }else{
                    // To Do : print an error message here
                }
            }else{
                // To Do : print an error message here
            }
            StateTreeNode stateObj = v[childLabel]->introEdge(introducedEdge1,introducedEdge2); 
            vector<shared_ptr<StateTreeNode> > children;
            children.push_back(v[childLabel]);
            stateObj.set_children(children);
            v[childLabel]->set_parent(stateObj.shared_from_this());
            if(v.size() < label+1){
                 v.resize(label+1);
             }
            v[label] = stateObj.shared_from_this();
            if(it!=end){
                 readStateTreeExpressionRecursive(it,end,v,kernel);
            }
            return stateObj.shared_from_this();
                
        }else if(readToken(it, "ForgetVertex_")){
            int forgottenVertex = readInteger(it);
            int childLabel = 0 ;
            if(readToken(it,"(")){
                childLabel = readInteger(it);
                if(readToken(it,")")){

                }else{
                    // To Do : print an error message here
                }
            }else{
                // To Do : print an error message here
            } 
            StateTreeNode stateObj = v[childLabel]->forgetVertex(forgottenVertex); 
            vector<shared_ptr<StateTreeNode> > children;
            children.push_back(v[childLabel]);
            stateObj.set_children(children);
            v[childLabel]->set_parent(stateObj.shared_from_this());
            if(v.size() < label+1){
                 v.resize(label+1);
             }
            v[label] = stateObj.shared_from_this();
            if(it!=end){
                 readStateTreeExpressionRecursive(it,end,v,kernel);
            }
            return stateObj.shared_from_this();
        }else if(readToken(it, "Join")){
             int childLabel1 = 0 ;
            int childLabel2 = 0;
            if(readToken(it,"(")){
                childLabel1 = readInteger(it);
                if(readToken(it,",")){
                    childLabel2 = readInteger(it);
                }else{
                    // TO DO: print an error message
                }
                if(readToken(it,")")){

                }else{
                    // To Do : print an error message here
                }

            }else{
                // To Do : print an error message here
            }

            StateTreeNode stateObj = join(*v[childLabel1], *v[childLabel2]); 
            vector<shared_ptr<StateTreeNode> > children;
            children.push_back(v[childLabel1]);
            children.push_back(v[childLabel2]);
            stateObj.set_children(children);
            v[childLabel1]->set_parent(stateObj.shared_from_this());
            v[childLabel2]->set_parent(stateObj.shared_from_this());
            if((int)v.size() < label+1){
                 v.resize(label+1);
             }
            v[label] = stateObj.shared_from_this();
            if(it!=end){
                 readStateTreeExpressionRecursive(it,end,v,kernel);
            }
            return stateObj.shared_from_this();

        }else if(readToken(it, "Empty")){

            StateTreeNode stateObj;
            State s;
            for(int i = 0 ; i < (int)kernel.get_properties().size() ; i++){
                s.addWitnessSet(kernel.pointerToCoreNumber(i)->get_initialSet());
            }
            v[label] = stateObj.shared_from_this();
            if(it!=end){
                 readStateTreeExpressionRecursive(it,end,v, kernel);
            }
            stateObj.set_S(s.shared_from_this());
            stateObj.set_kernel(kernel.shared_from_this());
            return stateObj.shared_from_this();
                
        }else{

            cout<<"ERROR: in readStateTreeExpressionRecursive token isn't matched "<<endl;
            exit(20);
        }

    }
}


void StateTree::readStateTree(string s, DynamicKernel &kernel) {

    s.erase(std::remove_if(s.begin(),s.end(),::isspace) ,s.end());
    string::iterator it = s.begin();
    vector<shared_ptr<StateTreeNode> > v;
    readStateTreeExpressionRecursive(it, s.end(), v, kernel);
    this->root = v[v.size()-1];
} 

