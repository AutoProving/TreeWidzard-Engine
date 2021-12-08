// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "BreadthFirstSearch.h"

#include <utility>

extern "C" {
    BreadthFirstSearch * create() {
        return new BreadthFirstSearch();
    }
    BreadthFirstSearch * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
        return new BreadthFirstSearch(dynamicKernel,conjecture,flags);
    }
}

BreadthFirstSearch::BreadthFirstSearch() {
    addAttribute("SearchName","BreadthFirstSearch");
}

BreadthFirstSearch::BreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags): SearchStrategy(dynamicKernel, conjecture, flags) {
    this->kernel = kernel;
	this->conjecture = conjecture;
	this->flags = flags;
    addAttribute("SearchName","BreadthFirstSearch");
}


AbstractTreeDecomposition BreadthFirstSearch::extractCounterExampleTerm(State::ptr state) {
    AbstractTreeDecomposition atd;
    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> rootNode;
    rootNode = bfsDAG.retrieveTermAcyclicAutomaton(state);
    atd.setRoot(rootNode);
    return atd;
}

StateTree BreadthFirstSearch::extractCounterExampleStateTree(State::ptr state) {
    StateTree stateTree;
    shared_ptr<StateTreeNode> root(new StateTreeNode());
    shared_ptr<DynamicKernel> sharedKernel = make_shared<DynamicKernel>(*kernel);
    root->set_kernel(sharedKernel);
    extractCounterExampleStateTreeNode(state,root);
    stateTree.root = root;
    return stateTree;
}

void BreadthFirstSearch::extractCounterExampleStateTreeNode(State::ptr state, shared_ptr<StateTreeNode> node) {
    //Assumes that the automaton is acyclic and that each state has a transition in which the state is the consequent
    if (!bfsDAG.getTransitions().empty()){
        AbstractTreeDecompositionNodeContent a;
        a.setSymbol(a.smallestContent()); //Creates a symbol of type TermNodeContent and set it to the smallest symbol
        vector<State::ptr> emptyAntecedents;
        Transition<State::ptr,AbstractTreeDecompositionNodeContent> t(state,a,emptyAntecedents); // This is the smallest transition with a consequent equal to state
        auto it = bfsDAG.getTransitions().upper_bound(t);
        if(it != bfsDAG.getTransitions().begin()){
            it--; // This is always defined, since the transition set is non-empty
        }
        auto itAux = it;
        if ( itAux->getConsequentState() != state){
            itAux++;
            if(itAux != bfsDAG.getTransitions().end()){
                if (itAux->getConsequentState() != state){
                    cout << "Error: No transition with consequent equal to the input state.";
                    exit(20);
                }
            }else{
                cout << "Error: No transition with consequent equal to the input state.";
                exit(20);
            }
        }
        node->set_nodeType(itAux->getTransitionContent().getSymbol());
        node->set_S(state);
        vector<shared_ptr<StateTreeNode>> children;
        for (int i = 0; i < itAux->getAntecedentStates().size(); i++){
            shared_ptr<StateTreeNode> child(new StateTreeNode);
            child->set_parent(node);
            child->set_kernel(node->get_kernel());
            children.push_back(child);
            extractCounterExampleStateTreeNode(itAux->getAntecedentStates()[i],child);
        }
        node->set_children(children);
    } else {
        cout << "Error: The automaton has no transitions." << endl;
        exit(20);
    }
}

void BreadthFirstSearch::search(){
  if(flags->get("Premise")){
    cout<< " Premise is ACTIVATED"<<endl;
  }else{
    cout<<"Premise is NOT ACTIVATED"<<endl;
  }
  bool printStateFlag = flags->get("PrintStates");
  State::ptr initialState = kernel->initialState();
  allStatesSet.insert(initialState);
  newStatesSet.insert(initialState);
  // Initialize the DAG
  bfsDAG.addState(initialState);
  AbstractTreeDecompositionNodeContent initialTransitionContent("Leaf");
  vector<State::ptr> initialAntecedents; // Empty vector since there are no children.
  Transition<State::ptr,AbstractTreeDecompositionNodeContent> initialTransition(initialState,initialTransitionContent,initialAntecedents);
  bfsDAG.addTransition(initialTransition);
  ////////////////////////////////////
  unsigned int width = kernel->get_width().get_value();
  vector<unsigned > numberOfWitnesses;
  numberOfWitnesses.resize(initialState->numberOfComponents());
  int iterationNumber = 0;
  cout<<left<<setw(25)<<"Iteration"<<setw(25)<<"ALLSTATES" << setw(25)<< "NEWSTATES"<<"Max WITNESSSET SIZE"<<endl;
  while(!newStatesSet.empty()){
    iterationNumber++;
    ////////////////////////////////////////////////////////////////////////////////////
    newStatesVector.clear(); // clear newStatesVector to add states in newStatesSet in it
    newStatesVector.resize(newStatesSet.size()); //
    std::copy(newStatesSet.begin(),newStatesSet.end(),newStatesVector.begin());
    newStatesSet.clear(); // clear newStatesSet to add new states that are generated in this loop
    //This loop is suitable for parallelization
    for(size_t l=0; l < newStatesVector.size(); l++){
      State::ptr statePointer = newStatesVector[l];
      Bag bag = statePointer->get_bag();
      set<unsigned > bagElement  = bag.get_elements();
      ///////////////////////////////////////////////////////
      //////////////////// Introduce Vertex /////////////////
      ///////////////////////////////////////////////////////
      // the +1 below comes from the fact that treewidth is
      // size of the bag minus one. So the loop iterates
      // from 1 to number of elements in the  bag.
      for (int i=1; i<= width+1; i++){
        if(bag.vertex_introducible(i)){
          State::ptr newStatePointer = kernel->intro_v(statePointer, i);
          bool premiseFlag = flags->get("Premise");
          bool satisfiesPremise = false;
          if(premiseFlag){
            satisfiesPremise = conjecture->evaluatePremiseOnState(*newStatePointer);
          }
          if(!premiseFlag or (premiseFlag and satisfiesPremise) ){
            if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer) ){
              newStatesSet.insert(newStatePointer);
              State::ptr consequentState = newStatePointer;
              bfsDAG.addState(consequentState);
              AbstractTreeDecompositionNodeContent transitionContent("IntroVertex_"+ to_string(i));
              vector<State::ptr> antecedentStates;
              antecedentStates.push_back(statePointer);
              Transition<State::ptr,AbstractTreeDecompositionNodeContent> transition(consequentState,transitionContent,antecedentStates);
              bfsDAG.addTransition(transition);
              if(printStateFlag){
                  cout<<endl;
                  cout<< "========================================================================" <<endl;
                  cout<< " Introduce Vertex: " << i << endl;
                  cout<< "========================================================================" <<endl;
                  cout<< " Current State:"<<endl;
                  statePointer->print();
                  cout<< " New State:"<<endl;
                  newStatePointer->print();
                  cout<< "========================================================================" <<endl;
                  cout<<endl;
              }
              // size of witnessSets
              for (int component = 0; component < numberOfWitnesses.size(); ++component) {
                numberOfWitnesses[component] = max(numberOfWitnesses[component],(unsigned)consequentState->getWitnessSet(component)->size());
              }
            }
          }
        }
      }
      ///////////////////////////////////////////////////////
      //////////////////// Forget Vertex ////////////////////
      ///////////////////////////////////////////////////////
      for (auto it= bagElement.begin(); it!=bagElement.end(); it++){
        State::ptr newStatePointer = kernel->forget_v(statePointer, *it);
        bool premiseFlag = flags->get("Premise");
        bool satisfiesPremise = false;
        if(premiseFlag){
          satisfiesPremise = conjecture->evaluatePremiseOnState(*newStatePointer);
        }
        if(!premiseFlag or (premiseFlag and satisfiesPremise) ) {
          if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer)) {
            newStatesSet.insert(newStatePointer);
            State::ptr consequentState = newStatePointer;
            AbstractTreeDecompositionNodeContent transitionContent("ForgetVertex_" + to_string(*it));
            bfsDAG.addState(consequentState);
            vector<State::ptr> antecedentStates;
            antecedentStates.push_back(statePointer);
            Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
              transitionContent,
              antecedentStates);
              bfsDAG.addTransition(transition);
              if(printStateFlag){
                  cout<<endl;
                  cout<< "========================================================================" <<endl;
                  cout<< " Forget Vertex: " << *it << endl;
                  cout<< "========================================================================" <<endl;
                  cout<< " Current State:"<<endl;
                  statePointer->print();
                  cout<< " New State:"<<endl;
                  newStatePointer->print();
                  cout<< "========================================================================" <<endl;
                  cout<<endl;
              }
              // size of witnessSets
              for (int component = 0; component < numberOfWitnesses.size(); ++component) {
                numberOfWitnesses[component] = max(numberOfWitnesses[component],
                  (unsigned) consequentState->getWitnessSet(
                    component)->size());
                  }
                }
              }
            }
            //Introduce Edge
            if(bag.get_elements().size()>1){
              for (auto it=bagElement.begin(); it!=bagElement.end(); it++){
                auto itX = it;
                itX++; // TODO write this more elegantly
                if(itX!=bagElement.end()){
                  for (auto itPrime = itX ; itPrime != bagElement.end(); itPrime++){
                    State::ptr newStatePointer = kernel->intro_e(statePointer, *it, *itPrime);
                    bool premiseFlag = flags->get("Premise");
                    bool satisfiesPremise = false;
                    if(premiseFlag){
                      satisfiesPremise = conjecture->evaluatePremiseOnState(*newStatePointer);
                    }
                    if(!premiseFlag or (premiseFlag and satisfiesPremise) ){
                      if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer)){
                            newStatesSet.insert(newStatePointer);
                            State::ptr consequentState = newStatePointer;
                            AbstractTreeDecompositionNodeContent transitionContent("IntroEdge_"+ to_string(*it)+"_"+to_string(*itPrime));
                            bfsDAG.addState(consequentState);
                            vector<State::ptr> antecedentStates;
                            antecedentStates.push_back(statePointer);
                            Transition<State::ptr,AbstractTreeDecompositionNodeContent> transition(consequentState,transitionContent,antecedentStates);
                            bfsDAG.addTransition(transition);
                          if(printStateFlag){
                              cout<<endl;
                              cout<< "========================================================================" <<endl;
                              cout<< " Introduce Edge: " << *it << " " << *itPrime<< endl;
                              cout<< "========================================================================" <<endl;
                              cout<< " Current State:"<<endl;
                              statePointer->print();
                              cout<< " New State:"<<endl;
                              newStatePointer->print();
                              cout<< "========================================================================" <<endl;
                              cout<<endl;
                          }
                            // size of witnessSets
                            for (int component = 0; component < numberOfWitnesses.size(); ++component) {
                              numberOfWitnesses[component] = max(numberOfWitnesses[component],(unsigned)consequentState->getWitnessSet(component)->size());
                            }
                      }
                    }
                  }
                }
              }
            }

            // join
            if (kernel->get_width().get_name() == "tree_width") {
              // join
              for (auto it = allStatesSet.begin(); it != allStatesSet.end(); it++) {
                if (statePointer->get_bag().joinable((*it)->get_bag())) {
                  State::ptr newStatePointer = kernel->join(statePointer, *it);
                  bool premiseFlag = flags->get("Premise");
                  bool satisfiesPremise = false;
                  if(premiseFlag){
                    satisfiesPremise = conjecture->evaluatePremiseOnState(*newStatePointer);
                  }
                  if(!premiseFlag or (premiseFlag and satisfiesPremise) ){
                    if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer)) {
                      newStatesSet.insert(newStatePointer);
                      State::ptr consequentState = newStatePointer;
                      AbstractTreeDecompositionNodeContent transitionContent("Join");
                      bfsDAG.addState(consequentState);
                      vector<State::ptr> antecedentStates;
                      antecedentStates.push_back(statePointer);
                      antecedentStates.push_back(*it);
                      Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
                        transitionContent,
                        antecedentStates);
                        bfsDAG.addTransition(transition);
                        if(printStateFlag){
                            cout<<endl;
                            cout<< "========================================================================" <<endl;
                            cout<< " Join: " <<endl;
                            cout<< "========================================================================" <<endl;
                            cout<< " State One:"<<endl;
                            statePointer->print();
                            cout<< " State Two:"<<endl;
                            (*it)->print();
                            cout<< " New State:"<<endl;
                            newStatePointer->print();
                            cout<< "========================================================================" <<endl;
                            cout<<endl;
                        }
                        // size of witnessSets
                        for (int component = 0; component < numberOfWitnesses.size(); ++component) {
                          numberOfWitnesses[component] = max(numberOfWitnesses[component],(unsigned)consequentState->getWitnessSet(component)->size());
                        }
                      }
                    }
                  }
                }
                //                for (auto it = newStatesSet.begin(); it != newStatesSet.end(); it++) {
                //                    if (statePointer->get_bag().joinable((*it)->get_bag())) {
                //                        State::ptr newStatePointer = kernel->join(statePointer, *it);
                //                        if (!allStatesSet.count(newStatePointer) and !newStatesSet.count(newStatePointer)) {
                //                            newStatesSet.insert(newStatePointer);
                //                            State::ptr consequentState = newStatePointer;
                //                            AbstractTreeDecompositionNodeContent transitionContent("Join");
                //                            bfsDAG.addState(consequentState);
                //                            vector<State::ptr> antecedentStates;
                //                            antecedentStates.push_back(statePointer);
                //                            antecedentStates.push_back(*it);
                //                            Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState,
                //                                                                                                    transitionContent,
                //                                                                                                    antecedentStates);
                //                            bfsDAG.addTransition(transition);
                //                        }
                //                    }
                //                }
              }
            }
            for(auto it = newStatesSet.begin(); it!=newStatesSet.end(); it++){
              if(!conjecture->evaluateConjectureOnState(**it)){
                State::ptr badState = *it;
                bfsDAG.addFinalState(badState);
                AbstractTreeDecomposition atd  = extractCounterExampleTerm(badState);
                string file = this->getOutputsPath();
                if(flags->get("Premise")){file + "_Premise";}
                file+="_CounterExample";
                atd.writeToFile(file+"_AbstractDecomposition.txt");
                ConcreteTreeDecomposition ctd = atd.convertToConcreteTreeDecomposition();
                ctd.writeToFile(file+"_ConcreteDecomposition.txt");
                RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = extractCounterExampleRun(badState);
                runTree.writeToFile(file+"_RunTree.txt");
                MultiGraph multiGraph = ctd.extractMultiGraph();
                multiGraph.printGraph();
                multiGraph.printToFile(file+"_Graph.txt");
                multiGraph.convertToGML(file+"_GMLGraph.gml");
                multiGraph.printToFilePACEFormat(file+"_GraphPaceFormat.gr");

                /*                cout<<"BAD STATE:"<<endl;
                State::ptr state = *it;
                (**it).print();
                bfsDAG.addFinalState(*it);
                cout<<"-----------------Term Print---------------------"<<endl;
                AbstractTreeDecomposition atd  = extractCounterExampleTerm(state);
                cout<<"=======ABSTRACT TREE========="<<endl;
                atd.printTermNodes();
                atd.writeToFile(this->getPropertyFilePath());
                cout<<"=======Concrete TREE========="<<endl;
                ConcreteTreeDecomposition ctd = atd.convertToConcreteTreeDecomposition();
                ctd.printTermNodes();
                ctd.writeToFile(this->getPropertyFilePath());
                RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = extractCounterExampleRun(state);
                runTree.writeToFile(this->getPropertyFilePath());
                if(flags->get("StateTree")==1){
                cout << "=======Concrete TREE=========" << endl;
                runTree.printTermNodes();
              }
              cout << "\n ------------------Constructing Counter Example Graph-------------------"<< endl;
              MultiGraph multiGraph = ctd.extractMultiGraph();
              multiGraph.printGraph();
              multiGraph.printToFile(this->getPropertyFilePath());
              multiGraph.convertToGML(this->getPropertyFilePath());
              multiGraph.printToFilePACEFormat(this->getPropertyFilePath());*/
              cout<<"Conjecture: Not Satisfied"<<endl;
              return;
            }
          }
          set<State::ptr> setUnion;
          set_union(allStatesSet.begin(),allStatesSet.end(),newStatesSet.begin(),newStatesSet.end(),inserter(setUnion,setUnion.begin()));
          allStatesSet = setUnion;
          setUnion.clear();
          if(flags->get("LoopTime") == 1){
            cout<<left<<setw(25)<<iterationNumber<<setw(25)<<allStatesSet.size()<<setw(25)<<newStatesSet.size();
            for (int component = 0; component < numberOfWitnesses.size() ; ++component) {
              cout<< numberOfWitnesses[component];
              if(component != numberOfWitnesses.size()-1)
              cout<<",";
            }
            cout << endl;

          }
        }
        cout<<"Conjecture: Satisfied"<<endl;
      }
RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
BreadthFirstSearch::extractCounterExampleRun(State::ptr state) {
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = bfsDAG.retrieveRunAcyclicAutomaton(state);
    return runTree;
}
