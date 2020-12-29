#ifndef TREEWIDZARD_ABSTRACTTREEDECOMPOSITION_H
#define TREEWIDZARD_ABSTRACTTREEDECOMPOSITION_H

#include <iostream>
#include <sstream>
#include <algorithm>
#include "Term.h"
using namespace std;

class AbstractTreeDecompositionNodeContent : public TermNodeContentType {
private:
    string symbol="Leaf";
public:
    AbstractTreeDecompositionNodeContent();

    AbstractTreeDecompositionNodeContent(const string &symbol);

    bool operator<(const AbstractTreeDecompositionNodeContent &rhs) const;

    bool operator>(const AbstractTreeDecompositionNodeContent &rhs) const;

    bool operator<=(const AbstractTreeDecompositionNodeContent &rhs) const;

    bool operator>=(const AbstractTreeDecompositionNodeContent &rhs) const;

    bool operator==(const AbstractTreeDecompositionNodeContent &rhs) const;

    bool operator!=(const AbstractTreeDecompositionNodeContent &rhs) const;

    const string &getSymbol() const;

    void setSymbol(const string &symbol);

    void print() override;

    int symbolType(string symbol) const; // returns "0 if Leaf, 1 if IntroVertex, 2 if  ForgetVertex, 3 if IntroEdge or 4 if Join"
    //ToDo: implement this
    vector<int> symbolNumbers(string s) const; // returns vector "[i]" if symbol="IntroVertex_i or ForgetVertex_i", Returns vector "[i,j]" if symbol="IntroEdge_i_j", Returns empty vector if symbol = "Join"

    vector<int> extractIntegerWords(string s) const;
    //Define the lexicographically smallest content.
    shared_ptr<TermNodeContentType> smallestContent(){
        shared_ptr<AbstractTreeDecompositionNodeContent> smallest(new AbstractTreeDecompositionNodeContent("Leaf") );
        return smallest;
    }
};

class AbstractTreeDecomposition: public Term<AbstractTreeDecompositionNodeContent>{
public:

    //	public:
//		ATD();
//		ATD(Multigraph G);
//		ATD(TreeDe)
//		Multigraph ATD_To_Multigraph();
//		ATD()
//
//}
//
};

#endif //TREEWIDZARD_ABSTRACTTREEDECOMPOSITION_H
