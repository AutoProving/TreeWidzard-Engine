// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "CertificateChecker.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "InputController.h"
#include "../Kernel/CertificateUtils.h"
#include "../Kernel/CertificateWriter.h"

namespace {

using TreeWidzard::Certificates::CanonMode;
using TreeWidzard::Certificates::dp_cores_fingerprint;
using TreeWidzard::Certificates::fnv1a64_file;
using TreeWidzard::Certificates::hex_u64;
using TreeWidzard::Certificates::parse_hex_u64;

struct Header {
	std::string width_name;
	unsigned width_value = 0;
	std::string search_name;
	CanonMode canon_mode = CanonMode::NONE;
	bool premise = false;
	uint64_t prop_hash = 0;
	std::optional<uint64_t> dpcores_hash;
};

static inline void trim_in_place(std::string &s) {
	while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
		s.erase(s.begin());
	while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
		s.pop_back();
}

static inline bool starts_with(const std::string &s, const char *prefix) {
	return s.rfind(prefix, 0) == 0;
}

static std::optional<std::string> next_token(std::istringstream &is) {
	std::string tok;
	if (!(is >> tok)) return std::nullopt;
	return tok;
}

static void require(bool ok, const std::string &msg) {
	if (!ok) {
		std::cerr << msg << std::endl;
		throw std::runtime_error("certificate rejected");
	}
}

static std::map<unsigned, unsigned> identity_map(const std::set<unsigned> &s) {
	std::map<unsigned, unsigned> m;
	for (auto v : s) m.insert(std::make_pair(v, v));
	return m;
}

static bool next_permutation_map(std::map<unsigned, unsigned> &permutation) {
	std::vector<unsigned> indexToValue;
	indexToValue.resize(permutation.size());
	unsigned counter = 0;
	for (auto item : permutation) {
		indexToValue[counter] = item.second;
		counter++;
	}
	if (!std::next_permutation(indexToValue.begin(), indexToValue.end())) {
		return false;
	}
	unsigned index = 0;
	for (auto &item : permutation) {
		item.second = indexToValue[index];
		index++;
	}
	return true;
}

static State::ptr canonical_state_bag_min(State::ptr state) {
	State::ptr canonicalState = state;
	std::map<unsigned, unsigned> m;
	std::set<unsigned> bagElements = state->get_bag().get_elements();
	unsigned i = 1;
	for (auto v : bagElements) {
		m.insert(std::make_pair(v, i));
		i++;
	}
	do {
		State::ptr relabeledState = state->relabel(m);
		if (relabeledState < canonicalState) {
			canonicalState = relabeledState;
		}
	} while (next_permutation_map(m));
	return canonicalState;
}

static State::ptr canon_if_needed(State::ptr s, CanonMode mode) {
	if (mode == CanonMode::NONE) return s;
	return canonical_state_bag_min(s);
}

static std::optional<std::map<unsigned, unsigned>>
parse_join_map(std::istringstream &is, const std::set<unsigned> &bag) {
	auto tok = next_token(is);
	if (!tok.has_value()) return std::nullopt;
	if (*tok != "MAP") {
		// put back? easiest: reject if extra tokens without MAP.
		throw std::runtime_error("Error: unexpected token in JOIN line");
	}
	std::vector<unsigned> nums;
	unsigned x = 0;
	while (is >> x) nums.push_back(x);
	require(nums.size() % 2 == 0,
			"Error: JOIN MAP must have an even number of integers");
	std::map<unsigned, unsigned> m;
	for (size_t i = 0; i < nums.size(); i += 2) {
		m.insert(std::make_pair(nums[i], nums[i + 1]));
	}
	require(m.size() == bag.size(),
			"Error: JOIN MAP size does not match bag size");
	{
		std::set<unsigned> keys;
		std::set<unsigned> vals;
		for (auto &kv : m) {
			keys.insert(kv.first);
			vals.insert(kv.second);
		}
		require(keys == bag, "Error: JOIN MAP keys do not match bag elements");
		require(vals == bag, "Error: JOIN MAP values do not match bag elements");
	}
	return m;
}

} // namespace

bool check_certificate_file(const std::string &property_file,
							const std::string &certificate_file) {
	try {
		std::ifstream in(certificate_file);
		if (!in) {
			std::cerr << "Error: could not open certificate file: "
					  << certificate_file << std::endl;
			return false;
		}

		std::string line;
		require(static_cast<bool>(std::getline(in, line)),
				"Error: certificate missing TWZCERT header");
		trim_in_place(line);
		require(line == "TWZCERT 1", "Error: certificate missing TWZCERT header");

		Header header;
		bool saw_result = false;
		bool saw_leaf = false;
		int bad_id = -1;
		bool result_satisfied = false;

		// Parse headers first (and keep reading until the first state/result).
		std::streampos after_preamble_pos = in.tellg();
		while (std::getline(in, line)) {
			trim_in_place(line);
			if (line.empty()) continue;
			if (!starts_with(line, "H ")) {
				in.seekg(after_preamble_pos);
				break;
			}
			after_preamble_pos = in.tellg();
			std::istringstream is(line);
			std::string H;
			std::string key;
			is >> H >> key;
			if (key == "WIDTH") {
				is >> header.width_name >> header.width_value;
			} else if (key == "SEARCH") {
				is >> header.search_name;
			} else if (key == "CANON") {
				std::string mode;
				is >> mode;
				if (mode == "NONE")
					header.canon_mode = CanonMode::NONE;
				else if (mode == "BAG_MIN")
					header.canon_mode = CanonMode::BAG_MIN;
				else {
					std::cerr << "Error: unknown CANON mode: " << mode
							  << std::endl;
					return false;
				}
			} else if (key == "PREMISE") {
				int v = 0;
				is >> v;
				header.premise = (v != 0);
			} else if (key == "PROP_HASH") {
				std::string hx;
				is >> hx;
				auto parsed = parse_hex_u64(hx);
				require(parsed.has_value(),
						"Error: invalid PROP_HASH in certificate");
				header.prop_hash = *parsed;
			} else if (key == "DPCORES_HASH") {
				std::string hx;
				is >> hx;
				auto parsed = parse_hex_u64(hx);
				require(parsed.has_value(),
						"Error: invalid DPCORES_HASH in certificate");
				header.dpcores_hash = *parsed;
			} else {
				// ignore unknown headers for forward-compat
			}
		}

		require(!header.width_name.empty() && header.width_value > 0,
				"Error: certificate missing WIDTH header");
		require(header.prop_hash != 0, "Error: certificate missing PROP_HASH");

		const uint64_t actual_prop_hash = fnv1a64_file(property_file);
		require(actual_prop_hash == header.prop_hash,
				"Error: property file hash mismatch (wrong property file?)");

		if (header.dpcores_hash.has_value()) {
			const uint64_t actual = dp_cores_fingerprint();
			require(actual == *header.dpcores_hash,
					"Error: DP-core fingerprint mismatch (wrong plugins?)");
		}

		Width width;
		width.set_name(header.width_name);
		width.set_value(static_cast<int>(header.width_value));
		InputController input_controller(property_file,
										 InputController::default_paths(),
										 width);
		DynamicKernel &kernel = input_controller.getDynamicKernel();
		Conjecture &conjecture = input_controller.getConjecture();

		if (header.premise) {
			auto *root = conjecture.getRoot();
			require(root && root->getType() == OPERATOR && root->getVal() == "implies",
					"Error: certificate has PREMISE=1 but the property is not an implication");
		}

		std::unordered_map<int, State::ptr> id_to_state;
		std::unordered_set<int> ids;
		std::unordered_set<State::ptr, State::ptr::Hash, State::ptr::Equal>
			state_set;
		std::vector<State::ptr> states_in_order;

		in.clear();
		in.seekg(0);
		// Skip preamble line.
		(void)std::getline(in, line);
		while (std::getline(in, line)) {
			trim_in_place(line);
			if (line.empty()) continue;

			if (starts_with(line, "H ")) continue;

			if (starts_with(line, "S ")) {
				require(!saw_result, "Error: state after result line");
				std::istringstream is(line);
				std::string S;
				int id = -1;
				std::string kind;
				is >> S >> id >> kind;
				require(id >= 0, "Error: invalid state id");
				require(!ids.count(id), "Error: duplicate state id");
				ids.insert(id);

				auto add_state = [&](State::ptr st) {
					require(!state_set.count(st),
							"Error: duplicate state content in certificate");
					state_set.insert(st);
					states_in_order.push_back(st);
					id_to_state.emplace(id, st);
				};

				if (kind == "LEAF") {
					require(!saw_leaf, "Error: multiple LEAF states");
					require(id == 0, "Error: LEAF state id must be 0");
					State::ptr s0 = kernel.initialState();
					s0 = canon_if_needed(s0, header.canon_mode);
					add_state(s0);
					saw_leaf = true;
				} else if (kind == "INTRO_V") {
					int parent = -1;
					unsigned u = 0;
					is >> parent >> u;
					require(id_to_state.count(parent),
							"Error: INTRO_V parent id not found");
					require(u >= 1 && u <= header.width_value + 1,
							"Error: INTRO_V label out of range: " +
								std::to_string(u));
					State::ptr p = id_to_state.at(parent);
					require(p->get_bag().vertex_introducible(u),
							"Error: INTRO_V label not introducible");
					State::ptr st = kernel.intro_v(p, u);
					st = canon_if_needed(st, header.canon_mode);
					add_state(st);
				} else if (kind == "FORGET_V") {
					int parent = -1;
					unsigned u = 0;
					is >> parent >> u;
					require(id_to_state.count(parent),
							"Error: FORGET_V parent id not found");
					State::ptr p = id_to_state.at(parent);
					require(p->get_bag().vertex_forgettable(u),
							"Error: FORGET_V label not forgettable");
					State::ptr st = kernel.forget_v(p, u);
					st = canon_if_needed(st, header.canon_mode);
					add_state(st);
				} else if (kind == "INTRO_E") {
					int parent = -1;
					unsigned u = 0, v = 0;
					is >> parent >> u >> v;
					require(id_to_state.count(parent),
							"Error: INTRO_E parent id not found");
					require(u != v, "Error: INTRO_E self-loop is not allowed");
					State::ptr p = id_to_state.at(parent);
					const auto bag = p->get_bag().get_elements();
					require(bag.count(u) && bag.count(v),
							"Error: INTRO_E labels are not both in the bag: " +
								std::to_string(u) + " " + std::to_string(v));
					State::ptr st = kernel.intro_e(p, u, v);
					st = canon_if_needed(st, header.canon_mode);
					add_state(st);
				} else if (kind == "JOIN") {
					int left = -1, right = -1;
					is >> left >> right;
					require(id_to_state.count(left),
							"Error: JOIN left parent id not found");
					require(id_to_state.count(right),
							"Error: JOIN right parent id not found");
					State::ptr l = id_to_state.at(left);
					State::ptr r = id_to_state.at(right);
					require(l->get_bag().joinable(r->get_bag()),
							"Error: JOIN parents are not joinable (bags differ)");

					std::optional<std::map<unsigned, unsigned>> m;
					{
						auto maybe_next = next_token(is);
						if (maybe_next.has_value()) {
							if (*maybe_next != "MAP") {
								require(false,
										"Error: unexpected token in JOIN line");
							}
							// Rewind stream back to before MAP token by
							// reconstructing with it.
							std::string rest = "MAP";
							{
								std::string tail;
								std::getline(is, tail);
								rest += tail;
							}
							std::istringstream is2(rest);
							m = parse_join_map(is2, l->get_bag().get_elements());
						}
					}
					State::ptr r2 = r;
					if (m.has_value()) {
						r2 = r->relabel(*m);
					}
					State::ptr st = kernel.join(l, r2);
					st = canon_if_needed(st, header.canon_mode);
					add_state(st);
				} else {
					std::cerr << "Error: unknown state kind: " << kind
							  << std::endl;
					return false;
				}
				continue;
			}

			if (starts_with(line, "R ")) {
				std::istringstream is(line);
				std::string R;
				std::string kind;
				is >> R >> kind;
				if (kind == "SATISFIED") {
					result_satisfied = true;
				} else if (kind == "NOT_SATISFIED") {
					is >> bad_id;
				} else if (kind == "INCOMPLETE") {
					std::cerr << "Error: certificate is incomplete" << std::endl;
					return false;
				} else {
					std::cerr << "Error: unknown result kind: " << kind
							  << std::endl;
					return false;
				}
				saw_result = true;
				break;
			}

			std::cerr << "Error: invalid certificate line: " << line << std::endl;
			return false;
		}

		require(saw_leaf, "Error: certificate missing LEAF state");
		require(saw_result, "Error: certificate missing result line");

		if (bad_id >= 0) {
			require(id_to_state.count(bad_id),
					"Error: bad state id not present in certificate");
			State::ptr bad = id_to_state.at(bad_id);
			require(!conjecture.evaluateConjectureOnState(*bad),
					"Certificate rejected: claimed bad state is not bad");
			std::cout << "Certificate ACCEPTED: PROPERTY NOT SATISFIED"
					  << std::endl;
			return true;
		}

		require(result_satisfied, "Error: missing SATISFIED result");

		// Safety: no bad state exists in the logged set.
		for (const auto &s : states_in_order) {
			if (!conjecture.evaluateConjectureOnState(*s)) {
				std::cout
					<< "Certificate rejected: found a bad reachable state"
					<< std::endl;
				return false;
			}
		}

		// Precompute grouping by bag for join closure.
		std::map<Bag, std::vector<State::ptr>> by_bag;
		for (const auto &s : states_in_order) {
			by_bag[s->get_bag()].push_back(s);
		}

		// Closure under successors.
		for (const auto &s : states_in_order) {
			Bag bag = s->get_bag();
			std::set<unsigned> bagElements = bag.get_elements();

			auto require_succ = [&](State::ptr succ) -> bool {
				succ = canon_if_needed(succ, header.canon_mode);
				if (header.premise) {
					if (!conjecture.evaluatePremiseOnState(*succ)) {
						return true; // pruned successor may be omitted
					}
				}
				if (!state_set.count(succ)) {
					std::cout
						<< "Certificate rejected: closure violation (missing successor)"
						<< std::endl;
					return false;
				}
				return true;
			};

			// IntroVertex
			for (unsigned u = 1; u <= header.width_value + 1; ++u) {
				if (!bag.vertex_introducible(u)) continue;
				if (!require_succ(kernel.intro_v(s, u))) return false;
			}

			// ForgetVertex
			for (auto u : bagElements) {
				if (!require_succ(kernel.forget_v(s, u))) return false;
			}

			// IntroEdge
			if (bagElements.size() > 1) {
				for (auto it = bagElements.begin(); it != bagElements.end();
					 ++it) {
					auto it2 = it;
					++it2;
					for (; it2 != bagElements.end(); ++it2) {
						if (!require_succ(kernel.intro_e(s, *it, *it2)))
							return false;
					}
				}
			}
		}

		// Join closure (treewidth only).
		if (header.width_name == "tree_width") {
			for (const auto &[bag, vec] : by_bag) {
				(void)bag;
				for (const auto &s1 : vec) {
					for (const auto &s2 : vec) {
						if (header.canon_mode == CanonMode::NONE) {
							State::ptr succ = kernel.join(s1, s2);
							succ = canon_if_needed(succ, header.canon_mode);
							if (header.premise &&
								!conjecture.evaluatePremiseOnState(*succ)) {
								continue;
							}
							if (!state_set.count(succ)) {
								std::cout << "Certificate rejected: closure violation (missing successor)"
										  << std::endl;
								return false;
							}
						} else {
							// Canonized join considers all boundary permutations,
							// mirroring isomorphism-BFS join exploration.
							std::map<unsigned, unsigned> m =
								identity_map(s1->get_bag().get_elements());
							do {
								State::ptr relabeled = s2->relabel(m);
								State::ptr succ = kernel.join(s1, relabeled);
								succ = canon_if_needed(succ, header.canon_mode);
								if (header.premise &&
									!conjecture.evaluatePremiseOnState(*succ)) {
									continue;
								}
								if (!state_set.count(succ)) {
									std::cout << "Certificate rejected: closure violation (missing successor)"
											  << std::endl;
									return false;
								}
							} while (next_permutation_map(m));
						}
					}
				}
			}
		}

		std::cout << "Certificate ACCEPTED: PROPERTY SATISFIED" << std::endl;
		return true;
	} catch (const std::exception &) {
		return false;
	}
}
