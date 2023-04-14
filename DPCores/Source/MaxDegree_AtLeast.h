#ifndef MAXDEGREE_GREATERTHAN_H
#define MAXDEGREE_GREATERTHAN_H
#include <iostream>
#include <map>
#include "Kernel/DynamicCore.h"

using namespace std;
class MaxDegree_AtLeast_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<MaxDegree_AtLeast_Witness> MaxDegree_AtLeast_WitnessPointer;

typedef shared_ptr<MaxDegree_AtLeast_Witness const>
	MaxDegree_AtLeast_WitnessPointerConst;
// Defines the type of a witness for the core MaxDegree_GreaterThanOrEqual
class MaxDegree_AtLeast_Witness
	: public Witness,
	  public enable_shared_from_this<MaxDegree_AtLeast_Witness> {
  public:
	bool is_equal_implementation(
		const MaxDegree_AtLeast_WitnessPointerConst w) const;
	bool is_less_implementation(
		const MaxDegree_AtLeast_WitnessPointerConst w) const;
	Witness &set_equal_implementation(MaxDegree_AtLeast_WitnessPointerConst w);
	MaxDegree_AtLeast_Witness(); // Constructor
	~MaxDegree_AtLeast_Witness();
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
	map<unsigned, unsigned> degreeCounter; // Counts the number neighbors seen
										   // so far for each vertex of the bag.
	bool found; // Set to true if and only if  a vertex of degree at least
				// maxDeg has been found.
				//*****************************
				//*****************************
};

class MaxDegree_AtLeast_WitnessSet
	: public WitnessSetTypeTwo<MaxDegree_AtLeast_WitnessSet> {
	shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<MaxDegree_AtLeast_WitnessSet>
	MaxDegree_AtLeast_WitnessSetPointer;

class MaxDegree_AtLeast_DynamicCore : public DynamicCore {
	void createInitialWitnessSet_implementation(WitnessSet &witnessSet);
	void copyWitness(MaxDegree_AtLeast_WitnessPointer w_input,
					 MaxDegree_AtLeast_WitnessPointer w_output);
	MaxDegree_AtLeast_WitnessPointer createWitness();
	void intro_v_implementation(unsigned i, const Bag &b,
								MaxDegree_AtLeast_WitnessPointerConst w,
								MaxDegree_AtLeast_WitnessSetPointer witnessSet);
	void forget_v_implementation(
		unsigned i, const Bag &b, MaxDegree_AtLeast_WitnessPointerConst w,
		MaxDegree_AtLeast_WitnessSetPointer witnessSet);
	void intro_e_implementation(unsigned i, unsigned j, const Bag &b,
								MaxDegree_AtLeast_WitnessPointerConst w,
								MaxDegree_AtLeast_WitnessSetPointer witnessSet);
	void join_implementation(const Bag &b,
							 MaxDegree_AtLeast_WitnessPointerConst w1,
							 MaxDegree_AtLeast_WitnessPointerConst w2,
							 MaxDegree_AtLeast_WitnessSetPointer witnessSet);
	MaxDegree_AtLeast_WitnessSetPointer clean_implementation(
		MaxDegree_AtLeast_WitnessSetPointer witnessSet);
	bool is_final_witness_implementation(
		MaxDegree_AtLeast_WitnessPointerConst w);
	int inv_implementation(const Bag &b,
						   MaxDegree_AtLeast_WitnessSetPointer witnessSet);

  public:
	unsigned maxDegree;
	MaxDegree_AtLeast_DynamicCore();
	MaxDegree_AtLeast_DynamicCore(unsigned parameter);
	MaxDegree_AtLeast_DynamicCore(const parameterType &parameter);
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
	virtual int inv(const Bag &b, WitnessSetPointer witnessSet) override;
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
