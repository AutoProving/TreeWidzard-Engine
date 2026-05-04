// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

// Copyright 2025

#include "../../CorePrelude.h"
#include <set>
#include <map>
#include <vector>

// A tiny proof-of-concept DP-core for Minor Containment of H=K2 (a single edge).
// State carries:
//  - tau: partial placement of the 2 pattern vertices {0,1} on boundary labels (or -1 for none)
//  - UF connectivity among boundary labels induced by introduced host edges
// Accept if there exist labels u,v with tau[u]=0 and tau[v]=1 that are connected in UF
// (i.e., an edge/path between them has been realized below).

struct MinorK2_Witness : WitnessWrapper<MinorK2_Witness>
{
    // tau[u] in {-1,0,1}
    std::map<unsigned,int> tau;
    // simple union-find over active labels in bag
    std::map<unsigned,unsigned> parent;
    // sticky accept flag: once true, stays true across forget/join
    bool ok = false;

    static unsigned find_root(const std::map<unsigned,unsigned>& parent, unsigned x) {
        auto it = parent.find(x);
        if (it==parent.end()) return x;
        unsigned r = it->second;
        while (true) {
            auto jt = parent.find(r);
            if (jt==parent.end()) return r;
            if (jt->second == r) return r;
            r = jt->second;
        }
    }

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        return l.tau == r.tau && l.parent == r.parent && l.ok == r.ok;
    }
    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        if (l.tau != r.tau) return l.tau < r.tau;
        if (l.parent != r.parent) return l.parent < r.parent;
        return l.ok < r.ok;
    }
    WitnessAlias relabel_implementation(const std::map<unsigned, unsigned> &relabel) const {
        auto w = WitnessAlias();
        for (auto [u,x] : tau) w.tau[relabel.at(u)] = x;
        for (auto [u,p] : parent) {
            unsigned nu = relabel.at(u);
            unsigned np = relabel.at(p);
            w.parent[nu] = np;
        }
        w.ok = ok;
        return w;
    }
    void hash(Hasher &h) const override {
        for (auto [u,x] : tau) { h<<u; h<<x; }
        for (auto [u,p] : parent) { h<<u; h<<p; }
        h<< (ok? 1u : 0u);
    }
    void witness_info(std::ostream &os) const {
        os << "tau:";
        for (auto [u,x] : tau) os << " ("<<u<<":"<<x<<")";
        os << " parents:";
        for (auto [u,p] : parent) os << " ("<<u<<"->"<<p<<")";
        os << " ok:" << (ok? "1":"0");
        os << "\n";
    }
};

struct MinorContainment_K2_Core : CoreWrapper<MinorContainment_K2_Core, MinorK2_Witness, WitnessSetTypeTwo>
{
    static std::map<std::string, std::string> metadata() {
        return {
            // Canonical public name: boolean structural property.
            {"CoreName","ContainsK2Minor"},
            {"CoreType","Decision"},
            {"ParameterType","None"}
        };
    }

    MinorContainment_K2_Core(const parameterType &params) {
        // no params needed
        (void)params;
    }

    void initialize_leaf(WitnessSet &ws) {
        ws.insert(std::make_shared<WitnessAlias>());
    }

    void intro_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        // Case 1: keep u as internal (tau=-1)
        auto a = w.clone();
        if (!a->parent.count(i)) a->parent[i]=i;
        if (!a->tau.count(i)) a->tau[i] = -1;
        ws.insert(a);
        // Case 2: place pattern vertex 0 at i
        auto b = w.clone();
        b->parent[i]=i; b->tau[i]=0; ws.insert(b);
        // Case 3: place pattern vertex 1 at i
        auto c = w.clone();
        c->parent[i]=i; c->tau[i]=1; ws.insert(c);
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        // union(i,j)
        auto a = w.clone();
        unsigned ri = MinorK2_Witness::find_root(a->parent,i);
        unsigned rj = MinorK2_Witness::find_root(a->parent,j);
        if (ri!=rj) a->parent[ri]=rj;
        // update sticky ok if any 0 and 1 become connected
        std::vector<unsigned> zeros, ones;
        for (auto [u,x] : a->tau) {
            if (x==0) zeros.push_back(u);
            else if (x==1) ones.push_back(u);
        }
        bool conn=false;
        for (unsigned u : zeros) {
            unsigned ru = MinorK2_Witness::find_root(a->parent,u);
            for (unsigned v : ones) {
                unsigned rv = MinorK2_Witness::find_root(a->parent,v);
                if (ru==rv) { conn=true; break; }
            }
            if (conn) break;
        }
        a->ok = a->ok || conn;
        ws.insert(a);
    }

    void forget_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        // When forgetting i, drop its tau entry and parent entry;
        // we conservatively keep parent of others unchanged.
        auto a = w.clone();
        a->tau.erase(i);
        a->parent.erase(i);
        // ok flag persists
        ws.insert(a);
    }

    void join_implementation(const Bag &, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &ws) {
        // Require same tau on interface labels that appear in both
        for (auto [u,x] : w1.tau) {
            auto it = w2.tau.find(u);
            if (it!=w2.tau.end() && it->second!=x) return;
        }
        // Merge parents by union of equivalences on common labels
        auto a = w1.clone();
        for (auto [u,p] : w2.parent) {
            if (!a->parent.count(u)) continue; // only common interface labels
            unsigned r2 = MinorK2_Witness::find_root(w2.parent,u);
            unsigned r1u = MinorK2_Witness::find_root(a->parent,u);
            unsigned r1r2 = MinorK2_Witness::find_root(a->parent,r2);
            if (r1u!=r1r2) a->parent[r1u]=r1r2;
        }
        // Merge tau: take entries from w2 when missing
        for (auto [u,x] : w2.tau) if (!a->tau.count(u)) a->tau[u]=x;
        // Sticky ok propagation and cross-child connectivity check on interface
        a->ok = w1.ok || w2.ok;
        if (!a->ok) {
            std::vector<unsigned> zeros, ones;
            for (auto [u,x] : a->tau) {
                if (x==0) zeros.push_back(u);
                else if (x==1) ones.push_back(u);
            }
            for (unsigned u : zeros) {
                unsigned ru = MinorK2_Witness::find_root(a->parent,u);
                for (unsigned v : ones) {
                    unsigned rv = MinorK2_Witness::find_root(a->parent,v);
                    if (ru==rv) { a->ok = true; break; }
                }
                if (a->ok) break;
            }
        }
        ws.insert(a);
    }

    bool is_final_witness_implementation(const Bag &, const WitnessAlias &w) {
        // Accept if there exist labels u,v with tau[u]=0 and tau[v]=1 connected in UF
        return w.ok;
    }

    void clean_implementation(WitnessSet &ws) {
        // Simple dedup by (tau,parent)
        auto vec = std::vector<std::shared_ptr<WitnessAlias>>(); vec.reserve(ws.size());
        for (auto &x : ws) vec.push_back(std::dynamic_pointer_cast<WitnessAlias>(x));
        std::sort(vec.begin(), vec.end(), [](const auto&a,const auto&b){return is_less_implementation(*a,*b);});
        vec.erase(std::unique(vec.begin(), vec.end(), [](const auto&a,const auto&b){return is_equal_implementation(*a,*b);}), vec.end());
        ws = WitnessSet();
        for (auto &p : vec) ws.insert(std::move(p));
    }
};
