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
    void printConsequentState();
    void printAntecedentStates();
    void printTransitions();
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

    // The next two functions are meant to return a term that reaches the state "state"
    void retrieveTermNodeAcyclicAutomaton(StateType state, shared_ptr<TermNode<TermNodeContent>> node);
    Term<TermNodeContent> retrieveTermAcyclicAutomaton(StateType state);
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

#endif //TREEWIDZARD_TREEAUTOMATON_H
