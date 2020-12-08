#include<iostream>
#include<vector>
#include<memory>
#include<set>
using namespace std;

class AbstractTreeDecompositionSymbol{
	string symbol;
	bool operator< (AbstractTreeDecompositionSymbol &rhs)  {
		int typeSymbol1 = symbolType(this->symbol); 
		int typeSymbol2 = symbolType(rhs.symbol);
		vector<int> numbersSymbol1 = symbolNumbers(this->symbol);
		vector<int> numbersSymbol2 = symbolNumbers(rhs.symbol);
		if (typeSymbol1<typeSymbol2) return true;
		if (typeSymbol1>typeSymbol2) return false;
		return numbersSymbol1 < numbersSymbol2;  
 	};  
	int symbolType(string symbol); // returns "0 if Leaf, 1 if IntroVertex, 2 if  ForgetVertex, 3 if IntroEdge or 4 if Join"
	//ToDo: implement this
	vector<int> symbolNumbers(string s); // returns vector "[i]" if symbol="IntroVertex_i or ForgetVertex_i", Returns vector "[i,j]" if symbol="IntroEdge_i_j", Returns empty vector if symbol = "Join"
	bool operator== (const AbstractTreeDecompositionSymbol &rhs) const {return this->symbol == rhs.symbol;}
	void setSymbol(string s);
};


template<class SymbolType> 
class TermNode{
private:
	SymbolType symbol; 
	vector<shared_ptr<TermNode<SymbolType> > > children;
	shared_ptr<TermNode> parent; 
public:
	//print functions
	void printSymbol();
	void printSubterm();
	//setting functions
	void setSymbol(SymbolType s);
	void setChildren(shared_ptr<TermNode> ch);
	void setParent(shared_ptr<TermNode> p);
	//get functions
	SymbolType getSymbol();
	shared_ptr<TermNode<SymbolType>> getParent();
    vector<shared_ptr<TermNode<SymbolType> > > getChildren();
    shared_ptr<TermNode<SymbolType>> getChild(int i); // returns a pointer to the i-th child
};


template<class SymbolType>
class Term{
public:
	shared_ptr<TermNode<SymbolType>> root;
	void printTerm();
    ////Format?////
	void printTermToFile(); 
	void readTerm(string inputFile);
	//////////////
};



template<class StateType, class SymbolType>
class Transition{
private:
	StateType consequentState; 
	SymbolType symbol; 
	vector<StateType> antecedentStates; 
public: 
	//print functions
	void printConsequentState();
	void printSymbol();
	void printAntecedentStates();
	void printTransitions();
	//setting functions
	void setConsequentState(StateType state);
	void setSymbol(SymbolType sym);
	void setAntecedentStates(vector<StateType> antS);
};



template<class StateType,class SymbolType>
class TreeAutomaton{
private:
	vector<SymbolType> alphabet;
	set<StateType> states;
	set<StateType> finalStates;
	set<Transition<StateType,SymbolType> > transitions; 
public:
	// print functions
	void printAlphabet();
	void printStates();
	void printFinalStates();
	void printTransitions();
	void printAutomaton();
	// insertion functions
	void addSymbol(SymbolType symbol);
	void addState(StateType state);
	void addFinalState(StateType state);
	void addTransition(Transition<StateType,SymbolType> transition);
	// membership functions
	bool findSymbol(SymbolType symbol);
	bool findState(StateType state);
	bool findFinalState(StateType state);
	bool findTransition(Transition<StateType,SymbolType> transition);
	bool membership(Term<SymbolType> term);
	friend TreeAutomaton<StateType,SymbolType> intersectionAutomata(TreeAutomaton<StateType,SymbolType> automaton1, TreeAutomaton<StateType,SymbolType> automaton2); 
	friend TreeAutomaton<StateType,SymbolType> unionAutomata(TreeAutomaton<StateType,SymbolType> automaton1, TreeAutomaton<StateType,SymbolType> automaton2); 
	TreeAutomaton<StateType,SymbolType> complementation(); 
};





