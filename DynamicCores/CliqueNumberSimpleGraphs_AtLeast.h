#ifndef DYNAMIC_CLIQUE_H
#define DYNAMIC_CLIQUE_H
#include "../Kernel/DynamicCore.h"
#include <utility>
#include <algorithm>
using namespace std;
class CliqueNumberSimpleGraphs_AtLeast_Witness;

typedef shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> CliqueNumberSimpleGraphs_AtLeast_WitnessPointer;
typedef shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness const> CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst;
class CliqueNumberSimpleGraphs_AtLeast_Witness: public Witness, public enable_shared_from_this<CliqueNumberSimpleGraphs_AtLeast_Witness>{
public:
	bool is_equal_implementation(const CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w) const;
	bool is_less_implementation(const CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w) const;
	Witness& set_equal_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w);
	CliqueNumberSimpleGraphs_AtLeast_Witness(); // Constructor
	~CliqueNumberSimpleGraphs_AtLeast_Witness();
	virtual bool is_equal(const Witness &rhs) const override;
	virtual bool is_less(const Witness &rhs) const override;
	virtual Witness& set_equal(const Witness &witness) override;
	virtual shared_ptr<Witness> relabel(const map<unsigned int, unsigned int> &relabelingMap) const override;
	virtual void print() const override;
	string witnessInformation() const override;
	virtual void hash(Hasher &h) const override;

	bool found; // Set to true if and only if a clique of the right size was found
	map<unsigned,unsigned> partialClique;
	unsigned size; // size of the clique has been found
	//216


};


class CliqueNumberSimpleGraphs_AtLeast_WitnessSet: public WitnessSetTypeTwo<CliqueNumberSimpleGraphs_AtLeast_WitnessSet>{
	shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<CliqueNumberSimpleGraphs_AtLeast_WitnessSet> CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer ;

class CliqueNumberSimpleGraphs_AtLeast_DynamicCore: public DynamicCore{

	bool isCompleteClique(CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w);
	void createInitialWitnessSet_implementation();
	void copyWitness(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_input, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_output);
	CliqueNumberSimpleGraphs_AtLeast_WitnessPointer createWitness();
	void intro_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
	void forget_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
	void intro_e_implementation(unsigned i,unsigned j, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
	void join_implementation(Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w1, CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w2, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
	CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer  clean_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
	bool is_final_witness_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w);
public:
	unsigned cliqueSize;
	CliqueNumberSimpleGraphs_AtLeast_DynamicCore();
	CliqueNumberSimpleGraphs_AtLeast_DynamicCore(unsigned parameter);
	CliqueNumberSimpleGraphs_AtLeast_DynamicCore(const parameterType &parameter);
	void createInitialWitnessSet();
	virtual WitnessSetPointer intro_v(unsigned i, Bag &b, const Witness &witness) override;
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, const Witness &witness) override;
	virtual WitnessSetPointer forget_v(unsigned i, Bag &b, const Witness &witness) override;
	virtual WitnessSetPointer join(Bag &b, const Witness &witness1, const Witness &witness2) override;
	virtual WitnessSetPointer clean(WitnessSetPointer witnessSet) override;
	virtual bool is_final_witness(const Witness &witness) override;
	//*****************************
	//*****************************
	// Only change this part. Here you can define attributes and functions that are specific to your core.
	//*****************************
	//*****************************
};


#endif
