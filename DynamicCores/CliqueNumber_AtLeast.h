#ifndef DYNAMIC_CLIQUE_H
#define DYNAMIC_CLIQUE_H
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <utility>
#include <algorithm>
using namespace std;
class CliqueNumber_AtLeast_Witness;

typedef shared_ptr<CliqueNumber_AtLeast_Witness> CliqueNumber_AtLeast_WitnessPointer;
typedef shared_ptr<CliqueNumber_AtLeast_Witness const> CliqueNumber_AtLeast_WitnessPointerConst;
class CliqueNumber_AtLeast_Witness: public Witness, public enable_shared_from_this<CliqueNumber_AtLeast_Witness>{
public:
	bool found; // Set to true if and only if a clique of the right size was found
	set<pair<unsigned,unsigned>> edges;
	CliqueNumber_AtLeast_Witness(){};
	~CliqueNumber_AtLeast_Witness(){};
	virtual bool is_equal(const Witness &rhs)const;
	virtual bool is_less(const Witness &rhs)const;
	virtual Witness& set_equal(Witness &witness);
	virtual void print();
	string witnessInformation() override;
	shared_ptr<Witness> relabel(map<unsigned int, unsigned int> relabelingMap) override;
	friend set<unsigned> verticesOnPartialClique(set<pair<int,int>> edges); // Auxiliary Function. Vertices seen in the partial clique.
	friend set<unsigned> neighborsOnPartialClique(set<pair<int,int>> edges, int i); // Auxiliary Function. Neighbors of i seen in the clique.
};


class CliqueNumber_AtLeast_WitnessSet : public WitnessSetTypeOne<CliqueNumber_AtLeast_WitnessSet>{
};

class CliqueNumber_AtLeast_DynamicCore: public DynamicCore{
public:
    unsigned cliqueSize;
    CliqueNumber_AtLeast_DynamicCore();
    CliqueNumber_AtLeast_DynamicCore(unsigned cliqueSize);
    ~CliqueNumber_AtLeast_DynamicCore(){};
    void createInitialWitnessSet();
    virtual WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    virtual WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    virtual WitnessSetPointer forget_v(unsigned i, Bag &b,Witness &witness);
    virtual WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    virtual bool is_final_witness(Witness &witness);
};


#endif
