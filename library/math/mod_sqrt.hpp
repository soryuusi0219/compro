// https://github.com/soryuusi0219/compro/blob/main/library/math/mod_sqrt.hpp
#pragma once

#include <cassert>
#include <cstdint>

#include "library/math/internal_math.hpp"
#include "library/math/modint_traits.hpp"

namespace soryuusi {
namespace internal {

// Tonelli-Shanks. p must be prime. Returns UINT64_MAX if no root exists.
constexpr std::uint64_t mod_sqrt64(std::uint64_t a, std::uint64_t p) {
    assert(is_prime64_constexpr(p));
    a %= p;
    if (a == 0 || p == 2) return a;
    if (pow_mod64_constexpr(a, (p - 1) / 2, p) != 1) return UINT64_MAX;
    if (p % 4 == 3) return pow_mod64_constexpr(a, (p + 1) / 4, p);

    std::uint64_t q = p - 1;
    int s = 0;
    while ((q & 1) == 0) {
        q >>= 1;
        ++s;
    }
    std::uint64_t z = 2;
    while (pow_mod64_constexpr(z, (p - 1) / 2, p) != p - 1) ++z;

    std::uint64_t c = pow_mod64_constexpr(z, q, p);
    std::uint64_t x = pow_mod64_constexpr(a, (q + 1) / 2, p);
    std::uint64_t t = pow_mod64_constexpr(a, q, p);
    int m = s;
    while (t != 1) {
        int i = 1;
        std::uint64_t y = mul_mod64(t, t, p);
        while (y != 1) {
            y = mul_mod64(y, y, p);
            ++i;
        }
        const std::uint64_t b =
            pow_mod64_constexpr(c, std::uint64_t{1} << (m - i - 1), p);
        x = mul_mod64(x, b, p);
        c = mul_mod64(b, b, p);
        t = mul_mod64(t, c, p);
        m = i;
    }
    return x;
}

}  // namespace internal

inline long long mod_sqrt(long long a, long long p) {
    assert(2 <= p && internal::is_prime64_constexpr(static_cast<std::uint64_t>(p)));
    const auto root = internal::mod_sqrt64(
        static_cast<std::uint64_t>(internal::safe_mod(a, p)),
        static_cast<std::uint64_t>(p));
    return root == UINT64_MAX ? -1 : static_cast<long long>(root);
}

template <int Mod = 998244353>
constexpr long long mod_sqrt(long long a) {
    static_assert(internal::is_prime<Mod>);
    const long long normalized = internal::safe_mod(a, Mod);
    const auto root = internal::mod_sqrt64(normalized, Mod);
    return root == UINT64_MAX ? -1 : static_cast<long long>(root);
}

template <class Mint, internal::is_modint_like_t<Mint>* = nullptr>
constexpr Mint mod_sqrt(Mint a) {
    const auto root =
        internal::mod_sqrt64(a.val(), static_cast<std::uint64_t>(Mint::mod()));
    return root == UINT64_MAX ? Mint(-1) : Mint::raw(root);
}

// Backward-compatible spelling.
inline long long modsqrt(long long a, long long p) { return mod_sqrt(a, p); }

template <long long Mod = 998244353>
constexpr long long modsqrt(long long a) {
    static_assert(Mod <= 2147483647LL);
    return mod_sqrt<static_cast<int>(Mod)>(a);
}

template <class Mint, internal::is_modint_like_t<Mint>* = nullptr>
constexpr Mint modsqrt(Mint a) {
    return mod_sqrt(a);
}

}  // namespace soryuusi
