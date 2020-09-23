// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "CliqueNumber_AtLeast.h"

uint16_t countNumOfBitsEqualsToOne(uint16_t value) {
	uint16_t cnt = 0;
	while (value > 0) {
		cnt++;
		value -= value & -value;
	}
	return cnt;
}

uint16_t edgeToInteger(pair<unsigned int, unsigned int> e) {
	int i = e.first;
	int j = e.second;
	return 10 - (((6 - i) * (5 - i)) >> 1) + j - i;
}

pair<uint16_t, uint16_t> integerToEdge(uint16_t value) {
	uint16_t i = 1;
	int16_t x = value;
	if (x - 4 > 0) i = 2;
	if (x - 7 > 0) i = 3;
	if (x - 9 > 0) i = 4;
	if (x - 10 > 0) i = 5;
	uint16_t j = x - 10 + (((6 - i) * (5 - i)) >> 1) + i;
	return make_pair(i, j);
}

void addEdge(uint16_t &edges, pair<uint16_t, uint16_t> e) {
	uint16_t v = edgeToInteger(e);
	edges |= (1 << v);
}

// A is lexicographically less than B
bool smallestThan(uint16_t a, uint16_t b) {
	int lastA = -1;
	int lastB = -1;

	for (int i = 10; i >= 1; i--)
		if ((a & (1 << i)) and lastA == -1) lastA = i;

	for (int i = 10; i >= 1; i--)
		if ((b & (1 << i)) and lastB == -1) lastB = i;

	for (int i = 1; i <= 10; i++) {
		bool bitA = (a & (1 << i));
		bool bitB = (b & (1 << i));
		if (bitA != bitB) {
			if (bitA) return i > lastB;
			if (bitB) return i < lastA;
		}
	}
	return false;
}

bool getFound(uint16_t edges) { return edges & 1; }

void setFound(uint16_t &edges, bool fl) {
	edges |= 1;
	if (fl == false) edges ^= 1;
}

uint16_t sizeSet(uint16_t edges) {
	uint16_t cnt = 0;
	edges &= (~1);
	return countNumOfBitsEqualsToOne(edges);
}

bool isInSet(uint16_t S, uint16_t x) { return S & (1 << x); }

bool CliqueNumber_AtLeast_Witness::is_equal(const Witness &rhs) const {
	if (CliqueNumber_AtLeast_Witness const *e =
			dynamic_cast<CliqueNumber_AtLeast_Witness const *>(&rhs)) {
		shared_ptr<CliqueNumber_AtLeast_Witness const> p =
			e->shared_from_this();
		return edges == p->edges;
	} else {
		cerr << "ERROR: in Dynamic_Clique_Witness::is_equal cast error" << endl;
		exit(20);
	}
}

bool CliqueNumber_AtLeast_Witness::is_less(const Witness &rhs) const {
	if (CliqueNumber_AtLeast_Witness const *e =
			dynamic_cast<CliqueNumber_AtLeast_Witness const *>(&rhs)) {
		shared_ptr<CliqueNumber_AtLeast_Witness const> p =
			e->shared_from_this();
		return getFound(edges) < getFound(p->edges) or
			   (getFound(edges) == getFound(p->edges) and
				smallestThan(edges, p->edges));
	} else {
		cerr << "ERROR: in Dynamic_Clique_Witness::is_less cast error" << endl;
		exit(20);
	}
}

Witness &CliqueNumber_AtLeast_Witness::set_equal(Witness &witness) {
	if (CliqueNumber_AtLeast_Witness *e =
			dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
		shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
		edges = p->edges;
		return *this;
	} else {
		cerr << "ERROR: in Dynamic_Clique_Witness::operator= cast error"
			 << endl;
		exit(20);
	}
}

void CliqueNumber_AtLeast_Witness::print() {
	// cout << "found = " << found << " ";
	// cout<<"E={";
	// for (auto edg : edges)
	//     cout << "(" << edg.first << "," << edg.second << ")";
	// cout<<"}";
}

/*void CliqueNumber_AtLeast_WitnessSet::print() {
	for (WitnessSetIterator it = this->begin() ; it != this->end() ; ++it) {
		(*it)->print();
	}

}*/

/*WitnessSet *CliqueNumber_AtLeast_WitnessSet::createWitnessSet() {
	CliqueNumber_AtLeast_WitnessSet *w = new CliqueNumber_AtLeast_WitnessSet;
	return w;
}*/

/*bool CliqueNumber_AtLeast_WitnessSet::is_equal(WitnessSet &rhs) {
	if (CliqueNumber_AtLeast_WitnessSet *p =
dynamic_cast<CliqueNumber_AtLeast_WitnessSet *>(&rhs)) {
		if(this->container.size() == p->container.size()){
			WitnessSetIterator itr = p->begin();
			for(WitnessSetIterator it = this->begin(); it != this->end() ;
++it){ if((**it == **itr)){ itr++; }else{ return false;
				}
			}
			return true;
		}else{
			return false;
		}
	}else{
		cerr<<"ERROR: in Dynamic_Clique_WitnessSet::is_equal cast error"<<endl;
		exit(20);
	}
}*/

// Auxiliary Function. Vertices seen in the partial clique.
uint16_t verticesOnPartialClique(uint16_t edges) {
	uint16_t aux = 0;
	for (int i = 1; i <= 10; i++) {
		if (edges & (1 << i)) {
			pair<uint16_t, uint16_t> e = integerToEdge(i);
			aux |= (1 << e.first);
			aux |= (1 << e.second);
		}
	}
	return aux;
}

// Auxiliary Function. Neighbors of i seen in the clique.
uint16_t neighborsOnPartialClique(uint16_t edges, int k) {
	uint16_t aux = 0;
	for (int i = 1; i <= 10; i++) {
		if (edges & (1 << i)) {
			pair<uint16_t, uint16_t> e = integerToEdge(i);
			if (e.first == k)
				aux |= (1 << e.second);
			else if (e.second == k)
				aux |= (1 << e.first);
		}
	}
	return aux;
}

CliqueNumber_AtLeast_Witness::~CliqueNumber_AtLeast_Witness() {}

CliqueNumber_AtLeast_DynamicCore::CliqueNumber_AtLeast_DynamicCore(
	unsigned cliqueSize) {
	this->cliqueSize = cliqueSize;
	createInitialWitnessSet();
}

void CliqueNumber_AtLeast_DynamicCore::createInitialWitnessSet() {
	shared_ptr<CliqueNumber_AtLeast_Witness> witness(
		new CliqueNumber_AtLeast_Witness);
	setFound(witness->edges, false);
	insertIntoInitialWitnessSet(witness);
}

shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::intro_v(
	unsigned i, Bag &b, Witness &witness) {
	if (CliqueNumber_AtLeast_Witness *e =
			dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
		shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
		shared_ptr<WitnessSet> witnessSet(new WitnessSet);
		witnessSet->insert(p);
		return witnessSet;
	} else {
		cerr << "ERROR: in Dynamic_Clique::intro_v cast error" << endl;
		exit(20);
	}
}

shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::intro_e(
	unsigned i, unsigned j, Bag &b, Witness &witness) {
	if (CliqueNumber_AtLeast_Witness *e =
			dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
		// shared_ptr<CliqueNumber_AtLeast_Witness> p =
		// make_shared<CliqueNumber_AtLeast_Witness>(*e);
		shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();

		shared_ptr<WitnessSet> witnessSet(new WitnessSet);
		if (getFound(p->edges)) {
			witnessSet->insert(p);
		} else {
			uint16_t aux = p->edges;

			uint16_t vertices = verticesOnPartialClique(aux);

			pair<unsigned int, unsigned int> newedge;
			if (i < j) {
				newedge = make_pair(i, j);
			} else if (j < i) {
				newedge = make_pair(j, i);
			}
			if (isInSet(vertices, i) and isInSet(vertices, j)) {
				// Suppose that both endpoints of the edge are in the partial
				// clique. Then we have no choice. We need to add the edge to
				// the partial clique.

				addEdge(aux, newedge);

				if (countNumOfBitsEqualsToOne(vertices) == cliqueSize and
					sizeSet(aux) == cliqueSize * (cliqueSize - 1) / 2) {
					// if the partial clique gets complete, we are done, meaning
					// that we set found to true and the edgeset to empty.
					shared_ptr<CliqueNumber_AtLeast_Witness> w(
						new CliqueNumber_AtLeast_Witness);
					setFound(w->edges, true);
					witnessSet->insert(w);
				} else {
					// otherwise, found is still false and the edge set is equal
					// to the augmented edge set.
					shared_ptr<CliqueNumber_AtLeast_Witness> w(
						new CliqueNumber_AtLeast_Witness);

					setFound(aux, false);
					w->edges = aux;
					witnessSet->insert(w);
				}
			} else {
				// If one of the endpoints is not in the partial clique, we can
				// choose either to add the edge or not to add.
				witnessSet->insert(p); // here we choose not to add the edge

				addEdge(aux, newedge);

				if (countNumOfBitsEqualsToOne(verticesOnPartialClique(aux)) <=
					cliqueSize) {
					shared_ptr<CliqueNumber_AtLeast_Witness> w(
						new CliqueNumber_AtLeast_Witness);

					setFound(aux, false);
					w->edges = aux;
					witnessSet->insert(w);
				}
			}
		}
		return witnessSet;
	} else {
		cerr << "ERROR: in Dynamic_Clique::intro_e cast error" << endl;
		exit(20);
	}
}

shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::forget_v(
	unsigned i, Bag &b, Witness &witness) {
	if (CliqueNumber_AtLeast_Witness *e =
			dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
		shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
		shared_ptr<WitnessSet> witnessSet(new WitnessSet);

		uint16_t vertices = verticesOnPartialClique(p->edges);

		if (isInSet(vertices, i) == false or getFound(p->edges)) {
			// this means that i does not belong to the clique and therefore it
			// does not affect the witness.
			witnessSet->insert(p); // MAKE SURE THAT p IS A SHARED POINTER
		} else {
			// Otherwise the witness will be affected in some way.
			uint16_t neighbors = neighborsOnPartialClique(p->edges, i);
			if (countNumOfBitsEqualsToOne(neighbors) == cliqueSize - 1) {
				witnessSet->insert(p); // We don't change the witness. Edges
									   // containing is still remain there in.
			}
			// Note that if the if condition is not satisfied, then the witness
			// is deffective, because we tried to delete a vertex before adding
			// all its neighbors on the clique. As a consequence, witnessSet is
			// emtpy (has no witness)
		}
		return witnessSet;
	} else {
		cerr << "ERROR: in Dynamic_Clique::forget_v cast error" << endl;
		exit(20);
	}
}

shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::join(
	Bag &b, Witness &witness1, Witness &witness2) {
	if (CliqueNumber_AtLeast_Witness *e1 =
			dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness1)) {
		if (CliqueNumber_AtLeast_Witness *e2 =
				dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness2)) {
			shared_ptr<CliqueNumber_AtLeast_Witness> p1 =
				e1->shared_from_this();
			shared_ptr<CliqueNumber_AtLeast_Witness> p2 =
				e2->shared_from_this();
			// Note: Do not try to join these two ifs into a single if using
			// AND. Potential for errors.
			shared_ptr<WitnessSet> witnessSet(new WitnessSet);

			uint16_t aux = (p1->edges | p2->edges);
			// set_union(p1->edges.begin(),p1->edges.end(),p2->edges.begin(),p2->edges.end(),std::inserter(aux,aux.begin()));
			uint16_t vertices = verticesOnPartialClique(aux);

			if (getFound(p1->edges)) {
				witnessSet->insert(p1);
			} else if (getFound(p2->edges)) {
				witnessSet->insert(p2);
			} else if (countNumOfBitsEqualsToOne(vertices) == cliqueSize and
					   sizeSet(aux) == cliqueSize * (cliqueSize - 1) / 2) {
				// In this case a clique was found.
				shared_ptr<CliqueNumber_AtLeast_Witness> w(
					new CliqueNumber_AtLeast_Witness);

				setFound(w->edges, true);
				witnessSet->insert(w); //
			} else if (countNumOfBitsEqualsToOne(vertices) <= cliqueSize and
					   sizeSet(aux) != cliqueSize * (cliqueSize - 1) / 2) {
				shared_ptr<CliqueNumber_AtLeast_Witness> w(
					new CliqueNumber_AtLeast_Witness);

				w->edges = aux;
				setFound(w->edges, false);
				witnessSet->insert(w); //
			} else {
				// In this case, the partial clique has more vertices than
				// required and the witness is deffective this implies that
				// witnessSet is empty. (This else has no commands.)
			}
			return witnessSet;
		} else {
			cerr << "ERROR: in Dynamic_Clique::join cast error" << endl;
			exit(20);
		}
	} else {
		cerr << "ERROR: in Dynamic_Clique::join cast error" << endl;
		exit(20);
	}
}

bool CliqueNumber_AtLeast_DynamicCore::is_final_witness(Witness &witness) {
	if (CliqueNumber_AtLeast_Witness *e =
			dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
		shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
		return getFound(p->edges);
	} else {
		cerr << "ERROR: in Dynamic_Clique::is_final_witness cast error" << endl;
		exit(20);
	}
}
