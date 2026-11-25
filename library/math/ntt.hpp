// https://github.com/soryuusi0219/compro/blob/main/library/math/ntt.hpp
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__))
#include <immintrin.h>
#define SORYUUSI_NTT_X86_SIMD 1
#else
#define SORYUUSI_NTT_X86_SIMD 0
#endif

#include "internal/internal_bit.hpp"
#include "internal/internal_math.hpp"
#include "internal/modint_traits.hpp"

namespace soryuusi {

// Iterative radix-4 NTT.  The transform order is the same as ACL's butterfly:
// no bit-reversal pass is necessary, and ntt()/intt() are inverse operations.
template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
struct NTTEngine {
    static constexpr int mod = Mint::mod();
    static constexpr int primitive_root = internal::primitive_root<mod>;
    static constexpr int rank2 = soryuusi::countr_zero(static_cast<unsigned int>(mod - 1));

  private:
    struct FFTInfo {
        std::array<Mint, rank2 + 1> root{};
        std::array<Mint, rank2 + 1> iroot{};
        std::array<Mint, rank2 + 1> inverse_size{};
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
            inverse_size[0] = Mint(1);
            if constexpr (rank2 > 0) {
                const Mint half = Mint::raw(static_cast<int>((mod + 1LL) / 2));
                for (int i = 1; i <= rank2; ++i) inverse_size[i] = inverse_size[i - 1] * half;
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

#if SORYUUSI_NTT_X86_SIMD
    __attribute__((target("avx2")))
    static __m256i load8x32(const Mint* p) {
        if constexpr (internal::has_native_ntt_residue<Mint>) {
            static_assert(sizeof(Mint) == 4 && std::is_trivially_copyable_v<Mint>);
            __m256i values;
            std::memcpy(&values, p, sizeof(values));
            return values;
        } else {
            return _mm256_setr_epi32(p[0].val(), p[1].val(), p[2].val(), p[3].val(), p[4].val(), p[5].val(), p[6].val(), p[7].val());
        }
    }

    __attribute__((target("avx2")))
    static void store8x32(Mint* p, __m256i x) {
        if constexpr (internal::has_native_ntt_residue<Mint>) {
            std::memcpy(static_cast<void*>(p), &x, sizeof(x));
        } else {
            alignas(32) std::uint32_t values[8];
            _mm256_store_si256(reinterpret_cast<__m256i*>(values), x);
            for (int i = 0; i < 8; ++i) p[i] = Mint::raw(values[i]);
        }
    }

    __attribute__((target("avx2")))
    static __m256i reduce32(__m256i x) {
        return _mm256_min_epu32(x, _mm256_sub_epi32(x, _mm256_set1_epi32(mod)));
    }

    __attribute__((target("avx2")))
    static __m256i add32(__m256i a, __m256i b) {
        return reduce32(_mm256_add_epi32(a, b));
    }

    __attribute__((target("avx2")))
    static __m256i sub32(__m256i a, __m256i b) {
        return reduce32(_mm256_sub_epi32(_mm256_add_epi32(a, _mm256_set1_epi32(mod)), b));
    }

    struct Shoup32 {
        __m256i value, quotient;

        __attribute__((target("avx2")))
        explicit Shoup32(Mint w)
            : value(_mm256_set1_epi32(w.val())),
              quotient(_mm256_set1_epi32((std::uint64_t(w.val()) << 32) / mod)) {}

        __attribute__((target("avx2")))
        __m256i mul(__m256i x) const {
            const __m256i even = _mm256_mul_epu32(x, quotient);
            const __m256i odd = _mm256_mul_epu32(_mm256_srli_epi64(x, 32), quotient);
            const __m256i q = _mm256_blend_epi32(_mm256_srli_epi64(even, 32), odd, 0xaa);
            return reduce32(_mm256_sub_epi32(_mm256_mullo_epi32(x, value), _mm256_mullo_epi32(q, _mm256_set1_epi32(mod))));
        }
    };

    template <bool Inverse>
    __attribute__((target("avx2")))
    static void radix2_avx2(Mint* a, Mint* b, int p, Mint rot) {
        const Shoup32 w(rot);
        for (int i = 0; i < p; i += 8) {
            const __m256i x = load8x32(a + i), y = load8x32(b + i);
            if constexpr (Inverse) {
                store8x32(a + i, add32(x, y));
                store8x32(b + i, w.mul(sub32(x, y)));
            } else {
                const __m256i z = w.mul(y);
                store8x32(a + i, add32(x, z));
                store8x32(b + i, sub32(x, z));
            }
        }
    }

    template <bool Inverse, bool Unit>
    __attribute__((target("avx2")))
    static void radix4_avx2(Mint* a0p, Mint* a1p, Mint* a2p, Mint* a3p, int p, Mint rot, Mint imag) {
        const Mint rot2 = rot * rot;
        const Shoup32 w1(rot), w2(rot2), w3(rot2 * rot), wi(imag);
        for (int i = 0; i < p; i += 8) {
            const __m256i a0 = load8x32(a0p + i);
            __m256i a1 = load8x32(a1p + i), a2 = load8x32(a2p + i), a3 = load8x32(a3p + i);
            if constexpr (Inverse) {
                const __m256i x0 = add32(a0, a1), x1 = sub32(a0, a1);
                const __m256i x2 = add32(a2, a3), x3 = wi.mul(sub32(a2, a3));
                a1 = add32(x1, x3);
                a2 = sub32(x0, x2);
                a3 = sub32(x1, x3);
                store8x32(a0p + i, add32(x0, x2));
                if constexpr (!Unit) {
                    a1 = w1.mul(a1); a2 = w2.mul(a2); a3 = w3.mul(a3);
                }
                store8x32(a1p + i, a1);
                store8x32(a2p + i, a2);
                store8x32(a3p + i, a3);
            } else {
                if constexpr (!Unit) {
                    a1 = w1.mul(a1); a2 = w2.mul(a2); a3 = w3.mul(a3);
                }
                const __m256i x0 = add32(a0, a2), x1 = sub32(a0, a2);
                const __m256i x2 = add32(a1, a3), x3 = wi.mul(sub32(a1, a3));
                store8x32(a0p + i, add32(x0, x2));
                store8x32(a1p + i, sub32(x0, x2));
                store8x32(a2p + i, add32(x1, x3));
                store8x32(a3p + i, sub32(x1, x3));
            }
        }
    }

    static bool avx2_available() {
        static const bool available = __builtin_cpu_supports("avx2");
        return available;
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i load8(const Mint* p) {
        return _mm512_maskz_cvtepu32_epi64(0xff, load8x32(p));
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static void store8(Mint* p, __m512i x) {
        store8x32(p, _mm512_maskz_cvtepi64_epi32(0xff, x));
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i add_mod(__m512i a, __m512i b) {
        const __m512i m = _mm512_set1_epi64(mod);
        __m512i x = _mm512_add_epi64(a, b);
        return _mm512_mask_sub_epi64(x, _mm512_cmp_epu64_mask(x, m, _MM_CMPINT_NLT), x, m);
    }

    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i sub_mod(__m512i a, __m512i b) {
        const __m512i m = _mm512_set1_epi64(mod);
        __m512i x = _mm512_sub_epi64(_mm512_add_epi64(a, m), b);
        return _mm512_mask_sub_epi64(x, _mm512_cmp_epu64_mask(x, m, _MM_CMPINT_NLT), x, m);
    }

    // Shoup multiplication.  Both operands are below mod, so one correction
    // after floor(x * floor(w*2^32/mod) / 2^32) is sufficient.
    __attribute__((target("avx512f,avx512dq,avx512vl")))
    static __m512i mul_const(__m512i x, unsigned int w) {
        const unsigned long long wp = (static_cast<unsigned long long>(w) << 32) / mod;
        const __m512i q = _mm512_srli_epi64(_mm512_mullo_epi64(x, _mm512_set1_epi64(wp)), 32);
        __m512i r = _mm512_sub_epi64(_mm512_mullo_epi64(x, _mm512_set1_epi64(w)), _mm512_mullo_epi64(q, _mm512_set1_epi64(mod)));
        const __m512i m = _mm512_set1_epi64(mod);
        return _mm512_mask_sub_epi64(r, _mm512_cmp_epu64_mask(r, m, _MM_CMPINT_NLT), r, m);
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
    static void radix4_avx512(Mint* a0p, Mint* a1p, Mint* a2p, Mint* a3p, int p, Mint rot, Mint imag, bool inverse) {
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
        static const bool available = __builtin_cpu_supports("avx512f") && __builtin_cpu_supports("avx512dq") && __builtin_cpu_supports("avx512vl");
        return available;
    }
#endif

    enum class Backend { scalar, avx2, avx512 };
    template <class It>
    static Backend select_backend() {
#if SORYUUSI_NTT_X86_SIMD
        if constexpr (std::contiguous_iterator<It>) {
            if (can_use_avx512<It>()) return Backend::avx512;
            if (avx2_available()) return Backend::avx2;
        }
#endif
        return Backend::scalar;
    }

#if SORYUUSI_NTT_X86_SIMD
    template <bool Inverse>
    static void simd_radix2(Backend backend, Mint* a, int p, Mint rot) {
        if (backend == Backend::avx512) radix2_avx512(a, a + p, p, rot, Inverse);
        else radix2_avx2<Inverse>(a, a + p, p, rot);
    }

    template <bool Inverse>
    static void simd_radix4(Backend backend, Mint* a, int p, Mint rot, Mint imag, bool unit) {
        if (backend == Backend::avx512)
            radix4_avx512(a, a + p, a + 2 * p, a + 3 * p, p, rot, imag, Inverse);
        else if (unit)
            radix4_avx2<Inverse, true>(a, a + p, a + 2 * p, a + 3 * p, p, rot, imag);
        else
            radix4_avx2<Inverse, false>(a, a + p, a + 2 * p, a + 3 * p, p, rot, imag);
    }

    __attribute__((target("avx2")))
    static void scale_avx2(Mint* first, int length, Mint scale) {
        const Shoup32 w(scale);
        int i = 0;
        for (; i + 8 <= length; i += 8) store8x32(first + i, w.mul(load8x32(first + i)));
        for (; i < length; ++i) first[i] *= scale;
    }
#endif

    template <std::random_access_iterator It>
    static constexpr void check(It first, It last) {
        static_assert(std::same_as<std::remove_cv_t<std::iter_value_t<It>>, Mint>, "NTTEngine iterator value_type must be Mint");
        const auto n = last - first;
        assert(n > 0 && (n & (n - 1)) == 0);
        assert(n <= (std::ptrdiff_t{1} << rank2));
    }

    template <std::random_access_iterator It>
    static void ntt_scalar(It first, It last, [[maybe_unused]] Backend backend) {
        const int n = static_cast<int>(last - first);
        const int h = soryuusi::countr_zero(static_cast<unsigned int>(n));
        int len = 0;
        while (len < h) {
            if (h - len == 1) {
                const int p = 1 << (h - len - 1);
                Mint rot = 1;
                for (int s = 0; s < (1 << len); ++s) {
                    const int offset = s << (h - len);
                    int i = 0;
#if SORYUUSI_NTT_X86_SIMD
                    if constexpr (std::contiguous_iterator<It>) {
                        if (backend != Backend::scalar && p >= 8) {
                            simd_radix2<false>(backend, std::to_address(first + offset), p, rot);
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
                        rot *= info.rate2[soryuusi::countr_zero(~static_cast<unsigned int>(s))];
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
#if SORYUUSI_NTT_X86_SIMD
                    if constexpr (std::contiguous_iterator<It>) {
                        if (backend != Backend::scalar && p >= 8) {
                            simd_radix4<false>(backend, std::to_address(first + offset), p, rot, imag, s == 0);
                            i = p;
                        }
                    }
#endif
                    for (; i < p; ++i) {
                        // Keep twiddle products unreduced and normalize only
                        // when writing the four outputs.
                        const unsigned long long mod2 = static_cast<unsigned long long>(mod) * mod;
                        const unsigned long long a0 = first[offset + i].val();
                        const unsigned long long a1 = static_cast<unsigned long long>(first[offset + i + p].val()) * rot.val();
                        const unsigned long long a2 = static_cast<unsigned long long>(first[offset + i + 2 * p].val()) * rot2.val();
                        const unsigned long long a3 = static_cast<unsigned long long>(first[offset + i + 3 * p].val()) * rot3.val();
                        const unsigned long long a1na3imag = static_cast<unsigned long long>(Mint(a1 + mod2 - a3).val()) * imag.val();
                        const unsigned long long na2 = mod2 - a2;
                        first[offset + i] = a0 + a1 + a2 + a3;
                        first[offset + i + p] = a0 + a2 + 2 * mod2 - a1 - a3;
                        first[offset + i + 2 * p] = a0 + na2 + a1na3imag;
                        first[offset + i + 3 * p] = a0 + na2 + mod2 - a1na3imag;
                    }
                    if (s + 1 != (1 << len)) {
                        rot *= info.rate3[soryuusi::countr_zero(~static_cast<unsigned int>(s))];
                    }
                }
                len += 2;
            }
        }
    }

    template <std::random_access_iterator It>
    static void intt_scalar(It first, It last, [[maybe_unused]] Backend backend) {
        const int n = static_cast<int>(last - first);
        const int h = soryuusi::countr_zero(static_cast<unsigned int>(n));
        int len = h;
        while (len > 0) {
            if (len == 1) {
                const int p = 1 << (h - len);
                Mint irot = 1;
                for (int s = 0; s < (1 << (len - 1)); ++s) {
                    const int offset = s << (h - len + 1);
                    int i = 0;
#if SORYUUSI_NTT_X86_SIMD
                    if constexpr (std::contiguous_iterator<It>) {
                        if (backend != Backend::scalar && p >= 8) {
                            simd_radix2<true>(backend, std::to_address(first + offset), p, irot);
                            i = p;
                        }
                    }
#endif
                    for (; i < p; ++i) {
                        const Mint l = first[offset + i];
                        const Mint r = first[offset + i + p];
                        first[offset + i] = l + r;
                        first[offset + i + p] = (static_cast<unsigned long long>(mod) + l.val() - r.val()) * irot.val();
                    }
                    if (s + 1 != (1 << (len - 1))) {
                        irot *= info.irate2[soryuusi::countr_zero(~static_cast<unsigned int>(s))];
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
#if SORYUUSI_NTT_X86_SIMD
                    if constexpr (std::contiguous_iterator<It>) {
                        if (backend != Backend::scalar && p >= 8) {
                            simd_radix4<true>(backend, std::to_address(first + offset), p, irot, iimag, s == 0);
                            i = p;
                        }
                    }
#endif
                    for (; i < p; ++i) {
                        const unsigned long long a0 = first[offset + i].val();
                        const unsigned long long a1 = first[offset + i + p].val();
                        const unsigned long long a2 = first[offset + i + 2 * p].val();
                        const unsigned long long a3 = first[offset + i + 3 * p].val();
                        const unsigned long long a2na3iimag = Mint((mod + a2 - a3) * iimag.val()).val();
                        first[offset + i] = a0 + a1 + a2 + a3;
                        first[offset + i + p] = (a0 + mod - a1 + a2na3iimag) * irot.val();
                        first[offset + i + 2 * p] = (a0 + a1 + 2ULL * mod - a2 - a3) * irot2.val();
                        first[offset + i + 3 * p] = (a0 + 2ULL * mod - a1 - a2na3iimag) * irot3.val();
                    }
                    if (s + 1 != (1 << (len - 2))) {
                        irot *= info.irate3[soryuusi::countr_zero(~static_cast<unsigned int>(s))];
                    }
                }
                len -= 2;
            }
        }
    }

    template <bool Inverse>
    static void shift_spectrum(std::span<Mint> spectrum) {
        check(spectrum.begin(), spectrum.end());
        const int n = static_cast<int>(spectrum.size() / 2);
        Mint rot = 1;
        for (int i = 0; i < n; ++i) {
            spectrum[2 * i] *= rot;
            spectrum[2 * i + 1] *= Mint(0) - rot;
            if (i + 1 != n) {
                const int index = soryuusi::countr_zero(~static_cast<unsigned int>(i));
                rot *= Inverse ? info.irate2[index] : info.rate2[index];
            }
        }
    }

    static void twist(std::span<Mint> values, Mint ratio, Mint scale) {
        for (Mint& value : values) {
            value *= scale;
            scale *= ratio;
        }
    }

    static std::vector<Mint> half_difference(std::span<const Mint> spectrum) {
        check(spectrum.begin(), spectrum.end());
        const int n = static_cast<int>(spectrum.size() / 2);
        assert(n > 0);
        std::vector<Mint> difference(spectrum.begin() + n, spectrum.end());
        const int level = soryuusi::countr_zero(static_cast<unsigned int>(n));
        intt_unnormalized(difference);
        twist(difference, info.iroot[level + 1], info.inverse_size[level]);
        ntt(difference);
        return difference;
    }

  public:
    static Mint inverse_size(int length) {
        assert(length > 0 && soryuusi::has_single_bit(static_cast<unsigned int>(length)));
        assert(length <= (1 << rank2));
        return info.inverse_size[soryuusi::countr_zero(static_cast<unsigned int>(length))];
    }

    // Scale any retained part of an unnormalized inverse transform of length n.
    template <std::random_access_iterator It>
    static void normalize(It first, It last, int n) {
        assert(0 <= last - first && last - first <= n);
        const Mint scale = inverse_size(n);
#if SORYUUSI_NTT_X86_SIMD
        if constexpr (std::contiguous_iterator<It>) {
            if (last - first >= 32 && avx2_available()) {
                scale_avx2(std::to_address(first), static_cast<int>(last - first), scale);
                return;
            }
        }
#endif
        for (; first != last; ++first) *first *= scale;
    }

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
#if SORYUUSI_NTT_X86_SIMD
        if constexpr (std::contiguous_iterator<It> && sizeof(Mint) == 4 && std::is_trivially_copyable_v<Mint> && std::is_standard_layout_v<Mint>) {
            return avx512_available();
        }
#endif
        return false;
    }

    template <std::random_access_iterator It>
    static void ntt(It first, It last) {
        check(first, last);
        ntt_scalar(first, last, select_backend<It>());
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
        intt_scalar(first, last, select_backend<It>());
    }

    template <std::ranges::random_access_range Range>
    static void intt_unnormalized(Range& range) {
        intt_unnormalized(std::ranges::begin(range), std::ranges::end(range));
    }

    template <std::random_access_iterator It>
    static void intt(It first, It last) {
        check(first, last);
        intt_scalar(first, last, select_backend<It>());
        normalize(first, last, static_cast<int>(last - first));
    }

    template <std::ranges::random_access_range Range>
    static void intt(Range& range) {
        intt(std::ranges::begin(range), std::ranges::end(range));
    }

    // Replace a length-n spectrum by the odd frequencies of its zero-padded
    // length-2n transform. The original spectrum contains the even frequencies.
    static void ntt_odd_extend(std::span<Mint> spectrum) {
        check(spectrum.begin(), spectrum.end());
        const int level = soryuusi::countr_zero(static_cast<unsigned int>(spectrum.size()));
        assert(level < rank2);
        if (level == 0) return;
        intt_unnormalized(spectrum);
        twist(spectrum, info.root[level + 1], info.inverse_size[level]);
        ntt(spectrum);
    }

    static void ntt_double(std::vector<Mint>& spectrum) {
        check(spectrum.begin(), spectrum.end());
        const int n = static_cast<int>(spectrum.size());
        assert(n < (1 << rank2));
        spectrum.resize(2 * n);
        std::copy_n(spectrum.begin(), n, spectrum.begin() + n);
        ntt_odd_extend(std::span<Mint>(spectrum).subspan(n));
    }

    // Transpose of ntt_odd_extend for the ordinary dot product on spectra.
    static void ntt_odd_extend_transposed(std::span<Mint> spectrum) {
        check(spectrum.begin(), spectrum.end());
        const int level = soryuusi::countr_zero(static_cast<unsigned int>(spectrum.size()));
        assert(level < rank2);
        if (level == 0) return;
        intt_unnormalized(spectrum);
        const Mint scale = info.inverse_size[level], ratio = info.iroot[level + 1];
        spectrum.front() *= scale;
        twist(spectrum.subspan(1), ratio, (Mint(0) - scale) * ratio);
        ntt(spectrum);
    }

    static void ntt_negate_argument(std::span<Mint> spectrum) {
        check(spectrum.begin(), spectrum.end());
        for (std::size_t i = 1; i < spectrum.size(); i += 2) {
            std::swap(spectrum[i - 1], spectrum[i]);
        }
    }

    static void ntt_square_argument(std::vector<Mint>& spectrum) {
        check(spectrum.begin(), spectrum.end());
        const int n = static_cast<int>(spectrum.size());
        assert(n < (1 << rank2));
        spectrum.resize(2 * n);
        for (int i = n - 1; i >= 0; --i) {
            const Mint value = spectrum[i];
            spectrum[2 * i] = spectrum[2 * i + 1] = value;
        }
    }

    // If g(x)g(-x) = f(x^2), produce the spectrum of f modulo x^n - 1.
    static void ntt_even_product(std::vector<Mint>& spectrum) {
        check(spectrum.begin(), spectrum.end());
        const int n = static_cast<int>(spectrum.size() / 2);
        assert(n > 0);
        for (int i = 0; i < n; ++i) spectrum[i] = spectrum[2 * i] * spectrum[2 * i + 1];
        spectrum.resize(n);
    }

    static void ntt_shift(std::span<Mint> spectrum) {
        shift_spectrum<false>(spectrum);
    }

    static std::vector<Mint> intt_parity(std::span<const Mint> spectrum, bool odd) {
        check(spectrum.begin(), spectrum.end());
        const int n = static_cast<int>(spectrum.size() / 2);
        assert(n > 0);
        std::vector<Mint> result(n);
        if (odd) {
            Mint rot = 1;
            for (int i = 0; i < n; ++i) {
                result[i] = (spectrum[2 * i] - spectrum[2 * i + 1]) * rot;
                if (i + 1 != n) rot *= info.irate2[soryuusi::countr_zero(~static_cast<unsigned int>(i))];
            }
        } else {
            for (int i = 0; i < n; ++i) result[i] = spectrum[2 * i] + spectrum[2 * i + 1];
        }
        intt_unnormalized(result);
        normalize(result.begin(), result.end(), 2 * n);
        return result;
    }

    static std::vector<Mint> ntt_half(std::span<const Mint> spectrum, bool high) {
        auto difference = half_difference(spectrum);
        const Mint half = info.inverse_size[1];
        for (std::size_t i = 0; i < difference.size(); ++i) {
            difference[i] = (high ? spectrum[i] - difference[i] : spectrum[i] + difference[i]) * half;
        }
        return difference;
    }

    static std::pair<std::vector<Mint>, std::vector<Mint>>
    ntt_split(std::span<const Mint> spectrum) {
        auto high = half_difference(spectrum);
        std::vector<Mint> low(high.size());
        const Mint half = info.inverse_size[1];
        for (std::size_t i = 0; i < high.size(); ++i) {
            low[i] = (spectrum[i] + high[i]) * half;
            high[i] = spectrum[i] - low[i];
        }
        return {std::move(low), std::move(high)};
    }

    static void ntt_inverse_argument(std::span<Mint> spectrum) {
        check(spectrum.begin(), spectrum.end());
        const std::size_t n = spectrum.size();
        // Negating a frequency reverses each block of equal leading bit.
        for (std::size_t i = 1; i < n; i *= 2) {
            std::reverse(spectrum.begin() + i, spectrum.begin() + 2 * i);
        }
    }

    static void ntt_reverse(std::span<Mint> spectrum) {
        ntt_inverse_argument(spectrum);
        shift_spectrum<true>(spectrum);
    }
};

}  // namespace soryuusi

#undef SORYUUSI_NTT_X86_SIMD
