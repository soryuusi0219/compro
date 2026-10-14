// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution.hpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "library/math/ntt.hpp"
#include "library/math/modint_traits.hpp"

namespace soryuusi {

namespace internal {

template <unsigned int Mod>
class convolution_mint {
  public:
    static constexpr unsigned int mod() { return Mod; }
    static constexpr convolution_mint raw(unsigned int value) {
        convolution_mint result;
        result.value_ = value;
        return result;
    }

    constexpr convolution_mint() = default;

    template <class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    constexpr convolution_mint(T value) {
        if constexpr (std::is_signed_v<T>) {
            __int128_t reduced = static_cast<__int128_t>(value) % Mod;
            if (reduced < 0) reduced += Mod;
            value_ = static_cast<unsigned int>(reduced);
        } else {
            value_ = static_cast<unsigned int>(
                static_cast<__uint128_t>(value) % Mod);
        }
    }

    constexpr unsigned int val() const { return value_; }

    constexpr convolution_mint& operator+=(const convolution_mint& rhs) {
        value_ += rhs.value_;
        if (value_ >= Mod) value_ -= Mod;
        return *this;
    }
    constexpr convolution_mint& operator-=(const convolution_mint& rhs) {
        value_ -= rhs.value_;
        if (value_ >= Mod) value_ += Mod;
        return *this;
    }
    constexpr convolution_mint& operator*=(const convolution_mint& rhs) {
        value_ = static_cast<unsigned int>(
            static_cast<unsigned long long>(value_) * rhs.value_ % Mod);
        return *this;
    }
    constexpr convolution_mint pow(long long exponent) const {
        convolution_mint base = *this, result = 1;
        while (exponent) {
            if (exponent & 1) result *= base;
            base *= base;
            exponent >>= 1;
        }
        return result;
    }
    constexpr convolution_mint inv() const { return pow(Mod - 2); }

    friend constexpr convolution_mint operator+(convolution_mint lhs,
                                                 const convolution_mint& rhs) {
        return lhs += rhs;
    }
    friend constexpr convolution_mint operator-(convolution_mint lhs,
                                                 const convolution_mint& rhs) {
        return lhs -= rhs;
    }
    friend constexpr convolution_mint operator*(convolution_mint lhs,
                                                 const convolution_mint& rhs) {
        return lhs *= rhs;
    }

  private:
    unsigned int value_ = 0;
};

template <class Mint>
std::vector<Mint> convolution_ntt(std::vector<Mint> a, std::vector<Mint> b) {
    const int result_size = static_cast<int>(a.size() + b.size() - 1);
    const int z = 1 << ceil_pow2(result_size);
    a.resize(z);
    b.resize(z);
    NTTEngine<Mint>::ntt(a);
    NTTEngine<Mint>::ntt(b);
    for (int i = 0; i < z; ++i) a[i] *= b[i];
    NTTEngine<Mint>::intt(a);
    a.resize(result_size);
    return a;
}

// Block convolution for results longer than the largest NTT supported by the
// modulus.  A block is half the transform length so adjacent block products
// can be added without cyclic wraparound.
template <class Mint>
std::vector<Mint> convolution_large(std::vector<Mint> a, std::vector<Mint> b) {
    using NTT = NTTEngine<Mint>;
    static_assert(NTT::rank2 >= 1, "large convolution requires a radix-2 root");
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    const int result_size = n + m - 1;
    const int transform_size = 1 << NTT::rank2;
    const int block_size = transform_size >> 1;

    auto transform_blocks = [&](const std::vector<Mint>& values) {
        const int count =
            (static_cast<int>(values.size()) + block_size - 1) / block_size;
        std::vector<std::vector<Mint>> blocks;
        blocks.reserve(count);
        for (int offset = 0; offset < static_cast<int>(values.size());
             offset += block_size) {
            const int length = std::min(block_size,
                                        static_cast<int>(values.size()) - offset);
            blocks.emplace_back(transform_size);
            std::copy_n(values.begin() + offset, length, blocks.back().begin());
            NTT::ntt(blocks.back());
        }
        return blocks;
    };

    auto transformed_a = transform_blocks(a);
    auto transformed_b = transform_blocks(b);
    std::vector<Mint>().swap(a);
    std::vector<Mint>().swap(b);

    std::vector<Mint> result(result_size);
    const int result_blocks =
        static_cast<int>(transformed_a.size() + transformed_b.size() - 1);
    for (int block = 0; block < result_blocks; ++block) {
        std::vector<Mint> frequency(transform_size);
        const int first_a = std::max(0, block + 1 - static_cast<int>(transformed_b.size()));
        const int last_a = std::min(block + 1, static_cast<int>(transformed_a.size()));
        for (int i = first_a; i < last_a; ++i) {
            const auto& x = transformed_a[i];
            const auto& y = transformed_b[block - i];
            for (int j = 0; j < transform_size; ++j) frequency[j] += x[j] * y[j];
        }
        NTT::intt(frequency);
        const int offset = block * block_size;
        const int length = std::min(transform_size, result_size - offset);
        for (int i = 0; i < length; ++i) result[offset + i] += frequency[i];
    }
    return result;
}

}  // namespace internal

template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
std::vector<Mint> convolution(std::vector<Mint> a, std::vector<Mint> b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (n == 0 || m == 0) return {};
    if (std::min(n, m) <= 60) {
        if (n < m) {
            std::swap(n, m);
            std::swap(a, b);
        }
        std::vector<Mint> ans(n + m - 1);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) ans[i + j] += a[i] * b[j];
        }
        return ans;
    }
    const long long result_size = static_cast<long long>(n) + m - 1;
    const long long max_transform = 1LL << NTTEngine<Mint>::rank2;
    if (result_size <= max_transform) {
        return internal::convolution_ntt(std::move(a), std::move(b));
    }
    return internal::convolution_large(std::move(a), std::move(b));
}

template <unsigned int Mod = 998244353, class T,
          std::enable_if_t<std::is_integral_v<T>>* = nullptr>
std::vector<T> convolution(const std::vector<T>& a, const std::vector<T>& b) {
    static_assert(Mod <= 2147483647U);
    using Mint = internal::convolution_mint<Mod>;
    std::vector<Mint> x(a.begin(), a.end()), y(b.begin(), b.end());
    const auto z = convolution(std::move(x), std::move(y));
    std::vector<T> result(z.size());
    for (std::size_t i = 0; i < z.size(); ++i) result[i] = static_cast<T>(z[i].val());
    return result;
}

inline std::vector<long long> convolution_ll(const std::vector<long long>& a,
                                              const std::vector<long long>& b) {
    if (a.empty() || b.empty()) return {};
    static constexpr unsigned long long m1 = 754974721;
    static constexpr unsigned long long m2 = 167772161;
    static constexpr unsigned long long m3 = 469762049;
    static constexpr unsigned long long m2m3 = m2 * m3;
    static constexpr unsigned long long m1m3 = m1 * m3;
    static constexpr unsigned long long m1m2 = m1 * m2;
    static constexpr unsigned long long m1m2m3 = m1 * m2 * m3;
    static constexpr unsigned long long i1 = internal::inv_gcd(m2 * m3, m1).second;
    static constexpr unsigned long long i2 = internal::inv_gcd(m1 * m3, m2).second;
    static constexpr unsigned long long i3 = internal::inv_gcd(m1 * m2, m3).second;
    const auto c1 = convolution<m1>(a, b);
    const auto c2 = convolution<m2>(a, b);
    const auto c3 = convolution<m3>(a, b);
    std::vector<long long> c(c1.size());
    for (std::size_t i = 0; i < c.size(); ++i) {
        unsigned long long x = 0;
        x += c1[i] * i1 % m1 * m2m3;
        x += c2[i] * i2 % m2 * m1m3;
        x += c3[i] * i3 % m3 * m1m2;
        long long diff = c1[i] - internal::safe_mod(static_cast<long long>(x), m1);
        if (diff < 0) diff += m1;
        static constexpr unsigned long long offset[] = {
            0, 0, m1m2m3, 2 * m1m2m3, 3 * m1m2m3
        };
        x -= offset[diff % 5];
        c[i] = static_cast<long long>(x);
    }
    return c;
}

}  // namespace soryuusi
