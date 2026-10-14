// https://github.com/soryuusi0219/compro/blob/main/library/math/modint_traits.hpp
#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

namespace soryuusi::internal {

template <class T, class = void>
struct is_modint_like : std::false_type {};

template <class T>
struct is_modint_like<
    T,
    std::void_t<decltype(T::mod()), decltype(T::raw(0)),
                decltype(std::declval<const T&>().val())>> : std::true_type {};

template <class T>
inline constexpr bool is_modint_like_v = is_modint_like<T>::value;

template <class T>
using is_modint_like_t = std::enable_if_t<is_modint_like_v<T>>;

template <class T, class = void>
struct is_static_modint_like : std::false_type {};

template <class T>
struct is_static_modint_like<
    T,
    std::void_t<decltype(std::integral_constant<
        std::uint64_t, static_cast<std::uint64_t>(T::mod())>{})>>
    : is_modint_like<T> {};

template <class T>
inline constexpr bool is_static_modint_like_v = is_static_modint_like<T>::value;

template <class T>
using is_static_modint_like_t = std::enable_if_t<is_static_modint_like_v<T>>;

}  // namespace soryuusi::internal

namespace soryuusi {

template <class T, class = void>
struct is_ntt_friendly : std::false_type {};

template <class T>
struct is_ntt_friendly<
    T, std::enable_if_t<internal::is_static_modint_like_v<T>>>
    : std::bool_constant<(static_cast<std::uint64_t>(T::mod() - 1) &
                          ((std::uint64_t{1} << 20) - 1)) == 0> {};

template <class T>
inline constexpr bool is_ntt_friendly_v = is_ntt_friendly<T>::value;

}  // namespace soryuusi
