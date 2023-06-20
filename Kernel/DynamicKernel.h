#ifndef DYNAMIC_KERNEL_H
#define DYNAMIC_KERNEL_H
#include <list>
#include <stack>
#include <unordered_map>
#include "../Conjecture/PropertyAssignment.h"
#include "DynamicCore.h"
#include "State.h"
#include "Width.h"

class DynamicKernel : public std::enable_shared_from_this<DynamicKernel> {
  private:
	Width width;
	// vector<PropertyAssignment> properties;
	std::vector<DynamicCore *> cores;
	std::map<std::string, std::pair<std::string, int>> varToNameAndIndex;

  public:
	DynamicKernel() {}
	DynamicKernel(const DynamicKernel &other)
		: enable_shared_from_this(),
		  width(other.width),
		  cores(other.cores),
		  varToNameAndIndex(other.varToNameAndIndex) {}

	void addCore(DynamicCore &core);
	size_t coreSize();
	Width &get_width();
	void set_width(Width &width);

	const std::map<std::string, std::pair<std::string, int>>
		&getVarToNameAndIndex() const;

	void setVarToNameAndIndex(
		const std::map<std::string, std::pair<std::string, int>>
			&varToNameAndIndex);

	DynamicCore *getCoreByVar(std::string var);
	bool isVarExists(std::string var);
	int getIndexByVar(std::string var);
	// void addProperty(PropertyAssignment &);
	// const vector<PropertyAssignment> &get_properties() const;

	State::ptr initialState();
	State::ptr intro_v(State::ptr q, unsigned i);
	State::ptr intro_e(State::ptr q, const unsigned i, const unsigned j);
	State::ptr forget_v(State::ptr q, unsigned i);
	State::ptr join(State::ptr q1, State::ptr q2);
	DynamicCore *pointerToCoreNumber(unsigned i);
};

#endif
