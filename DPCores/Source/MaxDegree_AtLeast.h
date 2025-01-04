// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

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
        if (r.found < l.found)
        {
            return true;
        }
        else if (r.found == l.found)
        {
            if (r.degreeCounter < l.degreeCounter)
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

        map<unsigned, unsigned> m;
        if (found)
        {
            relabeled.found = true;
            return relabeled;
        }
        else
        {
            for (auto element : degreeCounter)
            {
                auto it = relabelingMap.find(element.first);
                if (it != relabelingMap.end())
                {
                    m.insert(make_pair(it->second, element.second));
                }
                else
                {
                    std::cerr << "Error: MaxDegree_AtLeast_Witness::relabel " << element.first << " is not in the map" << endl;
                    std::cerr << " map:";
                    exit(20);
                }
            }
            relabeled.degreeCounter = m;
            relabeled.found = found;
        }

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
        for (auto [i, j] : degreeCounter)
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

        map<unsigned, unsigned> wDegreeCounter = degreeCounter;
        string info;
        info = "{";
        for (map<unsigned, unsigned>::iterator it = wDegreeCounter.begin(); it != wDegreeCounter.end(); it++)
        {
            auto tempIt = it;
            info = info + to_string(it->first) + " -> " + to_string(it->second);
            if (++tempIt != wDegreeCounter.end())
            {
                info = info + ", ";
            }
        }
        info = info + "} " + "found : " + to_string(found);
        os << info;

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
            {"CoreName", "MaxDegree"},
            {"CoreType", "Bool"},
            {"ParameterType", "UnsignedInt"},
            {"PrimaryOperator", "AtLeast"},
        };
    }

    MaxDegree_AtLeast_DynamicCore(const parameterType &parameters)
    {
        auto [n] = unpack_typed_args<int>(parameters);
        maxDegree = n;
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
            witness->degreeCounter = w.degreeCounter;
            witness->degreeCounter.insert(make_pair(i, 0));
            witnessSet.insert(witness);
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
        }
        else if (i != j and w.degreeCounter.find(i)->second < this->maxDegree - 1 and w.degreeCounter.find(j)->second < this->maxDegree - 1)
        {
            auto wPrime = w.clone();
            // wPrime->degreeCounter = w->degreeCounter;
            wPrime->degreeCounter.erase(i);
            wPrime->degreeCounter.erase(j);
            wPrime->degreeCounter.insert(make_pair(i, w.degreeCounter.find(i)->second + 1));
            wPrime->degreeCounter.insert(make_pair(j, w.degreeCounter.find(j)->second + 1));
            wPrime->found = false;
            witnessSet.insert(wPrime);
        }
        else if (i == j and w.degreeCounter.find(i)->second < this->maxDegree - 2)
        {
            // This case is inactive at the moment. It would be active if self-loops were allowed.
            auto wPrime = w.clone();
            // wPrime->degreeCounter = w->degreeCounter;
            wPrime->degreeCounter.erase(i);
            wPrime->degreeCounter.insert(make_pair(i, w.degreeCounter.find(i)->second + 2));
            witnessSet.insert(wPrime);
        }
        else
        {
            auto wPrime = w.clone();
            wPrime->found = true;
            map<unsigned, unsigned> emptyMap;
            wPrime->degreeCounter = emptyMap;
            witnessSet.insert(wPrime);
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
            wPrime->found = false;
            wPrime->degreeCounter = w.degreeCounter;
            wPrime->degreeCounter.erase(i);
            witnessSet.insert(wPrime);
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

        // MaxDegree_AtLeast_WitnessPointer wPrime = createWitness();
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
            map<unsigned, unsigned> temp1 = w1.degreeCounter;
            map<unsigned, unsigned> temp2 = w2.degreeCounter;
            map<unsigned, unsigned> temp;
            bool found = false;
            map<unsigned, unsigned>::iterator it1 = temp1.begin();
            for (map<unsigned, unsigned>::iterator it2 = temp2.begin(); it2 != temp2.end(); ++it2)
            {
                if (it1->second + it2->second < this->maxDegree)
                {
                    temp.insert(make_pair(it1->first, it1->second + it2->second));
                    it1++;
                }
                else
                {
                    found = true;
                    break;
                }
            }
            if (found == false)
            {
                auto wPrime = w1.clone();
                wPrime->found = false;
                wPrime->degreeCounter = temp;
                witnessSet.insert(wPrime);
            }
            else
            {
                auto wPrime = w1.clone();
                wPrime->found = true;
                map<unsigned, unsigned> emptyMap;
                wPrime->degreeCounter = emptyMap;
                witnessSet.insert(wPrime);
            }
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
