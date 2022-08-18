#include "DecompositionTree.h"

DecompositionNode::DecompositionNode(){
    parent = shared_ptr<DecompositionNode>(nullptr);
}

set<unsigned> DecompositionNode::getVertices(){
    return this->vertices;
}

void DecompositionNode::addVertex(unsigned i){

    this->vertices.insert(i);
}

void DecompositionNode::setVertices(set<unsigned> vertices){
    this->vertices = vertices;
}

void DecompositionNode::setParent(shared_ptr<DecompositionNode> node){
    this->parent = node;
}

shared_ptr<DecompositionNode> DecompositionNode::getParent(){
    return this->parent;
}

vector<shared_ptr<DecompositionNode>> DecompositionNode::getChildren(){
    return this->children;
}

void DecompositionNode::addChild(shared_ptr<DecompositionNode> child){
    children.push_back(child);
}

void DecompositionNode::setChildren(vector<shared_ptr<DecompositionNode>> children){
    this->children  = children;
}

Decomposition::Decomposition(){
    root = shared_ptr<DecompositionNode>(nullptr);
}

void Decomposition::setRoot(shared_ptr<DecompositionNode> node){

    this->root = node;

}

void Decomposition::print(){
    list<pair<int,int>> edges;
    int label = 0;
    int parentLabel = 0;
    string s = printNode(root,label,parentLabel,edges);
    for(auto item:edges){
        s+=to_string(item.first)+" "+to_string(item.second)+"\n";
    }
    findMaxBagSizeAndVertices(root);
    s  = "s td " + to_string(label)+ " " + to_string(maxBagSize) + " " + to_string(vertices.size())+"\n" + s;
    cout<<s;
}

string Decomposition::printNode(shared_ptr<DecompositionNode> node, int &label, int parentLabel,list<pair<int,int>> &edges){
    label++;
    if(parentLabel != 0)
        edges.push_back(make_pair(parentLabel,label));
    int l = label;
    string str = "b " + to_string(label);
    set<unsigned> v = node->getVertices();
    for(auto item:v){
        str+= " "+to_string(item); 
    }
    str+= "\n";
    vector<shared_ptr<DecompositionNode>> children = node->getChildren();
    for(int i = 0; i < children.size() ; i++){
        str += printNode(children[i],label,l,edges);
    }
    return str;
}

string Decomposition::stringPrint(){
    list<pair<int,int>> edges;
    int label = 0;
    int parentLabel = 0;
    string s = printNode(root,label,parentLabel,edges);
    for(auto item:edges){
        s+=to_string(item.first)+" "+to_string(item.second)+"\n";
    }
    findMaxBagSizeAndVertices(root);
    s  = "s td " + to_string(label)+ " " + to_string(maxBagSize) + " " + to_string(vertices.size())+"\n" + s;
    return s;
}

void Decomposition::writeToFile(string fileName) {
    ofstream decFile(fileName);
    if (decFile.is_open())
    {
        decFile << stringPrint();
        decFile.close();
    }
    else {
        cout << "Unable to open "<< fileName << endl;
        exit(20);
    }
}

void Decomposition::findMaxBagSizeAndVertices(shared_ptr<DecompositionNode> node){
        if(maxBagSize < node->getVertices().size())
        {
            maxBagSize = node->getVertices().size();
        }
        set<unsigned> nVertices;
        set<unsigned> nodeVertices = node->getVertices();
        set_union(nodeVertices.begin(),nodeVertices.end(),vertices.begin(),vertices.end(),inserter(nVertices,nVertices.begin()));
        vertices = nVertices;
        for(int i = 0; i < node->getChildren().size(); i++){
            findMaxBagSizeAndVertices(node->getChildren()[i]);
        }
}



