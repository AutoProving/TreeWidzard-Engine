// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

// Copyright 2025 CONTRIBUTORS.

#include "../../CorePrelude.h"

using namespace std;

struct VertexCover_AtMost_Witness : WitnessWrapper<VertexCover_AtMost_Witness>
{
    // S: selected vertices in current bag; s: selected outside the bag so far
    std::set<unsigned> S;
    unsigned s = 0;

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r)
    {
        return l.s == r.s && l.S == r.S;
    }

    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r)
    {
        if (l.S < r.S) return true;
        if (r.S < l.S) return false;
        return l.s < r.s;
    }

    WitnessAlias relabel_implementation(const std::map<unsigned, unsigned> &map) const
    {
        WitnessAlias w; w.s = s; w.S.clear();
        for (auto v : S) {
            auto it = map.find(v);
            if (it == map.end()) {
                std::cerr << "Error: VertexCover relabel: missing label" << std::endl;
                exit(20);
            }
            w.S.insert(it->second);
        }
        return w;
    }

    void hash(Hasher &h) const override
    {
        for (auto v : S) h << v;
        h << -1u; h << s;
    }

    void witness_info(std::ostream &os) const
    {
        os << "S={";
        for (auto it=S.begin(); it!=S.end(); ++it) {
            os << *it; if (next(it)!=S.end()) os << ",";
        }
        os << "}, s=" << s << "\n";
    }
};

struct VertexCover_AtMost_Core : CoreWrapper<VertexCover_AtMost_Core, VertexCover_AtMost_Witness, WitnessSetTypeTwo>
{
    unsigned r;

    static std::map<std::string, std::string> metadata()
    {
        return {
            // Canonical public name: boolean threshold for vertex cover number.
            {"CoreName", "VertexCoverNumber_AtMost"},
            {"CoreType", "Bool"},
            {"ParameterType", "UnsignedInt"},
        };
    }

    VertexCover_AtMost_Core(const parameterType &parameters)
    {
        auto [n] = unpack_typed_args<int>(parameters);
        r = static_cast<unsigned>(n);
    }

    void initialize_leaf(WitnessSet &ws)
    {
        ws.insert(std::make_shared<WitnessAlias>());
    }

    void intro_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws)
    {
        // Branch on whether i is selected into the cover.
        ws.insert(w.clone());
        auto w_selected = w.clone();
        w_selected->S.insert(i);
        ws.insert(std::move(w_selected));
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &, const WitnessAlias &w, WitnessSet &ws)
    {
        // Edge {i,j} must be covered by the current selection.
        if (w.S.count(i) || w.S.count(j)) ws.insert(w.clone());
    }

    void forget_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws)
    {
        auto wf = w.clone();
        // When forgetting a selected vertex, move it out of the bag and increment s
        if (wf->S.erase(i) > 0) {
            wf->s += 1;
        }
        ws.insert(std::move(wf));
    }

    void join_implementation(const Bag &, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &ws)
    {
        // Require bag selections to agree (same S at join)
        if (w1.S != w2.S) return;
        auto wj = std::make_shared<WitnessAlias>();
        wj->S = w1.S; // same
        // s counts already-forgotten selections from each subtree; they are disjoint by TD connectivity
        wj->s = w1.s + w2.s;
        ws.insert(std::move(wj));
    }

    void clean_implementation(WitnessSet &ws)
    {
        // Keep minimal s per S — use hash map for faster lookup
        struct SetHash {
            size_t operator()(const std::set<unsigned> &s) const {
                Hasher h(0);
                for (unsigned v : s) h << v;
                return h.get();
            }
        };
        std::unordered_map<std::set<unsigned>, unsigned, SetHash> best;
        for (const auto &w_ptr : ws) {
            const auto &w = WitnessAlias::as_witness(*w_ptr);
            auto it = best.find(w.S);
            if (it == best.end() || w.s < it->second) best[w.S] = w.s;
        }
        WitnessSetTypeTwo<WitnessAlias> cleaned;
        for (auto &[S, s] : best) {
            auto w = std::make_shared<WitnessAlias>();
            w->S = S; w->s = s;
            cleaned.insert(std::move(w));
        }
        ws.setEqual(cleaned);
    }

    bool is_final_witness_implementation(const Bag &, const WitnessAlias &w)
    {
        // Accept if budget not exceeded
        return (w.s + static_cast<unsigned>(w.S.size())) <= r;
    }
};
