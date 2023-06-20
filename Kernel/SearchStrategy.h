#ifndef SEARCHSTRATEGY_H
#define SEARCHSTRATEGY_H

#include <algorithm>
#include <chrono>
#include <iomanip>
#include "../Conjecture/Conjecture.h"
#include "../TreeAutomaton/ConcreteTreeDecomposition.h"
#include "DynamicKernel.h"
#include "Flags.h"
#include "State.h"
#include "StateTree.h"

class SearchStrategy {
  protected:
	DynamicKernel *kernel;
	Flags *flags;
	Conjecture *conjecture;
	std::map<std::string, std::string>
		attributes; // Characteristics of the search. This is initialized in the
					// constructor of the derived class.
	std::string propertyFilePath;
	std::string outputsPath;

  public:
	SearchStrategy(DynamicKernel *dynamicKernel, Conjecture *conjecture,
				   Flags *flags);
	SearchStrategy();
  virtual ~SearchStrategy() = default;

	const std::string &getPropertyFilePath() const;
	void setPropertyFilePath(const std::string &propertyFilePath);
	void addAttribute(std::string x, std::string y);
	bool isAttribute(std::string x, std::string y);
	std::string getAttributeValue(
		std::string x); // Returns "y" if (x,y) belongs to attributes.
	std::map<std::string, std::string> getAttributes();
	virtual void search();

	const std::string &getOutputsPath() const;

	void setOutputsPath(const std::string &outputsPath);
};

#endif
