// Copyright 2025

#include "../../CorePrelude.h"
#include <set>
#include <map>
#include <vector>
#include <sstream>

// Generic Minor Containment DP-core (runtime H)
// State carries:
//  - tau: partial assignment of boundary labels to pattern vertices in {0..nH-1}; -1 means unassigned/internal
//  - UF connectivity among boundary labels (parent map)
//  - ok_edges: set of realized pattern edges (by endpoint ids, a<b)
//  - placed: set of pattern vertices that have appeared at least once in some branch set below
// Acceptance: all pattern vertices placed and all pattern edges realized.

struct MinorGeneric_Witness : WitnessWrapper<MinorGeneric_Witness>
{
    std::map<unsigned,int> tau;                // label -> pattern vertex id or -1
    std::map<unsigned,unsigned> parent;        // union-find parent on interface labels
    std::set<std::pair<int,int>> ok_edges;     // realized H edges (a<b)
    std::set<int> placed;                      // pattern vertices that appeared

    static unsigned find_root(std::map<unsigned,unsigned>& parent, unsigned x) {
        auto it = parent.find(x);
        if (it == parent.end() || it->second == x) return x;
        unsigned root = find_root(parent, it->second);
        it->second = root;  // path compression
        return root;
    }

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        return std::tie(l.tau,l.parent,l.ok_edges,l.placed) == std::tie(r.tau,r.parent,r.ok_edges,r.placed);
    }
    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        if (l.tau!=r.tau) return l.tau<r.tau;
        if (l.parent!=r.parent) return l.parent<r.parent;
        if (l.ok_edges!=r.ok_edges) return l.ok_edges<r.ok_edges;
        return l.placed<r.placed;
    }
    WitnessAlias relabel_implementation(const std::map<unsigned, unsigned> &rho) const {
        auto w = WitnessAlias();
        for (auto [u,x] : tau) w.tau[rho.at(u)] = x;
        for (auto [u,p] : parent) {
            unsigned nu = rho.at(u);
            unsigned np = rho.at(p);
            w.parent[nu] = np;
        }
        w.ok_edges = ok_edges;
        w.placed = placed;
        return w;
    }
    void hash(Hasher &h) const override {
        for (auto [u,x] : tau) { h<<u; h<<x; }
        h<<-1u;
        for (auto [u,p] : parent) { h<<u; h<<p; }
        h<<-2u;
        for (auto [a,b] : ok_edges) { h<< (unsigned)a; h<< (unsigned)b; }
        h<<-3u;
        for (auto a : placed) { h<< (unsigned)a; }
    }
    void witness_info(std::ostream &os) const {
        os << "tau:";
        for (auto [u,x] : tau) os << " ("<<u<<":"<<x<<")";
        os << " parents:";
        for (auto [u,p] : parent) os << " ("<<u<<"->"<<p<<")";
        os << " okE:";
        for (auto [a,b] : ok_edges) os << " ("<<a<<","<<b<<")";
        os << " placed:";
        for (auto a : placed) os << " "<<a;
        os << "\n";
    }
};

struct MinorContainment_Generic_Core : CoreWrapper<MinorContainment_Generic_Core, MinorGeneric_Witness, WitnessSetTypeTwo>
{
    int nH = 0;
    std::set<std::pair<int,int>> H_edges; // (a<b)
    std::vector<std::vector<int>> H_adj;  // adjacency for quick neighbor test

    static std::map<std::string, std::string> metadata() {
        return {
            // Canonical public name: boolean structural property.
            {"CoreName","ContainsMinor"},
            {"CoreType","Decision"},
            {"ParameterType","MultiParameter"}
        };
    }

    MinorContainment_Generic_Core(const parameterType &params) {
        // Accept either: [int nH, char* edge_list] or [char* edge_list]
        if (!params.empty()) {
            try {
                // try two-arg form
                auto [n] = unpack_typed_args<int>(params);
                nH = n;
            } catch (...) {
                // ignore
            }
        }
        std::string edges_s;
        if (!params.empty()) {
            for (auto &v : params) {
                if (std::holds_alternative<char*>(v)) {
                    char *c = std::get<char*>(v);
                    if (c) { edges_s = c; break; }
                }
            }
        }
        if (!edges_s.empty()) parse_edges(edges_s);
        if (nH==0 && !H_edges.empty()) {
            int mx = 0; for (auto [a,b]: H_edges) mx = std::max(mx, std::max(a,b)); nH = mx+1;
        }
        H_adj.assign(nH, {});
        for (auto [a,b] : H_edges) { H_adj[a].push_back(b); H_adj[b].push_back(a); }
    }

    void parse_edges(const std::string &s) {
        std::istringstream iss(s);
        int a,b; while (iss>>a>>b) {
            if (a==b) continue;
            if (a>b) std::swap(a,b);
            H_edges.insert({a,b});
        }
    }

    void initialize_leaf(WitnessSet &ws) { ws.insert(std::make_shared<WitnessAlias>()); }

    void intro_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        // Option 1: internal
        {
            auto a = w.clone();
            if (!a->parent.count(i)) a->parent[i]=i;
            if (!a->tau.count(i)) a->tau[i] = -1;
            ws.insert(a);
        }
        // Option 2: assign to any pattern vertex x in [0..nH-1]
        for (int x=0;x<nH;++x) {
            auto b = w.clone(); b->parent[i]=i; b->tau[i]=x; b->placed.insert(x); ws.insert(b);
        }
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        auto a = w.clone();
        unsigned ri = MinorGeneric_Witness::find_root(a->parent, i);
        unsigned rj = MinorGeneric_Witness::find_root(a->parent, j);
        if (ri!=rj) a->parent[ri]=rj;
        // Update ok_edges: for any pair of labels mapped to endpoints of an H edge that are now connected
        // Build map from root to set of pattern labels at that root
        std::map<unsigned, std::vector<int>> root_labels;
        for (auto [u,x] : a->tau) if (x>=0) {
            unsigned r = MinorGeneric_Witness::find_root(a->parent, u);
            root_labels[r].push_back(x);
        }
        for (auto &[r,vec] : root_labels) {
            for (size_t p=0;p<vec.size();++p) for (size_t q=p+1;q<vec.size();++q) {
                int x = vec[p], y = vec[q];
                if (x==y) continue;
                if (x>y) std::swap(x,y);
                if (H_edges.count({x,y})) a->ok_edges.insert({x,y});
            }
        }
        ws.insert(a);
    }

    void forget_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        auto a = w.clone(); a->tau.erase(i); a->parent.erase(i); ws.insert(a);
    }

    void join_implementation(const Bag &bag, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &ws) {
        // tau consistency on interface
        for (auto [u,x] : w1.tau) {
            if (!bag.get_elements().count(u)) continue; // consider only interface
            auto it = w2.tau.find(u);
            if (it!=w2.tau.end() && it->second>=0 && x>=0 && it->second!=x) return;
        }
        auto a = w1.clone();
        // Merge parent unions on interface using w2
        auto w2_parent = w2.parent;  // clone for mutable find_root calls
        for (auto [u,p] : w2.parent) {
            if (!bag.get_elements().count(u)) continue;
            unsigned r2 = MinorGeneric_Witness::find_root(w2_parent, u);
            unsigned r1u = MinorGeneric_Witness::find_root(a->parent, u);
            unsigned r1r2 = MinorGeneric_Witness::find_root(a->parent, r2);
            if (r1u!=r1r2) a->parent[r1u]=r1r2;
        }
        // Merge tau: prefer existing, otherwise take w2's
        for (auto [u,x] : w2.tau) {
            if (!a->tau.count(u)) a->tau[u]=x;
        }
        // Merge ok_edges and placed
        a->ok_edges.insert(w2.ok_edges.begin(), w2.ok_edges.end());
        a->placed.insert(w2.placed.begin(), w2.placed.end());
        // Update ok_edges for any new connections created by merged unions
        std::map<unsigned, std::vector<int>> root_labels;
        for (auto [u,x] : a->tau) if (x>=0) {
            unsigned r = MinorGeneric_Witness::find_root(a->parent, u);
            root_labels[r].push_back(x);
        }
        for (auto &[r,vec] : root_labels) {
            for (size_t p=0;p<vec.size();++p) for (size_t q=p+1;q<vec.size();++q) {
                int x = vec[p], y = vec[q];
                if (x==y) continue;
                if (x>y) std::swap(x,y);
                if (H_edges.count({x,y})) a->ok_edges.insert({x,y});
            }
        }
        ws.insert(a);
    }

    bool is_final_witness_implementation(const Bag &, const WitnessAlias &w) {
        // All pattern vertices placed and all edges realized
        if ((int)w.placed.size() < nH) return false;
        if (H_edges.size() != w.ok_edges.size()) return false;
        return true;
    }

    void clean_implementation(WitnessSet &ws) {
        // simple dedup by (tau,parent,ok_edges,placed)
        auto vec = std::vector<std::shared_ptr<WitnessAlias>>(); vec.reserve(ws.size());
        for (auto &x : ws) vec.push_back(std::dynamic_pointer_cast<WitnessAlias>(x));
        std::sort(vec.begin(), vec.end(), [](const auto&a,const auto&b){return is_less_implementation(*a,*b);} );
        vec.erase(std::unique(vec.begin(), vec.end(), [](const auto&a,const auto&b){return is_equal_implementation(*a,*b);} ), vec.end());
        ws = WitnessSet();
        for (auto &p : vec) ws.insert(std::move(p));
    }
};
