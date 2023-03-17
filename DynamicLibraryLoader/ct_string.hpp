#pragma once
#include <algorithm>
#include <compare>
#include <cstddef>

template <std::size_t N>
struct CTString {
	char data[N];
	consteval CTString(const char (&src)[N]) { std::copy(src, src + N, data); }
	constexpr auto operator[](std::size_t i) const -> char { return data[i]; }
	constexpr auto c_str() -> char * { return data; }
	static constexpr auto size() -> std::size_t { return N - 1; }

	template <std::size_t M>
	constexpr friend auto operator<=>(const CTString<N> &lhs,
									  const CTString<M> &rhs)
		-> std::strong_ordering {
		for (std::size_t i = 0; i != lhs.size() && i != rhs.size(); ++i) {
			auto res = lhs[i] <=> rhs[i];
			if (res != std::strong_ordering::equal) return res;
		}
		return N <=> M;
	}

	template <std::size_t M>
	constexpr friend auto operator==(const CTString<N> &lhs,
									 const CTString<M> &rhs) -> bool {
		return (lhs <=> rhs) == std::strong_ordering::equal;
	}
	template <std::size_t M>
	constexpr friend auto operator!=(const CTString<N> &lhs,
									 const CTString<M> &rhs) -> bool {
		return (lhs <=> rhs) != std::strong_ordering::equal;
	}
	template <std::size_t M>
	constexpr friend auto operator<(const CTString<N> &lhs,
									const CTString<M> &rhs) -> bool {
		return (lhs <=> rhs) == std::strong_ordering::less;
	}
	template <std::size_t M>
	constexpr friend auto operator>(const CTString<N> &lhs,
									const CTString<M> &rhs) -> bool {
		return (lhs <=> rhs) == std::strong_ordering::greater;
	}
	template <std::size_t M>
	constexpr friend auto operator<=(const CTString<N> &lhs,
									 const CTString<M> &rhs) -> bool {
		return (lhs <=> rhs) != std::strong_ordering::greater;
	}
	template <std::size_t M>
	constexpr friend auto operator>=(const CTString<N> &lhs,
									 const CTString<M> &rhs) -> bool {
		return (lhs <=> rhs) != std::strong_ordering::less;
	}
};

