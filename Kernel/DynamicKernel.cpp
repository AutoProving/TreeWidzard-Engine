#include "DynamicKernel.h"

void DynamicKernel::addCore(DynamicCore& core) {
      core.createInitialWitnessSet();
      cores.push_back(&core);
}

size_t DynamicKernel::coreSize() {
    return cores.size();
}
State::ptr DynamicKernel::initialState() {
	auto initialState = State::createPooled();
	Bag emptyBag; // Empty
	for (size_t k = 0; k < cores.size(); ++k) {
		auto initialSet = cores[k]->getInitialSet();
		assert(initialSet);
		initialState->addWitnessSet(std::move(initialSet));
	}
	initialState->set_bag(emptyBag);
	return initialState;
}

State::ptr DynamicKernel::intro_v(State::ptr q, unsigned i) {
	const Bag &bag = q->get_bag();
	if (bag.vertex_introducible(i)) {
		auto aux = State::createPooled();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
				cores[r]->intro_v(i, bag, q->getWitnessSet(static_cast<int>(r))));
		}
		aux->set_bag(bag.intro_v(i));
		return aux;
	} else {
		std::cerr << "Error in function DynamicKernel::intro_v. The bag is not "<<i<<
				" introducible"
			 << std::endl;
		exit(10);
	}
}

State::ptr DynamicKernel::intro_e(const State::ptr q, const unsigned i,
								  const unsigned j) {
	const Bag &bag = q->get_bag();
	if (bag.edge_introducible(i, j)) {
		auto aux = State::createPooled();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
				cores[r]->intro_e(i, j, bag, q->getWitnessSet(static_cast<int>(r))));
		}
		aux->set_bag(bag.intro_e(i, j));
		return aux;
	} else {
		std::cerr << "Error in function DynamicKernel::intro_e. The bag is not "<<i <<", "<<j<<
				" introducible"
			 << std::endl;
		exit(10);
	}
}

State::ptr DynamicKernel::forget_v(State::ptr q, unsigned i) {
	const Bag &bag = q->get_bag();
	if (bag.vertex_forgettable(i)) {
		auto aux = State::createPooled();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
				cores[r]->forget_v(i, bag, q->getWitnessSet(static_cast<int>(r))));
		}
		aux->set_bag(bag.forget_v(i));
		return aux;
	} else {
		std::cerr << "Error in function DynamicKernel::forget_v. The bag is not "<<i<<
				" forgettable"
			 << std::endl;
		exit(10);
	}
}

State::ptr DynamicKernel::join(State::ptr q1, State::ptr q2) {
	const Bag &left_bag = q1->get_bag();
	const Bag &right_bag = q2->get_bag();
	if (left_bag.joinable(right_bag)) {
		auto aux = State::createPooled();
		Bag b;
		b.set_elements(left_bag.get_elements());
		// In join the bag is defined only by the shared labels, so the
		// transient edge marker must be cleared.
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
					cores[r]->join(b, q1->getWitnessSet(static_cast<int>(r)), q2->getWitnessSet(static_cast<int>(r))));
		}
		aux->set_bag(b);
		return aux;
	} else {
		std::cerr << "Error in function DynamicKernel::join. The bags are not "
				"joinable"
			 << std::endl;
		exit(10);
	}
}

// Change Name of this function from core_index to pointerToCoreNumber
DynamicCore* DynamicKernel::pointerToCoreNumber(unsigned i) {
	size_t j = i;
	if (cores.size() - 1 < j) {
		std::cerr << "ERROR: In DynamicKernel::pointerToCoreNumber, asked index "
				"bigger than the size of cores"
			 << std::endl;
		exit(20);
	} else {
		return cores[i];
	}
}

Width& DynamicKernel::get_width() { return width; }

void DynamicKernel::set_width(Width& w) { width = w; }


bool DynamicKernel::isVarExists(const std::string &var) {
    if(varToNameAndIndex.count(var)){
        return true;
    }else{
        return false;
    }
}

const std::map<std::string, std::pair<std::string, int>> &DynamicKernel::getVarToNameAndIndex() const {
    return varToNameAndIndex;
}

void DynamicKernel::setVarToNameAndIndex(const std::map<std::string, std::pair<std::string, int>> &newVarToNameAndIndex) {
    varToNameAndIndex = newVarToNameAndIndex;
}


DynamicCore* DynamicKernel::getCoreByVar(const std::string &var) {
    if(isVarExists(var)){
        return cores[static_cast<size_t>(varToNameAndIndex[var].second)] ;
    }else{
        std::cout<<"Error DynamicKernel::getCoreByVar core doesn't exist"<<std::endl;
        exit(20);
    }
}

int DynamicKernel::getIndexByVar(const std::string &var) {
    if(isVarExists(var)){
        return varToNameAndIndex[var].second ;
    }else{
        std::cout<<"Error DynamicKernel::getCoreByVar core doesn't exist"<<std::endl;
        exit(20);
    }
}


/* The old code
void DynamicKernel::addProperty(PropertyAssignment& p) {
	properties.push_back(p);
}

const vector<PropertyAssignment>& DynamicKernel::get_properties() const {
	return properties;
}
*/
