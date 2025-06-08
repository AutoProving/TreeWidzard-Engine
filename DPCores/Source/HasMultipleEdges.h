// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "../../CorePrelude.h"

using namespace std;

struct HasMultipleEdges_Witness : WitnessWrapper<HasMultipleEdges_Witness>
{
    //////
    std::set<pair<unsigned, unsigned>> edgeSet;
    bool found = false;
    //////

    /**
     * Check if the witnesses l and r are equal.
     */
    friend bool is_equal_implementation(const WitnessAlias &l,
                                        const WitnessAlias &r)
    {
        return l.found == r.found and l.edgeSet == r.edgeSet;
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
            if (l.edgeSet < r.edgeSet)
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

        if (this->found)
        {
            relabeled.found = true;
            return relabeled;
        }
        else
        {
            set<pair<unsigned, unsigned>> newEdgeSet;
            for (auto p : edgeSet)
            {
                auto it = relabelingMap.find(p.first);
                auto itr = relabelingMap.find(p.second);
                if (it != relabelingMap.end() and itr != relabelingMap.end())
                {
                    if (it->second < itr->second)
                    {
                        newEdgeSet.insert(make_pair(it->second, itr->second));
                    }
                    else
                    {
                        newEdgeSet.insert(make_pair(itr->second, it->second));
                    }
                }
                else
                {
                    std::cerr << "Error: HasMultipleEdges_Witness::relabel_implementation " << p.first << " or " << p.second << " is not in the map" << endl;
                    print();
                    std::cerr << "\nmap" << endl;
                    for (auto item : relabelingMap)
                    {
                        std::cerr <<item.first << "->" << item.second << endl;
                    }
                    exit(20);
                }
            }
            relabeled.edgeSet = newEdgeSet;
            relabeled.found = false;
            return relabeled;
        }
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
        for (auto p : edgeSet)
        {
            h << p.first << p.second;
        }
        h << -1u;
    }

    /**
     * Get a human readable string
     * representing the witness.
     */
    void witness_info(std::ostream &os) const
    {

        os << "HasMultipleEdges Witness:";
        string info;
        info = "{";
        for (set<pair<unsigned, unsigned>>::iterator it = edgeSet.begin(); it != edgeSet.end(); ++it)
        {
            info = info + "(" + to_string(it->first) + "," + to_string(it->second) + "),";
        }
        info = info + "} found: " + to_string(this->found);
        os << info;
        os << "\n";
    }
};

struct HasMultipleEdges_DynamicCore
    : CoreWrapper<HasMultipleEdges_DynamicCore, HasMultipleEdges_Witness, WitnessSetTypeTwo>
{
    static std::map<std::string, std::string> metadata()
    {
        return {
            {"CoreName", "HasMultipleEdges"},
            {"CoreType", "Bool"},
            {"ParameterType", "ParameterLess"},
            // {"PrimaryOperator", "---"},
        };
    }

    HasMultipleEdges_DynamicCore(const parameterType &parameters)
    {
        // auto [n] = unpack_typed_args<int>(parameters);
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
    void intro_v_implementation(unsigned , const Bag &, const WitnessAlias &w,
                                WitnessSet &witnessSet)
    {
        auto witness = w.clone();
        witnessSet.insert(witness);
    }

    /**
     * Insert what the witness w becomes
     * after inserting a new edge between i and j,
     * into the witnessSet.
     */
    void intro_e_implementation(unsigned int i, unsigned int j, const Bag &,
                                const WitnessAlias &w, WitnessSet &witnessSet)
    {

        if (i != j and !w.found)
        {
            pair<int, int> newEdge;
            if (i < j)
            {
                newEdge = make_pair(i, j);
            }
            else
            {
                newEdge = make_pair(j, i);
            }
            if (w.edgeSet.find(newEdge) != w.edgeSet.end())
            {
                auto witness = w.clone();
                witness->found = true;
                witness->edgeSet.clear();
                witnessSet.insert(witness);
            }
            else
            {
                auto witness = w.clone();
                witness->found = false;
                witness->edgeSet = w.edgeSet;
                witness->edgeSet.insert(newEdge);
                witnessSet.insert(witness);
            }
        }
        else
        {
            witnessSet.insert(w.clone());
        }
    }

    /**
     * Insert what the witness w becomes
     * after forgetting the label of the vertex
     * currently labeled i into the witnessSet.
     */
    void forget_v_implementation(unsigned int i, const Bag &b,
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
            auto witness = w.clone();
            witness->found = w.found;
            witness->edgeSet = w.edgeSet;
            set<unsigned> tempBag = b.get_elements();
            for (set<unsigned>::iterator it = tempBag.begin(); it != tempBag.end(); it++)
            {
                pair<unsigned, unsigned> e = make_pair(i, *it);
                witness->edgeSet.erase(e);
                e = make_pair(*it, i);
                witness->edgeSet.erase(e);
                witnessSet.insert(witness);
            }
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
            set<pair<unsigned, unsigned>> intersection_set;
            set<pair<unsigned, unsigned>> wOneSet = w1.edgeSet;
            set<pair<unsigned, unsigned>> wTwoSet = w2.edgeSet;
            auto witness = w1.clone();

            set_intersection(wOneSet.begin(), wOneSet.end(), wTwoSet.begin(), wTwoSet.end(), std::inserter(intersection_set, intersection_set.begin()));
            if (intersection_set.size() > 0)
            {
                witness->found = true;
                witness->edgeSet.clear();
                witnessSet.insert(witness);
            }
            else
            {
                set<pair<unsigned, unsigned>> union_set;
                set<pair<unsigned, unsigned>> wOneSet = w1.edgeSet;
                set<pair<unsigned, unsigned>> wTwoSet = w2.edgeSet;
                set_union(wOneSet.begin(), wOneSet.end(), wTwoSet.begin(), wTwoSet.end(), std::inserter(union_set, union_set.begin()));
                witness->found = false;
                witness->edgeSet = union_set;
                witnessSet.insert(witness);
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
