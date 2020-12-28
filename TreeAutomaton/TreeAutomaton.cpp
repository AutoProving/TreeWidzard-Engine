#include "TreeAutomaton.h"

int AbstractTreeDecompositionSymbol::symbolType(string s) {
    if(s.find("Leaf") != string::npos){
        return 0;
    }else if(s.find("IntroVertex") != string::npos){
        return 1;
    } else if(s.find("ForgetVertex") != string::npos) {
        return 2;
    }else     if(s.find("IntroEdge") != string::npos) {
        return 3;
    }else if(s.find("Join") != string::npos) {
        return 4;
    }else{
        cout<<"Error: AbstractTreeDecompositionSymbol::symbolType, string is not in formal format."<<endl;
        exit(20);
    }
}

vector<int> AbstractTreeDecompositionSymbol::symbolNumbers(string s) {
    // IntroVertex_i
}

void AbstractTreeDecompositionSymbol::setSymbol(string s) {
    symbol = s;
}
template<class SymbolType>
void TermNode<SymbolType>::printSymbol() {
    cout<<symbol;
}

template<class SymbolType>
void TermNode<SymbolType>::printSubterm() {
    cout<<symbol<<endl;
    for(auto child:children){
        child->printSubterm();
    }
}

template<class SymbolType>
void TermNode<SymbolType>::setSymbol(SymbolType s) {
    symbol = s;
}

template<class SymbolType>
void TermNode<SymbolType>::setChildren(shared_ptr<TermNode<SymbolType>> ch) {
    children = ch;
}

template<class SymbolType>
void TermNode<SymbolType>::setParent(shared_ptr<TermNode<SymbolType>> p) {
    parent = p ;
}

template<class SymbolType>
SymbolType TermNode<SymbolType>::getSymbol() {
    return this->symbol;
}

template<class SymbolType>
shared_ptr<TermNode<SymbolType>> TermNode<SymbolType>::getParent() {
    return this->parent;
}

template<class SymbolType>
vector<shared_ptr<TermNode<SymbolType>>> TermNode<SymbolType>::getChildren() {
    return this->children;
}

template<class SymbolType>
shared_ptr<TermNode<SymbolType>> TermNode<SymbolType>::getChild(int i) {
    return children[i];
}


template<class SymbolType>
void Term<SymbolType>::printTerm() {
    root->printSubterm();
}


template<class StateType, class SymbolType>
void Transition<StateType, SymbolType>::printConsequentState() {
    consequentState.print();
}

template<class StateType, class SymbolType>
void Transition<StateType, SymbolType>::printSymbol() {
    cout<<symbol;
}

template<class StateType, class SymbolType>
void Transition<StateType, SymbolType>::printAntecedentStates() {
    cout<<"( ";
    for(auto state:antecedentStates){
            antecedentStates->print();
            if(state!=antecedentStates.end()-1)
                cout<<",";
    }
    cout<<")";
}

template<class StateType, class SymbolType>
void Transition<StateType, SymbolType>::printTransitions() {
    printConsequentState();
    cout<<", ";
    printSymbol();
    cout<<", ";
}

template<class StateType, class SymbolType>
void Transition<StateType, SymbolType>::setConsequentState(StateType state) {
    consequentState = state;
}

template<class StateType, class SymbolType>
void Transition<StateType, SymbolType>::setSymbol(SymbolType sym) {
        symbol = sym;
}

template<class StateType, class SymbolType>
void Transition<StateType, SymbolType>::setAntecedentStates(vector<StateType> antS) {
    antecedentStates = antS;
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::printAlphabet() {
    for (auto s:alphabet) {
        cout<<*s;
        if(s!=alphabet.end()-1)
            cout<<", ";
    }
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::printStates() {
    for(auto state:states){
        (*state).print();
    }
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::printFinalStates() {
    for(auto finalState:finalStates){
        (*finalState).print();
    }
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::printTransitions() {
    for(auto transition:transitions){
        (*transition).printTransitions();
    }
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::printAutomaton() {
    cout<<"Symbols: ";
    printAlphabet();
    cout<<"States:"<<endl;
    printStates();
    cout<<"Final States:"<<endl;
    printFinalStates();
    cout<<"Transitions"<<endl;
    printTransitions();
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::addSymbol(SymbolType symbol) {
    alphabet.push_back(symbol);
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::addState(StateType state) {
    states.insert(state);
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::addFinalState(StateType state) {
    finalStates.insert(state);
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType, SymbolType>::addTransition(Transition<StateType, SymbolType> transition) {
    transitions.insert(transition);
}

template<class StateType, class SymbolType>
void TreeAutomaton<StateType,SymbolType>::retrieveTermNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<SymbolType> > node){
	//Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
	if (!this->transitions.empty()){
		SymbolType a;
		a.setSymbol(a.SmallestSymbol()); //Creates a symbol of type SymbolType and set it to the smallest symbol 
		vector<StateType> emptyAntecedents;
		Transition<StateType,SymbolType> t(state,a,emptyAntecedents); // This is the smallest transition with a consequent equal to state
		auto it = this->transitions.upper_bound();
		it--; // This is always defined, since the transition set is non-empty
		auto itAux = it; 
		if (itAux->consequentState != state){ 
			itAux++
		}
		if (itAux != this->transitions.end()){
			if (itAux->consequentState == state){
				node.symbol = itAux->Symbol;
				for (int i = 0; i< itAux->antecendentStates.size(); i++){
					shared_ptr<TermNode<SymbolType> > child(new TermNode<SymbolType>); 
					child->parent = node; 
					node.children.push_back(child); 
					retrieveTermNodeAcyclicAutomaton(itAux->antecedentStates[i],child); 
				}
			}
		}
		cout << "Error: No transition with consequent equal to the input state.";
		exit(20);
	} else {
		cout << "Error: The automaton has no transitions." << end;
		exit(20);	
	}

}


template<class StateType, class SymbolType>
Term<SymbolType> TreeAutomaton<StateType,SymbolType>::retrieveTermAcyclicAutomaton(StateType state){
	Term<SymbolType> term;
	shared_ptr<TermNode<SymbolType> > root(new TermNode<SymbolType>);
	root.parent = NULL; 
	retrieveTermNodeAcyclicAutomaton(state,root); 
	return term; 
}






template<class StateType, class SymbolType>
void TreeAutomaton<StateType,SymbolType>::retrieveRunNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<RunSymbol<SymbolType,StateType> > > node){
	//Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
	if (!this->transitions.empty()){
		SymbolType a;
		a.setSymbol(a.SmallestSymbol()); //Creates a symbol of type SymbolType and set it to the smallest symbol 
		vector<StateType> emptyAntecedents;
		Transition<StateType,SymbolType> t(state,a,emptyAntecedents); // This is the smallest transition with a consequent equal to state
		auto it = this->transitions.upper_bound();
		it--; // This is always defined, since the transition set is non-empty
		auto itAux = it; 
		if (itAux->consequentState != state){ 
			itAux++
		}
		if (itAux != this->transitions.end()){
			if (itAux->consequentState == state){
				node.symbol.symbol = itAux->Symbol;
				node.symbol.state = state; 
				for (int i = 0; i< itAux->antecendentStates.size(); i++){
					shared_ptr<TermNode<SymbolType> > child(new TermNode<RunSymbol<SymbolType,StateType> >); 
					child->parent = node; 
					node.children.push_back(child); 
					retrieveTermNodeAcyclicAutomaton(itAux->antecedentStates[i],child); 
				}
			}
		}
		cout << "Error: No transition with consequent equal to the input state.";
		exit(20);
	} else {
		cout << "Error: The automaton has no transitions." << end;
		exit(20);	
	}

}


template<class StateType, class SymbolType>
Term<SymbolType> TreeAutomaton<StateType,SymbolType>::retrieveRunAcyclicAutomaton(StateType state){
	Term<RunSymbol<SymbolType,StateType> > term;
	shared_ptr<TermNode<RunSymbol<SymbolType,StateType> > > root(new TermNode<RunSymbol<SymbolType,StateType> >);
	root.parent = NULL; 
	retrieveTermNodeAcyclicAutomaton(state,root); 
	return term; 
}






