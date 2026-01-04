#include "../../CorePrelude.h"
#include <limits>
#include <vector>
#include <set>

using namespace std;

struct FeedbackVertexSet_Witness : WitnessWrapper<FeedbackVertexSet_Witness>
{
    // comp[v] = representative vertex id for the kept component containing v
    std::map<unsigned, unsigned> comp;
    unsigned removed = 0;

    friend bool is_equal_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        return std::tie(l.comp, l.removed) == std::tie(r.comp, r.removed);
    }
    friend bool is_less_implementation(const WitnessAlias &l, const WitnessAlias &r) {
        if (l.comp != r.comp) return l.comp < r.comp;
        return l.removed < r.removed;
    }

    WitnessAlias relabel_implementation(const std::map<unsigned, unsigned> &rho) const {
        WitnessAlias w;
        w.removed = removed;
        for (auto &[v, root] : comp) {
            auto it_v = rho.find(v);
            auto it_r = rho.find(root);
            if (it_v == rho.end() || it_r == rho.end()) {
                std::cerr << "Error: FeedbackVertexSet relabel mismatch" << std::endl;
                exit(20);
            }
            w.comp[it_v->second] = it_r->second;
        }
        // Keep a canonical component representation after relabeling:
        // each component is represented by its smallest vertex label.
        std::map<unsigned, unsigned> canonical;
        for (auto &[v, r] : w.comp) {
            auto it = canonical.find(r);
            if (it == canonical.end() || v < it->second) canonical[r] = v;
        }
        for (auto &[v, r] : w.comp) {
            auto it = canonical.find(r);
            if (it != canonical.end()) r = it->second;
        }
        return w;
    }

    void hash(Hasher &h) const override {
        for (auto &[v, r] : comp) { h << v << r; }
        h << -1u << removed;
    }

    void witness_info(std::ostream &os) const {
        os << "removed=" << removed << " comp={";
        for (auto it = comp.begin(); it != comp.end(); ++it) {
            os << it->first << "->" << it->second;
            if (std::next(it) != comp.end()) os << ",";
        }
        os << "}\n";
    }
};

struct FeedbackVertexSet_Core : CoreWrapper<FeedbackVertexSet_Core, FeedbackVertexSet_Witness, WitnessSetTypeTwo>
{
    unsigned k = 0;

    static std::map<std::string, std::string> metadata() {
        return {
            {"CoreName", "FeedbackVertexSet"},
            {"CoreType", "Bool"},
            {"ParameterType", "UnsignedInt"},
        };
    }

    FeedbackVertexSet_Core(const parameterType &parameters) {
        auto [limit] = unpack_typed_args<int>(parameters);
        if (limit < 0) {
            std::cerr << "FeedbackVertexSet parameter must be non-negative" << std::endl;
            exit(20);
        }
        k = static_cast<unsigned>(limit);
    }

    static void normalize(WitnessAlias &w) {
        std::map<unsigned, unsigned> canonical;
        for (auto &[v, r] : w.comp) {
            auto it = canonical.find(r);
            if (it == canonical.end() || v < it->second) canonical[r] = v;
        }
        for (auto &[v, r] : w.comp) {
            auto it = canonical.find(r);
            if (it != canonical.end()) r = it->second;
        }
    }

    static void erase_vertex(WitnessAlias &w, unsigned v) {
        auto it = w.comp.find(v);
        if (it == w.comp.end()) return;
        unsigned root = it->second;
        w.comp.erase(it);
        unsigned new_root = std::numeric_limits<unsigned>::max();
        bool need = false;
        for (auto &[node, rep] : w.comp) {
            if (rep == root) {
                need = true;
                if (node < new_root) new_root = node;
            }
        }
        if (need) {
            for (auto &[node, rep] : w.comp) {
                if (rep == root) rep = new_root;
            }
        }
    }

    void initialize_leaf(WitnessSet &ws) {
        ws.insert(std::make_shared<WitnessAlias>());
    }

    void intro_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        // Keep branch
        {
            auto wk = w.clone();
            wk->comp[i] = i;
            normalize(*wk);
            ws.insert(std::move(wk));
        }
        // Remove branch
        if (w.removed + 1 <= k) {
            auto wr = w.clone();
            wr->removed += 1;
            ws.insert(std::move(wr));
        }
    }

    void intro_e_implementation(unsigned i, unsigned j, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        auto wn = w.clone();
        auto it_i = wn->comp.find(i);
        auto it_j = wn->comp.find(j);
        if (it_i == wn->comp.end() || it_j == wn->comp.end()) {
            ws.insert(std::move(wn));
            return;
        }
        unsigned ri = it_i->second;
        unsigned rj = it_j->second;
        // If both endpoints are kept and already connected, adding this edge would
        // create a cycle in the kept graph, so this partial solution is invalid.
        if (ri == rj) return;
        unsigned new_root = std::min(ri, rj);
        for (auto &kv : wn->comp) {
            if (kv.second == ri || kv.second == rj) kv.second = new_root;
        }
        normalize(*wn);
        ws.insert(std::move(wn));
    }

    void forget_v_implementation(unsigned i, const Bag &, const WitnessAlias &w, WitnessSet &ws) {
        auto keep_state = w.clone();
        erase_vertex(*keep_state, i);
        normalize(*keep_state);
        ws.insert(std::move(keep_state));
    }

    static void merge_connection(unsigned u, unsigned v, std::vector<std::shared_ptr<WitnessAlias>> &states, unsigned k) {
        std::vector<std::shared_ptr<WitnessAlias>> next;
        for (auto &state : states) {
            auto it_u = state->comp.find(u);
            auto it_v = state->comp.find(v);
            if (it_u == state->comp.end() || it_v == state->comp.end()) {
                next.push_back(state);
                continue;
            }
            unsigned ru = it_u->second;
            unsigned rv = it_v->second;
            // A connection between already-connected endpoints would create a cycle.
            if (ru == rv) continue;
            auto wn = state->clone();
            unsigned new_root = std::min(ru, rv);
            for (auto &kv : wn->comp) {
                if (kv.second == ru || kv.second == rv) kv.second = new_root;
            }
            normalize(*wn);
            next.push_back(std::move(wn));
        }
        states.swap(next);
    }

    void join_implementation(const Bag &bag, const WitnessAlias &w1, const WitnessAlias &w2, WitnessSet &ws) {
        if (w1.comp.size() != w2.comp.size()) return;
        for (const auto &[v, _] : w1.comp) {
            if (!w2.comp.count(v)) return;
        }
        unsigned bag_size = bag.size();
        unsigned kept_vertices = static_cast<unsigned>(w1.comp.size());
        unsigned removed_in_bag = (bag_size >= kept_vertices) ? (bag_size - kept_vertices) : 0;

        std::vector<std::shared_ptr<WitnessAlias>> states;
        states.push_back(w1.clone());

        std::map<unsigned, std::vector<unsigned>> components_r;
        for (auto &[v, r] : w2.comp) {
            components_r[r].push_back(v);
        }
        for (auto &[root, verts] : components_r) {
            if (verts.size() <= 1) continue;
            unsigned anchor = verts[0];
            for (size_t idx = 1; idx < verts.size(); ++idx) {
                merge_connection(anchor, verts[idx], states, k);
                if (states.empty()) return;
            }
        }

        unsigned unique_right = 0;
        if (w2.removed >= removed_in_bag) unique_right = w2.removed - removed_in_bag;

        for (auto &state : states) {
            state->removed += unique_right;
            if (state->removed > k) continue;
            normalize(*state);
            ws.insert(std::move(state));
        }
    }

    void clean_implementation(WitnessSet &ws) {
        using Key = std::map<unsigned, unsigned>;
        std::map<Key, unsigned> best;
        for (const auto &ptr : ws) {
            const auto &w = WitnessAlias::as_witness(*ptr);
            auto it = best.find(w.comp);
            if (it == best.end() || w.removed < it->second) best[w.comp] = w.removed;
        }
        WitnessSetTypeTwo<WitnessAlias> cleaned;
        for (auto &[comp, rem] : best) {
            auto w = std::make_shared<WitnessAlias>();
            w->comp = comp;
            w->removed = rem;
            cleaned.insert(std::move(w));
        }
        ws.setEqual(cleaned);
    }

    bool is_final_witness_implementation(const Bag &, const WitnessAlias &w) {
        return w.removed <= k;
    }
};
