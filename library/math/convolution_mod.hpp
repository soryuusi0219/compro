// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_mod.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <vector>

#include "convolution.hpp"
#include "internal_math.hpp"
#include "modint_traits.hpp"

namespace soryuusi {

namespace internal {

template <std::uint64_t StaticMod, class T, class Out, class Read, class Write>
std::vector<Out> convolution_mod_impl(const std::vector<T>& a, const std::vector<T>& b,
                                      std::uint64_t modulus, Read read, Write write) {
    if (a.empty() || b.empty()) return {};
    const std::uint64_t mod = StaticMod ? StaticMod : modulus;
    assert(mod > 0);
    assert(a.size() + b.size() - 1 <= std::numeric_limits<int>::max());
    const int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
    const int length = n + (m - 1);
    std::vector<Out> result(length);
    if (mod == 1) return result;

    if (std::min(n, m) <= 60) {
        std::vector<std::uint64_t> left(n), right(m);
        std::transform(a.begin(), a.end(), left.begin(), read);
        std::transform(b.begin(), b.end(), right.begin(), read);
        auto multiply = [&]<bool SmallMod>() {
            for (int k = 0; k < length; ++k) {
                const int lo = std::max(0, k - (m - 1)), hi = std::min(k, n - 1);
                __uint128_t sum = 0;
                for (int i = lo; i <= hi; ++i) {
                    if constexpr (SmallMod) sum += left[i] * right[k - i];
                    else sum = (sum + __uint128_t(left[i]) * right[k - i]) % mod;
                }
                result[k] = write(static_cast<std::uint64_t>(sum % mod));
            }
        };
        if (mod <= std::numeric_limits<std::uint32_t>::max()) multiply.template operator()<true>();
        else multiply.template operator()<false>();
        return result;
    }

    constexpr std::uint64_t p = 167772161, q = 469762049, r = 754974721;
    constexpr std::uint64_t inv_p = inv_gcd(p, q).second;
    constexpr std::uint64_t inv_pq = inv_gcd(p * q, r).second;
    const auto x = convolution_residues<p>(a, b, read);
    const auto y = convolution_residues<q>(a, b, read);
    const auto z = convolution_residues<r>(a, b, read);
    const std::uint64_t pq = p * q % mod;
    auto restore = [&]<class Wide>() {
        for (int i = 0; i < length; ++i) {
            const std::uint64_t lower = x[i].val() + (y[i].val() + q - x[i].val()) * inv_p % q * p;
            const std::uint64_t upper = (z[i].val() + r - lower % r) * inv_pq % r;
            result[i] = write(static_cast<std::uint64_t>((lower + Wide(upper) * pq) % mod));
        }
    };
    if (mod <= std::numeric_limits<std::uint32_t>::max()) restore.template operator()<std::uint64_t>();
    else restore.template operator()<__uint128_t>();
    return result;
}

}  // namespace internal

template <long long Mod = 1000000007>
std::vector<long long> convolution_mod(const std::vector<long long>& a,
                                       const std::vector<long long>& b) {
    static_assert(Mod > 0);
    auto read = [](long long x) {
        if (0 <= x && x < Mod) return static_cast<std::uint64_t>(x);
        return static_cast<std::uint64_t>(internal::safe_mod(x, Mod));
    };
    auto write = [](std::uint64_t x) { return static_cast<long long>(x); };
    return internal::convolution_mod_impl<Mod, long long, long long>(a, b, Mod, read, write);
}

template <class Mint, internal::is_modint_like_t<Mint>* = nullptr>
std::vector<Mint> convolution_mod(const std::vector<Mint>& a, const std::vector<Mint>& b) {
    constexpr std::uint64_t static_mod = [] {
        if constexpr (internal::is_static_modint_like_v<Mint>) return std::uint64_t(Mint::mod());
        else return std::uint64_t(0);
    }();
    auto read = [](const Mint& x) { return static_cast<std::uint64_t>(x.val()); };
    auto write = [](std::uint64_t x) { return Mint::raw(x); };
    return internal::convolution_mod_impl<static_mod, Mint, Mint>(a, b, Mint::mod(), read, write);
}

}  // namespace soryuusi
