
#ifndef TREEWIDZARD_TERM_H
#define TREEWIDZARD_TERM_H
#include <memory>
#include <vector>
#include <iostream>
#include <set>

class TermNodeContentType{
public:
    virtual ~TermNodeContentType() = default;
    virtual void print();
    virtual std::string nodeInformation();
    /*  virtual std::shared_ptr<TermNodeContentType> smallestContent();
    virtual void print();
    virtual bool operator<(const TermNodeContentType &rhs) const;
    virtual bool operator==(const TermNodeContentType &rhs) const;*/

//    bool operator>(const TermNodeContentType& rhs) const {
//        return !( (*this < rhs) or (*this==rhs));
//    }
//    bool operator!=(const TermNodeContentType &rhs) const{
//        return !(*this==rhs);
//    }
};

template<class TermNodeContent>
class TermNode{
private:
    TermNodeContent nodeContent;
    std::vector<std::shared_ptr<TermNode<TermNodeContent> > > children;
    std::shared_ptr<TermNode<TermNodeContent> > parent;
public:
    TermNode();

    TermNode(TermNodeContent nodeContent, const std::vector<std::shared_ptr<TermNode<TermNodeContent>>> &children,
             const std::shared_ptr<TermNode<TermNodeContent>> &parent);

    void setNodeContent(TermNodeContent nodeContent) {
        this->nodeContent = nodeContent;
    }

    TermNodeContent getNodeContent() const;

    std::vector<std::shared_ptr<TermNode<TermNodeContent>>> &getChildren() {
        return children;
    }
    void setChildren(const std::vector<std::shared_ptr<TermNode<TermNodeContent>>> &children) {
        TermNode::children = children;
    }
    const std::shared_ptr<TermNode<TermNodeContent>> &getParent() const {
        return parent;
    }

    void setParent(const std::shared_ptr<TermNode<TermNodeContent>> &parent) {
        TermNode::parent = parent;
    }
    void addChild(std::shared_ptr<TermNode<TermNodeContent> > child){
        children.push_back(child);
    }
    bool operator==(const TermNode &rhs) const {
        return nodeContent == rhs.nodeContent &&
               children == rhs.children &&
               parent == rhs.parent;
    }

    bool operator!=(const TermNode &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const TermNode& rhs) const{
        if (!(this->nodeContent==rhs.nodeContent)){
            return this->nodeContent < rhs.nodeContent;
        } else {
            if (!(this->children.size() == rhs.children.size())) {
                return this->children.size() < rhs.children.size();
            } else {
                for (int i = 0; i < children.size(); i++) {
                    if (!(*this->children[i] == *rhs.children[i])) {
                        return *this->children[i] < *rhs.children[i];
                    }
                }
            }
            return false; // in this case the terms are equal.
        }
    }

    bool operator>(const TermNode &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const TermNode &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const TermNode &rhs) const {
        return !(*this < rhs);
    }
    std::string nodeInformation(unsigned &label);
    void printTermNode(unsigned &label);
};

template<class TermNodeContent>
void TermNode<TermNodeContent>::printTermNode(unsigned &label) {

    std::set<unsigned > childrenLabel;
    for(auto child : children){
        child->printTermNode(label);
        childrenLabel.insert(label);
    }
    label++;
    std::cout<<label <<"  ";
    nodeContent.print() ;
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
    std::cout<<std::endl;

}

template<class TermNodeContent>
TermNodeContent TermNode<TermNodeContent>::getNodeContent() const {
    return nodeContent;
}

template<class TermNodeContent>
std::string TermNode<TermNodeContent>::nodeInformation(unsigned int &label) {
    std::string s;
    std::set<unsigned > childrenLabel;
    for(auto child : children){
        s+=child->nodeInformation(label);
        childrenLabel.insert(label);
    }
    label++;

    std::string t = std::to_string(label) + " "+ nodeContent.nodeInformation();
    if(childrenLabel.size()){
        t+= "(";
        for(auto childNo:childrenLabel){
            t+= std::to_string(childNo);
            if(childNo!=*(--childrenLabel.end())){
                t+=",";
            }
        }
        t+=")";
    }
    t+="\n";
    s = s+t;
    return s;
}

template<class TermNodeContent>
TermNode<TermNodeContent>::TermNode() {}

template<class TermNodeContent>
TermNode<TermNodeContent>::TermNode(TermNodeContent nodeContent,
                                    const std::vector<std::shared_ptr<TermNode<TermNodeContent>>> &children,
                                    const std::shared_ptr<TermNode<TermNodeContent>> &parent):nodeContent(nodeContent),
                                                                                         children(children),
                                                                                         parent(parent) {}



template<class TermNodeContent>
class Term{
private:
    std::shared_ptr<TermNode<TermNodeContent> > root= nullptr;
public:

    std::shared_ptr<TermNode<TermNodeContent>> &getRoot() {
        return root;
    }

    void setRoot(std::shared_ptr<TermNode<TermNodeContent>>& rootNode) {
        root = rootNode;
    }

    bool operator==(const Term &rhs) const {
        return root == rhs.root;
    }

    bool operator!=(const Term &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Term &rhs) const {
        return root < rhs.root;
    }

    bool operator>(const Term &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Term &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Term &rhs) const {
        return !(*this < rhs);
    }

    ///////TODO
    void printTermNodes();
    std::string termInformation();
    ////Format?////
};

template<class TermNodeContent>
void Term<TermNodeContent>::printTermNodes() {
    unsigned label=0;
    if(!root){
        std::cout<< "Term is empty"<<std::endl;
        return;
    }
    root->printTermNode(label);
}

template<class TermNodeContent>
std::string Term<TermNodeContent>::termInformation() {
    unsigned label = 0;
    if(!root){
        return "Term is empty";
    }
    return root->nodeInformation(label);
}


#endif //TREEWIDZARD_TERM_H
