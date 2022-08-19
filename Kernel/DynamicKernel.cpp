#include "DynamicKernel.h"

void DynamicKernel::addCore(DynamicCore& core) {
      cores.push_back(&core);
}

size_t DynamicKernel::coreSize() {
    return cores.size();
}
State::ptr DynamicKernel::initialState() {
	State *initialState = new State;
	Bag emptyBag; // Empty
	for (size_t k = 0; k < cores.size(); ++k) {
		initialState->addWitnessSet(cores[k]->getInitialSet());
	}
	initialState->set_bag(emptyBag);
	return std::shared_ptr<const State>(initialState);
}

State::ptr DynamicKernel::intro_v(State::ptr q, unsigned i) {
	if (q->get_bag().vertex_introducible(i)) {
		State *aux = new State;
		Bag b = q->get_bag();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(cores[r]->intro_v(i, b, (q->getWitnessSet(r))));
		}
		aux->set_bag(b.intro_v(i));
		return std::shared_ptr<const State>(aux);
	} else {
		std::cerr << "Error in function DynamicKernel::intro_v. The bag is not "<<i<<
				" introducible"
			 << std::endl;
		exit(10);
	}
}

State::ptr DynamicKernel::intro_e(const State::ptr q, const unsigned i,
								  const unsigned j) {
	if (q->get_bag().edge_introducible(i, j)) {
		State *aux = new State;
		Bag b = q->get_bag();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
				cores[r]->intro_e(i, j, b, (q->getWitnessSet(r))));
		}
		aux->set_bag(b.intro_e(i, j));
		return std::shared_ptr<const State>(aux);
	} else {
		std::cerr << "Error in function DynamicKernel::intro_e. The bag is not "<<i <<", "<<j<<
				" introducible"
			 << std::endl;
		exit(10);
	}
}

State::ptr DynamicKernel::forget_v(State::ptr q, unsigned i) {
	if (q->get_bag().vertex_forgettable(i)) {
		State *aux = new State;
		Bag b = q->get_bag();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
				cores[r]->forget_v(i, b, (q->getWitnessSet(r))));
		}
		aux->set_bag(b.forget_v(i));
		return std::shared_ptr<const State>(aux);
	} else {
		std::cerr << "Error in function DynamicKernel::forget_v. The bag is not "<<i<<
				" forgettable"
			 << std::endl;
		exit(10);
	}
}

State::ptr DynamicKernel::join(State::ptr q1, State::ptr q2) {
	if (q1->get_bag().joinable(q2->get_bag())) {
		State *aux = new State;
		std::set<unsigned> elements = q1->get_bag().get_elements();
		Bag b;
		b.set_elements(elements);
		// in join the new state should not have an edge
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(cores[r]->join(b, (q1->getWitnessSet(r)), (q2->getWitnessSet(r))));
		}
		aux->set_bag(b);
		return std::shared_ptr<const State>(aux);
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

void DynamicKernel::set_width(Width& width) { DynamicKernel::width = width; }


bool DynamicKernel::isVarExists(std::string var) {
    if(varToNameAndIndex.count(var)){
        return true;
    }else{
        return false;
    }
}

const std::map<std::string, std::pair<std::string, int>> &DynamicKernel::getVarToNameAndIndex() const {
    return varToNameAndIndex;
}

void DynamicKernel::setVarToNameAndIndex(const std::map<std::string, std::pair<std::string, int>> &varToNameAndIndex) {
    DynamicKernel::varToNameAndIndex = varToNameAndIndex;
}


DynamicCore* DynamicKernel::getCoreByVar(std::string var) {
    if(isVarExists(var)){
        return cores[varToNameAndIndex[var].second] ;
    }else{
        std::cout<<"Error DynamicKernel::getCoreByVar core doesn't exist"<<std::endl;
        exit(20);
    }
}

int DynamicKernel::getIndexByVar(std::string var) {
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
