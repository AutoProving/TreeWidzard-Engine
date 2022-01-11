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
