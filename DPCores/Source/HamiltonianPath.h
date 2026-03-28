// Hamiltonian Path DP-core (path-decomposition oriented)
#include "../../CorePrelude.h"
#include <deque>
#include <functional>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;

struct HP_Witness : WitnessWrapper<HP_Witness>
{
    // d: degree in {0,1,2} for bag vertices
    std::map<unsigned, unsigned> d;
    // M: endpoint matching for degree-1 vertices (canonical min->max storage)
    std::map<unsigned, unsigned> M;
    unsigned cc = 0; // cycle detector used during join
    unsigned ep = 0; // forgotten endpoints count
    unsigned comps = 0; // number of open components intersecting the bag

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        return std::tie(l.d, l.M, l.comps, l.cc, l.ep) == std::tie(r.d, r.M, r.comps, r.cc, r.ep);
    }
    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        if (l.d != r.d) return l.d < r.d;
        if (l.M != r.M) return l.M < r.M;
        if (l.comps != r.comps) return l.comps < r.comps;
        if (l.cc != r.cc) return l.cc < r.cc;
        return l.ep < r.ep;
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

    // Invariant: for reachable witnesses, comps == component_count(w).
    static unsigned component_count(const WitnessAlias &w);

    template<class Rho>
    static HP_Witness relabel_copy(const HP_Witness &w_, const Rho &rho) {
        HP_Witness w;
        for (auto [u,deg] : w_.d) w.d[rho.at(u)] = deg;
        for (auto [u,v] : w_.M) {
            auto ru = rho.at(u); auto rv = rho.at(v);
            if (ru < rv) w.M[ru] = rv; else w.M[rv] = ru;
        }
        w.cc   = w_.cc;
        w.ep   = w_.ep;
        w.comps = component_count(w);
        return w;
    }
    void hash(Hasher &h) const override {
        for (auto [u,deg] : d) { h << u << deg; }
        h << -1u;
        for (auto [u,v] : M) { h << u << v; }
        h << -1u << comps << cc << ep;
    }
    void witness_info(std::ostream &os) const {
        os << "d={"; for (auto it=d.begin(); it!=d.end(); ++it) { os<<it->first<<":"<<it->second; if (next(it)!=d.end()) os<<","; } os<<"} ";
        os << "M={"; for (auto it=M.begin(); it!=M.end(); ++it) { os<<it->first<<"->"<<it->second; if (next(it)!=M.end()) os<<","; } os<<"} ";
        os << "comps="<<comps<<" cc="<<cc<<" ep="<<ep<<"\n";
    }

    HP_Witness relabel_implementation(const std::map<unsigned, unsigned> &rho) const {
        HP_Witness w;
        w.cc = cc;
        w.ep = ep;
        w.comps = comps;
        for (auto [u,deg] : d) {
            auto it = rho.find(u);
            if (it != rho.end()) {
                w.d[it->second] = deg;
            }
        }
        for (auto [u,v] : M) {
            auto itu = rho.find(u);
            auto itv = rho.find(v);
            if (itu != rho.end() && itv != rho.end()) {
                unsigned ru = itu->second;
                unsigned rv = itv->second;
                if (ru < rv) w.M[ru] = rv; else w.M[rv] = ru;
            }
        }
        return w;
    }
};

struct HP_Core : CoreWrapper<HP_Core, HP_Witness, WitnessSetTypeTwo>
{
    static std::map<std::string, std::string> metadata() {
        return {{"CoreName","HamiltonianPath"}, {"CoreType","Bool"}, {"ParameterType","None"}};
    }

    static bool partner_of(const std::map<unsigned,unsigned> &M, unsigned a, unsigned &partner) {
        auto it = M.find(a);
        if (it!=M.end()) { partner = it->second; return true; }
        for (auto &kv : M) { if (kv.second == a) { partner = kv.first; return true; } }
        return false;
    }

    // Invariant: for reachable witnesses, comps == component_count(w).
    static unsigned component_count(const WitnessAlias &w) {
        // Count open components: degree-1 endpoints that are unmatched to another bag endpoint
        unsigned comps = 0;
        for (auto [v,deg] : w.d) {
            if (deg == 1) {
                unsigned partner;
                if (!partner_of(w.M, v, partner)) comps += 1;
            }
        }
        return comps;
    }

    static void seed_bag_zeros(const Bag &bag, HP_Witness &w) {
        for (unsigned v : bag.get_elements()) {
            if (!w.d.count(v)) w.d[v] = 0;
        }
    }

    void initialize_leaf(WitnessSet &ws) {
        auto w = std::make_shared<WitnessAlias>();
        w->comps = 0;
        ws.insert(std::move(w));
    }

    void intro_v_implementation(unsigned i, const Bag &bag, const WitnessAlias &w, WitnessSet &ws) {
        auto wn = w.clone();
        seed_bag_zeros(bag, *wn);
        wn->d[i]=0;
        wn->comps = component_count(*wn);
        ws.insert(std::move(wn));
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &bag, const WitnessAlias &w, WitnessSet &ws) {
        // skip edge
        {
            auto wc = w.clone();
            seed_bag_zeros(bag, *wc);
            ws.insert(std::move(wc));
        }
        // use edge if possible
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
            // start a new open path component within the bag
            HP_Witness::add_pair(M,i,j);
        } else if (i_ep && j_zero) {
            unsigned pi; bool hi = partner_of(M, i, pi);
            if (hi) { HP_Witness::erase_pair(M, i); HP_Witness::add_pair(M, pi, j); }
            // else: i was unmatched to bag endpoints (other end already forgotten), j becomes new unmatched endpoint; no pair to add
        } else if (i_zero && j_ep) {
            unsigned pj; bool hj = partner_of(M, j, pj);
            if (hj) { HP_Witness::erase_pair(M, j); HP_Witness::add_pair(M, i, pj); }
            // else: symmetric unmatched case; nothing to pair
        } else if (i_ep && j_ep) {
            unsigned pi, pj; bool hi = partner_of(M, i, pi); bool hj = partner_of(M, j, pj);
            if (hi && hj) {
                if (pi == j) {
                    // closing a cycle within the bag -> prune branch
                    return;
                } else {
                    HP_Witness::erase_pair(M, i); HP_Witness::erase_pair(M, j); HP_Witness::add_pair(M, pi, pj);
                }
            } else if (hi && !hj) {
                // i had a bag partner; j was unmatched (outside). Removing i endpoint, leave pi unmatched in the bag.
                HP_Witness::erase_pair(M, i);
            } else if (!hi && hj) {
                HP_Witness::erase_pair(M, j);
            } else {
                // both unmatched to bag endpoints; connecting two outside-reaching paths: nothing to pair within the bag
            }
        }
        wn->comps = component_count(*wn);
        ws.insert(std::move(wn));
    }

    void forget_v_implementation(unsigned i, const Bag &bag, const WitnessAlias &w, WitnessSet &ws) {
        auto wn = w.clone();
        seed_bag_zeros(bag, *wn);
        unsigned di = wn->d.at(i);
        if (di==0) {
            bool can_singleton =
                wn->M.empty() &&
                wn->cc == 0 &&
                wn->ep == 0 &&
                wn->d.size() == 1;
            if (!can_singleton) return;
            wn->ep += 2;
        } else if (di==1) {
            auto &M = wn->M;
            HP_Witness::erase_pair(M, i);
            wn->ep += 1;
        }
        wn->d.erase(i);
        wn->comps = component_count(*wn);
        ws.insert(std::move(wn));
    }

    void join_implementation(const Bag &bag, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &ws) {
        auto L = w1.clone();
        auto R = w2.clone();
        seed_bag_zeros(bag, *L);
        seed_bag_zeros(bag, *R);

        std::map<unsigned, unsigned> dsum = L->d;
        for (auto [v, deg] : R->d) dsum[v] = (dsum.count(v) ? dsum[v] + deg : deg);
        for (auto [v, deg] : dsum) if (deg > 2) return;

        auto has_partner = [&](const std::map<unsigned,unsigned> &M, unsigned a){
            auto it = M.find(a);
            if (it!=M.end()) return true;
            for (auto const &kv : M) if (kv.second == a) return true;
            return false;
        };

        using U64 = unsigned long long;
        struct Edge { U64 v; int c; };
        auto is_stub = [](U64 x){ return (x >> 63) != 0; };
        auto make_stub = [](int side, unsigned v) -> U64 {
            return (1ull<<63) | (static_cast<U64>(side & 3) << 32) | static_cast<U64>(v);
        };

        std::map<U64, std::vector<Edge>> adj;
        auto add_edge = [&](U64 a, U64 b, int color){
            adj[a].push_back({b,color});
            adj[b].push_back({a,color});
        };

        for (auto [a,b] : L->M) add_edge((U64)a, (U64)b, 1);
        for (auto [a,b] : R->M) add_edge((U64)a, (U64)b, 2);
        for (auto [v,deg] : L->d) if (deg==1 && !has_partner(L->M, v)) add_edge((U64)v, make_stub(1, v), 1);
        for (auto [v,deg] : R->d) if (deg==1 && !has_partner(R->M, v)) add_edge((U64)v, make_stub(2, v), 2);

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

        std::deque<unsigned> Q;
        for (auto [v,deg] : dsum) if (deg==2) Q.push_back(v);
        while (!Q.empty()) {
            unsigned v = Q.front(); Q.pop_front();
            if (dsum[v] != 2) continue;
            auto it = adj.find((U64)v);
            if (it == adj.end()) continue;
            auto inc = it->second;
            if (inc.empty()) continue;
            if (inc.size() == 1) {
                remove_both_once((U64)v, inc[0].v, inc[0].c);
            } else if (inc.size() == 2) {
                auto e1 = inc[0], e2 = inc[1];
                if (e1.c == e2.c) return;
                remove_both_once((U64)v, e1.v, e1.c);
                remove_both_once((U64)v, e2.v, e2.c);
                if (e1.v == e2.v) {
                    if (!is_stub(e1.v)) add_edge(e1.v, e1.v, 3);
                } else {
                    add_edge(e1.v, e2.v, 3);
                }
            } else {
                return;
            }
        }

        std::map<unsigned, unsigned> Mnew;
        std::set<unsigned> used;
        for (auto &[aNode, nb] : adj) {
            if (is_stub(aNode)) continue;
            unsigned a = (unsigned)aNode;
            if (dsum[a] != 1) continue;
            unsigned realCnt = 0; U64 realN = 0;
            for (auto const &e : nb) if (!is_stub(e.v)) { realN = e.v; realCnt++; }
            if (realCnt == 1) {
                unsigned b = (unsigned)realN;
                if (dsum[b] != 1) return;
                if (!used.count(a) && !used.count(b)) {
                    if (a < b) Mnew[a]=b; else Mnew[b]=a;
                    used.insert(a); used.insert(b);
                }
            } else if (realCnt > 1) {
                return;
            }
        }

        auto real_deg = [&](U64 x){
            unsigned cnt = 0;
            for (auto const &e : adj[x]) if (!is_stub(e.v)) ++cnt;
            return cnt;
        };

        unsigned add_cc = 0;
        std::set<U64> vis;
        for (auto const &kv : adj) {
            U64 s = kv.first;
            if (is_stub(s) || vis.count(s) || dsum[(unsigned)s]==0) continue;
            std::deque<U64> dq{ s };
            bool all2 = true;
            while (!dq.empty()) {
                U64 u = dq.front(); dq.pop_front();
                if (!vis.insert(u).second) continue;
                if (real_deg(u) != 2) all2 = false;
                for (auto const &e : adj[u]) if (!is_stub(e.v) && !vis.count(e.v)) dq.push_back(e.v);
            }
            if (all2) add_cc += 1;
        }

        auto wn = std::make_shared<WitnessAlias>();
        wn->d  = std::move(dsum);
        wn->M  = std::move(Mnew);
        wn->ep = w1.ep + w2.ep;
        wn->cc = w1.cc + w2.cc + add_cc;
        if (wn->cc > 0) return;
        wn->comps = component_count(*wn);
        ws.insert(std::move(wn));
    }

    void clean_implementation(WitnessSet &ws) {
        // Deduplicate identical states; no optimisation on cc/ep
        using Key = std::tuple<map<unsigned,unsigned>, map<unsigned,unsigned>, unsigned, unsigned>;
        std::set<Key> seen;
        WitnessSetTypeTwo<WitnessAlias> cleaned;
        for (const auto &wp : ws) {
            auto &w = WitnessAlias::as_witness(*wp);
            Key key = std::make_tuple(w.d, w.M, w.cc, w.ep);
            if (seen.insert(key).second) {
                auto wn = std::make_shared<WitnessAlias>(w);
                cleaned.insert(std::move(wn));
            }
        }
        ws.setEqual(cleaned);
    }

    bool is_final_witness_implementation(const Bag &bag, const WitnessAlias &w) {
        auto base_accept = [](const WitnessAlias &wf) {
            // Path: no closed cycles formed (cc==0) and exactly two endpoints forgotten; no residual endpoints
            return (wf.cc==0 && wf.comps==0 && wf.ep==2 && wf.M.empty() && wf.d.empty());
        };

        if (bag.size()==0) {
            return base_accept(w);
        }

        // If the root bag is non-empty, simulate a nice decomposition suffix by
        // forgetting the remaining bag vertices in some order.
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

                WitnessSetTypeTwo<HP_Witness> tmp;
                this->forget_v_implementation(verts[idx], curBag, cur, tmp);
                for (const auto &wp : tmp) {
                    if (dfs(idx + 1, HP_Witness::as_witness(*wp))) return true;
                }
                return false;
            };

        do {
            if (dfs(0, w)) return true;
        } while (std::next_permutation(verts.begin(), verts.end()));

        return false;
    }
};
