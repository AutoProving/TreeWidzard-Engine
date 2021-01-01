
#include "TreeAutomaton.h"


template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType,TermNodeContent>::retrieveRunNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<RunNodeContent<TermNodeContent,StateType> > > node){
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
            itAux++;
        }
        if (itAux != this->transitions.end()){
            if (itAux->consequentState == state){
                node->nodeContent.runNodeContent = itAux->transitionContent;
                node->nodeContent.state = state;
                for (int i = 0; i< itAux->antecendentStates.size(); i++){
                    shared_ptr<TermNode<TermNodeContent> > child(new TermNode<RunNodeContent<TermNodeContent,StateType> >);
                    child->parent = node;
                    node->children.push_back(child);
                    retrieveTermNodeAcyclicAutomaton(itAux->antecedentStates[i],child);
                }
                return node;
            }else{
                cout << "Error: No transition with consequent equal to the input state.";
                exit(20);
            }
        }
    } else {
        cout << "Error: The automaton has no transitions." << endl;
        exit(20);
    }

}

template<class StateType, class TermNodeContent>
Term<RunNodeContent<TermNodeContent,StateType> > TreeAutomaton<StateType,TermNodeContent>::retrieveRunAcyclicAutomaton(StateType state){
    Term<RunNodeContent<TermNodeContent,StateType> > term;
    shared_ptr<TermNode<RunNodeContent<TermNodeContent,StateType> > > root(new TermNode<RunNodeContent<TermNodeContent,StateType> >);
    root.parent = nullptr;
    retrieveTermNodeAcyclicAutomaton(state,root);
    return term;
}

