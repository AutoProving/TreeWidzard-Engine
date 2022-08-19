#include "SearchStrategy.h"

void SearchStrategy::search() {
	std::cout << "ERROR: Based class search function called!" << std::endl;
	exit(20);
}

SearchStrategy::SearchStrategy() {
    kernel = new DynamicKernel();
    conjecture = new Conjecture;
    flags = new Flags();

}

SearchStrategy::SearchStrategy(DynamicKernel *dynamicKernel,
							   Conjecture *conjecture, Flags *flags) {
	this->kernel = dynamicKernel;
	this->conjecture = conjecture;
	this->flags = flags;
}


void SearchStrategy::addAttribute(std::string x, std::string y) {
    attributes.insert(make_pair(x,y));
}

bool SearchStrategy::isAttribute(std::string x, std::string y) {
    auto it = attributes.find(x);
    if(it!=attributes.end()){
        if(it->second==y){
            return true;
        }
    }
    return false;
}

std::string SearchStrategy::getAttributeValue(std::string x) {
    auto it = attributes.find(x);
    if(it!=attributes.end()){
        return it->second;
    }else{
        return "NULL";
    }

}
std::map<std::string, std::string> SearchStrategy::getAttributes() {
    return attributes;
}

const std::string &SearchStrategy::getPropertyFilePath() const {
    return propertyFilePath;
}

void SearchStrategy::setPropertyFilePath(const std::string &propertyFilePath) {
    SearchStrategy::propertyFilePath = propertyFilePath;
}

const std::string &SearchStrategy::getOutputsPath() const {
    return outputsPath;
}

void SearchStrategy::setOutputsPath(const std::string &outputsPath) {
    SearchStrategy::outputsPath = outputsPath;
}
