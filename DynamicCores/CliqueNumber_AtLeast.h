// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_CLIQUE_H
#define DYNAMIC_CLIQUE_H
#include "Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <utility>
#include <algorithm>
using namespace std;
class CliqueNumber_AtLeast_Witness: public Witness, public enable_shared_from_this<CliqueNumber_AtLeast_Witness>{
public:
	bool found; // Set to true if and only if a clique of the right size was found
	set<pair<unsigned,unsigned>> edges;
	~CliqueNumber_AtLeast_Witness();
	virtual bool is_equal(const Witness &rhs)const;
	virtual bool is_less(const Witness &rhs)const;
	virtual Witness& set_equal(Witness &witness);
	virtual void print();
	friend set<unsigned> verticesOnPartialClique(set<pair<int,int>> edges); // Auxiliary Function. Vertices seen in the partial clique.
	friend set<unsigned> neighborsOnPartialClique(set<pair<int,int>> edges, int i); // Auxiliary Function. Neighbors of i seen in the clique.
};

class CliqueNumber_AtLeast_DynamicCore: public DynamicCore{
public:
    unsigned cliqueSize;
    CliqueNumber_AtLeast_DynamicCore(unsigned cliqueSize);
    shared_ptr<WitnessSet> createInitialWitnessSet();
    virtual shared_ptr<WitnessSet> intro_v(unsigned i, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual shared_ptr<WitnessSet> forget_v(unsigned i, Bag &b,Witness &witness);
    virtual shared_ptr<WitnessSet> join(Bag &b, Witness &witness1, Witness &witness2);
    virtual bool is_final_witness(Witness &witness);
};


#endif
