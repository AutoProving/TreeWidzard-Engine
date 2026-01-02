// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "../../CorePrelude.h"
#include <cassert>
#include <typeinfo>

using namespace std;

struct IndependentSet_Max_Witness : WitnessWrapper<IndependentSet_Max_Witness>
{
    int size = 0;
    std::set<unsigned> used;
    /*
    Function Definitions
    */
    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r)
    {
        return l.size == r.size && l.used == r.used;
    }

    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r)
    {
        if (l.size != r.size)
            return l.size < r.size;
        if (l.used != r.used)
            return l.used < r.used;
        return false;
    }

    WitnessAlias relabel_implementation(const std::map<unsigned, unsigned> &relabelingMap) const
    {
        auto relabeled = WitnessAlias();
        relabeled.size = size;
        for (unsigned i : used)
            relabeled.used.insert(relabelingMap.at(i));
        return relabeled;
    }
    void hash(Hasher &h) const override
    {
        h << size;
        for (unsigned i : used)
            h << i;
    }
    void witness_info(std::ostream &os) const
    {
        os << "Independent set of size " << size << " using";
        for (unsigned i : used)
            os << ' ' << i;
        os << '\n';
    }
};


struct IndependentSet_Max_Core: CoreWrapper<IndependentSet_Max_Core, IndependentSet_Max_Witness, WitnessSetTypeTwo> {
    /*
    No Attributes for this DP-core
    */
    static std::map<std::string, std::string> metadata() {
        return {
            {"CoreName", "IndependentSet"},
            {"CoreType", "Max"},
            {"ParameterType", "None"},
        };
    }
    
    IndependentSet_Max_Core(const parameterType &parameters) {
        assert(parameters.size() == 0);
    }

    void initialize_leaf(WitnessSet &witnessSet) {
        witnessSet.insert(std::make_shared<WitnessAlias>());
    }

    void intro_v_implementation(unsigned i, const Bag &bag, const WitnessAlias &w, WitnessSet &witnessSet) {
        // First case: ignoring the new label
    
        witnessSet.insert(w.clone());
    
        // Second case: inserting the new label in the independent set
        auto wPrime = w.clone();
        ++wPrime->size;
        wPrime->used.insert(i);
        witnessSet.insert(wPrime);
        
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &bag, const WitnessAlias &w, WitnessSet &witnessSet) {
        if (w.used.count(i) && w.used.count(j))
            return; // invalid partial solution
        witnessSet.insert(w.clone());
    }

    void forget_v_implementation(unsigned i, const Bag &bag, const WitnessAlias &w, WitnessSet &witnessSet) {
        auto wPrime = w.clone();
        wPrime->used.erase(i);
        witnessSet.insert(wPrime);
    }

    void join_implementation(const Bag &bag, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &witnessSet) {
        if (w1.used != w2.used) return;
        std::uint64_t joinedCount = w1.size + w2.size - w1.used.size();
        auto wPrime = w1.clone();
        wPrime->size = joinedCount;
        witnessSet.insert(wPrime);
    }

    bool is_final_witness_implementation(const Bag &b, const WitnessAlias &w) {
        return true;
    }

    void clean_implementation(WitnessSet &witnessSet) {
        // Step 1: Extract all witnesses into a vector for reordering
        auto witnesses = std::vector<std::shared_ptr<WitnessAlias>>();
        for (const auto &w : witnessSet)
            witnesses.push_back(std::dynamic_pointer_cast<WitnessAlias>(w));
    
        // Step 2: Sort witnesses by (used set, decreasing size).
        // This ensures that for each fixed "used" pattern we keep the witness with maximal size.
        std::sort(
            witnesses.begin(),
            witnesses.end(),
            [](const auto &w0, const auto &w1) {
                if (w0->used != w1->used) {
                    return w0->used < w1->used;
                }
                return w0->size > w1->size;
            }
        );
    
        // Step 3: Remove redundant witnesses
        // For each unique set of used labels, keep only the first (largest set size)
        witnesses.erase(std::unique(witnesses.begin(), witnesses.end(),
                                    [](const auto &w0, const auto &w1) {
                                        return w0->used == w1->used;
                                    }),
                        witnesses.end());
    
        // Step 4: Replace the original witness set with the cleaned one
        witnessSet = WitnessSet();
        for (auto &w : witnesses)
            witnessSet.insert(std::move(w));
    }

    int inv_implementation(const Bag &bag, const WitnessAlias &w) const {
        return w.size;
    }
};
