// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#pragma once

// Core kernel headers — these transitively include most STL headers needed by DP cores
// (algorithm, cassert, iterator, map, memory, set, string, utility, vector)
#include "Kernel/Bag.h"
#include "Kernel/CoreWrapper.h"
#include "Kernel/WitnessSet.h"
#include "Kernel/WitnessWrapper.h"
#include "Kernel/DynamicCore.h"

// Additional STL headers not guaranteed by kernel includes
#include <optional>
#include <unordered_map>
#include <iostream>

namespace detail {

template <std::size_t index, class... Ts>
auto try_unpack_typed_args_impl(std::tuple<Ts...> &result, const auto &vec)
	-> bool {
	if constexpr (index == sizeof...(Ts)) {
		return true;
	} else {
		using T = std::tuple_element_t<index, std::tuple<Ts...>>;
		if (auto ptr = std::get_if<T>(&vec[index])) {
			std::get<index>(result) = *ptr;
			return try_unpack_typed_args_impl<index + 1, Ts...>(result, vec);
		} else {
			return false;
		}
	}
}

} // namespace detail

template <class... Ts>
auto try_unpack_typed_args(const auto &vec)
	-> std::optional<std::tuple<Ts...>> {
	if (vec.size() != sizeof...(Ts)) return std::nullopt;

	auto result = std::tuple<Ts...>{};

	if (!detail::try_unpack_typed_args_impl<0, Ts...>(result, vec))
		return std::nullopt;

	return result;
}

template <class... Ts>
auto unpack_typed_args(const auto &vec) -> std::tuple<Ts...> {
	if (auto result = try_unpack_typed_args<Ts...>(vec))
		return *result;
	else
		throw std::runtime_error("unpack_typed: failed to unpack");
}
