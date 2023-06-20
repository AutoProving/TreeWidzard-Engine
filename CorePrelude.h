#include "Kernel/Bag.h"
#include "Kernel/CoreWrapper.h"
#include "Kernel/WitnessSet.h"
#include "Kernel/WitnessWrapper.h"
#include <optional>

namespace detail {

template<std::size_t index, class ... Ts>
  auto try_unpack_typed_args_impl(std::tuple<Ts...> &result, const auto &vec) -> bool {
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

/// Given a vector of variants,
/// try to unpack them into a tuple
/// that is no longer variant (i.e. typed).
///
/// Returns nullopt if the vector is not of the right size,
/// or if any of the variants are not of the right type.
template<class ... Ts>
auto try_unpack_typed_args(const auto &vec) -> std::optional<std::tuple<Ts...>> {
  if (vec.size() != sizeof...(Ts))
    return std::nullopt;

  auto result = std::tuple<Ts...>{};

  if (!detail::try_unpack_typed_args_impl<0, Ts...>(result, vec))
    return std::nullopt;

  return result;
}

/// Given a vector of variants,
/// try to unpack them into a tuple
/// that is no longer variant (i.e. typed).
///
/// Throws if the vector is not of the right size,
/// or if any of the variants are not of the right type.
template<class ... Ts>
auto unpack_typed_args(const auto &vec) -> std::tuple<Ts...> {
  if (auto result = try_unpack_typed_args<Ts...>(vec))
    return *result;
  else
    throw std::runtime_error("unpack_typed: failed to unpack");
}

