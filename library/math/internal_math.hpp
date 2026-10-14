// https://github.com/soryuusi0219/compro/blob/main/library/math/internal_math.hpp
#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace soryuusi {

// Integer division rounded toward minus/plus infinity. Unlike C++ integer
// division, these functions also have the mathematical meaning for negatives.
template <class Int, std::enable_if_t<std::is_integral_v<Int> &&
                                      !std::is_same_v<std::remove_cv_t<Int>, bool>>* = nullptr>
constexpr Int divfloor(Int a, Int b) {
    assert(b != 0);
    const Int q = a / b;
    const Int r = a % b;
    return q - (r != 0 && ((r < 0) != (b < 0)));
}

template <class Int, std::enable_if_t<std::is_integral_v<Int> &&
                                      !std::is_same_v<std::remove_cv_t<Int>, bool>>* = nullptr>
constexpr Int divceil(Int a, Int b) {
    assert(b != 0);
    const Int q = a / b;
    const Int r = a % b;
    return q + (r != 0 && ((r < 0) == (b < 0)));
}

namespace internal {

constexpr int ceil_pow2(int n) {
    int x = 0;
    while ((1U << x) < static_cast<unsigned int>(n)) ++x;
    return x;
}

constexpr int bsf(unsigned int n) {
    return __builtin_ctz(n);
}

// @param m 1 <= m
// @return x mod m in [0, m)
template<typename INT1, typename INT2>
constexpr INT2 safe_mod(INT1 x, INT2 m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

// Barrett reduction for a modulus that fits in 32 bits. This is the same
// representation and interface used by ACL's dynamic_modint.
struct barrett {
    unsigned int _m;
    unsigned long long im;

    explicit constexpr barrett(unsigned int m = 1)
        : _m(m), im(std::numeric_limits<unsigned long long>::max() / m + 1) {}

    constexpr unsigned int umod() const { return _m; }

    constexpr unsigned int mul(unsigned int a, unsigned int b) const {
        const unsigned long long z =
            static_cast<unsigned long long>(a) * static_cast<unsigned long long>(b);
        const unsigned long long x = static_cast<unsigned long long>(
            (static_cast<__uint128_t>(z) * im) >> 64);
        unsigned int v = static_cast<unsigned int>(z - x * _m);
        if (_m <= v) v += _m;
        return v;
    }
};

constexpr std::uint64_t mul_mod64(std::uint64_t a, std::uint64_t b,
                                  std::uint64_t m) {
    return static_cast<std::uint64_t>(static_cast<__uint128_t>(a) * b % m);
}

// Montgomery arithmetic for an odd modulus below 2^62. Values are kept in
// the lazy range [0, 2 * mod), avoiding a correction after every product.
// Unlike __uint128_t % mod, reduction needs no hardware division.
struct montgomery64 {
    using u64 = std::uint64_t;
    using u128 = __uint128_t;

    u64 mod;
    u64 twice_mod;
    u64 neg_inv;
    u64 r2;

    explicit constexpr montgomery64(u64 m)
        : mod(m), twice_mod(2 * m), neg_inv(m), r2(0) {
        assert((m & 1) != 0 && m < (u64{1} << 62));
        for (int i = 0; i < 6; ++i) neg_inv *= 2 - m * neg_inv;
        neg_inv = -neg_inv;
        r2 = static_cast<u64>(-u128(m) % m);
    }

    constexpr u64 reduce(u128 x) const {
        const u64 q = static_cast<u64>(x) * neg_inv;
        return static_cast<u64>((x + u128(q) * mod) >> 64);
    }

    constexpr u64 transform(u64 x) const { return reduce(u128(x % mod) * r2); }
    constexpr u64 canonical(u64 x) const { return x >= mod ? x - mod : x; }
    constexpr u64 value(u64 x) const { return canonical(reduce(x)); }
    constexpr u64 one() const { return transform(1); }
    constexpr u64 add(u64 x, u64 y) const {
        x += y;
        if (x >= twice_mod) x -= twice_mod;
        return x;
    }
    constexpr u64 sub(u64 x, u64 y) const {
        x += twice_mod - y;
        if (x >= twice_mod) x -= twice_mod;
        return x;
    }
    constexpr u64 mul(u64 x, u64 y) const { return reduce(u128(x) * y); }
    constexpr u64 pow(u64 x, u64 n) const {
        u64 result = one();
        while (n) {
            if (n & 1) result = mul(result, x);
            x = mul(x, x);
            n >>= 1;
        }
        return result;
    }
};

// @param n 0 <= n
// @param m 1 <= m
constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
    assert(0 <= n && 1 <= m);
    if (m == 1) return 0;
    const auto um = static_cast<unsigned int>(m);
    unsigned long long r = 1;
    unsigned long long y = static_cast<unsigned long long>(safe_mod(x, m));
    while (n) {
        if (n & 1) r = r * y % um;
        y = y * y % um;
        n >>= 1;
    }
    return static_cast<long long>(r);
}

constexpr std::uint64_t pow_mod64_constexpr(std::uint64_t x, std::uint64_t n,
                                            std::uint64_t m) {
    assert(m != 0);
    if (m == 1) return 0;
    std::uint64_t r = 1;
    x %= m;
    while (n) {
        if (n & 1) r = mul_mod64(r, x, m);
        x = mul_mod64(x, x, m);
        n >>= 1;
    }
    return r;
}

template <class X, class N, class M,
          std::enable_if_t<std::is_integral_v<X> && std::is_integral_v<N> &&
                           std::is_integral_v<M> && std::is_signed_v<M> &&
                           (sizeof(X) <= 8) && (sizeof(N) <= 8) &&
                           (sizeof(M) <= 8)>* = nullptr>
constexpr long long mod_pow(X x, N n, M m) {
    assert(m > 0);
    if constexpr (std::is_signed_v<N>) assert(n >= 0);
    std::uint64_t y;
    if constexpr (std::is_signed_v<X>) {
        __int128_t r = static_cast<__int128_t>(x) % static_cast<__int128_t>(m);
        if (r < 0) r += static_cast<__int128_t>(m);
        y = static_cast<std::uint64_t>(r);
    } else {
        y = static_cast<std::uint64_t>(x) % static_cast<std::uint64_t>(m);
    }
    return static_cast<long long>(pow_mod64_constexpr(
        y, static_cast<std::uint64_t>(n), static_cast<std::uint64_t>(m)));
}

template <class X, class N, class M,
          std::enable_if_t<std::is_integral_v<X> && std::is_integral_v<N> &&
                           std::is_integral_v<M> && std::is_unsigned_v<M> &&
                           (sizeof(X) <= 8) && (sizeof(N) <= 8) &&
                           (sizeof(M) <= 8)>* = nullptr>
constexpr std::uint64_t mod_pow(X x, N n, M m) {
    assert(m != 0);
    if constexpr (std::is_signed_v<N>) assert(n >= 0);
    std::uint64_t y;
    if constexpr (std::is_signed_v<X>) {
        __int128_t r = static_cast<__int128_t>(x) % static_cast<__int128_t>(m);
        if (r < 0) r += static_cast<__int128_t>(m);
        y = static_cast<std::uint64_t>(r);
    } else {
        y = static_cast<std::uint64_t>(x) % static_cast<std::uint64_t>(m);
    }
    return pow_mod64_constexpr(y, static_cast<std::uint64_t>(n),
                               static_cast<std::uint64_t>(m));
}

// Deterministic Miller-Rabin for signed 32-bit integers.
constexpr bool is_prime_constexpr(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 7 || n == 61) return true;
    if ((n & 1) == 0) return false;
    long long d = n - 1;
    while ((d & 1) == 0) d >>= 1;
    constexpr int bases[] = {2, 7, 61};
    for (int a : bases) {
        long long t = d;
        long long y = pow_mod_constexpr(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = y * y % n;
            t <<= 1;
        }
        if (y != n - 1 && (t & 1) == 0) return false;
    }
    return true;
}

template <int n>
inline constexpr bool is_prime = is_prime_constexpr(n);

// Deterministic Miller-Rabin for every uint64_t value.
constexpr bool is_prime64_constexpr(std::uint64_t n) {
    if (n < 2) return false;
    constexpr std::uint32_t small_primes[] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37
    };
    for (std::uint32_t p : small_primes) {
        if (n == p) return true;
        if (n % p == 0) return false;
    }

    const int s = __builtin_ctzll(n - 1);
    const std::uint64_t d = (n - 1) >> s;
    constexpr std::uint64_t bases[] = {
        2, 325, 9375, 28178, 450775, 9780504, 1795265022
    };
    for (std::uint64_t a : bases) {
        if (a % n == 0) continue;
        std::uint64_t x = pow_mod64_constexpr(a % n, d, n);
        if (x == 1 || x == n - 1) continue;
        bool composite = true;
        for (int r = 1; r < s; ++r) {
            x = mul_mod64(x, x, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite) return false;
    }
    return true;
}

inline bool is_prime64_runtime(std::uint64_t n) {
    if (n < 2) return false;
    constexpr std::uint32_t small_primes[] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37
    };
    for (std::uint32_t p : small_primes) {
        if (n == p) return true;
        if (n % p == 0) return false;
    }
    if (n >= (std::uint64_t{1} << 62)) return is_prime64_constexpr(n);

    const int s = __builtin_ctzll(n - 1);
    const std::uint64_t d = (n - 1) >> s;
    const montgomery64 reducer(n);
    const std::uint64_t one = reducer.one();
    const std::uint64_t minus_one = reducer.transform(n - 1);
    const auto is_composite = [&](std::uint64_t a) {
        if (a % n == 0) return false;
        std::uint64_t x = reducer.pow(reducer.transform(a), d);
        if (reducer.canonical(x) == reducer.canonical(one) ||
            reducer.canonical(x) == reducer.canonical(minus_one)) return false;
        for (int r = 1; r < s; ++r) {
            x = reducer.mul(x, x);
            if (reducer.canonical(x) == reducer.canonical(minus_one)) return false;
        }
        return true;
    };
    if (n < 4759123141ULL) {
        constexpr std::uint64_t bases32[] = {2, 7, 61};
        for (std::uint64_t a : bases32) if (is_composite(a)) return false;
    } else {
        constexpr std::uint64_t bases64[] = {
            2, 325, 9375, 28178, 450775, 9780504, 1795265022
        };
        for (std::uint64_t a : bases64) if (is_composite(a)) return false;
    }
    return true;
}

template <std::uint64_t n>
inline constexpr bool is_prime64 = is_prime64_constexpr(n);

// @return (g, x), where g=gcd(a,b), xa=g (mod b), 0<=x<b/g.
// @param b 1 <= b
template<typename INT1, typename INT2>
constexpr std::pair<INT1, INT1> inv_gcd(INT1 a, INT2 b) {
    assert(1 <= b);
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    INT2 s = b, t = a;
    INT2 m0 = 0, m1 = 1;
    while (t) {
        const INT2 u = s / t;
        s -= t * u;
        m0 -= m1 * u;
        const INT2 tmp_s = s;
        s = t;
        t = tmp_s;
        const INT2 tmp_m = m0;
        m0 = m1;
        m1 = tmp_m;
    }
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

constexpr std::pair<std::uint64_t, std::uint64_t> inv_gcd64(
    std::uint64_t a, std::uint64_t b) {
    assert(b != 0);
    a %= b;
    if (a == 0) return {b, 0};

    std::uint64_t s = b, t = a;
    __int128_t m0 = 0, m1 = 1;
    while (t) {
        const std::uint64_t u = s / t;
        s -= t * u;
        m0 -= m1 * static_cast<__int128_t>(u);
        const std::uint64_t tmp_s = s;
        s = t;
        t = tmp_s;
        const __int128_t tmp_m = m0;
        m0 = m1;
        m1 = tmp_m;
    }
    const std::uint64_t mod = b / s;
    if (m0 < 0) m0 += static_cast<__int128_t>(mod);
    return {s, static_cast<std::uint64_t>(m0)};
}

// @param m a 32-bit prime
constexpr int primitive_root_constexpr(int m) {
    assert(2 <= m && is_prime_constexpr(m));
    if (m == 2) return 1;
    if (m == 167772161) return 3;
    if (m == 469762049) return 3;
    if (m == 754974721) return 11;
    if (m == 998244353) return 3;

    int divs[20] = {};
    divs[0] = 2;
    int cnt = 1;
    int x = (m - 1) / 2;
    while (x % 2 == 0) x /= 2;
    for (int i = 3; static_cast<long long>(i) * i <= x; i += 2) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) x /= i;
        }
    }
    if (x > 1) divs[cnt++] = x;

    for (int g = 2;; ++g) {
        bool ok = true;
        for (int i = 0; i < cnt; ++i) {
            if (pow_mod_constexpr(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}

template <int m>
inline constexpr int primitive_root = primitive_root_constexpr(m);

}  // namespace internal
}  // namespace soryuusi
