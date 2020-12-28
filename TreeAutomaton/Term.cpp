
template<class TermNodeContent>
void TermNode<TermNodeContent>::printSymbol() {
    cout<<symbol;
}

template<class TermNodeContent>
void TermNode<TermNodeContent>::printSubterm() {
    cout<<symbol<<endl;
    for(auto child:children){
        child->printSubterm();
    }
}

template<class TermNodeContent>
void TermNode<TermNodeContent>::setSymbol(TermNodeContent s) {
    symbol = s;
}

template<class TermNodeContent>
void TermNode<TermNodeContent>::setChildren(shared_ptr<TermNode<TermNodeContent>> ch) {
    children = ch;
}

template<class TermNodeContent>
void TermNode<TermNodeContent>::setParent(shared_ptr<TermNode<TermNodeContent>> p) {
    parent = p ;
}

template<class TermNodeContent>
TermNodeContent TermNode<TermNodeContent>::getContent() {
    return this->symbol;
}

template<class TermNodeContent>
shared_ptr<TermNode<TermNodeContent>> TermNode<TermNodeContent>::getParent() {
    return this->parent;
}

template<class TermNodeContent>
vector<shared_ptr<TermNode<TermNodeContent>>> TermNode<TermNodeContent>::getChildren() {
    return this->children;
}

template<class TermNodeContent>
shared_ptr<TermNode<TermNodeContent>> TermNode<TermNodeContent>::getChild(int i) {
    return children[i];
}


template<class TermNodeContent>
void Term<TermNodeContent>::printTerm() {
    root->printSubterm();
}


