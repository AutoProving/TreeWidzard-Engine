#include "../../CorePrelude.h"

using namespace std;

struct PerfectMatching_Witness : WitnessWrapper<PerfectMatching_Witness>
{
    // pairs: store canonical pairs (u -> v) with u < v
    std::map<unsigned, unsigned> pairs;
    // reverse_pairs: store inverse pairs (v -> u) for O(log n) reverse lookups
    std::map<unsigned, unsigned> reverse_pairs;
    // satisfied: vertices already matched to a partner that has been forgotten
    std::set<unsigned> satisfied;

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        return std::tie(l.pairs, l.reverse_pairs, l.satisfied) == std::tie(r.pairs, r.reverse_pairs, r.satisfied);
    }
    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        if (l.pairs != r.pairs) return l.pairs < r.pairs;
        if (l.reverse_pairs != r.reverse_pairs) return l.reverse_pairs < r.reverse_pairs;
        return l.satisfied < r.satisfied;
    }
    WitnessAlias relabel_implementation(const std::map<unsigned, unsigned> &rho) const {
        WitnessAlias w;
        for (auto &[u, v] : pairs) {
            auto it_u = rho.find(u);
            auto it_v = rho.find(v);
            if (it_u == rho.end() || it_v == rho.end()) {
                std::cerr << "PerfectMatching relabel: missing vertex" << std::endl;
                exit(20);
            }
            unsigned a = std::min(it_u->second, it_v->second);
            unsigned b = std::max(it_u->second, it_v->second);
            w.pairs[a] = b;
            w.reverse_pairs[b] = a;
        }
        for (auto v : satisfied) {
            auto it = rho.find(v);
            if (it == rho.end()) {
                std::cerr << "PerfectMatching relabel: missing vertex" << std::endl;
                exit(20);
            }
            w.satisfied.insert(it->second);
        }
        return w;
    }
    void hash(Hasher &h) const override {
        for (auto &[u, v] : pairs) { h << u << v; }
        h << -1u;
        for (auto &[v, u] : reverse_pairs) { h << v << u; }
        h << -1u;
        for (auto v : satisfied) h << v;
    }
    void witness_info(std::ostream &os) const {
        os << "pairs={";
        for (auto it = pairs.begin(); it != pairs.end(); ++it) {
            os << "(" << it->first << "," << it->second << ")";
            if (next(it) != pairs.end()) os << ",";
        }
        os << "} satisfied={";
        for (auto it = satisfied.begin(); it != satisfied.end(); ++it) {
            os << *it;
            if (next(it) != satisfied.end()) os << ",";
        }
        os << "}\n";
    }
};

struct PerfectMatching_Core : CoreWrapper<PerfectMatching_Core, PerfectMatching_Witness, WitnessSetTypeTwo>
{
    PerfectMatching_Core(const parameterType &) {}

    static void remove_pair(WitnessAlias &w, unsigned a, unsigned b) {
        unsigned u = std::min(a, b);
        unsigned v = std::max(a, b);
        auto it = w.pairs.find(u);
        if (it != w.pairs.end()) {
            w.pairs.erase(it);
            w.reverse_pairs.erase(v);
        }
    }

    static std::map<std::string, std::string> metadata() {
        return {
            // Canonical public name: boolean structural property.
            {"CoreName","HasPerfectMatching"},
            {"CoreType","Bool"},
            {"ParameterType","None"}
        };
    }

    void initialize_leaf(WitnessSet &ws) {
        ws.insert(std::make_shared<WitnessAlias>());
    }

    static bool is_paired(const WitnessAlias &w, unsigned v, unsigned &partner) {
        auto it = w.pairs.find(v);
        if (it != w.pairs.end()) {
            partner = it->second;
            return true;
        }
        auto rit = w.reverse_pairs.find(v);
        if (rit != w.reverse_pairs.end()) {
            partner = rit->second;
            return true;
        }
        return false;
    }

    static bool is_free(const WitnessAlias &w, unsigned v) {
        unsigned dummy;
        if (is_paired(w, v, dummy)) return false;
        if (w.satisfied.count(v)) return false;
        return true;
    }

    void intro_v_implementation(unsigned, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        ws.insert(w.clone());
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        ws.insert(w.clone()); // skip edge
        if (!is_free(w, i) || !is_free(w, j)) return;
        auto wn = w.clone();
        unsigned u = std::min(i,j);
        unsigned v = std::max(i,j);
        wn->pairs[u] = v;
        wn->reverse_pairs[v] = u;
        ws.insert(std::move(wn));
    }

    void forget_v_implementation(unsigned v, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        unsigned partner;
        if (is_paired(w, v, partner)) {
            auto wn = w.clone();
            if (w.pairs.count(v)) {
                wn->pairs.erase(v);
                wn->reverse_pairs.erase(partner);
                wn->satisfied.insert(partner);
            } else {
                wn->pairs.erase(partner);
                wn->reverse_pairs.erase(v);
                wn->satisfied.insert(partner);
            }
            ws.insert(std::move(wn));
            return;
        }
        if (w.satisfied.count(v)) {
            auto wn = w.clone();
            wn->satisfied.erase(v);
            ws.insert(std::move(wn));
            return;
        }
        // unmatched vertex -> invalid branch
    }

    void join_implementation(const Bag &bag, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &ws) {
        auto wn = std::make_shared<WitnessAlias>();
        std::set<unsigned> used;
        auto in_bag = bag.get_elements();

        auto add_pair = [&](const WitnessAlias &src) {
            for (auto &[u, v] : src.pairs) {
                if (!in_bag.count(u) || !in_bag.count(v)) return false;
                if (used.count(u) || used.count(v)) return false;
                wn->pairs[u] = v;
                wn->reverse_pairs[v] = u;
                used.insert(u);
                used.insert(v);
            }
            return true;
        };
        if (!add_pair(w1)) return;
        if (!add_pair(w2)) return;

        for (auto v : w1.satisfied) {
            if (!in_bag.count(v)) return;
            if (used.count(v)) return;
            wn->satisfied.insert(v);
            used.insert(v);
        }
        for (auto v : w2.satisfied) {
            if (!in_bag.count(v)) return;
            if (used.count(v)) {
                if (wn->satisfied.count(v)) return;
                return;
            }
            wn->satisfied.insert(v);
            used.insert(v);
        }

        ws.insert(std::move(wn));
    }

    void clean_implementation(WitnessSet &ws) {
        std::set<std::pair<std::map<unsigned,unsigned>, std::set<unsigned>>> seen;
        WitnessSetTypeTwo<WitnessAlias> cleaned;
        for (auto const &ptr : ws) {
            auto &w = WitnessAlias::as_witness(*ptr);
            auto key = std::make_pair(w.pairs, w.satisfied);
            if (seen.insert(key).second) {
                cleaned.insert(std::make_shared<WitnessAlias>(w));
            }
        }
        ws.setEqual(cleaned);
    }

    static bool can_clear_bag(WitnessAlias state, const Bag &bag) {
        std::set<unsigned> remaining = bag.get_elements();
        if (remaining.empty()) {
            return state.pairs.empty() && state.satisfied.empty();
        }
        while (!remaining.empty()) {
            unsigned v = *remaining.begin();
            if (state.satisfied.count(v)) {
                state.satisfied.erase(v);
                remaining.erase(v);
                continue;
            }
            unsigned partner;
            if (is_paired(state, v, partner) && remaining.count(partner)) {
                remove_pair(state, v, partner);
                state.satisfied.insert(partner);
                remaining.erase(v);
                continue;
            }
            return false;
        }
        return state.pairs.empty() && state.satisfied.empty();
    }

    bool is_final_witness_implementation(const Bag &bag, const WitnessAlias &w) {
        return can_clear_bag(w, bag);
    }
};
