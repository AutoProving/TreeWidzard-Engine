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

namespace abstract_fs = std::experimental::filesystem;
class AbstractTreeDecompositionNodeContent : public TermNodeContentType {
    private:
        std::string symbol="Leaf";
    public:
        AbstractTreeDecompositionNodeContent();
        AbstractTreeDecompositionNodeContent(const std::string &symbol);
        AbstractTreeDecompositionNodeContent(const AbstractTreeDecompositionNodeContent &abstractTreeDecompositionNodeContent);
        AbstractTreeDecompositionNodeContent& operator=(const AbstractTreeDecompositionNodeContent & other);
        bool operator<(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator>(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator<=(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator>=(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator==(const AbstractTreeDecompositionNodeContent &rhs) const;
        bool operator!=(const AbstractTreeDecompositionNodeContent &rhs) const;
        const std::string &getSymbol() const;
        void setSymbol(const std::string &symbol);
        std::string nodeInformation() override;
        void print() override;
        int symbolType(std::string symbol) const; // returns "0 if Leaf, 1 if IntroVertex, 2 if  ForgetVertex, 3 if IntroEdge or 4 if Join"
        //ToDo: implement this
        std::vector<int> symbolNumbers(std::string s) const; // returns vector "[i]" if symbol="IntroVertex_i or ForgetVertex_i", Returns vector "[i,j]" if symbol="IntroEdge_i_j", Returns empty vector if symbol = "Join"
        std::vector<int> extractIntegerWords(std::string s) const;
        //Define the lexicographically smallest content.
        std::string smallestContent();
};
class AbstractTreeDecomposition: public Term<AbstractTreeDecompositionNodeContent>{
    public:
        void writeToFile(std::string fileName);
        std::shared_ptr<TermNode<ConcreteNode>> constructCTDNode(TermNode<AbstractTreeDecompositionNodeContent> &node);
        ConcreteTreeDecomposition convertToConcreteTreeDecomposition();
};
#endif //TREEWIDZARD_ABSTRACTTREEDECOMPOSITION_H
