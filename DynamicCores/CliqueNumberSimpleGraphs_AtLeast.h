#ifndef DYNAMIC_CLIQUE_H
#define DYNAMIC_CLIQUE_H
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <utility>
#include <algorithm>
using namespace std;
class CliqueNumberSimpleGraphs_AtLeast_Witness;

typedef shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness> CliqueNumberSimpleGraphs_AtLeast_WitnessPointer;
typedef shared_ptr<CliqueNumberSimpleGraphs_AtLeast_Witness const> CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst;
class CliqueNumberSimpleGraphs_AtLeast_Witness: public Witness, public enable_shared_from_this<CliqueNumberSimpleGraphs_AtLeast_Witness>{
public:
	bool found; // Set to true if and only if a clique of the right size was found
	//set<pair<unsigned,unsigned>> edges;
	map<unsigned,unsigned> partialClique;
	unsigned size; // size of the clique has been found
	CliqueNumberSimpleGraphs_AtLeast_Witness(){};
	~CliqueNumberSimpleGraphs_AtLeast_Witness(){};
    bool is_equal_implementation(const CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w) const;
    bool is_less_implementation(const CliqueNumberSimpleGraphs_AtLeast_WitnessPointerConst w) const;
    Witness& set_equal_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w);
	virtual bool is_equal(const Witness &rhs)const;
	virtual bool is_less(const Witness &rhs)const;
	virtual Witness& set_equal(Witness &witness);
	virtual void print();
	string witnessInformation() override;
	shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
};


class CliqueNumberSimpleGraphs_AtLeast_WitnessSet : public WitnessSetTypeOne<CliqueNumberSimpleGraphs_AtLeast_WitnessSet>{
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;

};
// Pointer type for Generic_Operator_WitnessSet.
typedef shared_ptr<CliqueNumberSimpleGraphs_AtLeast_WitnessSet> CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer ;


class CliqueNumberSimpleGraphs_AtLeast_DynamicCore: public DynamicCore{
private: 
    void createInitialWitnessSet_implementation();
    void copyWitness(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_input, 
            CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w_output);
    CliqueNumberSimpleGraphs_AtLeast_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w,
            CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w,
            CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w,
            CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w1,
            CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w2, CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w);
    CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer clean_implementation(CliqueNumberSimpleGraphs_AtLeast_WitnessSetPointer witnessSet);

    bool isCompleteClique(CliqueNumberSimpleGraphs_AtLeast_WitnessPointer w);
public:
    unsigned cliqueSize;
    CliqueNumberSimpleGraphs_AtLeast_DynamicCore();
    CliqueNumberSimpleGraphs_AtLeast_DynamicCore(unsigned cliqueSize);
    ~CliqueNumberSimpleGraphs_AtLeast_DynamicCore(){};
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b,Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    virtual bool is_final_witness(Witness &witness);
};


#endif
