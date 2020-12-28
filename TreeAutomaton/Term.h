class TermNodeContentType{
	//TermNodeContentType(); 
	virtual TermNodeContentType smallestContent(); 
	virtual void print();
	virtual bool operator<(const TermNodeContentType &rhs) const; 
	virtual bool operator==(const TermNodeContentType &rhs) const; 
	bool operator>(const Term<TermNodeContent>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}
	bool operator!=(const TermNodeContentType &rhs) const{
		return !(*this==rhs); 
	}

}

template<class TermNodeContent> 
class TermNode{
private:
	TermNodeContent nodeContent; 
	vector<shared_ptr<TermNode<TermNodeContent> > > children;
	shared_ptr<TermNode> parent; 
public:
	//print functions
	void printSymbol();
	void printSubterm();
	//setting functions
	void setSymbol(TermNodeContent s);
	void setChildren(shared_ptr<TermNode> ch);
	void setParent(shared_ptr<TermNode> p);
	//get functions
	TermNodeContent getContent();
	shared_ptr<TermNode<TermNodeContent>> getParent();
    vector<shared_ptr<TermNode<TermNodeContent> > > getChildren();
    shared_ptr<TermNode<TermNodeContent>> getChild(int i); // returns a pointer to the i-th child
        bool operator<(const TermNode<TermNodeContent>& rhs) const{
		if (!(this->nodeContent==rhs.nodeContent)){
			return this->nodeContent < rhs.nodeContent;
		} else {	
			if (!(this->children.size() == rhs.children.size())){
				return this->children.size() < rhs.children.size();
			} else {
				for (int i = 0; i< children.size(); i++){
					if (!(*this->children[i] == *rhs.children[i])){
						return *this->children[i] < *rhs.children[i]; 
					}
				}
			}
			return false; // in this case the terms are equal. 
		{
	}

	bool operator==(const TermNode<TermNodeContent>& rhs) const{ 
		if (!(this->nodeContent== rhs.nodeContent)) return false; 
		if (this->children.size() == rhs.children.size()){ 
			for (int i = 0; i< children.size(); i++){
				if (!(*this->children[i] == *rhs.children[i]) return false; // Note: this is a recursive step
			}
		} else{
			return false; 
		}
		return true;
	}

};


template<class TermNodeContent>
class Term{
public:
	shared_ptr<TermNode<TermNodeContent>> root;
	void printTerm();
    ////Format?////
	void printTermToFile(); 
	void readTerm(string inputFile);
	//////////////
	bool operator<(const Term<TermNodeContent>& rhs) const{
		return *this->root < *rhs.root; 
	}
	bool operator==(const Term<TermNodeContent>& rhs) const{
		return *this->root == *rhs.root; 
	}
	bool operator>(const Term<TermNodeContent>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}
};




