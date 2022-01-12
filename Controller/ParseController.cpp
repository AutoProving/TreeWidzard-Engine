#include "ParseController.h"
ParseController::ParseController(const Flags &flag, const string &inputPath){
    this->flag = flag;
    string file_path = __FILE__;
    string path = file_path.substr(0, file_path.rfind("/"));
    string dynamicPluginPath =path+"/../DynamicPlugins/";
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

    string output_file_path = fs::path(inputController->getInputPath()).parent_path().string();
    string abstract_file_name = fs::path(decompositionPath).stem().string();
    string property_file_name = fs::path(inputController->getInputPath()).stem().string();
    string name = output_file_path+"/PARSE_PACE_"+property_file_name+"_"+abstract_file_name;
    TreeDecompositionPACE td;

    td.multigraph= multigraph;
    // multigraph->printToFile(name+"_GraphMAIN.txt");
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
    //    cout<< " tree decomposition read from input"<<endl;
    //    td.print();
    /////////////////////////////////////////////////////////////////////////////////
    td.construct();
    //    td.print();
    //    td.printTree();
    //   cout<<"-----------Concrete decomposition"<<endl;
    shared_ptr<ConcreteTreeDecomposition> concreteTreeDecomposition;
    concreteTreeDecomposition = td.convertToConcreteTreeDecomposition();
    //  concreteTreeDecomposition->printTermNodes();
    cout<<"----Evaluating-----:"<<endl;
    concreteTreeDecomposition->conjectureCheck(this->inputController->getConjecture(),flag, name);
    AbstractTreeDecomposition abstractTreeDecomposition = concreteTreeDecomposition->convertToAbstractTreeDecomposition();

    abstractTreeDecomposition.writeToFile(name+"_AbstractDecomposition.txt");
    concreteTreeDecomposition->writeToFile(name+"_ConcreteDecomposition.txt");
    MultiGraph multiGraph = concreteTreeDecomposition->extractMultiGraph();
    multiGraph.printToFile(name+"_Graph.txt");
    multiGraph.convertToGML(name+"_GMLGraph.gml");
    multiGraph.printToFilePACEFormat(name+"_GraphPaceFormat.gr");
    if(flag.get("PrintDirectedBipartiteGraphNAUTY")){
        multiGraph.printToFileDirectedBipartiteGraphNAUTY(name+"_DirectedBipartiteGraphNAUTY.txt");
    }
    Decomposition decomposition = concreteTreeDecomposition->extractDecomposition(); 
//    decomposition.print();
    decomposition.writeToFile(name+"_Decomposition.td");
    //cout<<"---------------------------------------------State Tree"<<endl;
    //concreteTreeDecomposition->conjectureCheck(inputController->getConjecture());
    //WitnessTreePACE witnessTreePace;
    //witnessTreePace.stateTreeToWitnessTreePACE(stateTree, inputController->getDynamicKernel());
    //witnessTreePace.print();
}

void ParseController::parse_abstract(string abstractPath) {
    atd_in = fopen(abstractPath.c_str(), "r");
    if (!atd_in) {
        std::perror("File opening failed");
        cerr<<"\n "<<abstractPath<<" could not open."<<endl;
        exit(20);
    }
    AbstractTreeDecomposition abstractTreeDecomposition;
    int resultATD = 0; // if parsing successful result will be 0 otherwise 1
    resultATD = atd_parse(abstractTreeDecomposition, resultATD); // Parser function from Parser.hpp
    // check for successful parsing
    if (resultATD != 0) {
        cout << " Error: input file " << abstractPath << " is not in valid format"
            << endl;
        exit(20);
    }
    ConcreteTreeDecomposition concreteTreeDecomposition = abstractTreeDecomposition.convertToConcreteTreeDecomposition();
    //concreteTreeDecomposition.printTermNodes();
    cout<<"----Evaluating-----:"<<endl;
    string output_file_path = fs::path(inputController->getInputPath()).parent_path().string();
    string abstract_file_name = fs::path(abstractPath).stem().string();
    string property_file_name = fs::path(inputController->getInputPath()).stem().string();
    string name = output_file_path+"/PARSE_ABSTRACT_"+property_file_name+"_"+abstract_file_name;
    concreteTreeDecomposition.conjectureCheck(this->inputController->getConjecture(), flag, name);
    concreteTreeDecomposition.writeToFile(name+"_ConcreteDecomposition.txt");
    MultiGraph multiGraph = concreteTreeDecomposition.extractMultiGraph();
    multiGraph.printToFile(name+"_Graph.txt");
    multiGraph.convertToGML(name+"_GMLGraph.gml");
    multiGraph.printToFilePACEFormat(name+"_GraphPaceFormat.gr");
    if(flag.get("PrintDirectedBipartiteGraphNAUTY")){
        multiGraph.printToFileDirectedBipartiteGraphNAUTY(name+"_DirectedBipartiteGraphNAUTY.txt");
    }
    Decomposition decomposition = concreteTreeDecomposition->extractDecomposition(); 
//    decomposition.print();
    decomposition.writeToFile(name+"_Decomposition.td"); 

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

