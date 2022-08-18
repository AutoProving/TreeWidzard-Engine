// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "RandomLeveledSetSearchConnected.h"
extern "C" {
RandomLeveledSetSearchConnected * create() {
    return new RandomLeveledSetSearchConnected();
}
RandomLeveledSetSearchConnected * create_parameter(DynamicKernel* dynamicKernel, Conjecture* conjecture,Flags* flags) {
        return new RandomLeveledSetSearchConnected(dynamicKernel,conjecture,flags);
    }
}
RandomLeveledSetSearchConnected::RandomLeveledSetSearchConnected() {
    addAttribute("SearchName","RandomLeveledSetSearchConnected");
}
RandomLeveledSetSearchConnected::RandomLeveledSetSearchConnected(DynamicKernel *dynamicKernel, Conjecture *conjecture, Flags *flags) : SearchStrategy(dynamicKernel, conjecture, flags) {
    addAttribute("SearchName","RandomLeveledSetSearchConnected");
    this->kernel = kernel;
    this->conjecture = conjecture;
    this->flags = flags;
}
unsigned RandomLeveledSetSearchConnected::bagSetToNumber(set<unsigned> bagSet,unsigned width){
    unsigned number = 0;
    for (unsigned i = 1; i<= width+1; i++){
        if (bagSet.find(i)!=bagSet.end()){
            number = number + pow(2,i-1);
        }
    }
    return number;
}
shared_ptr<TermNode<ConcreteNode> > RandomLeveledSetSearchConnected::extractCTDNode( unsigned index){
    shared_ptr<TermNode <ConcreteNode > >  node (new TermNode<ConcreteNode>);
    ConcreteNode concrete;
    concrete.setBag(generatedVector[index].first->get_bag());
    node->setParent(nullptr);

    concrete.setSymbol(generatedVector[index].second);
    node->setNodeContent(concrete);
    if(index > 0 and  generatedVector[index].second !="Leaf"){
        shared_ptr<TermNode<ConcreteNode> > child = extractCTDNode(index-1);
        child->setParent(node);
        node->addChild(child);
    }
    return node;
}
ConcreteTreeDecomposition RandomLeveledSetSearchConnected::extractCTDDecomposition(){
    ConcreteTreeDecomposition T;
    shared_ptr<TermNode<ConcreteNode>> root = extractCTDNode(generatedVector.size()-1);
    T.setRoot(root);
    return T;
}

//shared_ptr<StateTreeNode> RandomLeveledSetSearchConnected::extractStateTreeNode(unsigned index){
//    shared_ptr<StateTreeNode> node (new StateTreeNode);
//    node->set_S(generatedVector[index].first);
//    node->set_parent(nullptr);
//    shared_ptr<DynamicKernel> sharedKernel(kernel);
//    node->set_kernel(sharedKernel);
//    node->set_nodeType(generatedVector[index].second);
//    if(index>0)
//    {
//        shared_ptr<StateTreeNode> child = extractStateTreeNode(index-1);
//        child->set_parent(node);
//        vector<shared_ptr<StateTreeNode>> children;
//        children.push_back(child);
//        node->set_children(children);
//    }
//    return node;
//}
//
//StateTree RandomLeveledSetSearchConnected::extractStateTreeDecomposition(){
//
//    StateTree stateTree;
//    stateTree.root = extractStateTreeNode(generatedVector.size()-1);
//    return stateTree;
//
//}

void RandomLeveledSetSearchConnected::search(){
    
    // Default Values	
    unsigned seedValue = time(NULL);
    int numberOfIterations = 1000; 
    float probAddVertex = 0.3; // Test if probAddVertex  + probForgetVertex <= 1.0
    float probForgetVertex = 0.2; 
    if(flags->get("seedValue")!=-1){
        seedValue = flags->get("seedValue");
    }
    if(flags->get("numberOfIterations")!=-1){
        numberOfIterations = flags->get("numberOfIterations");
    }
    if(flags->get("probAddVertex")!=-1){
        probAddVertex = flags->get("probAddVertex");
    }

    if(flags->get("probForgetVertex")!=-1){
        probForgetVertex = flags->get("probForgetVertex");
    }
    float probBacktrack = 1 - (probAddVertex + probForgetVertex); // Note: probAddVertex + probForgetVertex + probBacktrack = 1.0

    cout << " seedValue: "<<seedValue<<" probAddVertex: "<<
        probAddVertex<<" probForgetVertex: "<<probForgetVertex<<" probBacktrack: "<<probBacktrack << " number of iterations: "<< numberOfIterations <<endl;
    if(flags->get("Premise")){
        cout<<"Premise search active!"<<endl;
    }

    srand(seedValue); 
    // Add Initial state to vector
    generatedVector.push_back(make_pair(kernel->initialState(),"Leaf")); // This vector records a path decomposition + states
    vector<int> offsetVector; // Instructions are added in blocks. For instance: adding a vertex together with edges. This vector records
    			// how many instructions were added at each block. So if we backtrack we remove the block. 
    bool foundCounterexample = false;
    unsigned iterationNumber = 1;
    int counter = 0;
    int offset = 0;
    // ADD first vertex
    State::ptr s;
    s = kernel->intro_v(kernel->initialState(),1); // insert the vertex
    string typeState = "IntroVertex_" + to_string(1);
    auto it = mapState.find(s);
    if(it != mapState.end()){
	    if(flags->get("PrintStates")==1){
            	cout<<"Duplicate State."<<endl;
	    }
    }else{
    	generatedVector.push_back(make_pair(s,typeState));
	mapState.insert({s,generatedVector.size()-1});
	foundCounterexample = !(conjecture->evaluateConjectureOnState(*s));
    }
    while (!foundCounterexample){
	    counter++;
        State::ptr q = generatedVector.back().first;
	    string typeState = generatedVector.back().second;
	    set<unsigned> currentBagSet = q->get_bag().get_elements();
        if(flags->get("PrintStates")==1){
            cout<<"-----Currently Processing State------"<<endl;
            q->print();
	        cout<<" ------------------------------------"<<endl;
        }
	    float option = (rand() % 10000)/(10000.0);
	    //cout << "option: " <<  option << endl;
	
        if(flags->get("PrintStates")==1){
		    cout << "Option: " << option << endl;
	    }
        if (option <= probAddVertex and (currentBagSet.size()< kernel->get_width().get_value())){
		    //Add a vertex and random edges (at least 1)
		    //STEP 1: Select the first label not in the set and push it to the stack
		    int el = 1;
		    for (auto x:currentBagSet){
			    if (x==el){
				    el++;
			    }else{
				    break;
			    }
		    } // Result el is the smallest element not in the bag.
		    s = kernel->intro_v(q,el); // insert the vertex
            typeState = "IntroVertex_"+to_string(el);
		    if(flags->get("PrintStates")==1){
			    cout << "Last Introduced Vertex" << el << endl;
			    cout << "Generated State" << endl;
			    s.print();
			    cout << "----------";
		    }
        	generatedVector.push_back({s,typeState});
			offset=1; 
			foundCounterexample = false; // we will only test the conjecture after adding an edge.
			set<unsigned> oldBagSet = currentBagSet; // The bag with out the introduced vertex
		    if (!foundCounterexample){
			    //STEP 2: Select at random the number of edges to be added
			    //cout << "Position B" << endl;
			    int maxDegree = oldBagSet.size(); // add this as a parameter
			    int maxNElements = oldBagSet.size();
			    if (maxNElements > maxDegree) maxNElements = maxDegree;
			    int nElements = (rand() % maxNElements) + 1; //number of edges to be added
		       	//STEP 3: Construct a random subset 

			    //cout << "Position C " << endl;
			    set<unsigned> randomSubset = oldBagSet;
			    int sizeOldBag = randomSubset.size();
	   		    for (int i=0; i< sizeOldBag-nElements; i++ ){
				    int x = rand() % randomSubset.size();
				    randomSubset.erase(next(randomSubset.begin(),x));
			    }
			    //STEP 4: Introduce the edges
			    for (auto y:randomSubset){
				    int a = std::min(el,int(y));
				    int b = std::max(el,int(y));
                    s = kernel->intro_e(s,a,b);
                    typeState = "IntroEdge_"+to_string(a)+"_"+ to_string(b);
                    generatedVector.push_back({s,typeState});
                    if(flags->get("PrintStates")==1){
			            cout<<"-----Generated State------"<<endl;
				        cout<<"------Added Edge a:" << a  << " b:" << b << endl;
		        	    s->print();
				        cout <<"------------------" << endl;
                    }
				    offset++;
				    foundCounterexample = !(conjecture->evaluateConjectureOnState(*s));
				    if (foundCounterexample) break;
			    }
		    }
		    offsetVector.push_back(offset);
            if(flags->get("Premise") ){
                if(!conjecture->evaluatePremiseOnState(*s,kernel)){
                    if(flags->get("PrintStates")==1){
                        cout<<"Duplicate State."<<endl;
                    }else{
                        offset = offsetVector[offsetVector.size()-1];
                        offsetVector.pop_back();
                        pair<State::ptr,string>  deletedState = generatedVector[generatedVector.size()-1];
                        mapState.erase(deletedState.first); // Obs: only the last state of the block is deleted/added to mapState
                        generatedVector.erase(prev(generatedVector.end(),offset), generatedVector.end()); // deletes the block
                    }
                }
            }
            auto it = mapState.find(s); // This tries to determine if the state has been added before, and if yes we will delete it.
            // Note that if the premise flag is activated and s does not satisfy the premise, then s has already been removed and no state will be found.
            if(it != mapState.end()){
                if(flags->get("PrintStates")==1){
                    cout<<"Duplicate State."<<endl;
                }else{
                    offset = offsetVector[offsetVector.size()-1];
                    offsetVector.pop_back();
                    pair<State::ptr,string>  deletedState = generatedVector[generatedVector.size()-1];
                    mapState.erase(deletedState.first); // Obs: only the last state of the block is deleted/added to mapState
                    generatedVector.erase(prev(generatedVector.end(),offset), generatedVector.end()); // deletes the block
                }
            }
	    } else if (option <= probAddVertex + probForgetVertex){
		    //Forget a random vertex
            if(currentBagSet.size() >=2 ){
                unsigned toDeletePos = rand() % currentBagSet.size();
                unsigned toDelete = *(next(currentBagSet.begin(),toDeletePos));
                State::ptr s;
                s = kernel->forget_v(q,toDelete);
                typeState = "ForgetVertex_"+to_string(toDelete);
                if(flags->get("Premise")){
                    if(conjecture->evaluatePremiseOnState(*s,kernel)){
                        auto it = mapState.find(s);
                        if(it != mapState.end()){
                            if(flags->get("PrintStates")==1){
                                cout<<"Duplicate State."<<endl;
                            }
                        }else{
                            generatedVector.push_back({s,typeState});
                            mapState.insert({s,generatedVector.size()-1});
                            if(flags->get("PrintStates")==1){
                                cout<<"-----Generated State: Forgot: " << toDelete << " ------ "<<endl;
                                s->print();
                                cout << "--------" << endl;
                            }
                            offset = 1;
                            offsetVector.push_back(offset);
                            foundCounterexample = !(conjecture->evaluateConjectureOnState(*s));
                        }
                    }
                }
            }
        } else { // option = 2 or 3, then backtrack
            // Backtrack. Delete the last added block
            if (generatedVector.size() >= 3){ // obs: the first position is the initial state. The second the state with one vertex
                offset = offsetVector[offsetVector.size()-1];
                offsetVector.pop_back();
                pair<State::ptr,string>  deletedState = generatedVector[generatedVector.size()-1];
                        mapState.erase(deletedState.first); // Obs: only the last state of the block is deleted/added to mapState
                generatedVector.erase(prev(generatedVector.end(),offset), generatedVector.end()); // deletes the block
                        //s = generatedVector[generatedVector.size()-1].first;
                        //typeState = generatedVector[generatedVector.size()-1].second;
            }
	    }
	    if(foundCounterexample){
            cout<<"COUNTER EXAMPLE FOUND"<<endl;
            s->print();
            ConcreteTreeDecomposition T = extractCTDDecomposition();
            cout<<"=======Concrete TREE========="<<endl;
            T.printTermNodes();
            T.writeToFile(this->getPropertyFilePath());
            cout<<"=======ABSTRACT TREE========="<<endl;
            cout << "\n ------------------Constructing Counter Example Graph-------------------"<< endl;
            MultiGraph multiGraph = T.extractMultiGraph();
            multiGraph.printGraph();
            multiGraph.printToFile(this->getPropertyFilePath());
            multiGraph.printToFilePACEFormat(this->getPropertyFilePath());
            multiGraph.convertToGML(this->getPropertyFilePath());
            exit(20);
        }
        if(flags->get("PrintVectors")==1){
            cout<<"*******generated vector print"<<endl;
            for (unsigned i = 1; i < generatedVector.size(); ++i) {
                auto tempMapItr = mapState.find(generatedVector[i].first);
                cout<<"target state"<<endl;
                generatedVector[i].first->print();
                cout<<"First:"<<endl;
                tempMapItr->first->print();
                cout<<"Second:"<<endl;
                (generatedVector[tempMapItr->second]).first->print();
            }
            cout<<"*******"<<endl;
        }
        if(flags->get("LoopTime")==1 and counter == numberOfIterations){
            counter=0; // Reset the counter
            cout<<"----------Vector Size:"<<generatedVector.size()<<"---- Iteration number:"<< iterationNumber <<endl;
        }
        iterationNumber++;
    }
}
