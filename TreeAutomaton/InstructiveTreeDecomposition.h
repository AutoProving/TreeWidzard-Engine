#ifndef TREEWIDZARD_INSTRUCTIVETREEDECOMPOSITION_H
#define TREEWIDZARD_INSTRUCTIVETREEDECOMPOSITION_H
#include <algorithm>
#include <cstring>
#include <experimental/filesystem>
#include <iostream>
#include <sstream>
#include "../Multigraph/MultiGraph.h"
#include "ConcreteTreeDecomposition.h"
#include "Term.h"

class InstructiveTreeDecompositionNodeContent : public TermNodeContentType {
  private:
	std::string symbol = "Leaf";

  public:
	InstructiveTreeDecompositionNodeContent();
	InstructiveTreeDecompositionNodeContent(const std::string &symbol);
	InstructiveTreeDecompositionNodeContent(
		const InstructiveTreeDecompositionNodeContent
			&instructiveTreeDecompositionNodeContent);
	InstructiveTreeDecompositionNodeContent &operator=(
		const InstructiveTreeDecompositionNodeContent &other);
	bool operator<(const InstructiveTreeDecompositionNodeContent &rhs) const;
	bool operator>(const InstructiveTreeDecompositionNodeContent &rhs) const;
	bool operator<=(const InstructiveTreeDecompositionNodeContent &rhs) const;
	bool operator>=(const InstructiveTreeDecompositionNodeContent &rhs) const;
	bool operator==(const InstructiveTreeDecompositionNodeContent &rhs) const;
	bool operator!=(const InstructiveTreeDecompositionNodeContent &rhs) const;
	const std::string &getSymbol() const;
	void setSymbol(const std::string &symbol);
	std::string nodeInformation() override;
	void print() override;
	int symbolType(
		std::string symbol) const; // returns "0 if Leaf, 1 if IntroVertex, 2 if
								   // ForgetVertex, 3 if IntroEdge or 4 if Join"
	// ToDo: implement this
	std::vector<int> symbolNumbers(
		std::string s) const; // returns vector "[i]" if symbol="IntroVertex_i
							  // or ForgetVertex_i", Returns vector "[i,j]" if
							  // symbol="IntroEdge_i_j", Returns empty vector if
							  // symbol = "Join"
	std::vector<int> extractIntegerWords(std::string s) const;
	// Define the lexicographically smallest content.
	std::string smallestContent();
};
class InstructiveTreeDecomposition
	: public Term<InstructiveTreeDecompositionNodeContent> {
  public:
	void writeToFile(std::string fileName);
	std::shared_ptr<TermNode<ConcreteNode>> constructCTDNode(
		TermNode<InstructiveTreeDecompositionNodeContent> &node);
	ConcreteTreeDecomposition convertToConcreteTreeDecomposition();
};
#endif // TREEWIDZARD_INSTRUCTIVETREEDECOMPOSITION_H
