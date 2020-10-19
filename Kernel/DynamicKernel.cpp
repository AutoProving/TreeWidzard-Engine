// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "DynamicKernel.h"

void DynamicKernel::addCore(DynamicCore& core) {
      cores.push_back(&core);
}

State::ptr DynamicKernel::initialState() {
	State::ptr initialState;
	Bag emptyBag; // Empty
	for (size_t k = 0; k < cores.size(); ++k) {
		initialState->addWitnessSet(cores[k]->getInitialSet());
	}
	initialState->set_bag(emptyBag);
	return initialState;
}

State::ptr DynamicKernel::intro_v(State::ptr q, unsigned i) {
	if (q->get_bag().vertex_introducible(i)) {
		State::ptr aux;
		Bag b = q->get_bag();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(cores[r]->intro_v(i, b, (q->getWitnessSet(r))));
		}
		aux->set_bag(b.intro_v(i));
		return aux;
	} else {
		cerr << "Error in function DynamicKernel::intro_v. The bag is not i "
				"introducible"
			 << endl;
		exit(10);
	}
}

State::ptr DynamicKernel::intro_e(const State::ptr q, const unsigned i,
								  const unsigned j) {
	if (q->get_bag().edge_introducible(i, j)) {
		State::ptr aux;
		Bag b = q->get_bag();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
				cores[r]->intro_e(i, j, b, (q->getWitnessSet(r))));
		}
		aux->set_bag(b.intro_e(i, j));
		return aux;
	} else {
		cerr << "Error in function DynamicKernel::intro_e. The bag is not i j "
				"introducible"
			 << endl;
		exit(10);
	}
}

State::ptr DynamicKernel::forget_v(State::ptr q, unsigned i) {
	if (q->get_bag().vertex_forgettable(i)) {
		State::ptr aux;
		Bag b = q->get_bag();
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(
				cores[r]->forget_v(i, b, (q->getWitnessSet(r))));
		}
		aux->set_bag(b.forget_v(i));
		return aux;
	} else {
		cerr << "Error in function DynamicKernel::forget_v. The bag is not i "
				"forgettable"
			 << endl;
		exit(10);
	}
}

State::ptr DynamicKernel::join(State::ptr q1, State::ptr q2) {
	if (q1->get_bag().joinable(q2->get_bag())) {
		State::ptr aux;
		set<unsigned> elements = q1->get_bag().get_elements();
		Bag b;
		b.set_elements(elements);
		// in join the new state should not have an edge
		for (size_t r = 0; r < cores.size(); r++) {
			aux->addWitnessSet(cores[r]->join(b, (q1->getWitnessSet(r)), (q2->getWitnessSet(r))));
		}
		aux->set_bag(b);
		return aux;
	} else {
		cerr << "Error in function DynamicKernel::join. The bags are not "
				"joinable"
			 << endl;
		exit(10);
	}
}

// Change Name of this function from core_index to pointerToCoreNumber
DynamicCore* DynamicKernel::pointerToCoreNumber(unsigned i) {
	size_t j = i;
	if (cores.size() - 1 < j) {
		cerr << "ERROR: In DynamicKernel::pointerToCoreNumber, asked index "
				"bigger than the size of cores"
			 << endl;
		exit(20);
	} else {
		return cores[i];
	}
}

Width& DynamicKernel::get_width() { return width; }

void DynamicKernel::set_width(Width& width) { DynamicKernel::width = width; }

void DynamicKernel::addProperty(PropertyAssignment& p) {
	properties.push_back(p);
}

const vector<PropertyAssignment>& DynamicKernel::get_properties() const {
	return properties;
}
