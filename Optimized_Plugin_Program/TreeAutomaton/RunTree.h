#ifndef TREEWIDZARD_RUNTREE_H
#define TREEWIDZARD_RUNTREE_H
#include <iostream>
#include <fstream>
#include <memory>
#include <experimental/filesystem>
#include "Term.h"

namespace run_fs = std::experimental::filesystem;

template<class StateType, class TermNodeContent>
class RunNodeContent: public TermNodeContentType{
private:
    TermNodeContent runNodeContent;
    StateType state;
public:
    RunNodeContent() {};

    RunNodeContent(TermNodeContent runNodeContent, StateType state) : runNodeContent(runNodeContent), state(state) {};

    RunNodeContent(const RunNodeContent<StateType,TermNodeContent> &runNodeContent);

    TermNodeContent getRunNodeContent() const {
        return runNodeContent;
    }

    void setRunNodeContent(TermNodeContent runNodeContent) {
        RunNodeContent::runNodeContent = runNodeContent;
    }

    StateType getState() const {
        return state;
    }

    void setState(StateType state) {
        this->state = state;
    }

    RunNodeContent<StateType,TermNodeContent> & operator=(const RunNodeContent<StateType,TermNodeContent> & other);

    bool operator==(const RunNodeContent &rhs) const {
        return runNodeContent == rhs.runNodeContent &&
               state == rhs.state;
    }

    bool operator!=(const RunNodeContent &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const RunNodeContent &rhs) const {
        if (runNodeContent < rhs.runNodeContent)
            return true;
        if (rhs.runNodeContent < runNodeContent)
            return false;
        return state < rhs.state;
    }

    bool operator>(const RunNodeContent &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const RunNodeContent &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const RunNodeContent &rhs) const {
        return !(*this < rhs);
    }

    void print() {
        std::cout<<"Term Node Content:"<<std::endl;
        runNodeContent.print();
        std::cout<<"State:"<<std::endl;
        state.print();
    }

    std::string nodeInformation() override {
        std::string info;
        info = runNodeContent.nodeInformation() +" " + state.stateInformation();
        info+= "---------------------------------------------------------------\n";
        info+= "---------------------------------------------------------------\n";

        return info;
    }

};

template<class StateType, class TermNodeContent>
RunNodeContent<StateType, TermNodeContent>::RunNodeContent(const RunNodeContent<StateType,TermNodeContent> &other) {
    this->setRunNodeContent(other.getRunNodeContent());
    this->setState( other.getState());
}

template<class StateType, class TermNodeContent>
RunNodeContent<StateType, TermNodeContent> &
RunNodeContent<StateType, TermNodeContent>::operator=(const RunNodeContent<StateType, TermNodeContent> &other) {
    this->setRunNodeContent(other.getRunNodeContent());
    this->setState( other.getState());
    return *this;
}

template<class StateType, class TermNodeContent>
class RunTree : public Term<RunNodeContent<StateType,TermNodeContent> >{
public:
    void writeToFile(std::string fileName);
    //TODO
    void convertRunNodeToTermNode(std::shared_ptr<TermNode<RunNodeContent<StateType, TermNodeContent>>> runNode,
                                  std::shared_ptr<TermNode<TermNodeContent>> termNode);
    Term<TermNodeContent> convertRunToTerm(RunTree<StateType,TermNodeContent> runTree);
};

template<class StateType, class TermNodeContent>
void RunTree<StateType, TermNodeContent>::writeToFile(std::string fileName) {
    std::ofstream runFile;
    runFile.open(fileName,std::ios::out);
    if (runFile.is_open())
    {
        runFile << this->termInformation();
        runFile.close();
    }
    else {
        std::cout << "Unable to open "<< fileName << std::endl;
        exit(20);
    }
}

template<class StateType, class TermNodeContent>
Term<TermNodeContent>
RunTree<StateType, TermNodeContent>::convertRunToTerm(RunTree<StateType, TermNodeContent> runTree) {
    Term<TermNodeContent> term;
    std::shared_ptr<TermNode<TermNodeContent>> termRoot(new TermNode<TermNodeContent>);
    convertRunNodeToTermNode(this->getRoot(), termRoot);
    term.setRoot(termRoot);
    return term;
}

template<class StateType, class TermNodeContent>
void RunTree<StateType, TermNodeContent>::convertRunNodeToTermNode(std::shared_ptr<TermNode<RunNodeContent<StateType, TermNodeContent>>> runNode,
                                                                   std::shared_ptr<TermNode<TermNodeContent>> termNode) {
    termNode->setNodeContent(runNode->getNodeContent().getRunNodeContent());
    std::vector<std::shared_ptr<TermNode<TermNodeContent>>> children;
    for(auto item:runNode->getChildren()){
        std::shared_ptr<TermNode<TermNodeContent> > termChild(new TermNode<TermNodeContent>);
        convertRunNodeToTermNode(item,termChild);
        children.push_back(termChild);
        termChild->setParent(termNode);
        termNode->addChild(termChild);
    }
}


#endif //TREEWIDZARD_RUNTREE_H
