// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution.hpp
#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include "internal_math.hpp"
#include "modint_traits.hpp"
#include "ntt.hpp"

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
            using Wide = std::conditional_t<sizeof(T) <= 8, std::int64_t, __int128_t>;
            Wide reduced = static_cast<Wide>(value) % static_cast<Wide>(Mod);
            if (reduced < 0) reduced += Mod;
            value_ = static_cast<unsigned int>(reduced);
        } else {
            using Wide = std::conditional_t<sizeof(T) <= 8, std::uint64_t, __uint128_t>;
            value_ = static_cast<unsigned int>(static_cast<Wide>(value) % Mod);
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

template <unsigned int Mod>
inline constexpr bool has_native_ntt_residue<convolution_mint<Mod>> = true;

template <class Mint>
inline constexpr bool convolution_has_ntt = [] {
    if constexpr (Mint::mod() < 3 || Mint::mod() > 2147483647ULL) return false;
    else return internal::is_prime<static_cast<int>(Mint::mod())>;
}();

template <class Mint>
std::vector<Mint> convolution_slice_naive(std::span<const Mint> a, std::span<const Mint> b,
                                          int first, int last) {
    const int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
    std::vector<Mint> result(last - first);
    for (int k = first; k < last; ++k) {
        const int lo = std::max(0, k - (m - 1)), hi = std::min(k, n - 1);
        if constexpr (Mint::mod() <= 2147483647ULL) {
            __uint128_t sum = 0;
            for (int i = lo; i <= hi; ++i) sum += std::uint64_t(a[i].val()) * b[k - i].val();
            result[k - first] = Mint::raw(static_cast<int>(sum % Mint::mod()));
        } else {
            Mint sum = 0;
            for (int i = lo; i <= hi; ++i) sum += a[i] * b[k - i];
            result[k - first] = sum;
        }
    }
    return result;
}

template <bool Square, class Mint>
void frequency_product(std::vector<Mint>& a, std::vector<Mint>& b, int size) {
    using NTT = NTTEngine<Mint>;
    a.resize(size);
    NTT::ntt(a);
    if constexpr (Square) {
        for (Mint& x : a) x *= x;
    } else {
        b.resize(size);
        NTT::ntt(b);
        for (int i = 0; i < size; ++i) a[i] *= b[i];
    }
    NTT::intt_unnormalized(a);
}

template <class Mint>
std::vector<Mint> convolution_overlap(std::vector<Mint> a, std::vector<Mint> b, int size) {
    using NTT = NTTEngine<Mint>;
    const int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
    const int block = size - m + 1;
    std::vector<Mint> result(n + m - 1), buffer(size);
    b.resize(size);
    NTT::ntt(b);
    for (int offset = 0; offset < n;) {
        const int count = std::min(block, n - offset);
        std::copy_n(a.begin() + offset, count, buffer.begin());
        std::fill(buffer.begin() + count, buffer.end(), Mint(0));
        NTT::ntt(buffer);
        for (int i = 0; i < size; ++i) buffer[i] *= b[i];
        NTT::intt_unnormalized(buffer);
        const int length = count + (m - 1);
        NTT::normalize(buffer.begin(), buffer.begin() + length, size);
        for (int i = 0; i < length; ++i) result[offset + i] += buffer[i];
        offset += count;
    }
    return result;
}

// Half-transform blocks allow each block product to be added without wrapping.
template <class Mint, bool Square = false>
std::vector<Mint> convolution_large(std::vector<Mint> a, std::vector<Mint> b) {
    using NTT = NTTEngine<Mint>;
    const int n = static_cast<int>(a.size());
    const int m = Square ? n : static_cast<int>(b.size());
    const int result_size = n + (m - 1);
    const int size = 1 << NTT::rank2, block_size = size / 2;
    if constexpr (!Square) {
        if (m <= block_size) return convolution_overlap(std::move(a), std::move(b), size);
    }
    auto transform_blocks = [&](std::vector<Mint>& values) {
        std::vector<std::vector<Mint>> blocks;
        blocks.reserve((values.size() - 1) / block_size + 1);
        for (int offset = 0; offset < static_cast<int>(values.size());) {
            const int count = std::min(block_size, static_cast<int>(values.size()) - offset);
            blocks.emplace_back(size);
            std::copy_n(values.begin() + offset, count, blocks.back().begin());
            NTT::ntt(blocks.back());
            offset += count;
        }
        std::vector<Mint>().swap(values);
        return blocks;
    };

    auto left = transform_blocks(a);
    auto right_storage = Square ? std::vector<std::vector<Mint>>{} : transform_blocks(b);
    const auto& right = Square ? left : right_storage;
    std::vector<Mint> result(result_size), frequency(size);
    const int left_count = static_cast<int>(left.size()), right_count = static_cast<int>(right.size());
    for (int k = 0; k < left_count + right_count - 1; ++k) {
        const int first = std::max(0, k - (right_count - 1));
        const int last = std::min(left_count - 1, Square ? k / 2 : k);
        const auto& x = left[first];
        const auto& y = right[k - first];
        for (int j = 0; j < size; ++j) frequency[j] = x[j] * y[j];
        if constexpr (Square) {
            if (2 * first != k) for (Mint& value : frequency) value += value;
        }
        for (int i = first + 1; i <= last; ++i) {
            const auto& u = left[i];
            const auto& v = right[k - i];
            if constexpr (Square) {
                if (2 * i != k) {
                    for (int j = 0; j < size; ++j) {
                        const Mint value = u[j] * v[j];
                        frequency[j] += value + value;
                    }
                    continue;
                }
            }
            for (int j = 0; j < size; ++j) frequency[j] += u[j] * v[j];
        }
        NTT::intt_unnormalized(frequency);
        const int offset = k * block_size;
        const int length = std::min(size, result_size - offset);
        NTT::normalize(frequency.begin(), frequency.begin() + length, size);
        for (int i = 0; i < length; ++i) result[offset + i] += frequency[i];
    }
    return result;
}

template <bool Square, class Mint>
std::vector<Mint> convolution_linear(std::vector<Mint> a, std::vector<Mint> b) {
    if (a.empty() || (!Square && b.empty())) return {};
    if constexpr (!Square) {
        if (a.size() < b.size()) a.swap(b);
    }
    const int n = static_cast<int>(a.size()), m = Square ? n : static_cast<int>(b.size());
    assert(a.size() + static_cast<std::size_t>(m) - 1 <= std::numeric_limits<int>::max());
    const int length = n + (m - 1);
    if (m == 1) {
        const Mint scale = Square ? a[0] : b[0];
        for (Mint& value : a) value *= scale;
        return a;
    }
    const auto& right = Square ? a : b;
    if constexpr (!convolution_has_ntt<Mint>) {
        return convolution_slice_naive<Mint>(a, right, 0, length);
    } else {
        using NTT = NTTEngine<Mint>;
        if (m <= (Square ? 32 : 39)) return convolution_slice_naive<Mint>(a, right, 0, length);
        const unsigned int size = std::bit_ceil(static_cast<unsigned int>(length));
        const int max_size = 1 << NTT::rank2;
        if constexpr (!Square) {
            const unsigned int block_size = std::bit_ceil(2U * m - 1);
            if (block_size <= static_cast<unsigned int>(max_size) &&
                block_size <= size / 16 &&
                4 * std::countr_zero(block_size) + 2 < 3 * std::countr_zero(size)) {
                return convolution_overlap(std::move(a), std::move(b), block_size);
            }
            if (m <= 60) return convolution_slice_naive<Mint>(a, right, 0, length);
        }
        const int shortened = size / 2;
        if (n <= shortened && shortened <= max_size && length - shortened <= 32) {
            // The high tail wraps exactly once and can be removed from the low coefficients.
            auto tail = convolution_slice_naive<Mint>(a, right, shortened, length);
            frequency_product<Square>(a, b, shortened);
            NTT::normalize(a.begin(), a.end(), shortened);
            for (int i = 0; i < length - shortened; ++i) a[i] -= tail[i];
            a.insert(a.end(), tail.begin(), tail.end());
            return a;
        }
        if (size > static_cast<unsigned int>(max_size)) return convolution_large<Mint, Square>(std::move(a), std::move(b));
        frequency_product<Square>(a, b, static_cast<int>(size));
        a.resize(length);
        NTT::normalize(a.begin(), a.end(), static_cast<int>(size));
        return a;
    }
}

template <class T, class Mint>
std::vector<T> convolution_values(const std::vector<Mint>& values) {
    std::vector<T> result(values.size());
    for (std::size_t i = 0; i < values.size(); ++i) result[i] = static_cast<T>(values[i].val());
    return result;
}

template <unsigned int Mod, class T, class Map>
auto convolution_residues(const std::vector<T>& a, const std::vector<T>& b, Map map) {
    using Mint = convolution_mint<Mod>;
    std::vector<Mint> left(a.size()), right(b.size());
    std::transform(a.begin(), a.end(), left.begin(), map);
    std::transform(b.begin(), b.end(), right.begin(), map);
    return convolution_linear<false>(std::move(left), std::move(right));
}

}  // namespace internal

template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
std::vector<Mint> convolution(std::vector<Mint> a, std::vector<Mint> b) {
    return internal::convolution_linear<false>(std::move(a), std::move(b));
}

template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
std::vector<Mint> convolution_square(std::vector<Mint> a) {
    return internal::convolution_linear<true>(std::move(a), {});
}

template <unsigned int Mod = 998244353, class T,
          std::enable_if_t<std::is_integral_v<T>>* = nullptr>
std::vector<T> convolution(const std::vector<T>& a, const std::vector<T>& b) {
    static_assert(1 <= Mod && Mod <= 2147483647U);
    using Mint = internal::convolution_mint<Mod>;
    auto values = convolution(std::vector<Mint>(a.begin(), a.end()),
                              std::vector<Mint>(b.begin(), b.end()));
    return internal::convolution_values<T>(values);
}

template <unsigned int Mod = 998244353, class T,
          std::enable_if_t<std::is_integral_v<T>>* = nullptr>
std::vector<T> convolution_square(const std::vector<T>& a) {
    static_assert(1 <= Mod && Mod <= 2147483647U);
    using Mint = internal::convolution_mint<Mod>;
    auto values = convolution_square(std::vector<Mint>(a.begin(), a.end()));
    return internal::convolution_values<T>(values);
}

template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
std::vector<Mint> middle_product(std::vector<Mint> a, std::vector<Mint> b) {
    if (b.empty() || a.size() < b.size()) return {};
    assert(a.size() <= std::numeric_limits<int>::max());
    const int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
    const int first = m - 1;
    if constexpr (!internal::convolution_has_ntt<Mint>) {
        return internal::convolution_slice_naive<Mint>(a, b, first, n);
    } else {
        using NTT = NTTEngine<Mint>;
        if (std::min(m, n - first) <= 60) return internal::convolution_slice_naive<Mint>(a, b, first, n);
        const auto size = std::bit_ceil(static_cast<unsigned int>(n));
        const bool cyclic = size <= (1U << NTT::rank2);
        if (cyclic) internal::frequency_product<false>(a, b, static_cast<int>(size));
        else a = convolution(std::move(a), std::move(b));
        a.resize(n);
        a.erase(a.begin(), a.begin() + first);
        if (cyclic) NTT::normalize(a.begin(), a.end(), static_cast<int>(size));
        return a;
    }
}

template <unsigned int Mod = 998244353, class T,
          std::enable_if_t<std::is_integral_v<T>>* = nullptr>
std::vector<T> middle_product(const std::vector<T>& a, const std::vector<T>& b) {
    static_assert(1 <= Mod && Mod <= 2147483647U);
    using Mint = internal::convolution_mint<Mod>;
    auto values = middle_product(std::vector<Mint>(a.begin(), a.end()),
                                 std::vector<Mint>(b.begin(), b.end()));
    return internal::convolution_values<T>(values);
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
    const auto identity = [](long long x) { return x; };
    const auto c1 = internal::convolution_residues<m1>(a, b, identity);
    const auto c2 = internal::convolution_residues<m2>(a, b, identity);
    const auto c3 = internal::convolution_residues<m3>(a, b, identity);
    std::vector<long long> c(c1.size());
    for (std::size_t i = 0; i < c.size(); ++i) {
        unsigned long long x = 0;
        x += c1[i].val() * i1 % m1 * m2m3;
        x += c2[i].val() * i2 % m2 * m1m3;
        x += c3[i].val() * i3 % m3 * m1m2;
        long long diff = static_cast<long long>(c1[i].val()) - internal::safe_mod(static_cast<long long>(x), m1);
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
