// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_GENERIC_H
#define DYNAMIC_GENERIC_H
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class Connectivity_Witness;

typedef shared_ptr<Connectivity_Witness> Connectivity_WitnessPointer;
typedef shared_ptr<Connectivity_Witness const> Connectivity_WitnessPointerConst;

class Connectivity_Witness
    : public WitnessBase,
      public enable_shared_from_this<Connectivity_Witness> {
 public:
  bool is_equal_implementation(Connectivity_WitnessPointerConst w) const;
  bool is_less_implementation(Connectivity_WitnessPointerConst w) const;
  WitnessBase &set_equal_implementation(Connectivity_WitnessPointerConst w);
  ~Connectivity_Witness();
  virtual bool is_equal(const WitnessBase &rhs) const override;
  virtual bool is_less(const WitnessBase &rhs) const override;
  virtual WitnessBase &set_equal(const WitnessBase &witness) override;
  virtual shared_ptr<WitnessBase> relabel(
      const map<unsigned, unsigned> &relabelingMap) const override;
  virtual void print() const override;
  string witnessInformation() const override;
  virtual void hash(Hasher &h) const override;
  //*****************************
  //*****************************
  set<set<unsigned>> partition;
  uint8_t tag = 0;
  //*****************************
  //*****************************
};

class Connectivity_WitnessSet
    : public WitnessSetTypeTwo<Connectivity_WitnessSet> {
  shared_ptr<WitnessSetBase> createEmptyWitnessSet() override;
};

typedef shared_ptr<Connectivity_WitnessSet> Connectivity_WitnessSetPointer;

class Connectivity_DynamicCore : public DynamicCore {
 private:
  void createInitialWitnessSet_implementation();
  void copyWitness(Connectivity_WitnessPointer w_input,
                   Connectivity_WitnessPointer w_output);
  Connectivity_WitnessPointer createWitness();
  void intro_v_implementation(unsigned i, Bag &b,
                              Connectivity_WitnessPointerConst w,
                              Connectivity_WitnessSetPointer witnessSet);
  void forget_v_implementation(unsigned i, Bag &b,
                               Connectivity_WitnessPointerConst w,
                               Connectivity_WitnessSetPointer witnessSet);
  void intro_e_implementation(unsigned i, unsigned j, Bag &b,
                              Connectivity_WitnessPointerConst w,
                              Connectivity_WitnessSetPointer witnessSet);
  void join_implementation(Bag &b, Connectivity_WitnessPointerConst w1,
                           Connectivity_WitnessPointerConst w2,
                           Connectivity_WitnessSetPointer witnessSet);
  bool is_final_witness_implementation(Connectivity_WitnessPointerConst w);

 public:
  Connectivity_DynamicCore();
  ~Connectivity_DynamicCore(){};
  void createInitialWitnessSet();
  WitnessSetBasePointer intro_v(unsigned i, Bag &b, const WitnessBase &witness);
  WitnessSetBasePointer intro_e(unsigned i, unsigned j, Bag &b,
                                const WitnessBase &witness);
  WitnessSetBasePointer forget_v(unsigned i, Bag &b,
                                 const WitnessBase &witness);
  WitnessSetBasePointer join(Bag &b, const WitnessBase &witness1,
                             const WitnessBase &witness2);
  WitnessSetBasePointer clean(WitnessSetBasePointer witnessSet);
  bool is_final_witness(const WitnessBase &witness);
  //*****************************
  //*****************************
  // Only change this part. Here you can define attributes and functions that
  // are specific to your core.
  //*****************************
  //*****************************
};

#endif
