// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_CLIQUE_H
#define DYNAMIC_CLIQUE_H
#include <algorithm>
#include "../Kernel/WitnessSet.h"
#include "../Kernel/DynamicCore.h"
using namespace std;
class TriangleFreeWidthFour_Witness
	: public Witness,
	  public enable_shared_from_this<TriangleFreeWidthFour_Witness> {
  public:
	uint16_t edges = 0;
	// bit 0 represetnts the boolean found
	// Found is set to true if and only if a clique of the right size was
	// found
	// bits from 1 to 10 represents the edges
	~TriangleFreeWidthFour_Witness();
	virtual bool is_equal(const Witness &rhs) const;
	virtual bool is_less(const Witness &rhs) const;
	virtual Witness &set_equal(Witness &witness);
	virtual void print();
	friend unsigned int verticesOnPartialClique(unsigned int edges); // Auxiliary Function. Vertices seen in the partial clique.
	friend unsigned int neighborsOnPartialClique(unsigned int edges,int i); // Auxiliary Function. Neighbors of i seen in the clique.
};

class TriangleFreeWidthFour_WitnessSet : public WitnessSetTypeOne<TriangleFreeWidthFour_WitnessSet>{
};


class TriangleFreeWidthFour_DynamicCore : public DynamicCore {
  public:
	unsigned cliqueSize;
	TriangleFreeWidthFour_DynamicCore();
    TriangleFreeWidthFour_DynamicCore(unsigned cliqueSize);
	~TriangleFreeWidthFour_DynamicCore(){};
	void createInitialWitnessSet();
	virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
	virtual WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
	virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
	virtual bool is_final_witness(Witness &witness);
};

#endif
