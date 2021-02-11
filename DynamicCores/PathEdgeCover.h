#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"
#include <iostream>
#include <vector>

using namespace std;
class PathEdgeCover_Witness;
// Pointer type for Generic_Operator_Witness
typedef shared_ptr<PathEdgeCover_Witness> PathEdgeCover_WitnessPointer;
typedef shared_ptr<PathEdgeCover_Witness const > PathEdgeCover_WitnessPointerConst;
class PathEdgeCover_Witness:public Witness, public enable_shared_from_this<PathEdgeCover_Witness>{
public:
    set<pair<vector<int >, vector<bool> > > partialPathCover; //
    bool is_equal_implementation(const PathEdgeCover_WitnessPointerConst w) const;
    bool is_less_implementation(const PathEdgeCover_WitnessPointerConst w) const;
    Witness& set_equal_implementation(PathEdgeCover_WitnessPointer w);
    PathEdgeCover_Witness(); // Constructor
    ~PathEdgeCover_Witness();
    virtual void print();
    string witnessInformation() override;
    virtual bool is_equal(const Witness &rhs) const; //Comparator
    virtual bool is_less(const Witness &rhs) const;  //Comparator
    virtual Witness& set_equal(Witness &witness); //Assignment operator
    virtual shared_ptr<Witness> relabel(map<unsigned,unsigned> relabelingMap);

};

class PathEdgeCover_WitnessSet : public WitnessSetTypeOne<PathEdgeCover_WitnessSet>{
public:
    shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

// Pointer type for Generic_Operator_WitnessSet.
typedef shared_ptr<PathEdgeCover_WitnessSet> PathEdgeCover_WitnessSetPointer ;

//Defines the dynamic core for PathEdgeCover
class PathEdgeCover_DynamicCore: public DynamicCore {
private:
    void createInitialWitnessSet_implementation();
    void copyWitness(PathEdgeCover_WitnessPointer w_input, PathEdgeCover_WitnessPointer w_output);
    PathEdgeCover_WitnessPointer createWitness();
    void intro_v_implementation(unsigned i, Bag &b, PathEdgeCover_WitnessPointer w, PathEdgeCover_WitnessSetPointer witnessSet);
    void forget_v_implementation(unsigned i, Bag &b, PathEdgeCover_WitnessPointer w, PathEdgeCover_WitnessSetPointer witnessSet);
    void intro_e_implementation(unsigned i,unsigned j, Bag &b, PathEdgeCover_WitnessPointer w, PathEdgeCover_WitnessSetPointer witnessSet);
    void join_implementation(Bag &b, PathEdgeCover_WitnessPointer w1, PathEdgeCover_WitnessPointer w2, PathEdgeCover_WitnessSetPointer witnessSet);
    bool is_final_witness_implementation(PathEdgeCover_WitnessPointer w);
    shared_ptr<WitnessSet> clean_implementation(PathEdgeCover_WitnessSetPointer witnessSet);
    pair<bool, pair<vector<unsigned >, vector<bool>> > combinePartition(pair<vector<unsigned int>, vector<bool>> p1,
                                                                                        pair<vector<unsigned int>, vector<bool>> p2, unsigned width);
    pair<vector<vector<vector<unsigned >>>,vector<vector<vector<bool>>>> combinePartialPathCover(pair<vector<vector<unsigned >>,vector<vector<bool>> > p1,
                                                                                                 pair<vector<vector<unsigned >>,vector<vector<bool>> > p2, unsigned width);
public:
    unsigned parameter; // This is the length of a path, so the path has parameter+1 vertices
    PathEdgeCover_DynamicCore();
    PathEdgeCover_DynamicCore(unsigned parameter);
    ~PathEdgeCover_DynamicCore(){};
    void createInitialWitnessSet();
    WitnessSetPointer intro_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b, Witness &witness);
    WitnessSetPointer forget_v(unsigned i, Bag &b, Witness &witness);
    WitnessSetPointer join(Bag &b, Witness &witness1, Witness &witness2);
    WitnessSetPointer clean(WitnessSetPointer witnessSet);
    bool is_final_witness(Witness &witness);
};

