#include "TreeAutomaton.h"

template<class StateType, class TermNodeContent>
void Transition<StateType, TermNodeContent>::printConsequentState() {
    consequentState.print();
}

template<class StateType, class TermNodeContent>
void Transition<StateType, TermNodeContent>::printContent() {
	this->transitionContent.printContent();
}

template<class StateType, class TermNodeContent>
void Transition<StateType, TermNodeContent>::printAntecedentStates() {
    cout<<"( ";
    for(auto state:antecedentStates){
            antecedentStates->print();
            if(state!=antecedentStates.end()-1)
                cout<<",";
    }
    cout<<")";
}

template<class StateType, class TermNodeContent>
void Transition<StateType, TermNodeContent>::printTransitions() {
    printConsequentState();
    cout<<", ";
    printSymbol();
    cout<<", ";
}

template<class StateType, class TermNodeContent>
void Transition<StateType, TermNodeContent>::setConsequentState(StateType state) {
    consequentState = state;
}

template<class StateType, class TermNodeContent>
void Transition<StateType, TermNodeContent>::setSymbol(TermNodeContent content) {
        this->transitionContent = content;
}

template<class StateType, class TermNodeContent>
void Transition<StateType, TermNodeContent>::setAntecedentStates(vector<StateType> antS) {
    antecedentStates = antS;
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::printAlphabet() {
    for (auto s:alphabet) {
        cout<<*s;
        if(s!=alphabet.end()-1)
            cout<<", ";
    }
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::printStates() {
    for(auto state:states){
        (*state).print();
    }
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::printFinalStates() {
    for(auto finalState:finalStates){
        (*finalState).print();
    }
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::printTransitions() {
    for(auto transition:transitions){
        (*transition).printTransitions();
    }
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::printAutomaton() {
    cout<<"Symbols: ";
    printAlphabet();
    cout<<"States:"<<endl;
    printStates();
    cout<<"Final States:"<<endl;
    printFinalStates();
    cout<<"Transitions"<<endl;
    printTransitions();
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::addState(StateType state) {
    states.insert(state);
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::addFinalState(StateType state) {
    finalStates.insert(state);
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::addTransition(Transition<StateType, TermNodeContent> transition) {
    transitions.insert(transition);
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType,TermNodeContent>::retrieveTermNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<TermNodeContent> > node){
	//Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
	if (!this->transitions.empty()){
		TermNodeContent a;
		a.setSymbol(a.smallestContent()); //Creates a symbol of type TermNodeContent and set it to the smallest symbol 
		vector<StateType> emptyAntecedents;
		Transition<StateType,TermNodeContent> t(state,a,emptyAntecedents); // This is the smallest transition with a consequent equal to state
		auto it = this->transitions.upper_bound();
		it--; // This is always defined, since the transition set is non-empty
		auto itAux = it; 
		if (itAux->consequentState != state){ 
			itAux++
		}
		if (itAux != this->transitions.end()){
			if (itAux->consequentState == state){
				node->nodeContent = itAux->transitionContent;
				for (int i = 0; i< itAux->antecendentStates.size(); i++){
					shared_ptr<TermNode<TermNodeContent> > child(new TermNode<TermNodeContent>); 
					child->parent = node; 
					node->children.push_back(child); 
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


template<class StateType, class TermNodeContent>
Term<TermNodeContent> TreeAutomaton<StateType,TermNodeContent>::retrieveTermAcyclicAutomaton(StateType state){
	Term<TermNodeContent> term;
	shared_ptr<TermNode<TermNodeContent> > root(new TermNode<TermNodeContent>);
	root.parent = NULL; 
	retrieveTermNodeAcyclicAutomaton(state,root); 
	return term; 
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType,TermNodeContent>::retrieveRunNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<RunContent<TermNodeContent,StateType> > > node){
	//Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
	if (!this->transitions.empty()){
		TermNodeContent a;
		a.setSymbol(a.smallestContent()); //Creates a symbol of type TermNodeContent and set it to the smallest symbol 
		vector<StateType> emptyAntecedents;
		Transition<StateType,TermNodeContent> t(state,a,emptyAntecedents); // This is the smallest transition with a consequent equal to state
		auto it = this->transitions.upper_bound();
		it--; // This is always defined, since the transition set is non-empty
		auto itAux = it; 
		if (itAux->consequentState != state){ 
			itAux++
		}
		if (itAux != this->transitions.end()){
			if (itAux->consequentState == state){
				node->nodeContent.runNodeContent = itAux->transitionContent;
				node->nodeContent.state = state; 
				for (int i = 0; i< itAux->antecendentStates.size(); i++){
					shared_ptr<TermNode<TermNodeContent> > child(new TermNode<RunContent<TermNodeContent,StateType> >); 
					child->parent = node; 
					node->children.push_back(child); 
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


template<class StateType, class TermNodeContent>
Term<TermNodeContent> TreeAutomaton<StateType,TermNodeContent>::retrieveRunAcyclicAutomaton(StateType state){
	Term<RunContent<TermNodeContent,StateType> > term;
	shared_ptr<TermNode<RunContent<TermNodeContent,StateType> > > root(new TermNode<RunContent<TermNodeContent,StateType> >);
	root.parent = NULL; 
	retrieveTermNodeAcyclicAutomaton(state,root); 
	return term; 
}






