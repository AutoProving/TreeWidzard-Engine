
#include "InstructiveTreeDecomposition.h"
InstructiveTreeDecompositionNodeContent::
	InstructiveTreeDecompositionNodeContent() {}
InstructiveTreeDecompositionNodeContent::
	InstructiveTreeDecompositionNodeContent(const std::string &symbol)
	: symbol(symbol) {}

const std::string &InstructiveTreeDecompositionNodeContent::getSymbol() const {
	return symbol;
}

void InstructiveTreeDecompositionNodeContent::setSymbol(
	const std::string &symbol) {
	this->symbol = symbol;
}

InstructiveTreeDecompositionNodeContent &
InstructiveTreeDecompositionNodeContent::operator=(
	const InstructiveTreeDecompositionNodeContent &other) {
	setSymbol(other.getSymbol());
	return *this;
}

bool InstructiveTreeDecompositionNodeContent::operator==(
	const InstructiveTreeDecompositionNodeContent &rhs) const {
	return symbol == rhs.symbol;
}

bool InstructiveTreeDecompositionNodeContent::operator!=(
	const InstructiveTreeDecompositionNodeContent &rhs) const {
	return !(rhs == *this);
}

bool InstructiveTreeDecompositionNodeContent::operator<(
	const InstructiveTreeDecompositionNodeContent &rhs) const {
	int typeSymbol1 = symbolType(this->symbol);
	int typeSymbol2 = symbolType(rhs.symbol);
	if (typeSymbol1 < typeSymbol2) return true;
	if (typeSymbol1 > typeSymbol2) return false;
	std::vector<int> numbersSymbol1 = symbolNumbers(this->symbol);
	std::vector<int> numbersSymbol2 = symbolNumbers(rhs.symbol);
	return numbersSymbol1 < numbersSymbol2;
}

bool InstructiveTreeDecompositionNodeContent::operator>(
	const InstructiveTreeDecompositionNodeContent &rhs) const {
	return rhs < *this;
}

bool InstructiveTreeDecompositionNodeContent::operator<=(
	const InstructiveTreeDecompositionNodeContent &rhs) const {
	return !(rhs < *this);
}

bool InstructiveTreeDecompositionNodeContent::operator>=(
	const InstructiveTreeDecompositionNodeContent &rhs) const {
	return !(*this < rhs);
}

int InstructiveTreeDecompositionNodeContent::symbolType(std::string s) const {
	if (s.find("Leaf") != std::string::npos) {
		return 0;
	} else if (s.find("IntroVertex") != std::string::npos) {
		return 1;
	} else if (s.find("ForgetVertex") != std::string::npos) {
		return 2;
	} else if (s.find("IntroEdge") != std::string::npos) {
		return 3;
	} else if (s.find("Join") != std::string::npos) {
		return 4;
	} else {
		std::cout
			<< "Error: InstructiveTreeDecompositionNodeContent::symbolType, "
			   "std::string is not in formal format."
			<< std::endl;
		exit(20);
	}
}

std::vector<int> InstructiveTreeDecompositionNodeContent::symbolNumbers(
	std::string s) const {
	if (s.find("Leaf") != std::string::npos) {
	} else if (s.find("IntroVertex") != std::string::npos) {
	} else if (s.find("ForgetVertex") != std::string::npos) {
	} else if (s.find("IntroEdge") != std::string::npos) {
	} else if (s.find("Join") != std::string::npos) {
	} else {
		std::cout
			<< "Error: InstructiveTreeDecompositionNodeContent::symbolNumbers, "
			   "std::string is not in formal format."
			<< std::endl;
		exit(20);
	}
	return extractIntegerWords(s);
}

std::string InstructiveTreeDecompositionNodeContent::smallestContent() {
	return "Leaf";
}

void InstructiveTreeDecompositionNodeContent::print() { std::cout << symbol; }

std::vector<int> InstructiveTreeDecompositionNodeContent::extractIntegerWords(
	std::string s) const {
	std::replace(s.begin(), s.end(), '_', '\t');
	std::stringstream ss;
	/* Storing the whole std::string into std::string stream */
	ss << s;
	std::vector<int> numberVec;
	/* Running loop till the end of the stream */
	std::string temp;
	int found;
	while (!ss.eof()) {
		/* extracting word by word from stream */
		ss >> temp;

		/* Checking the given word is integer or not */
		if (std::stringstream(temp) >> found) {
			numberVec.push_back(found);
		}

		/* To save from space at the end of std::string */
		temp = "";
	}
	return numberVec;
}

InstructiveTreeDecompositionNodeContent::
	InstructiveTreeDecompositionNodeContent(
		const InstructiveTreeDecompositionNodeContent
			&instructiveTreeDecompositionNodeContent) {
	this->symbol = instructiveTreeDecompositionNodeContent.getSymbol();
}

std::string InstructiveTreeDecompositionNodeContent::nodeInformation() {
	return symbol;
}

std::shared_ptr<TermNode<ConcreteNode>>
InstructiveTreeDecomposition::constructCTDNode(
	TermNode<InstructiveTreeDecompositionNodeContent> &node) {
	std::shared_ptr<TermNode<ConcreteNode>> ctdNode(new TermNode<ConcreteNode>);
	std::vector<std::shared_ptr<TermNode<ConcreteNode>>> children;
	std::string symbol = node.getNodeContent().getSymbol();
	ConcreteNode concrete;
	concrete.setSymbol(symbol);
	// ctdNode->setNodeContent(concrete);
	for (auto &child : node.getChildren()) {
		std::shared_ptr<TermNode<ConcreteNode>> ctdChild =
			constructCTDNode(*child);
		ctdChild->setParent(ctdNode);
		children.push_back(ctdChild);
	}
	Bag b;
	if (symbol == "Leaf") {
		concrete.setBag(b);
		ctdNode->setNodeContent(concrete);
	} else {
		std::vector<int> numbersInString =
			node.getNodeContent().extractIntegerWords(symbol);
		if (strstr(symbol.c_str(), "IntroVertex")) {
			if (numbersInString.size() != 1) {
				std::cout
					<< "Error: ConcreteTreeDecomposition::constructCTDNode "
					   "children numbers not valid"
					<< std::endl;
				exit(20);
			} else {
				b = children[0]->getNodeContent().getBag();
				b.intro_v(numbersInString[0]);
				concrete.setBag(b);
				ctdNode->setNodeContent(concrete);
			}
		} else if (strstr(symbol.c_str(), "ForgetVertex")) {
			if (numbersInString.size() != 1) {
				std::cout
					<< "Error: ConcreteTreeDecomposition::constructCTDNode "
					   "children numbers not valid"
					<< std::endl;
				exit(20);
			} else {
				b = children[0]->getNodeContent().getBag();
				b.forget_v(numbersInString[0]);
				concrete.setBag(b);
				ctdNode->setNodeContent(concrete);
			}
		} else if (strstr(symbol.c_str(), "IntroEdge")) {
			if (numbersInString.size() != 2) {
				std::cout
					<< "Error: ConcreteTreeDecomposition::constructCTDNode "
					   "children numbers not valid"
					<< std::endl;
				exit(20);
			} else {
				b = children[0]->getNodeContent().getBag();
				b.intro_e(numbersInString[0], numbersInString[1]);
				concrete.setBag(b);
				ctdNode->setNodeContent(concrete);
			}
		} else if (symbol == "Join") {
			Bag b;
			b.set_elements(
				children[0]->getNodeContent().getBag().get_elements());
			concrete.setBag(b);
			ctdNode->setNodeContent(concrete);
		}
	}
	ctdNode->setChildren(children);
	return ctdNode;
}

ConcreteTreeDecomposition
InstructiveTreeDecomposition::convertToConcreteTreeDecomposition() {
	ConcreteTreeDecomposition concreteTreeDecomposition;
	std::shared_ptr<TermNode<ConcreteNode>> root =
		constructCTDNode(*this->getRoot());
	concreteTreeDecomposition.setRoot(root);
	return concreteTreeDecomposition;
}

void InstructiveTreeDecomposition::writeToFile(std::string fileName) {
	// fileName =
	// "CounterExample_InstructiveTreeDecomposition_"+fs::path(fileName).filename().std::string();
	std::ofstream atdFile(fileName);
	if (atdFile.is_open()) {
		atdFile << termInformation();
		atdFile.close();
	} else {
		std::cout << "Unable to open " << fileName << std::endl;
		exit(20);
	}
}
