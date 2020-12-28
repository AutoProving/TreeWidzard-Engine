#ifndef TREEAUTOMATON_H
#define TREEAUTOMATON_H
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
	//Operators
	bool operator>(const AbstractTreeDecompositionSymbol &rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}
	AbstractTreeDecompositionSymbol SmallestSymbol(){
		return "Leaf";
	}
};



template<class SymbolType, class StateType> 
class RunSymbol{
	SymbolType symbol; 
	StateType state;
	void printRunSymbol(){
		cout << "Symbol: "
		symbol.print(); 
		cout << " State: "
		state.print(); 	
	}
	//////////////
	bool operator<(const RunSymbol<SymbolType,StateType>& rhs) const{
		return this->root < rhs.root; 
	}
	bool operator==(const RunSymbol<SymbolType,StateType>& rhs) const{
		return this->root == rhs.root; 
	}
	bool operator>(const RunSymbol<SymbolType,StateType>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}

}



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
        bool operator<(const TermNode<SymbolType>& rhs) const{
		if (!(this->symbol==rhs.symbol)){
			return this->symbol < rhs.symbol;
		} else {	
			if (!(this->children.size() == rhs.children.size())){
				return this->children.size() < rhs.children.size();
			} else {
				for (int i = 0; i< children.size(); i++){
					if (!(this->children[i] == rhs.children[i])){
						return this->children[i] < rhs.children[i]; 
					}
				}
			}
			return false; // in this case the terms are equal. 
		{
	}

	bool operator==(const TermNode<SymbolType>& rhs) const{ 
		if (!(this->symbol== rhs.symbol)) return false; 
		if (this->children.size() == rhs.children.size()){ 
			for (int i = 0; i< children.size(); i++){
				if (!(this->children[i] == rhs.children[i]) return false; // Note: this is a recursive step
			}
		} else{
			return false; 
		}
		return true;
	}

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
	bool operator<(const Term<SymbolType>& rhs) const{
		return this->root < rhs.root; 
	}
	bool operator==(const Term<SymbolType>& rhs) const{
		return this->root == rhs.root; 
	}
	bool operator>(const Term<SymbolType>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}

	
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
	//Operators
	bool operator<(const Transition<StateType,SymbolType>& rhs) const{
		if (!(this->consequentState == rhs.consequentState)) return this->consequentState < rhs.consequentState; // Note that we have a comparator for shared_ptr<State>
		if (!(this->symbol == rhs.symbol)) return this->symbol < rhs.symbol; 
		for (int i=0; i< antecedentStates.size(); i++){
			if (!(this->antecedentStates[i] == rhs.antecedentStates[i])) return this->antecedentStates[i] < rhs.antecedentStates[i];	
		}
		return false; // In this case, the transitions are equal
	}
	bool operator==(const Transition<StateType,SymbolType>& rhs) const{
		return (this->consequentState == rhs.consequentState) and (this->symbol == rhs.symbol) and (this->antecedentStates == rhs.antecedentStates); 
	}
	bool operator>(const Transition<SymbolType,SymbolType>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}
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
	//////////////////////////////////////////////////////////////////////////////////////////
	// The next two functions are meant to return a term that reaches the state "state"
        void retrieveTermNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<SymbolType> >); 
        Term<SymbolType> retrieveTermAcyclicAutomaton(StateType state);
	//////////////////////////////////////////////////////////////////////////////////////////	
	//////////////////////////////////////////////////////////////////////////////////////////
	// The next two functions are meant to return a run that reaches the state "state"
	// A run is just a term where the symbol at each position is a pair of an "alphabet symbol" and a "state"
	void retrieveRunNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<RunSymbol<SymbolType, State> > > node);
	//Recovers the root node of a term that reaches state. Assumes that the automaton is acyclic, and that the language of each state is non-empty. 	
        Term<RunSymbol<SymbolType,StateType> > retrieveRunAcyclicAutomaton(StateType state); 
	//Recovers a term that reaches state. Assumes that the automaton is acyclic, and that the language of each state is non-empty. 	
	////////////////////////////////////////////////////////////////////////////////////////////
	//Operators
	bool operator<(const TreeAutomaton<StateType,SymbolType>& rhs) const;
	bool operator==(const TreeAutomaton<StateType,SymbolType>& rhs) const;
	bool operator>(const TreeAutomaton<SymbolType,SymbolType>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}

};
#endif




