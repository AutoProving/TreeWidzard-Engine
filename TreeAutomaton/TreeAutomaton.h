    #ifndef TREEWIDZARD_TREEAUTOMATON_H
#define TREEWIDZARD_TREEAUTOMATON_H

#include<iostream>
#include<vector>
#include<memory>
#include<set>
#include "Term.h"
using namespace std;

template<class StateType, class TermNodeContent>
class RunNodeContent{
private:
    TermNodeContent runNodeContent;
    StateType state;
public:
    TermNodeContent getRunNodeContent() const {
        return runNodeContent;
    }

    void setRunNodeContent(TermNodeContent runNodeContent) {
        RunNodeContent::runNodeContent = runNodeContent;
    }

    StateType getState() const {
        return state;
    }

    void setState(StateType state) {
        RunNodeContent::state = state;
    }

    bool operator==(const RunNodeContent &rhs) const {
        return runNodeContent == rhs.runNodeContent &&
               state == rhs.state;
    }

    bool operator!=(const RunNodeContent &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const RunNodeContent &rhs) const {
        if (runNodeContent < rhs.runNodeContent)
            return true;
        if (rhs.runNodeContent < runNodeContent)
            return false;
        return state < rhs.state;
    }

    bool operator>(const RunNodeContent &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const RunNodeContent &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const RunNodeContent &rhs) const {
        return !(*this < rhs);
    }

    void print() {
        cout<<"Term Node Content:"<<endl;
        runNodeContent.print();
        cout<<"State:"<<endl;
        state.print();
    }

};

template<class StateType, class TermNodeContent>
class Transition{
private:
    StateType consequentState;
    TermNodeContent transitionContent;
    vector<StateType> antecedentStates;
public:
    Transition(StateType consequentState, TermNodeContent transitionContent, const vector<StateType> &antecedentStates)
            : consequentState(consequentState), transitionContent(transitionContent),
              antecedentStates(antecedentStates) {}

    StateType getConsequentState() const {
        return consequentState;
    }

    void setConsequentState(StateType consequentState) {
        Transition::consequentState = consequentState;
    }

    TermNodeContent getTransitionContent() const {
        return transitionContent;
    }

    void setTransitionContent(TermNodeContent transitionContent) {
        Transition::transitionContent = transitionContent;
    }

    const vector<StateType> &getAntecedentStates() const {
        return antecedentStates;
    }

    void setAntecedentStates(const vector<StateType> &antecedentStates) {
        Transition::antecedentStates = antecedentStates;
    }

    //print functions
    void print() {
        cout<<"Antecedent States:"<<endl;
        printAntecedentStates();
        cout<<"Transition Content:"<<endl;
        printTransitionContent();
        cout<<"Consequent State:"<<endl;
        printConsequentState();
    };
    void printConsequentState(){
        consequentState.print();
    };
    void printAntecedentStates(){
        for(auto antecedent:antecedentStates){
            antecedent.print();
        }
    };
    void printTransitionContent(){
        transitionContent.print();
    }
    //Operators

    bool operator==(const Transition &rhs) const {
        return consequentState == rhs.consequentState &&
               transitionContent == rhs.transitionContent &&
               antecedentStates == rhs.antecedentStates;
    }

    bool operator!=(const Transition &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Transition &rhs) const {
        if (!(this->consequentState == rhs.consequentState)) return this->consequentState < rhs.consequentState; // Note that we have a comparator for shared_ptr<State>
        if (!(this->transitionContent == rhs.transitionContent)) return this->transitionContent < rhs.transitionContent;
        for (int i=0; i< antecedentStates.size(); i++){
            if (!(this->antecedentStates[i] == rhs.antecedentStates[i])) return this->antecedentStates[i] < rhs.antecedentStates[i];
        }
        return false; // In this case, the transitions are equal
    }

    bool operator>(const Transition &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Transition &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Transition &rhs) const {
        return !(*this < rhs);
    }
};


template<class StateType,class TermNodeContent>
class TreeAutomaton{
private:
    set<StateType> states; // Obs: StateType needs to have an equality comparatora and a less than comparator
    set<StateType> finalStates;
    set<Transition<StateType,TermNodeContent> > transitions;
public:

    const set<StateType> &getStates() const {
        return states;
    }

    void setStates(const set<StateType> &states) {
        TreeAutomaton::states = states;
    }

    const set<StateType> &getFinalStates() const {
        return finalStates;
    }

    void setFinalStates(const set<StateType> &finalStates) {
        TreeAutomaton::finalStates = finalStates;
    }

    const set<Transition<StateType, TermNodeContent>> &getTransitions() const {
        return transitions;
    }

    void setTransitions(const set<Transition<StateType, TermNodeContent>> &transitions) {
        TreeAutomaton::transitions = transitions;
    }

    void addTransition(Transition<StateType,TermNodeContent> transition){
        transitions.insert(transition);
    }
    void addState(StateType state){
        states.insert(state);
    };

    void addFinalState(StateType state){
        finalStates.insert(state);
    };

    bool operator==(const TreeAutomaton &rhs) const {
        return states == rhs.states &&
               finalStates == rhs.finalStates &&
               transitions == rhs.transitions;
    }

    bool operator!=(const TreeAutomaton &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const TreeAutomaton &rhs) const {
        if (states < rhs.states)
            return true;
        if (rhs.states < states)
            return false;
        if (finalStates < rhs.finalStates)
            return true;
        if (rhs.finalStates < finalStates)
            return false;
        return transitions < rhs.transitions;
    }

    bool operator>(const TreeAutomaton &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const TreeAutomaton &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const TreeAutomaton &rhs) const {
        return !(*this < rhs);
    }

    void print(){
        cout<<"States:"<<endl;
        for(auto state:states){
            state.print();
        }
        cout<<"Final State:"<<endl;
        for(auto state:finalStates){
            state.print();
        }
        cout<<"Transition:"<<endl;
        for(auto transition:transitions){
            cout<<"****************transition***********************"<<endl;
            transition.print();
        }
    }
    // The next two functions are meant to return a term that reaches the state "state"
    void retrieveTermNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<TermNodeContent>> node);
    shared_ptr<TermNode<TermNodeContent>> retrieveTermAcyclicAutomaton(StateType state);
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // The next two functions are meant to return a run that reaches the state "state"
    // A run is just a term where the symbol at each position is a pair of an "alphabet symbol" and a "state"
    void retrieveRunNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<RunNodeContent<TermNodeContent, StateType> > > node);
    //Recovers the root node of a term that reaches state. Assumes that the automaton is acyclic, and that the language of each state is non-empty.
    Term<RunNodeContent<TermNodeContent,StateType> > retrieveRunAcyclicAutomaton(StateType state);
    //Recovers a term that reaches state. Assumes that the automaton is acyclic, and that the language of each state is non-empty.

//    // print functions
//    void printAlphabet();
//    void printStates();
//    void printFinalStates();
//    void printTransitions();
//    void printAutomaton();
//    // insertion functions
//    void addSymbol(TermNodeContent symbol);
//    void addState(StateType state);
//    void addFinalState(StateType state);
//    void addTransition(Transition<StateType,TermNodeContent> transition);
//    // membership functions
//    bool findSymbol(TermNodeContent symbol);
//    bool findState(StateType state);
//    bool findFinalState(StateType state);
//    bool findTransition(Transition<StateType,TermNodeContent> transition);
//    bool membership(Term<TermNodeContent> term);
//    friend TreeAutomaton<StateType,TermNodeContent> intersectionAutomata(TreeAutomaton<StateType,TermNodeContent> automaton1, TreeAutomaton<StateType,TermNodeContent> automaton2);
//    friend TreeAutomaton<StateType,TermNodeContent> unionAutomata(TreeAutomaton<StateType,TermNodeContent> automaton1, TreeAutomaton<StateType,TermNodeContent> automaton2);
//    TreeAutomaton<StateType,TermNodeContent> complementation();
    ////////////////////////////////////////////////////////////////////////////////////////////
//    //Operators
//    bool operator<(const TreeAutomaton<StateType,TermNodeContent>& rhs) const;
//    bool operator==(const TreeAutomaton<StateType,TermNodeContent>& rhs) const;
//    bool operator>(const TreeAutomaton<TermNodeContent,TermNodeContent>& rhs) const {
//        return !( (*this < rhs) or (*this==rhs));
//    }

};

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType,TermNodeContent>::retrieveTermNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<TermNodeContent> > node){
    //Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
    if (!this->transitions.empty()){
        TermNodeContent a;
        a.setSymbol(a.smallestContent()); //Creates a symbol of type TermNodeContent and set it to the smallest symbol
        vector<StateType> emptyAntecedents;
        Transition<StateType,TermNodeContent> t(state,a,emptyAntecedents); // This is the smallest transition with a consequent equal to state
        auto it = transitions.upper_bound(t);
        if(it != transitions.begin()){
            it--; // This is always defined, since the transition set is non-empty
        }
        auto itAux = it;
        if ( itAux->getConsequentState() != state){
            itAux++;
            if(itAux != transitions.end()){
                if (itAux->getConsequentState() != state){
                    cout << "Error: No transition with consequent equal to the input state.";
                    exit(20);
                }
            }else{
                cout << "Error: No transition with consequent equal to the input state.";
                exit(20);
            }
        }
        node->setNodeContent(itAux->getTransitionContent());
        for (int i = 0; i < itAux->getAntecedentStates().size(); i++){
            shared_ptr<TermNode<TermNodeContent> > child(new TermNode<TermNodeContent>);
            child->setParent(node);
            node->addChild(child);
            retrieveTermNodeAcyclicAutomaton(itAux->getAntecedentStates()[i],child);
        }
    } else {
        cout << "Error: The automaton has no transitions." << endl;
        exit(20);
    }
}


template<class StateType, class TermNodeContent>
shared_ptr<TermNode<TermNodeContent>> TreeAutomaton<StateType,TermNodeContent>::retrieveTermAcyclicAutomaton(StateType state){
    shared_ptr<TermNode<TermNodeContent> > root(new TermNode<TermNodeContent>);
    root->setParent(nullptr);
    retrieveTermNodeAcyclicAutomaton(state,root);
    return root;
}


#endif //TREEWIDZARD_TREEAUTOMATON_H
