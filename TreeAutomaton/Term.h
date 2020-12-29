
#ifndef TREEWIDZARD_TERM_H
#define TREEWIDZARD_TERM_H
#include <memory>
#include <vector>
#include <iostream>
using namespace std;

class TermNodeContentType{
public:
    virtual void print();
    /*  virtual shared_ptr<TermNodeContentType> smallestContent();
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
class TermNode : enable_shared_from_this<TermNodeContent> {
private:
    TermNodeContent nodeContent;
    vector<shared_ptr<TermNode<TermNodeContent> > > children;
    shared_ptr<TermNode<TermNodeContent> > parent;
public:
    void setNodeContent(TermNodeContent nodeContent) {
        TermNode::nodeContent = nodeContent;
    }
    const vector<shared_ptr<TermNode<TermNodeContent>>> &getChildren() const {
        return children;
    }
    void setChildren(const vector<shared_ptr<TermNode<TermNodeContent>>> &children) {
        TermNode::children = children;
    }
    const shared_ptr<TermNode<TermNodeContent>> &getParent() const {
        return parent;
    }

    void setParent(const shared_ptr<TermNode<TermNodeContent>> &parent) {
        TermNode::parent = parent;
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
    void print();
};

template<class TermNodeContent>
void TermNode<TermNodeContent>::print() {
    nodeContent.print();
    for(auto child : children){
        child->print();
    }
}


template<class TermNodeContent>
class Term{

private:
    shared_ptr<TermNode<TermNodeContent>> root;
public:
    const shared_ptr<TermNode<TermNodeContent>> &getRoot() const {
        return root;
    }

    void setRoot(const shared_ptr<TermNode<TermNodeContent>> &root) {
        Term::root = root;
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
    void printTerm();
    ////Format?////
    void printTermToFile();
    void readTerm(string inputFile);
    //////////////
};

template<class TermNodeContent>
void Term<TermNodeContent>::printTerm() {
    root->print();
}


#endif //TREEWIDZARD_TERM_H
