// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "SearchStrategy.h"

void SearchStrategy::search() {
	cout << "ERROR: Based class search function called!" << endl;
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


void SearchStrategy::addAttribute(string x, string y) {
    attributes.insert(make_pair(x,y));
}

bool SearchStrategy::isAttribute(string x, string y) {
    auto it = attributes.find(x);
    if(it!=attributes.end()){
        if(it->second==y){
            return true;
        }
    }
    return false;
}

string SearchStrategy::getAttributeValue(string x) {
    auto it = attributes.find(x);
    if(it!=attributes.end()){
        return it->second;
    }else{
        return "NULL";
    }

}
map<string, string> SearchStrategy::getAttributes() {
    return attributes;
}


