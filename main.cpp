// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include <stdio.h>
#include <iostream>
#include "DynamicCores/TriangleFreeWidthFour.h"
#include "DynamicCores/ChromaticNumber_AtMost.h"
#include "DynamicCores/MaxDegree_AtLeast.h"
#include "DynamicCores/HasMultipleEdges.h"
#include "Kernel/ConcreteTreeDecomposition.h"
#include "Parser/parser.hpp"
#include "Kernel/DynamicCoreHandler.h"
#include "Kernel/DynamicKernel.h"
#include "Kernel/Flags.h"
#include "Kernel/PropertyAssignment.h"
#include <experimental/filesystem>
#include "Kernel/SearchStrategyHandler.h"
#include "Translation/PACE/TreeDecompositionPACE.h"
#include "Translation/PACE/WitnessTreePACE.h"
#include "Translation/PACE/Parser/td_parser.hpp"
#include "Translation/PACE/Parser/gr_parser.hpp"
#include "Translation/TreeAutomaton/term_parser.hpp"

using namespace std;
extern FILE *yyin;
extern FILE *gr_in;
extern FILE *td_in;
extern FILE *term_in;

namespace fs = std::experimental::filesystem;

int main(int argc, char *arg[]) {
    cout<<"Automaton Parse started!"<<endl;

    //Test for term's parser
    char* termFilePath = "../Tests/term.tr";
    term_in = fopen(termFilePath, "r");
    if (!term_in) {
        std::perror("File opening failed");
        return EXIT_FAILURE;
    }
    Term<string> term;
    int resultTerm = 0; // if parsing successful result will be 0 otherwise 1
    resultTerm = term_parse(term, resultTerm); // Parser function from Parser.hpp
    cout<<"result: "<<resultTerm<<endl;
    // check for successful parsing
    if (resultTerm != 0) {
        cout << " Error: input file " << termFilePath << " is not in valid format"
             << endl;
        exit(20);
    }
    cout<<"Automaton Parse Finished!"<<endl;


    char* path = "../Tests/input.txt";
    char* functionType="../Tests/treedecomposition.td";
	if (argc < 2) {
		cout << "Input command was not correct." << endl;
		exit(20);
	}
	path = arg[1];
	functionType = arg[2];
    int printStates = 0;
    int loopTime = 1;
    int printVector = 0;
    std::string searchMethod(functionType);
    searchMethod.erase(0,8);
    cout<<searchMethod<<endl;
    string dynamicPlugin="../DynamicPlugins/";
    map<string, map<string,string> > coreList; // Each entry is a pair of core name and core attributes.
    map<string, string> coreNamesToFiles;
    for (const auto & entry : fs::directory_iterator(dynamicPlugin)){
        string s = entry.path();
        if (s.find(".so") != std::string::npos) {
            cout<<s<<endl;
            char *MyClassLibraryName = const_cast<char *>(s.c_str());
            DynamicCoreHandler factory(MyClassLibraryName);
            std::unique_ptr<DynamicCore> core = factory.create();
            map<string,string> attributes = core->getAttributes();
            string fileName = entry.path().filename();
            coreNamesToFiles.insert(make_pair(core->getAttributeValue("CoreName"),fileName));
            coreList.insert(make_pair(core->getAttributeValue("CoreName"),attributes));
            cout<<"file name: "<<fileName<<endl;
            for (auto it : attributes) {
                cout << it.first << " " << it.second << endl;
            }
            cout << "-----" << endl;
        }
    }
    cout << "Configuration: input path: " << path
         << " printState: " << printStates << " printVector: " << printVector
         << " loopTime: " << loopTime << " search method: " << searchMethod
         << endl;
    Width width;
    vector<PropertyAssignment *> pl;
    Conjecture conjecture;
    yyin = fopen(path, "r");
    if (!yyin) {
        std::perror("File opening failed");
        return EXIT_FAILURE;
    }
    int result = 1; // if parsing successful result will be 0 otherwise 1
    yyparse(pl, conjecture, width, result); // Parser function from Parser.hpp
    // check for successful parsing
    if (result != 0) {
        cout << " Error: input file " << path << " is not in valid format"
             << endl;
        exit(20);
    }
    DynamicKernel dynamicKernel;
    dynamicKernel.get_width().set_name(width.get_name());
    dynamicKernel.get_width().set_value(width.get_value());
    vector<PropertyAssignment> propertyList;
    // change pl (vector<PropertyAssignment*>) to propertyList
    // (vector<PropertyAssignment>).
    for (size_t i = 0; i < pl.size(); ++i) {
        PropertyAssignment p;
        p.set_value(pl[i]->get_value());
        p.set_name(pl[i]->get_name());
        p.set_operatorSign(pl[i]->get_operatorSign());
        p.set_type(pl[i]->get_type());
        p.set_label(pl[i]->get_label());
        p.set_filePath(pl[i]->get_filePath());
        propertyList.push_back(p);
        dynamicKernel.addProperty(p);
    }
    cout << dynamicKernel.get_width().get_name() << " "
         << dynamicKernel.get_width().get_value() << endl;

    DynamicCoreHandler* factory;
    // adding Cores to dynamicKernel
    for (size_t j = 0; j < propertyList.size(); ++j) {
        auto it = coreList.find(propertyList[j].get_name());
        if(it!=coreList.end()){
            cout<< "("<<j+1<<") " << propertyList[j].get_label()<<":= "<< it->first <<" ";
          map<string,string> itMap = it->second;
          if(itMap["ParameterType"]=="NoParameter"){
              string corePath = dynamicPlugin+coreNamesToFiles[it->first];
              factory = new DynamicCoreHandler(corePath.c_str());
              std::unique_ptr<DynamicCore> handlerCore = factory->create();
              DynamicCore* core = handlerCore.release();
              dynamicKernel.addCore(*core);
          }else if(itMap["ParameterType"]=="UnsignedInt"){
              cout<<propertyList[j].get_operatorSign() << propertyList[j].get_value();
              if(itMap["PrimaryOperator"]=="AtLeast"){
                  unsigned int callingParameter=propertyList[j].get_value();
                  if(propertyList[j].get_operatorSign()==">"){
                      callingParameter = callingParameter+1;
                  }
                  string corePath = dynamicPlugin+coreNamesToFiles[it->first];
                  //MaxDegree_AtLeast_DynamicCore *core = new MaxDegree_AtLeast_DynamicCore(callingParameter);
                  factory = new DynamicCoreHandler(corePath.c_str());
                  std::unique_ptr<DynamicCore> handlerCore = factory->create_int(callingParameter);
                  DynamicCore* core = handlerCore.release();
                  dynamicKernel.addCore(*core);
              }else if(itMap["PrimaryOperator"]=="AtMost") {
                  unsigned int callingParameter = propertyList[j].get_value();
                  if (propertyList[j].get_operatorSign() == "<") {
                      callingParameter = callingParameter - 1;
                  }
                  string corePath = dynamicPlugin + coreNamesToFiles[it->first];
                  factory = new DynamicCoreHandler(corePath.c_str());
                  std::unique_ptr<DynamicCore> handlerCore = factory->create_int(callingParameter);
                  DynamicCore *core = handlerCore.release();
                  dynamicKernel.addCore(*core);
              }else if(itMap["ParameterType"]=="InputFile"){

              }
          }
        }else{
            cout<<"The core "<<propertyList[j].get_name()<<" was not found."<<endl;
            exit(20);
        }
        cout<<endl;
    }
    Flags flags;
    flags.add_flag("PrintStates", printStates);
    flags.add_flag("LoopTime", loopTime);
    flags.add_flag("PrintVectors", printVector);
    conjecture.kernel = &dynamicKernel;
    cout<<"Conjecture:"<<endl;
    conjecture.print();
    cout<<"\n";
    if(strstr(functionType,"-search=")!=NULL){
        //search libraries
        string searchPlugin="../SearchPlugins/";
        map<string, map<string,string> > searchList; // Each entry is a pair of core name and core attributes.
        map<string, string> searchNamesToFiles;
        cout<<endl;
        for (const auto & entry : fs::directory_iterator(searchPlugin)){
            string s = entry.path();
            if (s.find(".so") != std::string::npos) {
                char *MyClassLibraryName = const_cast<char *>(s.c_str());
                SearchStrategyHandler factory(MyClassLibraryName);
                std::unique_ptr<SearchStrategy> search = factory.create();
                map<string,string> attributes = search->getAttributes();
                string fileName = entry.path().filename();
                searchNamesToFiles.insert(make_pair(search->getAttributeValue("SearchName"),fileName));
                searchList.insert(make_pair(search->getAttributeValue("SearchName"),attributes));
                cout<<"file name: "<<fileName<<endl;
                for (auto it : attributes) {
                    cout << it.first << " " << it.second << endl;
                }
                cout << "-----" << endl;
            }
        }
        auto it = searchList.find(searchMethod);
        SearchStrategyHandler* searchStrategyHandler;
        if(it != searchList.end()){
            string libPath = searchPlugin+searchNamesToFiles[searchMethod];
            searchStrategyHandler = new SearchStrategyHandler(libPath.c_str());
            std::unique_ptr<SearchStrategy> search = searchStrategyHandler->create_parameter(&dynamicKernel,&conjecture,&flags);
            SearchStrategy* searchStrategy = search.release();
            searchStrategy->search();
        }else{
            cout<<"Error: Search method " << searchMethod<< " was not found."<<endl;
            exit(20);
        }
	}else{
        if(!arg[3]){
            cout<<"Graph file, did not set"<<endl;
            exit(20);
        }
        char* graphPath = arg[3];
        shared_ptr<MultiGraph> multigraph(new MultiGraph);
        gr_in = fopen(graphPath,"r");
        if(!gr_in) {
            std::perror("Reading Graph: File opening failed ");
            return EXIT_FAILURE;
        }
        int result = 0; // if parsing successful result will be 0 otherwise 1
        gr_parse(*multigraph,  result); // Parser function from Parser.hpp
        // check for successful parsing
        if(result != 0){
            cout<<" Error: input file "<< path<< " is not in valid format"<<endl;
            exit(20);
        }
        multigraph->printGraph();
	    TreeDecompositionPACE td;
	    cout<<functionType<<endl;
	    td.multigraph= multigraph;
	    td_in = fopen(functionType,"r");
        if(!td_in) {
            std::perror("Reading Tree Decomposition: File opening failed");
            return EXIT_FAILURE;
        }
        result = 0; // if parsing successful result will be 0 otherwise 1
        td_parse(td,  result); // Parser function from Parser.hpp
        // check for successful parsing
        if(result != 0){
            cout<<" Error: input file "<< path<< " is not in valid format"<<endl;
            exit(20);
        }

        td.constructRaw();
        td.printTree();
        cout<<"after modification"<<endl;
        td.construct();
        td.printTree();
        cout<<"Concrete decomposition"<<endl;
        shared_ptr<ConcreteTreeDecomposition> concreteTreeDecomposition;
        concreteTreeDecomposition = td.convertToConcreteTreeDecomposition();
        concreteTreeDecomposition->printTree();
        shared_ptr<DynamicKernel> sharedKernel = make_shared<DynamicKernel>(dynamicKernel);
        StateTree stateTree = concreteTreeDecomposition->convertToStateTree(sharedKernel);
        stateTree.printStateTree();
        WitnessTreePACE witnessTreePace;
        witnessTreePace.stateTreeToWitnessTreePACE(stateTree, dynamicKernel);
        witnessTreePace.print();
    }
	return 0;
}
