#include "SearchController.h"

SearchController::SearchController(const string &inputPath, const string &searchStrategy, const Flags &flags,
                                    Width &width){
    string file_path = __FILE__;
    //string file_path = "";
    this->searchStrategy = searchStrategy;
    this->flags = flags;
    this->width = width;
    string path = file_path.substr(0, file_path.rfind("/"));
    this->searchPluginPath = path+"/../SearchPlugins/";
    inputController = new InputController(inputPath, path+"/../DynamicPlugins/",width);

}

void SearchController::check_search() {
    try {
        for (const auto & entry : fs::directory_iterator(searchPluginPath)){
            string s = entry.path();
            if (s.find(".so") != std::string::npos) {
                char *MyClassLibraryName = const_cast<char *>(s.c_str());
                SearchStrategyHandler factory(MyClassLibraryName);
                std::unique_ptr<SearchStrategy> search = factory.create();
                map<string,string> attributes = search->getAttributes();
                string fileName = entry.path().filename();
                searchNamesToFiles.insert(make_pair(search->getAttributeValue("SearchName"),fileName));
                searchList.insert(make_pair(search->getAttributeValue("SearchName"),attributes));
            }
        }
    }
    catch(exception& e){
        cerr << "There is an error during reading search plugins." << endl;
        exit(20);
    }

}

void SearchController::action() {
    cout<<"Search Method: "<<searchStrategy<<endl;
    check_search();
    auto it = searchList.find(searchStrategy);
    SearchStrategyHandler* searchStrategyHandler;
    if(it != searchList.end()){
        string libPath = searchPluginPath+searchNamesToFiles[searchStrategy];
        searchStrategyHandler = new SearchStrategyHandler(libPath.c_str());
        std::unique_ptr<SearchStrategy> search = searchStrategyHandler->create_parameter(
                &inputController->getDynamicKernel(), &inputController->getConjecture(), &flags);
        SearchStrategy* searchStrategy = search.release();
        searchStrategy->setPropertyFilePath(inputController->getInputPath());
        string output_file_path = fs::path(inputController->getInputPath()).parent_path().string();
        output_file_path = output_file_path+"/" + fs::path(inputController->getInputPath()).stem().string()+"_"+this->searchStrategy+"_"+width.get_name()+"_"+to_string(width.get_value());
        searchStrategy->setOutputsPath(output_file_path);
        searchStrategy->search();
    }else{
        cout<<"Error: Search method " << searchStrategy << " was not found."<<endl;
        exit(20);
    }
}





