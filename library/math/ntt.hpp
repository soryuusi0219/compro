// https://github.com/soryuusi0219/compro/blob/main/library/math/ntt.hpp
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>

#if defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__))
#include <immintrin.h>
#define SORYUUSI_NTT_AVX512 1
#else
#define SORYUUSI_NTT_AVX512 0
#endif

#include "library/math/internal_math.hpp"
#include "library/math/modint_traits.hpp"

namespace soryuusi {

// Iterative radix-4 NTT.  The transform order is the same as ACL's butterfly:
// no bit-reversal pass is necessary, and ntt()/intt() are inverse operations.
template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
struct NTTEngine {
    static constexpr int mod = Mint::mod();
    static constexpr int primitive_root = internal::primitive_root<mod>;
    static constexpr int rank2 = std::countr_zero(static_cast<unsigned int>(mod - 1));

  private:
    struct FFTInfo {
        std::array<Mint, rank2 + 1> root{};
        std::array<Mint, rank2 + 1> iroot{};
        std::array<Mint, (rank2 > 1 ? rank2 - 1 : 1)> rate2{};
        std::array<Mint, (rank2 > 1 ? rank2 - 1 : 1)> irate2{};
        std::array<Mint, (rank2 > 2 ? rank2 - 2 : 1)> rate3{};
        std::array<Mint, (rank2 > 2 ? rank2 - 2 : 1)> irate3{};

        constexpr FFTInfo() {
            root[rank2] = Mint(primitive_root).pow((mod - 1) >> rank2);
            iroot[rank2] = root[rank2].inv();
            for (int i = rank2 - 1; i >= 0; --i) {
                root[i] = root[i + 1] * root[i + 1];
                iroot[i] = iroot[i + 1] * iroot[i + 1];
            }
            Mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 2; ++i) {
                rate2[i] = root[i + 2] * prod;
                prod *= iroot[i + 2];
            }
            for (int i = 0; i <= rank2 - 2; ++i) {
                irate2[i] = iroot[i + 2] * iprod;
                iprod *= root[i + 2];
            }
            prod = iprod = 1;
            for (int i = 0; i <= rank2 - 3; ++i) {
                rate3[i] = root[i + 3] * prod;
                irate3[i] = iroot[i + 3] * iprod;
                prod *= iroot[i + 3];
                iprod *= root[i + 3];
            }
        }
    };

    inline static const FFTInfo info{};

#if SORYUUSI_NTT_AVX512
    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i load8(const Mint* p) {
        return _mm512_cvtepu32_epi64(
            _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p)));
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static void store8(Mint* p, __m512i x) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(p),
                            _mm512_cvtepi64_epi32(x));
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i add_mod(__m512i a, __m512i b) {
        const __m512i m = _mm512_set1_epi64(mod);
        __m512i x = _mm512_add_epi64(a, b);
        return _mm512_mask_sub_epi64(
            x, _mm512_cmp_epu64_mask(x, m, _MM_CMPINT_NLT), x, m);
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i sub_mod(__m512i a, __m512i b) {
        const __m512i m = _mm512_set1_epi64(mod);
        __m512i x = _mm512_sub_epi64(_mm512_add_epi64(a, m), b);
        return _mm512_mask_sub_epi64(
            x, _mm512_cmp_epu64_mask(x, m, _MM_CMPINT_NLT), x, m);
    }

    // Shoup multiplication.  Both operands are below mod, so one correction
    // after floor(x * floor(w*2^32/mod) / 2^32) is sufficient.
    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i mul_const(__m512i x, unsigned int w) {
        const unsigned long long wp =
            (static_cast<unsigned long long>(w) << 32) / mod;
        const __m512i q = _mm512_srli_epi64(
            _mm512_mullo_epi64(x, _mm512_set1_epi64(wp)), 32);
        __m512i r = _mm512_sub_epi64(
            _mm512_mullo_epi64(x, _mm512_set1_epi64(w)),
            _mm512_mullo_epi64(q, _mm512_set1_epi64(mod)));
        const __m512i m = _mm512_set1_epi64(mod);
        return _mm512_mask_sub_epi64(
            r, _mm512_cmp_epu64_mask(r, m, _MM_CMPINT_NLT), r, m);
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static void radix2_avx512(Mint* a, Mint* b, int p, Mint rot, bool inverse) {
        for (int i = 0; i < p; i += 8) {
            const __m512i x = load8(a + i);
            const __m512i y = load8(b + i);
            if (!inverse) {
                const __m512i z = mul_const(y, rot.val());
                store8(a + i, add_mod(x, z));
                store8(b + i, sub_mod(x, z));
            } else {
                store8(a + i, add_mod(x, y));
                store8(b + i, mul_const(sub_mod(x, y), rot.val()));
            }
        }
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static void radix4_avx512(Mint* a0p, Mint* a1p, Mint* a2p, Mint* a3p,
                              int p, Mint rot, Mint imag, bool inverse) {
        const unsigned int r1 = rot.val();
        const unsigned int r2 = (rot * rot).val();
        const unsigned int r3 = (rot * rot * rot).val();
        for (int i = 0; i < p; i += 8) {
            __m512i a0 = load8(a0p + i);
            __m512i a1 = load8(a1p + i);
            __m512i a2 = load8(a2p + i);
            __m512i a3 = load8(a3p + i);
            if (!inverse) {
                a1 = mul_const(a1, r1);
                a2 = mul_const(a2, r2);
                a3 = mul_const(a3, r3);
                const __m512i a1na3imag = mul_const(sub_mod(a1, a3), imag.val());
                store8(a0p + i, add_mod(add_mod(a0, a1), add_mod(a2, a3)));
                store8(a1p + i, add_mod(sub_mod(a0, a1), sub_mod(a2, a3)));
                store8(a2p + i, sub_mod(add_mod(a0, a1na3imag), a2));
                store8(a3p + i, sub_mod(sub_mod(a0, a1na3imag), a2));
            } else {
                const __m512i a2na3iimag = mul_const(sub_mod(a2, a3), imag.val());
                store8(a0p + i, add_mod(add_mod(a0, a1), add_mod(a2, a3)));
                store8(a1p + i, mul_const(add_mod(sub_mod(a0, a1), a2na3iimag), r1));
                store8(a2p + i, mul_const(sub_mod(add_mod(a0, a1), add_mod(a2, a3)), r2));
                store8(a3p + i, mul_const(sub_mod(sub_mod(a0, a1), a2na3iimag), r3));
            }
        }
    }

    static bool avx512_available() {
        static const bool available = __builtin_cpu_supports("avx512f") &&
                                      __builtin_cpu_supports("avx512dq") &&
                                      __builtin_cpu_supports("avx512vl");
        return available;
    }
#endif

    template <std::random_access_iterator It>
    static constexpr void check(It first, It last) {
        static_assert(std::same_as<std::remove_cv_t<std::iter_value_t<It>>, Mint>,
                      "NTTEngine iterator value_type must be Mint");
        const auto n = last - first;
        assert(n > 0 && (n & (n - 1)) == 0);
        assert(n <= (std::ptrdiff_t{1} << rank2));
    }

    template <std::random_access_iterator It>
    static void ntt_scalar(It first, It last,
                           [[maybe_unused]] bool use_avx512) {
        const int n = static_cast<int>(last - first);
        const int h = std::countr_zero(static_cast<unsigned int>(n));
        int len = 0;
        while (len < h) {
            if (h - len == 1) {
                const int p = 1 << (h - len - 1);
                Mint rot = 1;
                for (int s = 0; s < (1 << len); ++s) {
                    const int offset = s << (h - len);
                    int i = 0;
#if SORYUUSI_NTT_AVX512
                    if constexpr (std::contiguous_iterator<It>) {
                        if (use_avx512 && p >= 8) {
                            radix2_avx512(std::to_address(first + offset),
                                          std::to_address(first + offset + p),
                                          p, rot, false);
                            i = p;
                        }
                    }
#endif
                    for (; i < p; ++i) {
                        const Mint l = first[offset + i];
                        const Mint r = first[offset + i + p] * rot;
                        first[offset + i] = l + r;
                        first[offset + i + p] = l - r;
                    }
                    if (s + 1 != (1 << len)) {
                        rot *= info.rate2[std::countr_zero(~static_cast<unsigned int>(s))];
                    }
                }
                ++len;
            } else {
                const int p = 1 << (h - len - 2);
                Mint rot = 1;
                const Mint imag = info.root[2];
                for (int s = 0; s < (1 << len); ++s) {
                    const Mint rot2 = rot * rot;
                    const Mint rot3 = rot2 * rot;
                    const int offset = s << (h - len);
                    int i = 0;
#if SORYUUSI_NTT_AVX512
                    if constexpr (std::contiguous_iterator<It>) {
                        if (use_avx512 && p >= 8) {
                            radix4_avx512(std::to_address(first + offset),
                                          std::to_address(first + offset + p),
                                          std::to_address(first + offset + 2 * p),
                                          std::to_address(first + offset + 3 * p),
                                          p, rot, imag, false);
                            i = p;
                        }
                    }
#endif
                    for (; i < p; ++i) {
                        // Keep twiddle products unreduced and normalize only
                        // when writing the four outputs.
                        const unsigned long long mod2 =
                            static_cast<unsigned long long>(mod) * mod;
                        const unsigned long long a0 = first[offset + i].val();
                        const unsigned long long a1 =
                            static_cast<unsigned long long>(first[offset + i + p].val()) * rot.val();
                        const unsigned long long a2 =
                            static_cast<unsigned long long>(first[offset + i + 2 * p].val()) * rot2.val();
                        const unsigned long long a3 =
                            static_cast<unsigned long long>(first[offset + i + 3 * p].val()) * rot3.val();
                        const unsigned long long a1na3imag =
                            static_cast<unsigned long long>(Mint(a1 + mod2 - a3).val()) * imag.val();
                        const unsigned long long na2 = mod2 - a2;
                        first[offset + i] = a0 + a1 + a2 + a3;
                        first[offset + i + p] = a0 + a2 + 2 * mod2 - a1 - a3;
                        first[offset + i + 2 * p] = a0 + na2 + a1na3imag;
                        first[offset + i + 3 * p] = a0 + na2 + mod2 - a1na3imag;
                    }
                    if (s + 1 != (1 << len)) {
                        rot *= info.rate3[std::countr_zero(~static_cast<unsigned int>(s))];
                    }
                }
                len += 2;
            }
        }
    }

    template <std::random_access_iterator It>
    static void intt_scalar(It first, It last,
                            [[maybe_unused]] bool use_avx512) {
        const int n = static_cast<int>(last - first);
        const int h = std::countr_zero(static_cast<unsigned int>(n));
        int len = h;
        while (len > 0) {
            if (len == 1) {
                const int p = 1 << (h - len);
                Mint irot = 1;
                for (int s = 0; s < (1 << (len - 1)); ++s) {
                    const int offset = s << (h - len + 1);
                    int i = 0;
#if SORYUUSI_NTT_AVX512
                    if constexpr (std::contiguous_iterator<It>) {
                        if (use_avx512 && p >= 8) {
                            radix2_avx512(std::to_address(first + offset),
                                          std::to_address(first + offset + p),
                                          p, irot, true);
                            i = p;
                        }
                    }
#endif
                    for (; i < p; ++i) {
                        const Mint l = first[offset + i];
                        const Mint r = first[offset + i + p];
                        first[offset + i] = l + r;
                        first[offset + i + p] =
                            static_cast<unsigned long long>(mod + l.val() - r.val()) * irot.val();
                    }
                    if (s + 1 != (1 << (len - 1))) {
                        irot *= info.irate2[std::countr_zero(~static_cast<unsigned int>(s))];
                    }
                }
                --len;
            } else {
                const int p = 1 << (h - len);
                Mint irot = 1;
                const Mint iimag = info.iroot[2];
                for (int s = 0; s < (1 << (len - 2)); ++s) {
                    const Mint irot2 = irot * irot;
                    const Mint irot3 = irot2 * irot;
                    const int offset = s << (h - len + 2);
                    int i = 0;
#if SORYUUSI_NTT_AVX512
                    if constexpr (std::contiguous_iterator<It>) {
                        if (use_avx512 && p >= 8) {
                            radix4_avx512(std::to_address(first + offset),
                                          std::to_address(first + offset + p),
                                          std::to_address(first + offset + 2 * p),
                                          std::to_address(first + offset + 3 * p),
                                          p, irot, iimag, true);
                            i = p;
                        }
                    }
#endif
                    for (; i < p; ++i) {
                        const unsigned long long a0 = first[offset + i].val();
                        const unsigned long long a1 = first[offset + i + p].val();
                        const unsigned long long a2 = first[offset + i + 2 * p].val();
                        const unsigned long long a3 = first[offset + i + 3 * p].val();
                        const unsigned long long a2na3iimag =
                            Mint((mod + a2 - a3) * iimag.val()).val();
                        first[offset + i] = a0 + a1 + a2 + a3;
                        first[offset + i + p] =
                            (a0 + mod - a1 + a2na3iimag) * irot.val();
                        first[offset + i + 2 * p] =
                            (a0 + a1 + 2ULL * mod - a2 - a3) * irot2.val();
                        first[offset + i + 3 * p] =
                            (a0 + 2ULL * mod - a1 - a2na3iimag) * irot3.val();
                    }
                    if (s + 1 != (1 << (len - 2))) {
                        irot *= info.irate3[std::countr_zero(~static_cast<unsigned int>(s))];
                    }
                }
                len -= 2;
            }
        }
    }

  public:
    static constexpr Mint root(int level) {
        assert(0 <= level && level <= rank2);
        return info.root[level];
    }

    static constexpr Mint inv_root(int level) {
        assert(0 <= level && level <= rank2);
        return info.iroot[level];
    }

    static constexpr Mint rate2(int index) {
        assert(0 <= index && index <= rank2 - 2);
        return info.rate2[index];
    }

    static constexpr Mint inv_rate2(int index) {
        assert(0 <= index && index <= rank2 - 2);
        return info.irate2[index];
    }

    static constexpr Mint rate3(int index) {
        assert(0 <= index && index <= rank2 - 3);
        return info.rate3[index];
    }

    static constexpr Mint inv_rate3(int index) {
        assert(0 <= index && index <= rank2 - 3);
        return info.irate3[index];
    }

    template <std::random_access_iterator It>
    static bool can_use_avx512() {
#if SORYUUSI_NTT_AVX512
        if constexpr (std::contiguous_iterator<It> && sizeof(Mint) == 4 &&
                      std::is_trivially_copyable_v<Mint> &&
                      std::is_standard_layout_v<Mint>) {
            return avx512_available();
        }
#endif
        return false;
    }

    template <std::random_access_iterator It>
    static void ntt(It first, It last) {
        check(first, last);
        ntt_scalar(first, last, can_use_avx512<It>());
    }

    template <std::ranges::random_access_range Range>
    static void ntt(Range& range) {
        ntt(std::ranges::begin(range), std::ranges::end(range));
    }

    // Inverse transform without the final 1/n multiplication.  This is useful
    // in FPS algorithms that combine several normalization factors.
    template <std::random_access_iterator It>
    static void intt_unnormalized(It first, It last) {
        check(first, last);
        intt_scalar(first, last, can_use_avx512<It>());
    }

    template <std::ranges::random_access_range Range>
    static void intt_unnormalized(Range& range) {
        intt_unnormalized(std::ranges::begin(range), std::ranges::end(range));
    }

    template <std::random_access_iterator It>
    static void intt(It first, It last) {
        check(first, last);
        intt_scalar(first, last, can_use_avx512<It>());
        const Mint inv_n = Mint(last - first).inv();
        for (; first != last; ++first) *first *= inv_n;
    }

    template <std::ranges::random_access_range Range>
    static void intt(Range& range) {
        intt(std::ranges::begin(range), std::ranges::end(range));
    }
};

}  // namespace soryuusi

#undef SORYUUSI_NTT_AVX512
