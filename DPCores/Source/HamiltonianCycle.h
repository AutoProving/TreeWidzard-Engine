// Hamiltonian Cycle DP-core (tree/path decomposition friendly)
#include "../../CorePrelude.h"
#include <deque>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

struct HC_Witness : WitnessWrapper<HC_Witness>
{
    // d[v] ∈ {0,1,2} counts selected incident edges while v is in the bag
    std::map<unsigned, unsigned> d;
    // M pairs bag endpoints (degree-1 vertices) that belong to the same partial component
    std::map<unsigned, unsigned> M;
    unsigned cc = 0;     // number of closed cycles already formed
    unsigned comps = 0;  // number of open components touching the bag (informational)

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        return std::tie(l.d, l.M, l.comps, l.cc) == std::tie(r.d, r.M, r.comps, r.cc);
    }
    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        if (l.d != r.d) return l.d < r.d;
        if (l.M != r.M) return l.M < r.M;
        if (l.comps != r.comps) return l.comps < r.comps;
        return l.cc < r.cc;
    }
    HC_Witness relabel_implementation(const std::map<unsigned, unsigned> &rho) const {
        HC_Witness w; w.cc = cc; w.comps = comps;
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
        h << -1u << comps << cc;
    }
    void witness_info(std::ostream &os) const {
        os << "d={";
        for (auto it=d.begin(); it!=d.end(); ++it){
            os<<it->first<<":"<<it->second;
            if (next(it)!=d.end()) os<<",";
        }
        os<<"} ";
        os << "M={";
        for (auto it=M.begin(); it!=M.end(); ++it){
            os<<it->first<<"->"<<it->second;
            if (next(it)!=M.end()) os<<",";
        }
        os<<"} ";
        os << "comps="<<comps<<" cc="<<cc<<"\n";
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
    static void erase_pair(std::map<unsigned,unsigned> &M, unsigned a) {
        auto it = M.find(a);
        if (it!=M.end()) { unsigned b = it->second; M.erase(a); M.erase(b); return; }
        for (auto it2 = M.begin(); it2!=M.end(); ++it2) {
            if (it2->second == a) { unsigned b = it2->first; M.erase(b); M.erase(a); return; }
        }
    }
    static void add_pair(std::map<unsigned,unsigned> &M, unsigned a, unsigned b) {
        if (a==b) return;
        if (a<b) M[a]=b; else M[b]=a;
    }
    static unsigned component_count(const HC_Witness &w) {
        unsigned comps = 0;
        comps += w.M.size();
        for (auto &[v,deg] : w.d) {
            if (deg==1) {
                unsigned partner;
                if (!partner_of(w.M, v, partner)) comps += 1;
            }
        }
        return comps;
    }

    static void seed_bag_zeros(const Bag &bag, HC_Witness &w) {
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
        {
            auto wc = w.clone();
            seed_bag_zeros(bag, *wc);
            ws.insert(std::move(wc));
        }
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
            add_pair(M,i,j);
        } else if (i_ep && j_zero) {
            unsigned pi; bool hi = partner_of(M, i, pi);
            if (hi) { erase_pair(M, i); add_pair(M, pi, j); }
        } else if (i_zero && j_ep) {
            unsigned pj; bool hj = partner_of(M, j, pj);
            if (hj) { erase_pair(M, j); add_pair(M, i, pj); }
        } else if (i_ep && j_ep) {
            unsigned pi, pj; bool hi = partner_of(M, i, pi); bool hj = partner_of(M, j, pj);
            if (hi && hj) {
                if (pi == j) {
                    erase_pair(M, i);
                    wn->cc += 1;
                    if (wn->cc > 1) return;
                } else {
                    erase_pair(M, i); erase_pair(M, j); add_pair(M, pi, pj);
                }
            } else if (hi && !hj) {
                erase_pair(M, i);
            } else if (!hi && hj) {
                erase_pair(M, j);
            }
        }
        wn->comps = component_count(*wn);
        ws.insert(std::move(wn));
    }

    void forget_v_implementation(unsigned i, const Bag &bag, const WitnessAlias &w, WitnessSet &ws) {
        auto wn = w.clone();
        seed_bag_zeros(bag, *wn);
        unsigned di = wn->d.at(i);
        if (di != 2) return; // in a cycle every forgotten vertex must already have degree 2
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
            unsigned dummy; return partner_of(M, a, dummy);
        };

        using U64 = unsigned long long;
        auto is_stub = [](U64 x){ return (x >> 63) != 0; };
        auto make_stub = [](int side, unsigned v) -> U64 {
            return (1ull<<63) | (static_cast<U64>(side & 3) << 32) | static_cast<U64>(v);
        };

        std::map<U64, std::vector<U64>> adj;
        auto add_edge = [&](U64 a, U64 b){ adj[a].push_back(b); adj[b].push_back(a); };
        auto add_matching_edges = [&](const std::map<unsigned,unsigned> &M){
            for (auto [a,b] : M) add_edge((U64)a, (U64)b);
        };
        add_matching_edges(L->M);
        add_matching_edges(R->M);

        for (auto [v,deg] : L->d) if (deg==1 && !has_partner(L->M, v)) add_edge((U64)v, make_stub(1, v));
        for (auto [v,deg] : R->d) if (deg==1 && !has_partner(R->M, v)) add_edge((U64)v, make_stub(2, v));

        unsigned add_cc = 0;
        {
            std::set<U64> vis;
            for (auto const &kv : adj) {
                U64 s = kv.first; if (vis.count(s)) continue;
                std::deque<U64> dq{ s };
                bool has_stub_node = false;
                std::vector<U64> comp;
                while (!dq.empty()) {
                    U64 u = dq.front(); dq.pop_front();
                    if (!vis.insert(u).second) continue;
                    comp.push_back(u);
                    if (is_stub(u)) has_stub_node = true;
                    for (U64 v : adj[u]) dq.push_back(v);
                }
                if (has_stub_node) continue;
                bool all2 = true;
                for (U64 u : comp) {
                    if (is_stub(u)) continue;
                    if (adj[u].size() != 2) { all2 = false; break; }
                }
                if (all2 && !comp.empty()) add_cc += 1;
            }
        }

        auto remove_edge_once = [&](U64 u, U64 v){
            auto &U = adj[u]; auto &V = adj[v];
            auto iu = std::find(U.begin(), U.end(), v); if (iu != U.end()) U.erase(iu);
            auto iv = std::find(V.begin(), V.end(), u); if (iv != V.end()) V.erase(iv);
        };

        std::deque<unsigned> Q;
        for (auto [v,deg] : dsum) if (deg==2) Q.push_back(v);
        while (!Q.empty()) {
            unsigned v = Q.front(); Q.pop_front();
            if (dsum[v] != 2) continue;
            auto &L = adj[(U64)v];
            if (L.empty()) continue;
            else if (L.size() == 1) {
                U64 u = L[0];
                remove_edge_once((U64)v, u);
            } else if (L.size() == 2) {
                U64 u = L[0], w = L[1];
                remove_edge_once((U64)v, u);
                remove_edge_once((U64)v, w);
                if (u == w) {
                    if (!is_stub(u)) add_cc += 1;
                } else {
                    add_edge(u, w);
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
            if (nb.size() != 1) continue;
            U64 bNode = nb[0];
            if (is_stub(bNode)) continue;
            unsigned b = (unsigned)bNode;
            if (dsum[b] != 1) return;
            if (used.count(a) || used.count(b)) continue;
            add_pair(Mnew, a, b);
            used.insert(a); used.insert(b);
        }

        auto wn = std::make_shared<WitnessAlias>();
        wn->d  = std::move(dsum);
        wn->M  = std::move(Mnew);
        wn->cc = L->cc + R->cc + add_cc;
        if (wn->cc > 1) return;
        wn->comps = component_count(*wn);
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
        if (bag.size()!=0) return false;
        return (w.cc==1 && w.M.empty() && w.d.empty());
    }
};
