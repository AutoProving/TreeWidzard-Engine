#include "InputController.h"
void InputController::check_available_cores() {
  for (const auto &entry : fs::directory_iterator(dynamicPluginPath)) {
    std::string s = entry.path();
    if (s.find(".so") != std::string::npos) {
      std::string fileName = entry.path().filename();
      std::cout << "File " << fileName;
      try {
        char *MyClassLibraryName = const_cast<char *>(s.c_str());
        DynamicCoreHandler factory(MyClassLibraryName);
        std::unique_ptr<DynamicCore> core = factory.create();
        std::map<std::string, std::string> attributes = core->getAttributes();
        coreNamesToFiles.insert(
            std::make_pair(core->getAttributeValue("CoreName"), fileName));
        coreList.insert(
            std::make_pair(core->getAttributeValue("CoreName"), attributes));
        std::cout << " Loaded. Core Name: "
                  << core->getAttributeValue("CoreName") << '\n';
      } catch (std::exception &e) {
        std::cerr << " failed to load." << e.what() << std::endl;
      }
    }
  }
}

void InputController::parse_input() {
  input_in = fopen(inputPath.c_str(), "r");
  if (!input_in) {
    std::perror("Input File opening failed, given path: ");
    std::cout << inputPath << std::endl;
    exit(20);
  }
  int result = 1;  // if parsing successful result will be 0 otherwise 1
  result = input_parse(conjecture, result, coreList, varToCoreName,
                       varToProperty);  // Parser function from Parser.hpp
  // check for successful parsing
  if (result != 0) {
    std::cout << " Error: input file " << inputPath << " is not in valid format"
              << std::endl;
    exit(20);
  }
  std::cout << "CONJECTURE: ";
  conjecture.print();
  std::cout << std::endl;
  conjecture.setVariablesToCoreName(varToCoreName);
  if (!conjecture.checkConjectureStructure(conjecture.getRoot())) {
    exit(20);
  }

  //    cout<< "varToCoreName"<<endl;
  //    for(auto item: varToCoreName){
  //        cout<<item.first << " ,"<< item.second<<endl;
  //    }
  //    cout<< "nameToProperty"<<endl;
  //    for(auto item:varToProperty){
  //        cout<< item.first<< " ,"  << item.second->getName()<< ",
  //        "<<item.second->getType()<<endl;
  //    }

  width.print();
  dynamicKernel.set_width(width);
  // change pl (vector<PropertyAssignment*>) to propertyList
  // (vector<PropertyAssignment>).

  //    for (size_t i = 0; i < pl.size(); ++i) {
  //        PropertyAssignment p;
  //        p.set_value(pl[i]->get_value());
  //        p.set_name(pl[i]->get_name());
  //        p.set_operatorSign(pl[i]->get_operatorSign());
  //        p.set_type(pl[i]->get_type());
  //        p.set_label(pl[i]->get_label());
  //        p.set_filePath(pl[i]->get_filePath());
  //        p.set_parameters(pl[i]->get_parameters());
  //        propertyList.push_back(p);
  //        dynamicKernel.addProperty(p);
  //    }
  //    cout << "width type: "<< dynamicKernel.get_width().get_name()<< " " <<
  //    dynamicKernel.get_width().get_value() << endl;
}

void InputController::construct_dynamicKernel() {
  DynamicCoreHandler *factory;
  int coreIndex = 1;
  for (auto core : varToProperty) {
    if (coreList.count(core.second->getName())) {
      std::cout << std::left << std::setw(2) << coreIndex << std::setw(5)
                << "- Variable: " << std::setw(5) << core.first << std::setw(5)
                << "NAME: " << std::setw(30) << core.second->getName();
      coreIndex++;
      if (coreList[core.second->getName()].count("ParameterType")) {
        std::cout << std::setw(10) << "ParameterType: " << std::setw(10)
                  << coreList[core.second->getName()]["ParameterType"];
        if (coreList[core.second->getName()]["ParameterType"] ==
                "UnsignedInt" and
            core.second->getParameterType() == "UnsignedInt") {
          std::cout << " " << core.second->getParameter();
        }
        std::cout << std::endl;
        if (coreList[core.second->getName()]["ParameterType"] ==
                "ParameterLess" and
            core.second->getParameterType() == "ParameterLess") {
          std::string corePath =
              dynamicPluginPath + coreNamesToFiles[core.second->getName()];
          factory = new DynamicCoreHandler(corePath.c_str());
          std::unique_ptr<DynamicCore> handlerCore = factory->create();
          DynamicCore *corePointer = handlerCore.release();
          corePointer->setWidth(dynamicKernel.get_width().get_value());
          if (dynamicKernel.isVarExists(core.first)) {
            std::cout << "ERROR: " << core.first << " exists in DynamicKernel"
                      << std::endl;
            exit(20);
          }
          dynamicKernel.addCore(*corePointer);
          auto varToNameAndIndex = dynamicKernel.getVarToNameAndIndex();
          varToNameAndIndex.insert(make_pair(
              core.first,
              make_pair(core.second->getName(), dynamicKernel.coreSize() - 1)));
          dynamicKernel.setVarToNameAndIndex(varToNameAndIndex);
        } else if (coreList[core.second->getName()]["ParameterType"] ==
                       "UnsignedInt" and
                   core.second->getParameterType() == "UnsignedInt") {
          unsigned parameter = core.second->getParameter();
          if (core.second->getOp() == ">") parameter++;
          if (core.second->getOp() == "<") parameter--;
          if (parameter < 0) {
            std::cout << " The given parameter for the core "
                      << core.second->getName() << " is " << parameter
                      << ". Parameter could not be negative." << std::endl;
            exit(20);
          }
          std::string corePath =
              dynamicPluginPath + coreNamesToFiles[core.second->getName()];
          factory = new DynamicCoreHandler(corePath.c_str());
          std::unique_ptr<DynamicCore> handlerCore =
              factory->create_int(parameter);
          DynamicCore *corePointer = handlerCore.release();
          corePointer->setWidth(dynamicKernel.get_width().get_value());
          dynamicKernel.addCore(*corePointer);
          auto varToNameAndIndex = dynamicKernel.getVarToNameAndIndex();
          varToNameAndIndex.insert(make_pair(
              core.first,
              make_pair(core.second->getName(), dynamicKernel.coreSize() - 1)));
          dynamicKernel.setVarToNameAndIndex(varToNameAndIndex);
        } else {
          std::cout << "ERROR: ParameterType of the core "
                    << core.second->getName()
                    << "  is not valid. The Given ParameterType is "
                    << core.second->getParameterType() << std::endl;
          exit(20);
        }
      } else {
        std::cout << "ERROR: " << core.first
                  << " hasn't ParameterType, check the properties of the core. "
                  << std::endl;
        exit(20);
      }
    } else {
      std::cout << "Core " << core.first << " is not exist" << std::endl;
      exit(20);
    }
  }
  conjecture.setKernel(&dynamicKernel);
  std::cout << "core size: " << dynamicKernel.coreSize() << std::endl;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // adding Cores to dynamicKernel
  //    for (size_t j = 0; j < nameToProperty.size(); ++j) {
  //        auto it = coreList.find(propertyList[j].get_name());
  //        if(it!=coreList.end()){
  //            cout<< "("<< j+1 <<") " << propertyList[j].get_label()<<":= "<<
  //            it->first <<" "; map<string,string> itMap = it->second;
  //            if(itMap["ParameterType"]=="NoParameter"){
  //                if(!propertyList[j].get_operatorSign().empty()){
  //                    cout<<"\n Error: Core Type Error,
  //                    "<<propertyList[j].get_name() <<" is NoParameter
  //                    type!"<<endl; exit(20);
  //                }
  //                string corePath =
  //                dynamicPluginPath+coreNamesToFiles[it->first]; factory = new
  //                DynamicCoreHandler(corePath.c_str());
  //                std::unique_ptr<DynamicCore> handlerCore =
  //                factory->create(); DynamicCore* core =
  //                handlerCore.release();
  //                core->setWidth(dynamicKernel.get_width().get_value());
  //                dynamicKernel.addCore(*core);
  //            }else if(itMap["ParameterType"]=="UnsignedInt"){
  //
  //                cout<<propertyList[j].get_operatorSign() <<
  //                propertyList[j].get_value();
  //                if(itMap["PrimaryOperator"]=="AtLeast"){
  //                    unsigned int
  //                    callingParameter=propertyList[j].get_value();
  //                    if(propertyList[j].get_operatorSign()==">"){
  //                        callingParameter = callingParameter+1;
  //                    }else if(propertyList[j].get_operatorSign()!=">=" ){
  //                        cerr<<"\n Error: The property of "<<
  //                        propertyList[j].get_name() <<" is 'At Least', but
  //                        the sign is "
  //                            << propertyList[j].get_operatorSign()<<endl;
  //                        exit(20);
  //                    }
  //                    string corePath =
  //                    dynamicPluginPath+coreNamesToFiles[it->first];
  //                    //MaxDegree_AtLeast_DynamicCore *core = new
  //                    MaxDegree_AtLeast_DynamicCore(callingParameter); factory
  //                    = new DynamicCoreHandler(corePath.c_str());
  //                    std::unique_ptr<DynamicCore> handlerCore =
  //                    factory->create_int(callingParameter); DynamicCore* core
  //                    = handlerCore.release();
  //                    core->setWidth(dynamicKernel.get_width().get_value());
  //                    dynamicKernel.addCore(*core);
  //                }else if(itMap["PrimaryOperator"]=="AtMost") {
  //                    unsigned int callingParameter =
  //                    propertyList[j].get_value(); if
  //                    (propertyList[j].get_operatorSign() == "<") {
  //                        callingParameter = callingParameter - 1;
  //                    }else if(propertyList[j].get_operatorSign()!="<=" ){
  //
  //                        cerr<<"\n Error: The property of "<<
  //                        propertyList[j].get_name() <<" is 'At Most', but the
  //                        sign is "
  //                            << propertyList[j].get_operatorSign()<<endl;
  //                        exit(20);
  //                    }
  //                    string corePath = dynamicPluginPath +
  //                    coreNamesToFiles[it->first]; factory = new
  //                    DynamicCoreHandler(corePath.c_str());
  //                    std::unique_ptr<DynamicCore> handlerCore =
  //                    factory->create_int(callingParameter); DynamicCore *core
  //                    = handlerCore.release();
  //                    core->setWidth(dynamicKernel.get_width().get_value());
  //                    dynamicKernel.addCore(*core);
  //                }else if(itMap["PrimaryOperator"]=="Equal"){
  //                    unsigned int callingParameter =
  //                    propertyList[j].get_value(); if
  //                    (propertyList[j].get_operatorSign() == "=") {
  //                        callingParameter = callingParameter ;
  //                    }else{
  //
  //                        cerr<<"\n Error: The property of "<<
  //                        propertyList[j].get_name() <<" is 'At Most', but the
  //                        sign is "
  //                            << propertyList[j].get_operatorSign()<<endl;
  //                        exit(20);
  //                    }
  //                    string corePath = dynamicPluginPath +
  //                    coreNamesToFiles[it->first]; factory = new
  //                    DynamicCoreHandler(corePath.c_str());
  //                    std::unique_ptr<DynamicCore> handlerCore =
  //                    factory->create_int(callingParameter); DynamicCore *core
  //                    = handlerCore.release();
  //                    core->setWidth(dynamicKernel.get_width().get_value());
  //                    dynamicKernel.addCore(*core);
  //                }
  //            }else if(itMap["ParameterType"]=="InputFile"){
  //                string graph_path = propertyList[j].get_filePath();
  //                graph_path.erase(remove(graph_path.begin(),
  //                graph_path.end(), '"'), graph_path.end()); // remove " from
  //                begin and end of the file path cout<<graph_path<<endl;
  //                ifstream input;
  //                input.open(graph_path);
  //                if(!input) {
  //                    cout << "file path problem";
  //                    exit(20);
  //                }else{
  //                    MultiGraph multiGraph;
  //                    multiGraph.readFromFile(input);
  //                    string corePath = dynamicPluginPath +
  //                    coreNamesToFiles[it->first]; factory = new
  //                    DynamicCoreHandler(corePath.c_str());
  //                    std::unique_ptr<DynamicCore> handlerCore =
  //                    factory->create_multiGraph(multiGraph); DynamicCore
  //                    *core = handlerCore.release();
  //                    core->setWidth(dynamicKernel.get_width().get_value());
  //                    dynamicKernel.addCore(*core);
  //                }
  //            }else if(itMap["ParameterType"]=="MultiParameter"){
  //                string corePath = dynamicPluginPath +
  //                coreNamesToFiles[it->first]; factory = new
  //                DynamicCoreHandler(corePath.c_str());
  //                std::unique_ptr<DynamicCore> handlerCore =
  //                factory->create_parameters(propertyList[j].get_parameters());
  //                DynamicCore *core = handlerCore.release();
  //                core->setWidth(dynamicKernel.get_width().get_value());
  //                dynamicKernel.addCore(*core);
  //            }else{
  //                cout<<"Parameter Type  "<< itMap["ParameterType"] << " could
  //                not find!"<<endl; exit(20);
  //            }
  //        }else{
  //            cout<<"The core "<<propertyList[j].get_name()<<" was not
  //            found."<<endl; exit(20);
  //        }
  //        cout<<endl;
  //    }
  //    if(!dynamicKernel.coreSize()){
  //        cerr<<"Error: Core list is empty"<<endl;
  //        exit(20);
  //    }
  //    conjecture.kernel = &dynamicKernel;
  //    cout<<"Conjecture: "<<endl;
  //    conjecture.print();
  //    cout<<endl;
}

DynamicKernel &InputController::getDynamicKernel() { return dynamicKernel; }

Conjecture &InputController::getConjecture() { return conjecture; }

InputController::InputController(const std::string &inputPath,
                                 const std::string &dynamicPluginPath)
    : inputPath(inputPath), dynamicPluginPath(dynamicPluginPath) {
  check_available_cores();
  parse_input();
  construct_dynamicKernel();
}

const std::string &InputController::getInputPath() const { return inputPath; }

InputController::InputController(const std::string &inputPath,
                                 const std::string &dynamicPluginPath,
                                 const Width &width)
    : inputPath(inputPath), dynamicPluginPath(dynamicPluginPath), width(width) {
  check_available_cores();
  parse_input();
  construct_dynamicKernel();
}
