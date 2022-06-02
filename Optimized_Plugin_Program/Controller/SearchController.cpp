#include "SearchController.h"

SearchController::SearchController(const std::string &inputPath, const std::string &searchStrategy, const Flags &flags,
                                    Width &width){
    std::string file_path = __FILE__;
    //string file_path = "";
    this->searchStrategy = searchStrategy;
    this->flags = flags;
    this->width = width;
    std::string path = file_path.substr(0, file_path.rfind("/"));
    this->searchPluginPath = path+"/../SearchPlugins/";
    inputController = new InputController(inputPath, path+"/../DynamicPlugins/",width);

}

void SearchController::check_search() {
    try {
        for (const auto & entry : fs::directory_iterator(searchPluginPath)){
            std::string s = entry.path();
            if (s.find(".so") != std::string::npos) {
                char *MyClassLibraryName = const_cast<char *>(s.c_str());
                SearchStrategyHandler factory(MyClassLibraryName);
                std::unique_ptr<SearchStrategy> search = factory.create();
                std::map<std::string, std::string> attributes = search->getAttributes();
                std::string fileName = entry.path().filename();
                searchNamesToFiles.insert(make_pair(search->getAttributeValue("SearchName"),fileName));
                searchList.insert(make_pair(search->getAttributeValue("SearchName"),attributes));
            }
        }
    }
    catch(std::exception& e){
        std::cerr << "There is an error during reading search plugins." << std::endl;
        exit(20);
    }

}

void SearchController::action() {
    std::cout<<"Search Method: "<<searchStrategy<<std::endl;
    check_search();
    auto it = searchList.find(searchStrategy);
    SearchStrategyHandler* searchStrategyHandler;
    if(it != searchList.end()){
        std::string libPath = searchPluginPath+searchNamesToFiles[searchStrategy];
        searchStrategyHandler = new SearchStrategyHandler(libPath.c_str());
        std::unique_ptr<SearchStrategy> search = searchStrategyHandler->create_parameter(
                &inputController->getDynamicKernel(), &inputController->getConjecture(), &flags);
        SearchStrategy* searchStrategy = search.release();
        searchStrategy->setPropertyFilePath(inputController->getInputPath());
        std::string output_file_path = fs::path(inputController->getInputPath()).parent_path().string();
        if(output_file_path == ""){
            output_file_path =  fs::path(inputController->getInputPath()).stem().string()+"_"+this->searchStrategy+"_"+width.get_name()+"_"+std::to_string(width.get_value());

        }else{
            output_file_path = output_file_path+"/" + fs::path(inputController->getInputPath()).stem().string()+"_"+this->searchStrategy+"_"+width.get_name()+"_"+std::to_string(width.get_value());
        }
        searchStrategy->setOutputsPath(output_file_path);
        searchStrategy->search();
    }else{
        std::cout<<"Error: Search method " << searchStrategy << " was not found."<<std::endl;
        exit(20);
    }
}
