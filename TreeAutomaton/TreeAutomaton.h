#ifndef TREEWIDZARD_TREEAUTOMATON_H
#define TREEWIDZARD_TREEAUTOMATON_H

#include<iostream>
#include<vector>
#include<memory>
#include<set>
#include "Term.h"
#include "RunTree.h"

template<class StateType, class TermNodeContent>
class Transition{
private:
    StateType consequentState;
    TermNodeContent transitionContent;
    std::vector<StateType> antecedentStates;
public:
    Transition(StateType consequentState, TermNodeContent transitionContent, const std::vector<StateType> &antecedentStates)
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

    const std::vector<StateType> &getAntecedentStates() const {
        return antecedentStates;
    }

    void setAntecedentStates(const std::vector<StateType> &antecedentStates) {
        Transition::antecedentStates = antecedentStates;
    }

    //print functions
    void print() {
        std::cout<<"Antecedent States:"<<std::endl;
        printAntecedentStates();
        std::cout<<std::endl;
        std::cout<<"Transition Content:"<<std::endl;
        printTransitionContent();
        std::cout<<std::endl;
        std::cout<<"Consequent State:"<<std::endl;
        printConsequentState();
        std::cout<<std::endl;
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
        if (!(this->consequentState == rhs.consequentState)) return this->consequentState < rhs.consequentState; // Note that we have a comparator for std::shared_ptr<State>
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
    std::set<StateType> states; // Obs: StateType needs to have an equality comparatora and a less than comparator
    std::set<StateType> finalStates;
    std::set<Transition<StateType,TermNodeContent> > transitions;
public:

    const std::set<StateType> &getStates() const {
        return states;
    }

    void setStates(const std::set<StateType> &states) {
        TreeAutomaton::states = states;
    }

    const std::set<StateType> &getFinalStates() const {
        return finalStates;
    }

    void setFinalStates(const std::set<StateType> &finalStates) {
        TreeAutomaton::finalStates = finalStates;
    }

    const std::set<Transition<StateType, TermNodeContent>> &getTransitions() const {
        return transitions;
    }

    void setTransitions(const std::set<Transition<StateType, TermNodeContent>> &transitions) {
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
        std::cout<<"States:"<<std::endl;
        for(auto state:states){
            state.print();
        }
        std::cout<<"Final State:"<<std::endl;
        for(auto state:finalStates){
            state.print();
        }
        std::cout<<"Transition:"<<std::endl;
        for(auto transition:transitions){
            std::cout<<"****************transition***********************"<<std::endl;
            transition.print();
        }
    }
    // The next two functions are meant to return a term that reaches the state "state"
    void retrieveTermNodeAcyclicAutomaton(StateType state, std::shared_ptr<TermNode<TermNodeContent>> node);
    std::shared_ptr<TermNode<TermNodeContent>> retrieveTermAcyclicAutomaton(StateType state);
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // The next two functions are meant to return a run that reaches the state "state"
    // A run is just a term where the symbol at each position is a pair of an "alphabet symbol" and a "state"
    void retrieveRunNodeAcyclicAutomaton(StateType state, std::shared_ptr<TermNode<RunNodeContent<StateType,TermNodeContent> > > node);
    //Recovers the root node of a term that reaches state. Assumes that the automaton is acyclic, and that the language of each state is non-empty.
    RunTree<StateType,TermNodeContent> retrieveRunAcyclicAutomaton(StateType state);
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
void TreeAutomaton<StateType,TermNodeContent>::retrieveTermNodeAcyclicAutomaton(StateType state, std::shared_ptr<TermNode<TermNodeContent> > node){
    //Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
    if (!this->transitions.empty()){
        TermNodeContent a;
        a.setSymbol(a.smallestContent()); //Creates a symbol of type TermNodeContent and set it to the smallest symbol
        std::vector<StateType> emptyAntecedents;
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
                    std::cout << "Error1: No transition with consequent equal to the input state." ;
                    exit(20);
                }
            }else{
                std::cout << "Error2: No transition with consequent equal to the input state.";
                exit(20);
            }
        }
        node->setNodeContent(itAux->getTransitionContent());
        for (int i = 0; i < itAux->getAntecedentStates().size(); i++){
            std::shared_ptr<TermNode<TermNodeContent> > child(new TermNode<TermNodeContent>);
            child->setParent(node);
            node->addChild(child);
            retrieveTermNodeAcyclicAutomaton(itAux->getAntecedentStates()[i],child);
        }
    } else {
        std::cout << "Error: The automaton has no transitions." << std::endl;
        exit(20);
    }
}


template<class StateType, class TermNodeContent>
std::shared_ptr<TermNode<TermNodeContent>> TreeAutomaton<StateType,TermNodeContent>::retrieveTermAcyclicAutomaton(StateType state){
    std::shared_ptr<TermNode<TermNodeContent> > root(new TermNode<TermNodeContent>);
    root->setParent(nullptr);
    retrieveTermNodeAcyclicAutomaton(state,root);
    return root;
}

template<class StateType, class TermNodeContent>
void TreeAutomaton<StateType, TermNodeContent>::retrieveRunNodeAcyclicAutomaton(StateType state,
                                                                                    std::shared_ptr<TermNode<RunNodeContent<StateType,TermNodeContent>>> node) {
//Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
    if (!transitions.empty()) {
        TermNodeContent a;
        a.setSymbol(a.smallestContent()); //Creates a symbol of type TermNodeContent and set it to the smallest symbol
        std::vector<StateType> emptyAntecedents;
        Transition<StateType, TermNodeContent> t(state, a,
                                                 emptyAntecedents); // This is the smallest transition with a consequent equal to state
        auto it = transitions.upper_bound(t);
        if (it != transitions.begin()) {
            it--; // This is always defined, since the transition set is non-empty
        }
        auto itAux = it;
        if (itAux->getConsequentState() != state) {
            itAux++;
            if (itAux != transitions.end()) {
                if (itAux->getConsequentState() != state) {
                    std::cout << "Error1: No transition with consequent equal to the input state.";
                    exit(20);
                }
            } else {
                std::cout << "Error2: No transition with consequent equal to the input state.";
                exit(20);
            }
        }
       RunNodeContent<StateType, TermNodeContent> runNode;
        runNode.setState(state);
        runNode.setRunNodeContent(itAux->getTransitionContent());
        node->setNodeContent(runNode);
        std::vector<std::shared_ptr<TermNode<RunNodeContent<StateType, TermNodeContent> > >> children;
        for (int i = 0; i < itAux->getAntecedentStates().size(); i++) {
            std::shared_ptr<TermNode<RunNodeContent<StateType, TermNodeContent>>>
                    child(new TermNode<RunNodeContent<StateType, TermNodeContent>>);
            child->setParent(node);
            children.push_back(child);
            retrieveRunNodeAcyclicAutomaton(itAux->getAntecedentStates()[i], child);
        }
        node->setChildren(children);

    } else {
        std::cout << "Error: The automaton has no transitions." << std::endl;
        exit(20);
    }

}

template<class StateType, class TermNodeContent>
RunTree<StateType,TermNodeContent>
TreeAutomaton<StateType, TermNodeContent>::retrieveRunAcyclicAutomaton(StateType state) {
    std::shared_ptr<TermNode<RunNodeContent<StateType,TermNodeContent>>> root(new TermNode<RunNodeContent<StateType,TermNodeContent>>);
    retrieveRunNodeAcyclicAutomaton(state,root);
    RunTree<StateType,TermNodeContent> runTree;
    runTree.setRoot(root);
    return runTree;
}


#endif //TREEWIDZARD_TREEAUTOMATON_H
