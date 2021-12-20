// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "IsomorphismBreadthFirstSearch.h"

extern "C" {
IsomorphismBreadthFirstSearch * create() {
    return new IsomorphismBreadthFirstSearch();
}
IsomorphismBreadthFirstSearch * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
    return new IsomorphismBreadthFirstSearch(dynamicKernel,conjecture,flags);
}
}

IsomorphismBreadthFirstSearch::IsomorphismBreadthFirstSearch() {
    addAttribute("SearchName","IsomorphismBreadthFirstSearch");
}

IsomorphismBreadthFirstSearch::IsomorphismBreadthFirstSearch(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags): SearchStrategy(dynamicKernel, conjecture, flags) {
    this->kernel = kernel;
    this->conjecture = conjecture;
    this->flags = flags;
    addAttribute("SearchName","IsomorphismBreadthFirstSearch");
}

void IsomorphismBreadthFirstSearch::search(){
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
            // from 1 to number of elements inteh bag.
            for (int i=1; i<= width+1; i++){
                if(bag.vertex_introducible(i)){
                    State::ptr newStatePointer = kernel->intro_v(statePointer, i);
                    State::ptr relabeledNewStatePointer = canonicalState(newStatePointer); // Computes the canonical state derived from newStatePointer.
                    bool premiseFlag = flags->get("Premise");
                    bool satisfiesPremise = false;
                    if(premiseFlag){
                        satisfiesPremise = conjecture->evaluatePremiseOnState(*relabeledNewStatePointer);
                    }
                    if(!premiseFlag or (premiseFlag and satisfiesPremise) ){
                        if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer) ){
                            newStatesSet.insert(relabeledNewStatePointer);
                            State::ptr consequentState = relabeledNewStatePointer;
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
                                cout<< " New State (Not Relabeled):"<<endl;
                                newStatePointer->print();
                                cout<< "========================================================================" <<endl;
                                cout<< " New State (Relabeled):"<<endl;
                                relabeledNewStatePointer->print();
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
                State::ptr relabeledNewStatePointer = canonicalState(newStatePointer); // Computes the canonical state derived from newStatePointer.
                bool premiseFlag = flags->get("Premise");
                bool satisfiesPremise = false;
                if(premiseFlag){
                    satisfiesPremise = conjecture->evaluatePremiseOnState(*relabeledNewStatePointer);
                }
                if(!premiseFlag or (premiseFlag and satisfiesPremise) ) {
                    if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer)) {
                        newStatesSet.insert(relabeledNewStatePointer);
                        State::ptr consequentState = relabeledNewStatePointer;
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
                            cout<< " New State (Not Relabeled):"<<endl;
                            newStatePointer->print();
                            cout<< "========================================================================" <<endl;
                            cout<< " New State (Relabeled):"<<endl;
                            relabeledNewStatePointer->print();
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
                            State::ptr relabeledNewStatePointer = canonicalState(newStatePointer); // Computes the canonical state derived from newStatePointer.
                            bool premiseFlag = flags->get("Premise");
                            bool satisfiesPremise = false;
                            if(premiseFlag){
                                satisfiesPremise = conjecture->evaluatePremiseOnState(*relabeledNewStatePointer);
                            }
                            if(!premiseFlag or (premiseFlag and satisfiesPremise) ){
                                if (!allStatesSet.count(relabeledNewStatePointer) and !newStatesSet.count(relabeledNewStatePointer)){
                                    newStatesSet.insert(relabeledNewStatePointer);
                                    State::ptr consequentState = relabeledNewStatePointer;
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
                                        cout<< " New State (Not Relabeled):"<<endl;
                                        newStatePointer->print();
                                        cout<< "========================================================================" <<endl;
                                        cout<< " New State (Relabeled):"<<endl;
                                        relabeledNewStatePointer->print();
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
                        map<unsigned, unsigned> m = identityMap(statePointer->get_bag().get_elements());
                        do {
                            State::ptr relabeledState = (*it)->relabel(m);
                            State::ptr newStatePointer = kernel->join(statePointer, relabeledState);
                            State::ptr relabeledNewStatePointer = canonicalState(
                                    newStatePointer); // Computes the canonical state derived from newStatePointer.
                            bool premiseFlag = flags->get("Premise");
                            bool satisfiesPremise = false;
                            if (premiseFlag) {
                                satisfiesPremise = conjecture->evaluatePremiseOnState(*relabeledNewStatePointer);
                            }
                            if (!premiseFlag or (premiseFlag and satisfiesPremise)) {
                                if (!allStatesSet.count(relabeledNewStatePointer) and
                                    !newStatesSet.count(relabeledNewStatePointer)) {
                                    newStatesSet.insert(relabeledNewStatePointer);
                                    State::ptr consequentState = relabeledNewStatePointer;
                                    AbstractTreeDecompositionNodeContent transitionContent("Join");
                                    bfsDAG.addState(consequentState);
                                    vector<State::ptr> antecedentStates;
                                    antecedentStates.push_back(statePointer);
                                    antecedentStates.push_back(relabeledNewStatePointer);
                                    Transition<State::ptr, AbstractTreeDecompositionNodeContent> transition(
                                            consequentState,
                                            transitionContent,
                                            antecedentStates);
                                    bfsDAG.addTransition(transition);
                                    if (printStateFlag) {
                                        cout << endl;
                                        cout
                                                << "========================================================================"
                                                << endl;
                                        cout << " Join: " << endl;
                                        cout
                                                << "========================================================================"
                                                << endl;
                                        cout << " State One:" << endl;
                                        statePointer->print();
                                        cout << " State Two:" << endl;
                                        relabeledState->print();
                                        cout
                                                << "========================================================================"
                                                << endl;
                                        cout << " New State (Not Relabeled):" << endl;
                                        newStatePointer->print();
                                        cout
                                                << "========================================================================"
                                                << endl;
                                        cout << " New State (Relabeled):" << endl;
                                        relabeledNewStatePointer->print();
                                        cout
                                                << "========================================================================"
                                                << endl;
                                        cout << endl;
                                    }
                                    // size of witnessSets
                                    for (int component = 0; component < numberOfWitnesses.size(); ++component) {
                                        numberOfWitnesses[component] = max(numberOfWitnesses[component],
                                                                           (unsigned) consequentState->getWitnessSet(
                                                                                   component)->size());
                                    }

                                }
                            }
                        } while (nextPermutation(m));
                    }
                }
            }
        }
        for(auto it = newStatesSet.begin(); it!=newStatesSet.end(); it++){
            if(!conjecture->evaluateConjectureOnState(**it)){
                cout<<"Conjecture: Not Satisfied"<<endl;
                State::ptr badState = *it;
                bfsDAG.addFinalState(badState);
                string file = this->getOutputsPath();
                if (flags->get("Premise")) { file + "_Premise"; }
                file += "_CounterExample";

                RunTree<State::ptr,AbstractTreeDecompositionNodeContent> runTree = extractRunTree(*it);
                runTree.writeToFile(file + "_RunTree.txt");
                ///=======ABSTRACT TREE=========///
                Term<AbstractTreeDecompositionNodeContent>* term = new AbstractTreeDecomposition;
                *term = runTree.convertRunToTerm(runTree);
                AbstractTreeDecomposition* atd = static_cast<AbstractTreeDecomposition *>(term);
                atd->writeToFile(file + "_AbstractDecomposition.txt");
                ///=======Concrete TREE=========///
                ConcreteTreeDecomposition ctd = atd->convertToConcreteTreeDecomposition();
                ctd.writeToFile(file + "_ConcreteDecomposition.txt");
                MultiGraph multiGraph = ctd.extractMultiGraph();
                multiGraph.printGraph();
                multiGraph.printToFile(file + "_Graph.txt");
                multiGraph.convertToGML(file + "_GMLGraph.gml");
                multiGraph.printToFilePACEFormat(file + "_GraphPaceFormat.gr");
                if(flags->get("PrintDirectedBipartiteGraphNAUTY")){
                    multiGraph.printToFileDirectedBipartiteGraphNAUTY(file+"_DirectedBipartiteGraphNAUTY.txt");
                }
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

map<unsigned, unsigned> IsomorphismBreadthFirstSearch::relabeledMapGenerator(set<unsigned int> bagElements) {
    map<unsigned int, unsigned int> map;
    unsigned i =1;
    for(auto v:bagElements){
        map.insert(make_pair(v,i));
        i++;
    }
    return map;
}

map<unsigned int, unsigned int> IsomorphismBreadthFirstSearch::identityMap(unsigned int k) {
    map<unsigned , unsigned > m;
    for(unsigned index = 1; index <= k ; index++){
        m.insert(make_pair(index,index));
    }
    return m;
}

map<unsigned int, unsigned int> IsomorphismBreadthFirstSearch::identityMap(set<unsigned int> bagSet) {
    map<unsigned , unsigned > m;
    for(auto item:bagSet){
        m.insert(make_pair(item,item));
    }
    return m;
}

bool IsomorphismBreadthFirstSearch::nextPermutation(map<unsigned int, unsigned int> &permutation) {
    vector<unsigned > indexToValue;
    indexToValue.resize(permutation.size());
    unsigned counter = 0;
    for(auto item:permutation){
        indexToValue[counter] = item.second;
        counter++;
    }
    if(!next_permutation(indexToValue.begin(),indexToValue.end())){
        return false;
    }else{
        unsigned index= 0;
        for(auto& item:permutation){
            item.second = indexToValue[index];
            index++;
        }
        return true;
    }

}


State::ptr IsomorphismBreadthFirstSearch::canonicalState(State::ptr state) {
    // Assume that the bag.elements is equal to {1,...,k} for some k.
    State::ptr canonicalState = state;
    //map<unsigned ,unsigned > m = identityMap(state->get_bag().get_elements());
    map<unsigned ,unsigned > m;
    set<unsigned> bagElements = state->get_bag().get_elements();
    unsigned i =1;
    for(auto v:bagElements){
        m.insert(make_pair(v,i));
        i++;
    }
    do{

        State::ptr relabeledState = state->relabel(m);
        if(relabeledState < canonicalState){
            canonicalState = relabeledState;
        }

//        for(auto item: m){cout<< item.first << " -> " << item.second << ", "; }
//        cout <<"------------------------------------------------====" <<  endl;
////        cout << "canonicalState: " << endl;
////        canonicalState.print();

    }while(nextPermutation(m));
    return canonicalState;
}

void IsomorphismBreadthFirstSearch::extractRunTreeNode(
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> wrongRunNode,
        shared_ptr<TermNode<RunNodeContent<State::ptr, AbstractTreeDecompositionNodeContent>>> correctedRunNode,
        map<unsigned int, unsigned int> &m) {
    string wrongSymbol = wrongRunNode->getNodeContent().getRunNodeContent().getSymbol();
    ///////////////////////////////////////////////////////
//    cout<<wrongSymbol<<endl;
//    cout<<"Input Map"<<endl;
//    for(auto item:m){
//        cout<<item.first<<"->"<<item.second<<" ";
//    }
//    cout<<endl;
//    cout<<"Wrong node"<<endl;
//    wrongRunNode->getNodeContent().getState().print();
//    cout<<endl;
    //////////////////////////////////////////////////////
    if(strstr(wrongSymbol.c_str(),"Leaf")){
        if(m.empty()){
            State::ptr correctedState = wrongRunNode->getNodeContent().getState();
            AbstractTreeDecompositionNodeContent abs("Leaf");
            RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
            correctedRunNode->setNodeContent(correctedRunNodeContent);

        }else{
            cout<<"Error: In Empty node, map is not valid. "<<endl;
            exit(20);
        }
    }else if(strstr(wrongSymbol.c_str(),"IntroVertex")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        /////////////////////////////////////////////////////////////////////
//        cout<<"corrected state"<<endl;
//        correctedState.print();
//        cout<<endl;
//        cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
        /////////////////////////////////////////////////////////////////////

        // discovers IntroducedVertex and child's map;
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        unsigned introducedVertex = 0;
        map<unsigned ,unsigned > childMap;
        bool breakFor = false; // If true then vertex and map have been found
        for(auto v:setElements){
            set<unsigned > childDomain = setElements;
            childDomain.erase(v);
//            map<unsigned ,unsigned > initialChildMap = m;
//            for(auto item : initialChildMap){
//                if(item.second == v){
//                    initialChildMap.erase(item.first);
//                    break;
//                }
//            }

            map<unsigned ,unsigned > initialChildMap;
            bool afterV = false;
            for(auto item:m){
                if(item.second != v and afterV){
                    initialChildMap.insert(make_pair(item.first-1,item.second));
                }else if(item.second == v){
                    afterV = true;
                }else{
                    initialChildMap.insert(make_pair(item.first,item.second));
                }
            }
            map<unsigned ,unsigned > testMap  = identityMap(childDomain);
            do {
                map<unsigned,unsigned > compositionMap;
                for(auto item:initialChildMap){
                    compositionMap.insert(make_pair(item.first,testMap[item.second]));
                }
                State::ptr childState = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap);
                State::ptr testState = kernel->intro_v(childState,v);
                if(testState == correctedState){
//                    cout<<"child state"<<endl;
//                    childState.print();
//                    cout<<endl;
//                    cout<<"test state"<<endl;
//                    testState.print();
//                    cout<<endl;
//                    cout<<"THE MAP"<<endl;
//                    for(auto item:compositionMap){
//                        cout<<item.first<<"->"<<item.second<<" ";
//                    }
//                    cout<<"#####################################"<<endl;
                    ///////////////////////////////////////////////////////
                    introducedVertex = v;
                    childMap = compositionMap;
                    breakFor = true;
                    break;
                }
            }while(nextPermutation(testMap));
            if(breakFor){
                break;
            }
        }
        // Discover the child's label and which vertex was introduced.
        if(introducedVertex==0){
            cout<<"Error: child state is not consistent with the current state"<<endl;
            exit(20);
        }
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("IntroVertex_"+to_string(introducedVertex));

        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);

        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,childMap);

    }else if(strstr(wrongSymbol.c_str(),"ForgetVertex")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        /////////////////////////////////////////////////////////////////////
//        cout<<"corrected state"<<endl;
//        correctedState.print();
//        cout<<endl;
//        cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;

        /////////////////////////////////////////////////////////////////////
        // discovers ForgottenVertex and child's map;
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        unsigned forgottenVertex = 0;
        map<unsigned ,unsigned > childMap;
        bool breakFor = false; // If true then vertex and map have been found
        set<unsigned > allVertices;
        for(unsigned i = 1; i <= kernel->get_width().get_value()+1; i++){
            allVertices.insert(i);
        }
        set<unsigned > availableVertices;
        set_difference(allVertices.begin(),allVertices.end(),setElements.begin(),setElements.end(),inserter(availableVertices,availableVertices.begin()));

        for(auto v:availableVertices){
            set<unsigned > childDomain = setElements;
            childDomain.insert(v);
            map<unsigned ,unsigned > initialChildMap = m;
            // add v to initialChildMap
            initialChildMap.insert(make_pair(m.size()+1,v));

            map<unsigned ,unsigned > testMap  = identityMap(childDomain);
            do {
                map<unsigned,unsigned > compositionMap;
                for(auto item:initialChildMap){
                    compositionMap.insert(make_pair(item.first,testMap[item.second]));
                }
                State::ptr childState = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap);
                State::ptr testState = kernel->forget_v(childState,v);
                if(testState == correctedState){
//                    cout<<"child state"<<endl;
//                    childState.print();
//                    cout<<endl;
//                    cout<<"test state"<<endl;
//                    testState.print();
//                    cout<<endl;
//                    cout<<"THE MAP"<<endl;
//                    for(auto item:compositionMap){
//                        cout<<item.first<<"->"<<item.second<<" ";
//                    }
//                    cout<<"#####################################"<<endl;
                    ///////////////////////////////////////////////////////
                    forgottenVertex = v;
                    childMap = compositionMap;
                    breakFor = true;
                    break;
                }
            }while(nextPermutation(testMap));
            if(breakFor){
                break;
            }
        }
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("ForgetVertex_"+to_string(forgottenVertex));
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,childMap);
    }else if(strstr(wrongSymbol.c_str(),"IntroEdge")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        // discovers IntroducedEdge and child's map;
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        unsigned e_i = 0;
        unsigned e_j = 0;
        map<unsigned ,unsigned > childMap;
        bool breakFor = false; // If true then vertex and map have been found
        for(auto it = setElements.begin(); it!= setElements.end(); it++){
            auto itr = it;
            itr++;
            while(itr!=setElements.end()){
                set<unsigned > childDomain = setElements;
                map<unsigned ,unsigned > initialChildMap = m;
                map<unsigned ,unsigned > testMap  = identityMap(childDomain);
                do {
                    map<unsigned,unsigned > compositionMap;
                    for(auto item:initialChildMap){
                        compositionMap.insert(make_pair(item.first,testMap[item.second]));
                    }
                    State::ptr childState = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap);
                    State::ptr testState = kernel->intro_e(childState,*it,*itr);
                    if(testState == correctedState){
                        e_i=*it;
                        e_j=*itr;
                        childMap = compositionMap;
                        breakFor = true;
                        break;
                    }
                }while(nextPermutation(testMap));
                if(breakFor){
                    break;
                }
                itr++;
            }
            if(breakFor){
                break;
            }
        }
        if(!breakFor){
            cout<<"Error: in IntroEdge, there is not a consistent state"<<endl;
            exit(20);
        }
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("IntroEdge_"+to_string(e_i)+"_"+to_string(e_j));
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child->setParent(correctedRunNode);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        children.push_back(child);
        correctedRunNode->setChildren(children);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child,childMap);

    }else if(strstr(wrongSymbol.c_str(),"Join")){
        State::ptr correctedState = wrongRunNode->getNodeContent().getState()->relabel(m);
        set<unsigned > setElements = correctedState->get_bag().get_elements();
        AbstractTreeDecompositionNodeContent abs;
        abs.setSymbol("Join");
        RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunNodeContent(abs,correctedState);
        correctedRunNode->setNodeContent(correctedRunNodeContent);
        vector<shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >> children;
        // Child 1
        map<unsigned ,unsigned > initialChildMap1 = m;
        set<unsigned > childDomain1 = setElements;
        map<unsigned ,unsigned > testMap1  = identityMap(childDomain1);
        map<unsigned ,unsigned > childMap1;
        map<unsigned ,unsigned > childMap2;
        bool checkContinue = false;
        do {
            map<unsigned,unsigned > compositionMap1;
            for(auto item:initialChildMap1){
                compositionMap1.insert(make_pair(item.first,testMap1[item.second]));
            }
            State::ptr childState1 = wrongRunNode->getChildren()[0]->getNodeContent().getState()->relabel(compositionMap1);
            // Child 2
            map<unsigned ,unsigned > initialChildMap2 = m;
            set<unsigned > childDomain2 = setElements;
            map<unsigned ,unsigned > testMap2  = identityMap(childDomain2);
            do {
                map<unsigned,unsigned > compositionMap2;
                for(auto item:initialChildMap2){
                    compositionMap2.insert(make_pair(item.first,testMap2[item.second]));
                }
                State::ptr childState2 = wrongRunNode->getChildren()[1]->getNodeContent().getState()->relabel(compositionMap2);
                State::ptr testState = kernel->join(childState1,childState2);
                if(correctedState == testState){
                    childMap1 = compositionMap1;
                    childMap2 = compositionMap2;
                    checkContinue = true;
                    break;
                }
            } while (nextPermutation(testMap2));
            if(checkContinue){
                break;
            }
        }while(nextPermutation(testMap1));
        // child 1
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child1(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child1->setParent(correctedRunNode);
        children.push_back(child1);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child1,childMap1);
        // child 2
        shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent> > >
                child2(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
        child2->setParent(correctedRunNode);
        children.push_back(child2);
        extractRunTreeNode(wrongRunNode->getChildren()[0],child2,childMap2);
        correctedRunNode->setChildren(children);
    }else{
        cout<<"Error: Node Type is not valid."<<endl;
        exit(20);
    }

}

RunTree<State::ptr, AbstractTreeDecompositionNodeContent>
IsomorphismBreadthFirstSearch::extractRunTree(State::ptr state) {
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> wrongRunTree = bfsDAG.retrieveRunAcyclicAutomaton(state);
    RunTree<State::ptr,AbstractTreeDecompositionNodeContent> correctedRunTree;
    shared_ptr<TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>>
            correctedRoot(new TermNode<RunNodeContent<State::ptr,AbstractTreeDecompositionNodeContent>>);
    map<unsigned,unsigned> m = identityMap(state->get_bag().get_elements());
    extractRunTreeNode(wrongRunTree.getRoot(),correctedRoot,m);
    correctedRunTree.setRoot(correctedRoot);
    return correctedRunTree;
}
