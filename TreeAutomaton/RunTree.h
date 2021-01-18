#ifndef TREEWIDZARD_RUNTREE_H
#define TREEWIDZARD_RUNTREE_H
#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include "Term.h"
using namespace std;
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
        RunNodeContent::state = state;
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
        cout<<"Term Node Content:"<<endl;
        runNodeContent.print();
        cout<<"State:"<<endl;
        state.print();
    }

    string nodeInformation() override {
        string info;
        info = runNodeContent.nodeInformation() +" " + state.stateInformation();
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
    void writeToFile(string fileName);
    //TODO
    Term<TermNodeContent> convertRunToTerm(RunTree<StateType,TermNodeContent> runTree);
};

template<class StateType, class TermNodeContent>
void RunTree<StateType, TermNodeContent>::writeToFile(string fileName) {
    fileName = "Counterexample_RunTreeDec_" + run_fs::path(fileName).filename().string();
    ofstream runFile (fileName);
    if (runFile.is_open())
    {
        runFile << this->termInformation();
        runFile.close();
    }
    else {
        cout << "Unable to open "<< fileName << endl;
        exit(20);
    }
}


#endif //TREEWIDZARD_RUNTREE_H
