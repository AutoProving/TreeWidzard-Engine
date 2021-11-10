#include "SearchController.h"

SearchController::SearchController(const string &inputPath, const string &searchStrategy, const Flags &flags,
                                   const Width &width) : searchStrategy(searchStrategy),
                                                         searchPluginPath(searchPluginPath), flags(flags),
                                                         width(width) {
    string file_path = __FILE__;
    //string file_path = "";
    string path = file_path.substr(0, file_path.rfind("/"));
    inputController = new InputController(inputPath, path+"/../DynamicPlugins/",width);
    searchPluginPath = path+"/../SearchPlugins/";
}
void SearchController::check_search() {
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
            //cout<<"file name: "<<fileName<<endl;
//            for (auto it : attributes) {
//                cout << it.first << " " << it.second << endl;
//            }
//            cout << "-----" << endl;
        }
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
        string path = "Counterexample_" + run_fs::path(inputController->getInputPath()).filename().string();
        searchStrategy->setPropertyFilePath(path);
        searchStrategy->search();
    }else{
        cout<<"Error: Search method " << searchStrategy << " was not found."<<endl;
        exit(20);
    }
}





