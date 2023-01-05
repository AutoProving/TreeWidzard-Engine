#include "BreadthFirstSearch.h"

#include <utility>

extern "C" {
BreadthFirstSearch* create() { return new BreadthFirstSearch(); }
BreadthFirstSearch* create_parameter(DynamicKernel* dynamicKernel,
                                     Conjecture* conjecture, Flags* flags) {
  return new BreadthFirstSearch(dynamicKernel, conjecture, flags);
}
}

BreadthFirstSearch::BreadthFirstSearch() {
  addAttribute("SearchName", "BreadthFirstSearch");
}

BreadthFirstSearch::BreadthFirstSearch(DynamicKernel* dynamicKernel,
                                       Conjecture* conjecture, Flags* flags)
    : SearchStrategy(dynamicKernel, conjecture, flags) {
  this->kernel = kernel;
  this->conjecture = conjecture;
  this->flags = flags;
  addAttribute("SearchName", "BreadthFirstSearch");
  this->noBFSDAG = flags->get("NoBFSDAG");
}

AbstractTreeDecomposition BreadthFirstSearch::extractCounterExampleTerm(
    State::ptr state) {
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
  extractCounterExampleStateTreeNode(state, root);
  stateTree.root = root;
  return stateTree;
}

void BreadthFirstSearch::extractCounterExampleStateTreeNode(
    State::ptr state, shared_ptr<StateTreeNode> node) {
  // Assumes that the automaton is acyclic and that each state has a transition
  // in which the state is the consequent
  if (!bfsDAG.getTransitions().empty()) {
    AbstractTreeDecompositionNodeContent a;
    a.setSymbol(
        a.smallestContent());  // Creates a symbol of type TermNodeContent and
                               // set it to the smallest symbol
    vector<State::ptr> emptyAntecedents;
    Transition<State::ptr, AbstractTreeDecompositionNodeContent> t(
        state, a, emptyAntecedents);  // This is the smallest transition with a
                                      // consequent equal to state
    auto it = bfsDAG.getTransitions().upper_bound(t);
    if (it != bfsDAG.getTransitions().begin()) {
      it--;  // This is always defined, since the transition set is non-empty
    }
    auto itAux = it;
    if (itAux->getConsequentState() != state) {
      itAux++;
      if (itAux != bfsDAG.getTransitions().end()) {
        if (itAux->getConsequentState() != state) {
          std::cout << "Error: No transition with consequent equal to the "
                       "input state.";
          exit(20);
        }
      } else {
        std::cout
            << "Error: No transition with consequent equal to the input state.";
        exit(20);
      }
    }
    node->set_nodeType(itAux->getTransitionContent().getSymbol());
    node->set_S(state);
    vector<shared_ptr<StateTreeNode>> children;
    for (int i = 0; i < itAux->getAntecedentStates().size(); i++) {
      shared_ptr<StateTreeNode> child(new StateTreeNode);
      child->set_parent(node);
      child->set_kernel(node->get_kernel());
      children.push_back(child);
      extractCounterExampleStateTreeNode(itAux->getAntecedentStates()[i],
                                         child);
    }
    node->set_children(children);
  } else {
    std::cout << "Error: The automaton has no transitions." << std::endl;
    exit(20);
  }
}

void BreadthFirstSearch::search() {
  if (flags->get("Premise")) {
    std::cout << " Premise is ACTIVATED" << std::endl;
  } else {
    std::cout << "Premise is NOT ACTIVATED" << std::endl;
  }
  bool printStateFlag = flags->get("PrintStates");
  State::ptr initialState = kernel->initialState();
  allStatesSet.insert(initialState);
  newStatesSet.insert(initialState);
  if (!noBFSDAG) {
    // Initialize the DAG
    bfsDAG.addState(initialState);
    AbstractTreeDecompositionNodeContent initialTransitionContent("Leaf");
    vector<State::ptr>
        initialAntecedents;  // Empty vector since there are no children.
    Transition<State::ptr, AbstractTreeDecompositionNodeContent>
        initialTransition(initialState, initialTransitionContent,
                          initialAntecedents);
    bfsDAG.addTransition(initialTransition);
    ////////////////////////////////////
  }
  unsigned int width = kernel->get_width().get_value();
  vector<unsigned> numberOfWitnesses;
  numberOfWitnesses.resize(initialState->numberOfComponents());
  int iterationNumber = 0;
  std::cout << left << setw(25) << "Iteration" << setw(25) << "ALLSTATES"
            << setw(25) << "NEWSTATES"
            << "Max WITNESSSET SIZE" << std::endl;
  while (!newStatesSet.empty()) {
    iterationNumber++;
    ////////////////////////////////////////////////////////////////////////////////////
    newStatesVector
        .clear();  // clear newStatesVector to add states in newStatesSet in it
    newStatesVector.resize(newStatesSet.size());  //
    std::copy(newStatesSet.begin(), newStatesSet.end(),
              newStatesVector.begin());
    newStatesSet.clear();  // clear newStatesSet to add new states that are
                           // generated in this loop
    // This loop is suitable for parallelization
    for (size_t l = 0; l < newStatesVector.size(); l++) {
      State::ptr statePointer = newStatesVector[l];
      Bag bag = statePointer->get_bag();
      set<unsigned> bagElement = bag.get_elements();
      ///////////////////////////////////////////////////////
      //////////////////// Introduce Vertex /////////////////
      ///////////////////////////////////////////////////////
      // the +1 below comes from the fact that treewidth is
      // size of the bag minus one. So the loop iterates
      // from 1 to number of elements in the  bag.
      for (int i = 1; i <= width + 1; i++) {
        if (bag.vertex_introducible(i)) {
          State::ptr newStatePointer = kernel->intro_v(statePointer, i);
          bool premiseFlag = flags->get("Premise");
          bool satisfiesPremise = false;
          if (premiseFlag) {
            satisfiesPremise =
                conjecture->evaluatePremiseOnState(*newStatePointer);
          }
          if (!premiseFlag or (premiseFlag and satisfiesPremise)) {
            if (!allStatesSet.count(newStatePointer) and
                !newStatesSet.count(newStatePointer)) {
              newStatesSet.insert(newStatePointer);
              State::ptr consequentState = newStatePointer;
              if (!noBFSDAG) {
                bfsDAG.addState(consequentState);
                AbstractTreeDecompositionNodeContent transitionContent(
                    "IntroVertex_" + to_string(i));
                vector<State::ptr> antecedentStates;
                antecedentStates.push_back(statePointer);
                Transition<State::ptr, AbstractTreeDecompositionNodeContent>
                    transition(consequentState, transitionContent,
                               antecedentStates);
                bfsDAG.addTransition(transition);
              }
              if (printStateFlag) {
                std::cout << std::endl;
                std::cout << "================================================="
                             "======================="
                          << std::endl;
                std::cout << " Introduce Vertex: " << i << std::endl;
                std::cout << "================================================="
                             "======================="
                          << std::endl;
                std::cout << " Current State:" << std::endl;
                statePointer->print();
                std::cout << " New State:" << std::endl;
                newStatePointer->print();
                std::cout << "================================================="
                             "======================="
                          << std::endl;
                std::cout << std::endl;
              }
              // size of witnessSets
              for (int component = 0; component < numberOfWitnesses.size();
                   ++component) {
                numberOfWitnesses[component] =
                    max(numberOfWitnesses[component],
                        (unsigned)consequentState->getWitnessSet(component)
                            ->size());
              }
            }
          }
        }
      }
      ///////////////////////////////////////////////////////
      //////////////////// Forget Vertex ////////////////////
      ///////////////////////////////////////////////////////
      for (auto it = bagElement.begin(); it != bagElement.end(); it++) {
        State::ptr newStatePointer = kernel->forget_v(statePointer, *it);
        bool premiseFlag = flags->get("Premise");
        bool satisfiesPremise = false;
        if (premiseFlag) {
          satisfiesPremise =
              conjecture->evaluatePremiseOnState(*newStatePointer);
        }
        if (!premiseFlag or (premiseFlag and satisfiesPremise)) {
          if (!allStatesSet.count(newStatePointer) and
              !newStatesSet.count(newStatePointer)) {
            newStatesSet.insert(newStatePointer);
            State::ptr consequentState = newStatePointer;
            if (!noBFSDAG) {
              AbstractTreeDecompositionNodeContent transitionContent(
                  "ForgetVertex_" + to_string(*it));
              bfsDAG.addState(consequentState);
              vector<State::ptr> antecedentStates;
              antecedentStates.push_back(statePointer);
              Transition<State::ptr, AbstractTreeDecompositionNodeContent>
                  transition(consequentState, transitionContent,
                             antecedentStates);
              bfsDAG.addTransition(transition);
            }
            if (printStateFlag) {
              std::cout << std::endl;
              std::cout << "==================================================="
                           "====================="
                        << std::endl;
              std::cout << " Forget Vertex: " << *it << std::endl;
              std::cout << "==================================================="
                           "====================="
                        << std::endl;
              std::cout << " Current State:" << std::endl;
              statePointer->print();
              std::cout << " New State:" << std::endl;
              newStatePointer->print();
              std::cout << "==================================================="
                           "====================="
                        << std::endl;
              std::cout << std::endl;
            }
            // size of witnessSets
            for (int component = 0; component < numberOfWitnesses.size();
                 ++component) {
              numberOfWitnesses[component] = max(
                  numberOfWitnesses[component],
                  (unsigned)consequentState->getWitnessSet(component)->size());
            }
          }
        }
      }
      // Introduce Edge
      if (bag.get_elements().size() > 1) {
        for (auto it = bagElement.begin(); it != bagElement.end(); it++) {
          auto itX = it;
          itX++;  // TODO write this more elegantly
          if (itX != bagElement.end()) {
            for (auto itPrime = itX; itPrime != bagElement.end(); itPrime++) {
              State::ptr newStatePointer =
                  kernel->intro_e(statePointer, *it, *itPrime);
              bool premiseFlag = flags->get("Premise");
              bool satisfiesPremise = false;
              if (premiseFlag) {
                satisfiesPremise =
                    conjecture->evaluatePremiseOnState(*newStatePointer);
              }
              if (!premiseFlag or (premiseFlag and satisfiesPremise)) {
                if (!allStatesSet.count(newStatePointer) and
                    !newStatesSet.count(newStatePointer)) {
                  newStatesSet.insert(newStatePointer);
                  State::ptr consequentState = newStatePointer;
                  if (!noBFSDAG) {
                    AbstractTreeDecompositionNodeContent transitionContent(
                        "IntroEdge_" + to_string(*it) + "_" +
                        to_string(*itPrime));
                    bfsDAG.addState(consequentState);
                    vector<State::ptr> antecedentStates;
                    antecedentStates.push_back(statePointer);
                    Transition<State::ptr, AbstractTreeDecompositionNodeContent>
                        transition(consequentState, transitionContent,
                                   antecedentStates);
                    bfsDAG.addTransition(transition);
                  }
                  if (printStateFlag) {
                    std::cout << std::endl;
                    std::cout << "============================================="
                                 "==========================="
                              << std::endl;
                    std::cout << " Introduce Edge: " << *it << " " << *itPrime
                              << std::endl;
                    std::cout << "============================================="
                                 "==========================="
                              << std::endl;
                    std::cout << " Current State:" << std::endl;
                    statePointer->print();
                    std::cout << " New State:" << std::endl;
                    newStatePointer->print();
                    std::cout << "============================================="
                                 "==========================="
                              << std::endl;
                    std::cout << std::endl;
                  }
                  // size of witnessSets
                  for (int component = 0; component < numberOfWitnesses.size();
                       ++component) {
                    numberOfWitnesses[component] =
                        max(numberOfWitnesses[component],
                            (unsigned)consequentState->getWitnessSet(component)
                                ->size());
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
            if (premiseFlag) {
              satisfiesPremise =
                  conjecture->evaluatePremiseOnState(*newStatePointer);
            }
            if (!premiseFlag or (premiseFlag and satisfiesPremise)) {
              if (!allStatesSet.count(newStatePointer) and
                  !newStatesSet.count(newStatePointer)) {
                newStatesSet.insert(newStatePointer);
                State::ptr consequentState = newStatePointer;
                if (!noBFSDAG) {
                  AbstractTreeDecompositionNodeContent transitionContent("Join");
                  bfsDAG.addState(consequentState);
                  vector<State::ptr> antecedentStates;
                  antecedentStates.push_back(statePointer);
                  antecedentStates.push_back(*it);
                  Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(consequentState, transitionContent,antecedentStates);
                  bfsDAG.addTransition(transition);
                }
                if (printStateFlag) {
                  std::cout << std::endl;
                  std::cout << "==============================================="
                               "========================="
                            << std::endl;
                  std::cout << " Join: " << std::endl;
                  std::cout << "==============================================="
                               "========================="
                            << std::endl;
                  std::cout << " State One:" << std::endl;
                  statePointer->print();
                  std::cout << " State Two:" << std::endl;
                  (*it)->print();
                  std::cout << " New State:" << std::endl;
                  newStatePointer->print();
                  std::cout << "==============================================="
                               "========================="
                            << std::endl;
                  std::cout << std::endl;
                }
                // size of witnessSets
                for (int component = 0; component < numberOfWitnesses.size();
                     ++component) {
                  numberOfWitnesses[component] =
                      max(numberOfWitnesses[component],
                          (unsigned)consequentState->getWitnessSet(component)
                              ->size());
                }
              }
            }
          }
        }
        //                for (auto it = newStatesSet.begin(); it !=
        //                newStatesSet.end(); it++) {
        //                    if
        //                    (statePointer->get_bag().joinable((*it)->get_bag()))
        //                    {
        //                        State::ptr newStatePointer =
        //                        kernel->join(statePointer, *it); if
        //                        (!allStatesSet.count(newStatePointer) and
        //                        !newStatesSet.count(newStatePointer)) {
        //                            newStatesSet.insert(newStatePointer);
        //                            State::ptr consequentState =
        //                            newStatePointer;
        //                            AbstractTreeDecompositionNodeContent
        //                            transitionContent("Join");
        //                            bfsDAG.addState(consequentState);
        //                            vector<State::ptr> antecedentStates;
        //                            antecedentStates.push_back(statePointer);
        //                            antecedentStates.push_back(*it);
        //                            Transition<State::ptr,
        //                            AbstractTreeDecompositionNodeContent>
        //                            transition(consequentState,
        //                                                                                                    transitionContent,
        //                                                                                                    antecedentStates);
        //                            bfsDAG.addTransition(transition);
        //                        }
        //                    }
        //                }
      }
    }
    for (auto it = newStatesSet.begin(); it != newStatesSet.end(); it++) {
      if (!conjecture->evaluateConjectureOnState(**it)) {
        std::cout << "Conjecture: Not Satisfied" << std::endl;
        
        if (noBFSDAG) {
          std::cerr<< "Rerun without -no-bfs-dag to construct a counter example."<< std::endl;
          return;
        }
        
        State::ptr badState = *it;
        /* printing the conjectures and values of the variables */
        std::cout<<"The assignment that makes the formula false:" <<std::endl;
        conjecture->printValues(*badState,conjecture->getRoot());
        std::cout<<std::endl;

        bfsDAG.addFinalState(badState);
        AbstractTreeDecomposition atd = extractCounterExampleTerm(badState);
        string file = this->getOutputsPath();
        if (flags->get("Premise")) {
          file += "_Premise";
        }
        file += "_CounterExample";
        ConcreteTreeDecomposition ctd = atd.convertToConcreteTreeDecomposition();
        RunTree<State::ptr, AbstractTreeDecompositionNodeContent> runTree = extractCounterExampleRun(badState);
        MultiGraph multiGraph = ctd.extractMultiGraph();
        multiGraph.printGraph();
        atd.writeToFile(file + "_AbstractDecomposition.txt");
        ctd.writeToFile(file + "_ConcreteDecomposition.txt");
        runTree.writeToFile(file + "_RunTree.txt");
        multiGraph.printToFile(file + "_Graph.txt");
        multiGraph.convertToGML(file + "_GMLGraph.gml");
        multiGraph.printToFilePACEFormat(file + "_GraphPaceFormat.gr");

        if (flags->get("PrintDirectedBipartiteGraphNAUTY")) {
          multiGraph.printToFileDirectedBipartiteGraphNAUTY(file + "_DirectedBipartiteGraphNAUTY.txt");
        }

        return;
      }
    }
    set<State::ptr> setUnion;
    std::set_union(allStatesSet.begin(), allStatesSet.end(),
                   newStatesSet.begin(), newStatesSet.end(),
                   inserter(setUnion, setUnion.begin()));
    allStatesSet = setUnion;
    setUnion.clear();
    if (flags->get("LoopTime") == 1) {
      std::cout << left << setw(25) << iterationNumber << setw(25)
                << allStatesSet.size() << setw(25) << newStatesSet.size();
      for (int component = 0; component < numberOfWitnesses.size();
           ++component) {
        std::cout << numberOfWitnesses[component];
        if (component != numberOfWitnesses.size() - 1) std::cout << ",";
      }
      std::cout << std::endl;
    }
  }
  std::cout << "Conjecture: Satisfied" << std::endl;
}
RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
BreadthFirstSearch::extractCounterExampleRun(State::ptr state) {
  RunTree<State::ptr, AbstractTreeDecompositionNodeContent> runTree =
      bfsDAG.retrieveRunAcyclicAutomaton(state);
  return runTree;
}
