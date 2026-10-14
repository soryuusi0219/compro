// https://github.com/soryuusi0219/compro/blob/main/library/math/math.hpp
#pragma once

#include <algorithm>
#include <cassert>
#include <tuple>
#include <utility>
#include <vector>

#include "library/math/internal_math.hpp"

namespace soryuusi {

// ACL-compatible public number-theory interface.
constexpr bool is_prime(long long n) {
    if (n < 0) return false;
    const auto value = static_cast<std::uint64_t>(n);
    if (std::is_constant_evaluated()) return internal::is_prime64_constexpr(value);
    return internal::is_prime64_runtime(value);
}

constexpr int primitive_root(int m) {
    return internal::primitive_root_constexpr(m);
}

constexpr long long pow_mod(long long x, long long n, int m) {
    assert(0 <= n && 1 <= m);
    if (m == 1) return 0;
    internal::barrett bt(static_cast<unsigned int>(m));
    unsigned int r = 1;
    unsigned int y = static_cast<unsigned int>(internal::safe_mod(x, m));
    while (n) {
        if (n & 1) r = bt.mul(r, y);
        y = bt.mul(y, y);
        n >>= 1;
    }
    return r;
}

template<typename INT1, typename INT2>
constexpr INT2 inv_mod(INT1 x, INT2 m) {
    assert(1 <= m);
    const auto z = internal::inv_gcd(x, m);
    assert(z.first == 1);
    return z.second;
}

// Returns (remainder, lcm). Returns (0, 0) when the system is inconsistent.
template<typename INT = long long>
inline std::pair<INT, INT> crt(const std::vector<INT>& r, const std::vector<INT>& m) {
    assert(r.size() == m.size());
    INT r0 = 0, m0 = 1;
    for (std::size_t i = 0; i < r.size(); ++i) {
        assert(1 <= m[i]);
        INT r1 = internal::safe_mod(r[i], m[i]);
        INT m1 = m[i];
        if (m0 < m1) {
            std::swap(r0, r1);
            std::swap(m0, m1);
        }
        if (m0 % m1 == 0) {
            if (r0 % m1 != r1) return {0, 0};
            continue;
        }

        auto [g, im] = internal::inv_gcd(m0, m1);
        const INT u1 = m1 / g;
        if ((r1 - r0) % g != 0) return {0, 0};
        const INT x = (r1 - r0) / g % u1 * im % u1;

        r0 += x * m0;
        m0 *= u1;
        if (r0 < 0) r0 += m0;
    }
    return {r0, m0};
}

template<typename INT>
constexpr INT floor_sum(INT n, INT m, INT a, INT b) {
    assert(0 <= n && 1 <= m && 0 <= a && 0 <= b);
    INT ans = 0;
    while (true) {
        if (a >= m) {
            ans += (n - 1) * n * (a / m) / 2;
            a %= m;
        }
        if (b >= m) {
            ans += n * (b / m);
            b %= m;
        }
        const INT y_max = a * n + b;
        if (y_max < m) break;
        n = y_max / m;
        b = y_max % m;
        std::swap(m, a);
    }
    return ans;
}

}  // namespace soryuusi
