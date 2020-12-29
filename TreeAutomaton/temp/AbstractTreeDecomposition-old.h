class AbstractTreeDecompositionNodeContent : public NodeContentType {
	string symbol;
	AbstractTreeDecompositionNodeContent(string symbol){
		this->symbol = symbol; 
	}
	bool operator< (AbstractTreeDecompositionNodeContent &rhs)  {
		int typeSymbol1 = symbolType(this->symbol);
		int typeSymbol2 = symbolType(rhs.symbol);
		vector<int> numbersSymbol1 = symbolNumbers(this->symbol);
		vector<int> numbersSymbol2 = symbolNumbers(rhs.symbol);
		if (typeSymbol1<typeSymbol2) return true;
		if (typeSymbol1>typeSymbol2) return false;
		return numbersSymbol1 < numbersSymbol2;
 	};
	int symbolType(string symbol) const; // returns "0 if Leaf, 1 if IntroVertex, 2 if  ForgetVertex, 3 if IntroEdge or 4 if Join"
	//ToDo: implement this
    vector<int> symbolNumbers(string s) const; // returns vector "[i]" if symbol="IntroVertex_i or ForgetVertex_i", Returns vector "[i,j]" if symbol="IntroEdge_i_j", Returns empty vector if symbol = "Join"
	bool operator== (const AbstractTreeDecompositionNodeContent &rhs) const {return this->symbol == rhs.symbol;}
	void setSymbol(string s);
	//Operators
	bool operator>(const AbstractTreeDecompositionNodeContent &rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}
	//Define the lexicographically smallest content. 
	AbstractTreeDecompositionNodeContent smallestContent(){
		return "Leaf";
	}
};





class AbstractTreeDecomposition: public Term<AbstractTreeDecompositionNodeContent>{
//class ATD: public Term<AbstractTreeDecompositionSymbol>{
//	public:
//		ATD(); 
//		ATD(Multigraph G);
//		ATD(TreeDe)
//		Multigraph ATD_To_Multigraph();
//		ATD()
//
//}
//
}













