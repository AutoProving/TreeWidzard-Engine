#include<iostream>
#include<vector>
#include<memory>
#include<set>
using namespace std;

template<class TermNodeContent, class StateType> 
class RunNodeContent{
	TermNodeContent runNodeContent; 
	StateType state;
	void printRunSymbol(){
		cout << "NodeContent: ";
        runNodeContent.print();
		cout << " State: ";
		state.print(); 	
	}
	//////////////
	bool operator<(const RunSymbol<TermNodeContent,StateType>& rhs) const{
		return this->root < rhs.root; 
	}
	bool operator==(const RunSymbol<TermNodeContent,StateType>& rhs) const{
		return this->root == rhs.root; 
	}
	bool operator>(const RunSymbol<TermNodeContent,StateType>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}

};

template<class StateType, class TermNodeContent>
class Transition{
private:
	StateType consequentState; 
	TermNodeContent transitionContent; 
	vector<StateType> antecedentStates; 
public: 
	//print functions
	void printConsequentState();
	void printSymbol();
	void printAntecedentStates();
	void printTransitions();
	//setting functions
	void setConsequentState(StateType state);
	void setSymbol(TermNodeContent sym);
	void setAntecedentStates(vector<StateType> antS);
	//Operators
	bool operator<(const Transition<StateType,TermNodeContent>& rhs) const{
		if (!(this->consequentState == rhs.consequentState)) return this->consequentState < rhs.consequentState; // Note that we have a comparator for shared_ptr<State>
		if (!(this->symbol == rhs.symbol)) return this->symbol < rhs.symbol; 
		for (int i=0; i< antecedentStates.size(); i++){
			if (!(this->antecedentStates[i] == rhs.antecedentStates[i])) return this->antecedentStates[i] < rhs.antecedentStates[i];	
		}
		return false; // In this case, the transitions are equal
	}
	bool operator==(const Transition<StateType,TermNodeContent>& rhs) const{
		return (this->consequentState == rhs.consequentState) and (this->symbol == rhs.symbol) and (this->antecedentStates == rhs.antecedentStates); 
	}
	bool operator>(const Transition<TermNodeContent,TermNodeContent>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}
};



template<class StateType,class TermNodeContent>
class TreeAutomaton{
private:
	//vector<TermNodeContent> alphabet; 
	set<StateType> states; // Obs: StateType needs to have an equality comparatora and a less than comparator
	set<StateType> finalStates;
	set<Transition<StateType,TermNodeContent> > transitions; 
public:
	// print functions
	void printAlphabet();
	void printStates();
	void printFinalStates();
	void printTransitions();
	void printAutomaton();
	// insertion functions
	void addSymbol(TermNodeContent symbol);
	void addState(StateType state);
	void addFinalState(StateType state);
	void addTransition(Transition<StateType,TermNodeContent> transition);
	// membership functions
	bool findSymbol(TermNodeContent symbol);
	bool findState(StateType state);
	bool findFinalState(StateType state);
	bool findTransition(Transition<StateType,TermNodeContent> transition);
	bool membership(Term<TermNodeContent> term);
	friend TreeAutomaton<StateType,TermNodeContent> intersectionAutomata(TreeAutomaton<StateType,TermNodeContent> automaton1, TreeAutomaton<StateType,TermNodeContent> automaton2); 
	friend TreeAutomaton<StateType,TermNodeContent> unionAutomata(TreeAutomaton<StateType,TermNodeContent> automaton1, TreeAutomaton<StateType,TermNodeContent> automaton2); 
	TreeAutomaton<StateType,TermNodeContent> complementation();
	//////////////////////////////////////////////////////////////////////////////////////////
	// The next two functions are meant to return a term that reaches the state "state"
        void retrieveTermNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<TermNodeContent> >); 
        Term<TermNodeContent> retrieveTermAcyclicAutomaton(StateType state);
	//////////////////////////////////////////////////////////////////////////////////////////	
	//////////////////////////////////////////////////////////////////////////////////////////
	// The next two functions are meant to return a run that reaches the state "state"
	// A run is just a term where the symbol at each position is a pair of an "alphabet symbol" and a "state"
	void retrieveRunNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<RunSymbol<TermNodeContent, State> > > node);
	//Recovers the root node of a term that reaches state. Assumes that the automaton is acyclic, and that the language of each state is non-empty. 	
	Term<RunSymbol<TermNodeContent,StateType> > retrieveRunAcyclicAutomaton(StateType state);
	//Recovers a term that reaches state. Assumes that the automaton is acyclic, and that the language of each state is non-empty. 	
	////////////////////////////////////////////////////////////////////////////////////////////
	//Operators
	bool operator<(const TreeAutomaton<StateType,TermNodeContent>& rhs) const;
	bool operator==(const TreeAutomaton<StateType,TermNodeContent>& rhs) const;
	bool operator>(const TreeAutomaton<TermNodeContent,TermNodeContent>& rhs) const {
		return !( (*this < rhs) or (*this==rhs)); 
	}

};




