#ifndef HASMULTIPLEEDGES_H
#define HASMULTIPLEEDGES_H

#include <algorithm>
#include "Kernel/DynamicCore.h"

using namespace std;

class HasMultipleEdges_Witness;

typedef shared_ptr<HasMultipleEdges_Witness> HasMultipleEdges_WitnessPointer;
typedef shared_ptr<HasMultipleEdges_Witness const>
	HasMultipleEdges_WitnessPointerConst;
class HasMultipleEdges_Witness
	: public Witness,
	  public enable_shared_from_this<HasMultipleEdges_Witness> {
  public:
	bool is_equal_implementation(
		const HasMultipleEdges_WitnessPointerConst w) const;
	bool is_less_implementation(
		const HasMultipleEdges_WitnessPointerConst w) const;
	Witness &set_equal_implementation(HasMultipleEdges_WitnessPointerConst w);
	HasMultipleEdges_Witness(); // Constructor
	~HasMultipleEdges_Witness();
	virtual bool is_equal(const Witness &rhs) const override;
	virtual bool is_less(const Witness &rhs) const override;
	virtual Witness &set_equal(const Witness &witness) override;
	virtual shared_ptr<Witness> relabel(
		const map<unsigned int, unsigned int> &relabelingMap) const override;
	virtual void print() const override;
	string witnessInformation() const override;
	virtual void hash(Hasher &h) const override;
	//*****************************
	//*****************************
	// Attributes:
	std::set<pair<unsigned, unsigned>> edgeSet;
	bool found;
	//*****************************
	//****************************
};

class HasMultipleEdges_WitnessSet
	: public WitnessSetTypeTwo<HasMultipleEdges_WitnessSet> {
	shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<HasMultipleEdges_WitnessSet>
	HasMultipleEdges_WitnessSetPointer;

class HasMultipleEdges_DynamicCore : public DynamicCore {
	void createInitialWitnessSet_implementation(WitnessSet &witnessSet);
	void copyWitness(HasMultipleEdges_WitnessPointer w_input,
					 HasMultipleEdges_WitnessPointer w_output);
	HasMultipleEdges_WitnessPointer createWitness();
	void intro_v_implementation(unsigned i, const Bag &b,
								HasMultipleEdges_WitnessPointerConst w,
								HasMultipleEdges_WitnessSetPointer witnessSet);
	void forget_v_implementation(unsigned i, const Bag &b,
								 HasMultipleEdges_WitnessPointerConst w,
								 HasMultipleEdges_WitnessSetPointer witnessSet);
	void intro_e_implementation(unsigned i, unsigned j, const Bag &b,
								HasMultipleEdges_WitnessPointerConst w,
								HasMultipleEdges_WitnessSetPointer witnessSet);
	void join_implementation(const Bag &b,
							 HasMultipleEdges_WitnessPointerConst w1,
							 HasMultipleEdges_WitnessPointerConst w2,
							 HasMultipleEdges_WitnessSetPointer witnessSet);
	HasMultipleEdges_WitnessSetPointer clean_implementation(
		HasMultipleEdges_WitnessSetPointer witnessSet);
	bool is_final_witness_implementation(
		HasMultipleEdges_WitnessPointerConst w);

  public:
	unsigned maxDegree;
	HasMultipleEdges_DynamicCore();
	HasMultipleEdges_DynamicCore(const parameterType &parameter);
	void createInitialWitnessSet();
	virtual WitnessSetPointer intro_v(unsigned i, const Bag &b,
									  const Witness &witness) override;
	virtual WitnessSetPointer intro_e(unsigned i, unsigned j, const Bag &b,
									  const Witness &witness) override;
	virtual WitnessSetPointer forget_v(unsigned i, const Bag &b,
									   const Witness &witness) override;
	virtual WitnessSetPointer join(const Bag &b, const Witness &witness1,
								   const Witness &witness2) override;
	virtual WitnessSetPointer clean(WitnessSetPointer witnessSet) override;
	virtual bool is_final_witness(const Bag &b,
								  const Witness &witness) override;
	//*****************************
	//*****************************
	// Only change this part. Here you can define attributes and functions that
	// are specific to your core.
	//*****************************
	//*****************************
};

#endif
