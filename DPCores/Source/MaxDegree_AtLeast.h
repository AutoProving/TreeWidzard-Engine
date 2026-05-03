// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.


#include "../../CorePrelude.h"

using namespace std;

struct MaxDegree_AtLeast_Witness : WitnessWrapper<MaxDegree_AtLeast_Witness>
{
    //////

    map<unsigned, unsigned> degreeCounter; // Counts the number neighbors seen so far for each vertex of the bag.
    bool found = false;                    // Set to true if and only if  a vertex of degree at least maxDeg has been found.
    //////

    /**
     * Check if the witnesses l and r are equal.
     */
    friend bool is_equal_implementation(const WitnessAlias &l,
                                        const WitnessAlias &r)
    {
        return l.found == r.found and l.degreeCounter == r.degreeCounter;
    }

    /**
     * Check if the witness l is "less" than r.
     * Less here can mean anything
     * as long as it's a strict total order.
     */
    friend bool is_less_implementation(const WitnessAlias &l,
                                       const WitnessAlias &r)
    {
        if (l.found < r.found)
        {
            return true;
        }
        else if (l.found == r.found)
        {
            if (l.degreeCounter < r.degreeCounter)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Optional
     * used for isomorphism search and relabeling search,
     * but not regular bfs.
     *
     * Create a copy of the witness where the vertices
     * are renamed according to the relabeling map.
     */
    WitnessAlias relabel_implementation(
        const map<unsigned, unsigned> &relabelingMap) const
    {
        auto relabeled = WitnessAlias();

        if (found)
        {
            relabeled.found = true;
            return relabeled;
        }

        map<unsigned, unsigned> m;
        for (const auto &[vertex, degree] : degreeCounter)
        {
            auto it = relabelingMap.find(vertex);
            if (it != relabelingMap.end())
            {
                m.emplace(it->second, degree);
            }
            else
            {
                std::cerr << "Error: MaxDegree_AtLeast_Witness::relabel " << vertex << " is not in the map" << endl;
                std::cerr << " map:";
                exit(20);
            }
        }
        relabeled.degreeCounter = std::move(m);
        relabeled.found = found;

        return relabeled;
    }

    /**
     * Optional
     * Is used for the parallel versions of atp.
     *
     * Feed hashable tokens to the hasher object,
     * to create a sequence that uniquely determines
     * the value of the witness.
     *
     * If two witnesses are equal they should supply
     * the same sequence to the hasher, and if they
     * are different they should probably supply
     * different hashes.
     */
    void hash(Hasher &h) const override
    {

        h << found;
        h << -1u;
        for (const auto &[i, j] : degreeCounter)
        {
            h << i << j;
        }
        h << -1u;
    }

    /**
     * Get a human readable string
     * representing the witness.
     */
    void witness_info(std::ostream &os) const
    {
        os << "{";
        for (auto it = degreeCounter.begin(); it != degreeCounter.end(); ++it)
        {
            auto tempIt = it;
            os << it->first << " -> " << it->second;
            if (++tempIt != degreeCounter.end())
            {
                os << ", ";
            }
        }
        os << "} found : " << found;
        os << "\n";
    }
};

struct MaxDegree_AtLeast_DynamicCore
    : CoreWrapper<MaxDegree_AtLeast_DynamicCore, MaxDegree_AtLeast_Witness, WitnessSetTypeTwo>
{
    unsigned maxDegree;

    static std::map<std::string, std::string> metadata()
    {
        return {
            // Canonical public name: boolean threshold for maximum degree.
            {"CoreName", "MaximumDegree_AtLeast"},
            {"CoreType", "Bool"},
            {"ParameterType", "UnsignedInt"},
            // {"PrimaryOperator", "AtLeast"},
        };
    }

    MaxDegree_AtLeast_DynamicCore(const parameterType &parameters)
    {
        auto [n] = unpack_typed_args<int>(parameters);
        maxDegree = static_cast<unsigned>(n);
    }

    /**
     * Initialize the witnessset corresponding to leaf
     * nodes in the decomposition.
     */
    void initialize_leaf(WitnessSet &witnessSet)
    {
        witnessSet.insert(make_shared<WitnessAlias>());
    }

    /**
     * Insert what the witness w becomes
     * after inserting the new vertex i,
     * into the witnessSet.
     *
     * Multiple results can be inserted if there are multiple
     * choices for what to do with the new vertex, and if there
     * is no way to get a valid witness after inserting the vertex,
     * insert no results.
     */
    void intro_v_implementation(unsigned i, const Bag &, const WitnessAlias &w,
                                WitnessSet &witnessSet)
    {
        if (w.found)
        {
            witnessSet.insert(w.clone());
        }
        else
        {
            auto witness = w.clone();
            witness->degreeCounter.emplace(i, 0);
            witnessSet.insert(std::move(witness));
        }
    }

    /**
     * Insert what the witness w becomes
     * after inserting a new edge between i and j,
     * into the witnessSet.
     */
    void intro_e_implementation(unsigned int i, unsigned int j, const Bag &,
                                const WitnessAlias &w, WitnessSet &witnessSet)
    {

        if (w.found)
        {
            witnessSet.insert(w.clone());
            return;
        }

        const auto it_i = w.degreeCounter.find(i);
        if (i != j)
        {
            const auto it_j = w.degreeCounter.find(j);
            if (it_i->second < this->maxDegree - 1 &&
                it_j->second < this->maxDegree - 1)
            {
                auto wPrime = w.clone();
                auto prime_i = wPrime->degreeCounter.find(i);
                auto prime_j = wPrime->degreeCounter.find(j);
                ++prime_i->second;
                ++prime_j->second;
                witnessSet.insert(std::move(wPrime));
            }
            else
            {
                auto wPrime = w.clone();
                wPrime->found = true;
                wPrime->degreeCounter.clear();
                witnessSet.insert(std::move(wPrime));
            }
            return;
        }

        if (it_i->second < this->maxDegree - 2)
        {
            // This case is inactive at the moment. It would be active if self-loops were allowed.
            auto wPrime = w.clone();
            auto prime_i = wPrime->degreeCounter.find(i);
            prime_i->second += 2;
            witnessSet.insert(std::move(wPrime));
        }
        else
        {
            auto wPrime = w.clone();
            wPrime->found = true;
            wPrime->degreeCounter.clear();
            witnessSet.insert(std::move(wPrime));
        }
    }

    /**
     * Insert what the witness w becomes
     * after forgetting the label of the vertex
     * currently labeled i into the witnessSet.
     */
    void forget_v_implementation(unsigned int i, const Bag &,
                                 const WitnessAlias &w,
                                 WitnessSet &witnessSet)
    {

        if (w.found)
        {
            auto witness = w.clone();
            witnessSet.insert(witness);
        }
        else
        {
            auto wPrime = w.clone();
            wPrime->degreeCounter.erase(i);
            witnessSet.insert(std::move(wPrime));
        }
    }

    /**
     * Optional
     * Is used for treewidth, but not for pathwidth.
     *
     * Insert what the witness results from joining
     * w1 and w2 into the witnessSet.
     */
    void join_implementation(const Bag &, const WitnessAlias &w1,
                             const WitnessAlias &w2, WitnessSet &witnessSet)
    {
        if (w1.found)
        {
            witnessSet.insert(w1.clone());
        }
        else if (w2.found)
        {
            witnessSet.insert(w2.clone());
        }
        else
        {
            // Validate that both witnesses have the same vertex set (same bag)
            if (w1.degreeCounter.size() != w2.degreeCounter.size())
            {
                return; // incompatible witnesses
            }

            auto wPrime = std::make_shared<WitnessAlias>();
            bool found = false;

            auto it1 = w1.degreeCounter.begin();
            auto it2 = w2.degreeCounter.begin();
            while (it1 != w1.degreeCounter.end() && it2 != w2.degreeCounter.end())
            {
                // Verify both iterators refer to the same vertex
                if (it1->first != it2->first)
                {
                    return; // different vertex sets, incompatible
                }

                unsigned combined = it1->second + it2->second;
                if (combined >= this->maxDegree)
                {
                    found = true;
                    break;
                }
                wPrime->degreeCounter.emplace(it1->first, combined);
                ++it1;
                ++it2;
            }

            if (found)
            {
                wPrime->found = true;
                wPrime->degreeCounter.clear();
            }
            witnessSet.insert(std::move(wPrime));
        }
    }

    /**
     * Remove redundant witnesses from the witnessSet
     */
    void clean_implementation(WitnessSet &)
    {
        // In most cases, you will not need to change this function.
    }

    /**
     * Return whether w is a final witness
     */
    bool is_final_witness_implementation(const Bag &, const WitnessAlias &w)
    {
        return w.found;
    }
};
