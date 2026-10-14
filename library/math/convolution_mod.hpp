// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_mod.hpp
#pragma once
#include <vector>

#include "library/math/convolution.hpp"
#include "library/math/modint_traits.hpp"

namespace soryuusi {

using namespace std;

template<long long MOD = 1000000007>
vector<long long> convolution_mod(const vector<long long>& a, const vector<long long>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    static constexpr unsigned long long MOD1 = 167772161;
    static constexpr unsigned long long MOD2 = 469762049;
    static constexpr unsigned long long MOD3 = 754974721;
    static constexpr unsigned long long M1M2 = (MOD1 * MOD2) % MOD;
    static constexpr unsigned long long i1 = internal::inv_gcd(MOD1, MOD2).second;
    static constexpr unsigned long long i2 = internal::inv_gcd(MOD1 * MOD2, MOD3).second;
    auto c1 = soryuusi::convolution<MOD1>(a, b);
    auto c2 = soryuusi::convolution<MOD2>(a, b);
    auto c3 = soryuusi::convolution<MOD3>(a, b);
    vector<long long> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
      long long s = c1[i] + (((c2[i] + MOD2 - c1[i]) * i1) % MOD2) * MOD1;
      c[i] = (s + (((c3[i] + MOD3 - (s % MOD3)) * i2) % MOD3) * M1M2) % MOD;
    }
    return c;
}

template <class Mint, internal::is_modint_like_t<Mint>* = nullptr>
vector<Mint> convolution_mod(const vector<Mint>& a, const vector<Mint>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    static constexpr unsigned long long MOD1 = 167772161;
    static constexpr unsigned long long MOD2 = 469762049;
    static constexpr unsigned long long MOD3 = 754974721;
    static constexpr unsigned long long i1 = internal::inv_gcd(MOD1, MOD2).second;
    static constexpr unsigned long long i2 = internal::inv_gcd(MOD1 * MOD2, MOD3).second;
    const auto mod = static_cast<unsigned long long>(Mint::mod());
    const unsigned long long M1M2 = (MOD1 * MOD2) % mod;
    vector<long long> a2(n);
    for (int i = 0; i < n; i++) a2[i] = a[i].val();
    vector<long long> b2(m);
    for (int i = 0; i < m; i++) b2[i] = b[i].val();
    auto c1 = soryuusi::convolution<MOD1>(a2, b2);
    auto c2 = soryuusi::convolution<MOD2>(a2, b2);
    auto c3 = soryuusi::convolution<MOD3>(a2, b2);
    vector<Mint> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
      long long s = c1[i] + (((c2[i] + MOD2 - c1[i]) * i1) % MOD2) * MOD1;
      c[i] = s + (((c3[i] + MOD3 - (s % MOD3)) * i2) % MOD3) * M1M2;
    }
    return c;
}

}  // namespace soryuusi
