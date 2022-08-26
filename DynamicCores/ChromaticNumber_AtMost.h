// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef CHROMATICNUMBER_LESSTHAN_H
#define CHROMATICNUMBER_LESSTHAN_H
#include "DynamicCore.h"
#include <map>
using namespace std;

class ChromaticNumber_AtMost_Witness;

typedef shared_ptr<ChromaticNumber_AtMost_Witness> ChromaticNumber_AtMost_WitnessPointer;
typedef shared_ptr<ChromaticNumber_AtMost_Witness const> ChromaticNumber_AtMost_WitnessPointerConst;
class ChromaticNumber_AtMost_Witness: public Witness, public enable_shared_from_this<ChromaticNumber_AtMost_Witness> {
public:
    //std::map<unsigned,unsigned> coloring; //The first coordinate of each pair is a vertex and the second a color.
    set<set<unsigned>> partialColoring;
    //unsigned numberUsedColors;
    bool is_equal_implementation(const ChromaticNumber_AtMost_WitnessPointerConst w) const;
    bool is_less_implementation(const ChromaticNumber_AtMost_WitnessPointerConst w) const;
    Witness& set_equal_implementation(ChromaticNumber_AtMost_WitnessPointerConst w);
    ChromaticNumber_AtMost_Witness(); // Constructor
    ~ChromaticNumber_AtMost_Witness();
    virtual bool is_equal(const Witness &rhs) const override;
    virtual bool is_less(const Witness &rhs) const override;
    virtual Witness& set_equal(const Witness &witness) override;
    virtual shared_ptr<Witness> relabel(const map<unsigned int, unsigned int> &relabelingMap) const override;
    virtual void print() const override;
    string witnessInformation() const override;
    virtual void hash(Hasher &h) const override;
};


class ChromaticNumber_AtMost_WitnessSet: public WitnessSetTypeTwo<ChromaticNumber_AtMost_WitnessSet>{
  shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<ChromaticNumber_AtMost_WitnessSet> ChromaticNumber_AtMost_WitnessSetPointer ;

class ChromaticNumber_AtMost_DynamicCore: public DynamicCore{
  void createInitialWitnessSet_implementation();
  void copyWitness(ChromaticNumber_AtMost_WitnessPointer w_input, ChromaticNumber_AtMost_WitnessPointer w_output);
  ChromaticNumber_AtMost_WitnessPointer createWitness();
  void intro_v_implementation(unsigned i, Bag &b, ChromaticNumber_AtMost_WitnessPointerConst w, ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
  void forget_v_implementation(unsigned i, Bag &b, ChromaticNumber_AtMost_WitnessPointerConst w, ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
  void intro_e_implementation(unsigned i,unsigned j, Bag &b, ChromaticNumber_AtMost_WitnessPointerConst w, ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
  void join_implementation(Bag &b, ChromaticNumber_AtMost_WitnessPointerConst w1, ChromaticNumber_AtMost_WitnessPointerConst w2, ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
  ChromaticNumber_AtMost_WitnessSetPointer  clean_implementation(ChromaticNumber_AtMost_WitnessSetPointer witnessSet);
  bool is_final_witness_implementation(ChromaticNumber_AtMost_WitnessPointerConst w);
public:
  unsigned k;
  ChromaticNumber_AtMost_DynamicCore();
  ChromaticNumber_AtMost_DynamicCore(unsigned parameter);
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
