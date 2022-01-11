// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef TREEWIDZARD_ABSTRACTTREEDECOMPOSITION_H
#define TREEWIDZARD_ABSTRACTTREEDECOMPOSITION_H
#include <iostream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <experimental/filesystem>
#include "Term.h"
#include "../Multigraph/MultiGraph.h"
#include "ConcreteTreeDecomposition.h"
using namespace std;
namespace abstract_fs = std::experimental::filesystem;
class AbstractTreeDecompositionNodeContent : public TermNodeContentType {
    private:
        string symbol="Leaf";
    public:
        AbstractTreeDecompositionNodeContent();
        AbstractTreeDecompositionNodeContent(const string &symbol);
        AbstractTreeDecompositionNodeContent(const AbstractTreeDecompositionNodeContent &abstractTreeDecompositionNodeContent);
        AbstractTreeDecompositionNodeContent& operator=(const AbstractTreeDecompositionNodeContent & other);
        bool operator<(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator>(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator<=(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator>=(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator==(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator!=(const AbstractTreeDecompositionNodeContent &rhs) const;
        const string &getSymbol() const;
        void setSymbol(const string &symbol);
        string nodeInformation() override;
        void print() override;
        int symbolType(string symbol) const; // returns "0 if Leaf, 1 if IntroVertex, 2 if  ForgetVertex, 3 if IntroEdge or 4 if Join"
        //ToDo: implement this
        vector<int> symbolNumbers(string s) const; // returns vector "[i]" if symbol="IntroVertex_i or ForgetVertex_i", Returns vector "[i,j]" if symbol="IntroEdge_i_j", Returns empty vector if symbol = "Join"
        vector<int> extractIntegerWords(string s) const;
        //Define the lexicographically smallest content.
        string smallestContent();
};
class AbstractTreeDecomposition: public Term<AbstractTreeDecompositionNodeContent>{
    public:
        void writeToFile(string fileName);
        shared_ptr<TermNode<ConcreteNode>> constructCTDNode(TermNode<AbstractTreeDecompositionNodeContent> &node);
        ConcreteTreeDecomposition convertToConcreteTreeDecomposition();
};
#endif //TREEWIDZARD_ABSTRACTTREEDECOMPOSITION_H
