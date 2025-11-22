// Hamiltonian Cycle DP-core (patched join: color-aware + safe contractions)
#include "../../CorePrelude.h"
#include <cassert>
#include <deque>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

/*
Invariants:
- d[v] ∈ {0,1,2}: number of selected incident edges while v is in the bag
- M: pairs degree-1 bag vertices that lie in the same partial component (min->max canonical)
- cc: number of cycle components formed so far (counted as soon as they close, even if the
      vertices are still in the bag). We only allow cc ∈ {0,1}; cc>1 is pruned immediately.
*/

struct HC_Witness : WitnessWrapper<HC_Witness>
{
    std::map<unsigned, unsigned> d;
    std::map<unsigned, unsigned> M;
    unsigned cc = 0;

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        return std::tie(l.d, l.M, l.cc) == std::tie(r.d, r.M, r.cc);
    }
    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        if (l.d != r.d) return l.d < r.d;
        if (l.M != r.M) return l.M < r.M;
        return l.cc < r.cc;
    }

    static void add_pair(std::map<unsigned,unsigned> &M, unsigned a, unsigned b) {
        if (a==b) return;
        if (a < b) M[a] = b; else M[b] = a;
    }
    static void erase_pair(std::map<unsigned,unsigned> &M, unsigned a) {
        auto it = M.find(a);
        if (it != M.end()) { M.erase(it); return; }
        for (auto it2 = M.begin(); it2!=M.end(); ++it2) {
            if (it2->second == a) { M.erase(it2); return; }
        }
    }

    HC_Witness relabel_implementation(const std::map<unsigned, unsigned> &rho) const {
        HC_Witness w; w.cc = cc;
        for (auto [u,deg] : d) w.d[rho.at(u)] = deg;
        for (auto [u,v] : M) {
            auto ru = rho.at(u); auto rv = rho.at(v);
            if (ru < rv) w.M[ru] = rv; else w.M[rv] = ru;
        }
        return w;
    }
    void hash(Hasher &h) const override {
        for (auto [u,deg] : d) { h << u << deg; }
        h << -1u;
        for (auto [u,v] : M) { h << u << v; }
        h << -1u << cc;
    }
    void witness_info(std::ostream &os) const {
        os << "d={"; for (auto it=d.begin(); it!=d.end(); ++it) { os<<it->first<<":"<<it->second; if (next(it)!=d.end()) os<<","; } os<<"} ";
        os << "M={"; for (auto it=M.begin(); it!=M.end(); ++it) { os<<it->first<<"->"<<it->second; if (next(it)!=M.end()) os<<","; } os<<"} ";
        os << "cc="<<cc<<"\n";
    }
};

struct HC_Core : CoreWrapper<HC_Core, HC_Witness, WitnessSetTypeTwo>
{
    static std::map<std::string, std::string> metadata() {
        return {{"CoreName","HC"}, {"CoreType","Bool"}, {"ParameterType","None"}};
    }

    static bool partner_of(const std::map<unsigned,unsigned> &M, unsigned a, unsigned &partner) {
        auto it = M.find(a);
        if (it!=M.end()) { partner = it->second; return true; }
        for (auto &kv : M) { if (kv.second == a) { partner = kv.first; return true; } }
        return false;
    }

    static void seed_bag_zeros(const Bag &bag, HC_Witness &w) {
        for (unsigned v : bag.get_elements())
            if (!w.d.count(v)) w.d[v] = 0;
    }

    void initialize_leaf(WitnessSet &ws) {
        auto w = std::make_shared<WitnessAlias>();
        w->cc = 0;
        ws.insert(std::move(w));
    }

    void intro_v_implementation(unsigned i, const Bag &bag, const WitnessAlias &w, WitnessSet &ws) {
        auto wn = w.clone();
        seed_bag_zeros(bag, *wn);
        wn->d[i] = 0;
        ws.insert(std::move(wn));
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &bag, const WitnessAlias &w, WitnessSet &ws) {
        // Skip edge
        {
            auto wc = w.clone();
            seed_bag_zeros(bag, *wc);
            ws.insert(std::move(wc));
        }
        // Take edge if both endpoints still have degree < 2
        auto wc = w.clone();
        seed_bag_zeros(bag, *wc);
        unsigned di = wc->d.at(i);
        unsigned dj = wc->d.at(j);
        if (di>=2 || dj>=2) return;
        auto wn = wc->clone();
        wn->d[i]=di+1; wn->d[j]=dj+1;
        auto &M = wn->M;

        bool i_ep = (di==1), j_ep=(dj==1);
        bool i_zero=(di==0), j_zero=(dj==0);
        if (i_zero && j_zero) {
            HC_Witness::add_pair(M, i, j);
        } else if (i_ep && j_zero) {
            unsigned pi; bool hi = partner_of(M, i, pi);
            if (hi) {
                HC_Witness::erase_pair(M, i);
                HC_Witness::add_pair(M, pi, j);
            }
        } else if (i_zero && j_ep) {
            unsigned pj; bool hj = partner_of(M, j, pj);
            if (hj) {
                HC_Witness::erase_pair(M, j);
                HC_Witness::add_pair(M, i, pj);
            }
        } else if (i_ep && j_ep) {
            unsigned pi, pj; bool hi = partner_of(M, i, pi); bool hj = partner_of(M, j, pj);
            if (hi && hj) {
                if (pi == j) {
                    HC_Witness::erase_pair(M, i);
                    wn->cc += 1;
                    if (wn->cc > 1) return;
                } else {
                    HC_Witness::erase_pair(M, i);
                    HC_Witness::erase_pair(M, j);
                    HC_Witness::add_pair(M, pi, pj);
                }
            } else if (hi && !hj) {
                HC_Witness::erase_pair(M, i);
            } else if (!hi && hj) {
                HC_Witness::erase_pair(M, j);
            }
        }
        ws.insert(std::move(wn));
    }

    void forget_v_implementation(unsigned i, const Bag &bag, const WitnessAlias &w, WitnessSet &ws) {
        auto wn = w.clone();
        seed_bag_zeros(bag, *wn);
        unsigned di = wn->d.at(i);
        if (di != 2) return; // in a Hamiltonian cycle, every forgotten vertex must already have degree 2
#ifndef NDEBUG
        // Degree-2 vertices must never remain in M
        assert(wn->M.find(i) == wn->M.end());
#endif
        wn->d.erase(i);
        ws.insert(std::move(wn));
    }

    void join_implementation(const Bag &bag, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &ws) {
        auto L = w1.clone();
        auto R = w2.clone();
        seed_bag_zeros(bag, *L);
        seed_bag_zeros(bag, *R);

        // Degree sum and quick feasibility
        std::map<unsigned, unsigned> dsum = L->d;
        for (auto [v, deg] : R->d) dsum[v] = (dsum.count(v) ? dsum[v] + deg : deg);
        for (auto [v, deg] : dsum) if (deg > 2) return;

        // Build colored adjacency among bag vertices plus "stub" terminals
        using U64 = unsigned long long;
        struct Edge { U64 v; int c; }; // c: 1 = left, 2 = right, 3 = neutral (after contraction)
        auto is_stub = [](U64 x){ return (x >> 63) != 0; };
        auto make_stub = [](int side, unsigned v) -> U64 {
            return (1ull<<63) | (static_cast<U64>(side & 3) << 32) | static_cast<U64>(v);
        };

        std::map<U64, std::vector<Edge>> adj;
        auto add_edge = [&](U64 a, U64 b, int color){
            adj[a].push_back({b,color});
            adj[b].push_back({a,color});
        };
        auto remove_both_once = [&](U64 u, U64 v, int color){
            auto &U = adj[u];
            for (auto it = U.begin(); it!=U.end(); ++it) {
                if (it->v==v && (color==0 || it->c==color)) { U.erase(it); break; }
            }
            auto &V = adj[v];
            for (auto it = V.begin(); it!=V.end(); ++it) {
                if (it->v==u && (color==0 || it->c==color)) { V.erase(it); break; }
            }
        };

        auto has_partner = [&](const std::map<unsigned,unsigned> &M, unsigned a){
            auto it = M.find(a);
            if (it!=M.end()) return true;
            for (auto const &kv : M) if (kv.second == a) return true;
            return false;
        };

        // Add matching edges (within-bag paths) and stubs for unmatched bag endpoints
        for (auto [a,b] : L->M) add_edge((U64)a, (U64)b, 1);
        for (auto [a,b] : R->M) add_edge((U64)a, (U64)b, 2);
        for (auto [v,deg] : L->d) if (deg==1 && !has_partner(L->M, v)) add_edge((U64)v, make_stub(1, v), 1);
        for (auto [v,deg] : R->d) if (deg==1 && !has_partner(R->M, v)) add_edge((U64)v, make_stub(2, v), 2);

        // Contract all bag vertices with final degree 2.
        // For correctness with this representation, each such vertex must receive exactly one "port" from each side.
        // If both ports come from the same side (or a child contributes deg 2 at v), we cannot represent connectivity -> prune.
        std::deque<unsigned> Q;
        for (auto [v,deg] : dsum) if (deg==2) Q.push_back(v);

        while (!Q.empty()) {
            unsigned v = Q.front(); Q.pop_front();
            if (dsum[v] != 2) continue;
            auto it = adj.find((U64)v);
            std::vector<Edge> inc = (it==adj.end() ? std::vector<Edge>{} : it->second);

            // Count how many degree contributions come from each child.
            // If one child already contributes degree 2 at v, all incident
            // edges lie entirely in that child. In that case we do not need
            // to contract v at this join; simply leave its local structure
            // untouched and move on.
            unsigned dl = L->d.count(v) ? L->d.at(v) : 0u;
            unsigned dr = R->d.count(v) ? R->d.at(v) : 0u;
            if (dl==2 || dr==2) continue;

            if (inc.size() != 2) {
                // The only representable cross-child case is exactly one
                // incident "port" from each side.
                return;
            }
            auto e1 = inc[0], e2 = inc[1];
            if (e1.c == e2.c) return; // both ports from the same side -> not representable

            // Remove the two ports (symmetrically) and connect their other ends
            remove_both_once((U64)v, e1.v, e1.c);
            remove_both_once((U64)v, e2.v, e2.c);
            if (e1.v == e2.v) {
                // Contracting v yields a loop at neighbor -> that is a closed cycle component
                if (!is_stub(e1.v)) {
                    // loop on a bag vertex with no stubs -> counts as a closed cycle below
                    // Represent it by incrementing cc later via a counter rather than storing the loop
                    add_edge(e1.v, e1.v, 3); // temporary mark; will be counted and removed
                } else {
                    // Loop on stub is meaningless; ignore
                }
            } else {
                add_edge(e1.v, e2.v, 3);
            }
        }

        // Count closed cycle components (no stubs, every real vertex has degree 2 in this adjacency)
        unsigned add_cc = 0;
        std::set<U64> vis;
        for (auto const &kv : adj) {
            U64 s = kv.first;
            if (vis.count(s) || is_stub(s)) continue;
            // skip isolated vertices; they carry no adjacency info
            if (adj[s].empty()) continue;
            std::deque<U64> dq{ s };
            bool has_stub = false;
            bool all2 = true;
            std::vector<U64> comp;
            while (!dq.empty()) {
                U64 u = dq.front(); dq.pop_front();
                if (!vis.insert(u).second) continue;
                comp.push_back(u);
                for (auto const &e : adj[u]) {
                    if (is_stub(e.v)) has_stub = true;
                    if (!is_stub(e.v) && !vis.count(e.v)) dq.push_back(e.v);
                }
            }
            if (has_stub) continue;
            // compute real degree in this comp
            for (U64 u : comp) {
                size_t cnt = 0;
                for (auto const &e : adj[u]) if (!is_stub(e.v)) ++cnt;
                if (cnt != 2) { all2 = false; break; }
            }
            if (all2) add_cc += 1;
        }

        auto wn = std::make_shared<WitnessAlias>();
        wn->d  = std::move(dsum);

        // Build M' from remaining degree-1 bag vertices
        std::map<unsigned, unsigned> Mnew;
        std::set<unsigned> used;
        for (auto const &[aNode, nb] : adj) {
            if (is_stub(aNode)) continue;
            unsigned a = (unsigned)aNode;
            if (wn->d[a] != 1) continue;

            // Count real (non-stub) neighbors of a
            unsigned realCnt = 0; U64 realN = 0;
            for (auto const &e : nb) if (!is_stub(e.v)) { realN = e.v; realCnt++; }
            if (realCnt == 0) {
                // a connects only to stubs -> remains unmatched in M
                continue;
            } else if (realCnt == 1) {
                unsigned b = (unsigned)realN;
                if (wn->d[b] != 1) return;           // inconsistent
                if (!used.count(a) && !used.count(b)) {
                    if (a < b) Mnew[a] = b; else Mnew[b] = a;
                    used.insert(a); used.insert(b);
                }
            } else {
                // >1 real neighbor would imply branching
                return;
            }
        }
        wn->M  = std::move(Mnew);

        // Accumulate cycles; prune if we ever exceed 1
        wn->cc = L->cc + R->cc + add_cc;
        if (wn->cc > 1) return;

        ws.insert(std::move(wn));
    }

    void clean_implementation(WitnessSet &ws) {
        using Key = std::tuple<map<unsigned,unsigned>, map<unsigned,unsigned>, unsigned>;
        std::set<Key> seen;
        WitnessSetTypeTwo<WitnessAlias> cleaned;
        for (const auto &wp : ws) {
            auto &w = WitnessAlias::as_witness(*wp);
            Key key = std::make_tuple(w.d, w.M, w.cc);
            if (seen.insert(key).second) {
                auto wn = std::make_shared<WitnessAlias>(w);
                cleaned.insert(std::move(wn));
            }
        }
        ws.setEqual(cleaned);
    }

    bool is_final_witness_implementation(const Bag &bag, const WitnessAlias &w) {
        auto base_accept = [](const WitnessAlias &wf) {
            return (wf.cc == 1 && wf.M.empty() && wf.d.empty());
        };

        if (bag.size() == 0) {
            return base_accept(w);
        }

        // Collect bag vertices
        std::vector<unsigned> verts;
        for (unsigned v : bag.get_elements()) verts.push_back(v);
        if (verts.empty()) return false;
        std::sort(verts.begin(), verts.end());

        std::function<bool(size_t, const WitnessAlias &)> dfs =
            [&](size_t idx, const WitnessAlias &cur) -> bool {
                if (idx == verts.size()) {
                    return base_accept(cur);
                }

                Bag curBag;
                std::set<unsigned> remaining;
                for (size_t j = idx; j < verts.size(); ++j) remaining.insert(verts[j]);
                curBag.set_elements(remaining);

                WitnessSetTypeTwo<HC_Witness> tmp;
                this->forget_v_implementation(verts[idx], curBag, cur, tmp);
                for (const auto &wp : tmp) {
                    if (dfs(idx + 1, HC_Witness::as_witness(*wp))) return true;
                }
                return false;
            };

        do {
            if (dfs(0, w)) return true;
        } while (std::next_permutation(verts.begin(), verts.end()));

        return false;
    }
};
