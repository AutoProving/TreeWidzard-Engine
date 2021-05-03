#include "ParseController.h"
ParseController::ParseController(const Flags &flag, const string &inputPath) : flag(flag) {
    //string file_path = __FILE__;
    string file_path = "";
    string path = file_path.substr(0, file_path.rfind("/"));
    string dynamicPluginPath =path+"../DynamicPlugins/";
    inputController = new InputController(inputPath, dynamicPluginPath);
}

void ParseController::parse_pace(string graphPath, string decompositionPath) {
    shared_ptr<MultiGraph> multigraph(new MultiGraph);
    gr_in = fopen(graphPath.c_str(),"r");
    if(!gr_in) {
        std::perror("Reading Graph: File opening failed. ");
        cout<<graphPath<< " could not open."<<endl;
        exit(20);
    }
    int result = 1; // if parsing successful result will be 0 otherwise 1
    result = gr_parse(*multigraph,  result); // Parser function from Parser.hpp
    // check for successful parsing
    if(result != 0){
        cout<<" Error: input file "<< graphPath<< " is not in valid format"<<endl;
        exit(20);
    }
    multigraph->printGraph();
    TreeDecompositionPACE td;
    td.multigraph= multigraph;
    td_in = fopen(decompositionPath.c_str(),"r");
    if(!td_in) {
        std::perror("Reading Tree Decomposition: File opening failed");
        cout<<decompositionPath<< " could not open."<<endl;
        exit(20);
    }
    result = 1; // if parsing successful result will be 0 otherwise 1
    result= td_parse(td,  result); // Parser function from Parser.hpp
    // check for successful parsing
    if(result != 0){
        cout<<" Error: input file "<< decompositionPath<< " is not in valid format"<<endl;
        exit(20);
    }

    /// Value and type of width should be same in tree decomposition and input's file
//    if(td.getWidthType()!= inputController->getDynamicKernel().get_width().get_name()){
//        cerr<<"Error: width types in tree decomposition and input's file.";
//        cerr<<" tree decomposition width type: "<< td.getWidthType()<< ", input width type: "
//        <<inputController->getDynamicKernel().get_width().get_name()<<endl;
//        exit(20);
//    }else if(td.getWidth() != inputController->getDynamicKernel().get_width().get_value()){
//        cerr<<"Error: width value in tree decomposition and input's file.";
//        cerr<<" tree decomposition width value: "<< td.getWidth()<< ", input width value: "
//            <<inputController->getDynamicKernel().get_width().get_value()<<endl;
//        exit(20);
//    }
    /////////////////////////////////////////////////////////////////////////////////
    cout<<"Tree Decomposition "<<endl;
    td.construct();
    td.printTree();
    cout<<"--------------------------------------------Concrete decomposition"<<endl;
    shared_ptr<ConcreteTreeDecomposition> concreteTreeDecomposition;
    concreteTreeDecomposition = td.convertToConcreteTreeDecomposition();
    //concreteTreeDecomposition->printTermNodes();
    cout<<"----Evaluating-----:"<<endl;
    concreteTreeDecomposition->conjectureCheck(this->inputController->getConjecture());
    AbstractTreeDecomposition abstractTreeDecomposition = concreteTreeDecomposition->convertToAbstractTreeDecomposition();
    abstractTreeDecomposition.writeToFile(this->inputController->getInputPath());
    concreteTreeDecomposition->writeToFile(this->inputController->getInputPath());

    //cout<<"---------------------------------------------State Tree"<<endl;
    //concreteTreeDecomposition->conjectureCheck(inputController->getConjecture());
    //WitnessTreePACE witnessTreePace;
    //witnessTreePace.stateTreeToWitnessTreePACE(stateTree, inputController->getDynamicKernel());
    //witnessTreePace.print();
}

void ParseController::parse_abstract(string abstractPath) {
  /*  ctd_in = fopen(abstractPath.c_str(), "r");
    if (!ctd_in) {
        std::perror("File opening failed");
        cerr<<"\n "<<abstractPath<<" could not open."<<endl;
        exit(20);
    }
    ConcreteTreeDecomposition concreteTreeDecomposition;
    int resultCTD = 0; // if parsing successful result will be 0 otherwise 1
    resultCTD = ctd_parse(concreteTreeDecomposition, resultCTD); // Parser function from Parser.hpp
    cout<<"result: "<<resultCTD<<endl;
    // check for successful parsing
    if (resultCTD != 0) {
        cout << " Error: input file " << abstractPath << " is not in valid format"
             << endl;
        exit(20);
    }
    concreteTreeDecomposition.printTermNodes();
    concreteTreeDecomposition.conjectureCheck(this->inputController->getConjecture());
   */
    atd_in = fopen(abstractPath.c_str(), "r");
    if (!atd_in) {
        std::perror("File opening failed");
        cerr<<"\n "<<abstractPath<<" could not open."<<endl;
        exit(20);
    }
    AbstractTreeDecomposition abstractTreeDecomposition;
    int resultATD = 0; // if parsing successful result will be 0 otherwise 1
    resultATD = atd_parse(abstractTreeDecomposition, resultATD); // Parser function from Parser.hpp
    cout<<"result: "<<resultATD<<endl;
    // check for successful parsing
    if (resultATD != 0) {
        cout << " Error: input file " << abstractPath << " is not in valid format"
             << endl;
        exit(20);
    }
    abstractTreeDecomposition.printTermNodes();
    ConcreteTreeDecomposition concreteTreeDecomposition = abstractTreeDecomposition.convertToConcreteTreeDecomposition();
    
    cout<<"----Evaluating-----:"<<endl;
    concreteTreeDecomposition.conjectureCheck(this->inputController->getConjecture());
    /*if(!ctd.conjectureCheck(inputController->getConjecture())){
      cout<<"not satisfied"<<endl;
        //  shared_ptr<DynamicKernel> sharedKernel = make_shared<DynamicKernel>(inputController->getDynamicKernel());
      //  ctd.convertToStateTree(sharedKernel).printStateTree();
      //  ctd.extractMultiGraph().printGraph();
    };*/

}

void ParseController::test_term() {

//    AbstractTreeDecompositionNodeContent abstractTreeDecompositionNodeContent1("IntroVertex_13456");
//    AbstractTreeDecompositionNodeContent abstractTreeDecompositionNodeContent2("IntroVertex_7_9");
//    ConcreteTreeDecomposition abstractTreeDecomposition;
//    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> a1(new TermNode<AbstractTreeDecompositionNodeContent>);
//    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> a2(new TermNode<AbstractTreeDecompositionNodeContent>);
//    a1->setNodeContent(abstractTreeDecompositionNodeContent1);
//    a2->setNodeContent(abstractTreeDecompositionNodeContent2);
//    vector<shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> > children;
//    children.push_back(a2);
//    a2->setParent(a1);
//    a1->setChildren(children);
//    abstractTreeDecomposition.setRoot(a1);
//    abstractTreeDecomposition.printTermNodes();
//    TreeAutomaton<FarhadState,AbstractTreeDecompositionNodeContent> treeAutomaton;

}

