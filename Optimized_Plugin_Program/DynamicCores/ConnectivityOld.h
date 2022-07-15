// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#ifndef DYNAMIC_GENERIC_H
#define DYNAMIC_GENERIC_H
#include "../Kernel/DynamicCore.h"
#include "../Kernel/WitnessSet.h"

using namespace std;
class Connectivity_Witness_Old;

typedef shared_ptr<Connectivity_Witness_Old> Connectivity_Witness_OldPointer;
typedef shared_ptr<Connectivity_Witness_Old const> Connectivity_Witness_OldPointerConst;

class Connectivity_Witness_Old
    : public Witness,
      public enable_shared_from_this<Connectivity_Witness_Old> {
 public:
  bool is_equal_implementation(Connectivity_Witness_OldPointerConst w) const;
  bool is_less_implementation(Connectivity_Witness_OldPointerConst w) const;
  Witness &set_equal_implementation(Connectivity_Witness_OldPointerConst w);
  ~Connectivity_Witness_Old();
  virtual bool is_equal(const Witness &rhs) const override;
  virtual bool is_less(const Witness &rhs) const override;
  virtual Witness &set_equal(const Witness &witness) override;
  virtual shared_ptr<Witness> relabel(
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

class Connectivity_Witness_OldSet
    : public WitnessSetTypeTwo<Connectivity_Witness_OldSet> {
  shared_ptr<WitnessSet> createEmptyWitnessSet() override;
};

typedef shared_ptr<Connectivity_Witness_OldSet> Connectivity_Witness_OldSetPointer;

class Connectivity_DynamicCore_Old : public DynamicCore {
 private:
  void createInitialWitnessSet_implementation();
  void copyWitness(Connectivity_Witness_OldPointer w_input,
                   Connectivity_Witness_OldPointer w_output);
  Connectivity_Witness_OldPointer createWitness();
  void intro_v_implementation(unsigned i, Bag &b,
                              Connectivity_Witness_OldPointerConst w,
                              Connectivity_Witness_OldSetPointer witnessSet);
  void forget_v_implementation(unsigned i, Bag &b,
                               Connectivity_Witness_OldPointerConst w,
                               Connectivity_Witness_OldSetPointer witnessSet);
  void intro_e_implementation(unsigned i, unsigned j, Bag &b,
                              Connectivity_Witness_OldPointerConst w,
                              Connectivity_Witness_OldSetPointer witnessSet);
  void join_implementation(Bag &b, Connectivity_Witness_OldPointerConst w1,
                           Connectivity_Witness_OldPointerConst w2,
                           Connectivity_Witness_OldSetPointer witnessSet);
  bool is_final_witness_implementation(Connectivity_Witness_OldPointerConst w);

 public:
  Connectivity_DynamicCore_Old();
  ~Connectivity_DynamicCore_Old(){};
  void createInitialWitnessSet();
  WitnessSetPointer intro_v(unsigned i, Bag &b, const Witness &witness);
  WitnessSetPointer intro_e(unsigned i, unsigned j, Bag &b,
                                const Witness &witness);
  WitnessSetPointer forget_v(unsigned i, Bag &b,
                                 const Witness &witness);
  WitnessSetPointer join(Bag &b, const Witness &witness1,
                             const Witness &witness2);
  WitnessSetPointer clean(WitnessSetPointer witnessSet);
  bool is_final_witness(const Witness &witness);
  //*****************************
  //*****************************
  // Only change this part. Here you can define attributes and functions that
  // are specific to your core.
  //*****************************
  //*****************************
};

#endif
