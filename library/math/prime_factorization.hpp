// https://github.com/soryuusi0219/compro/blob/main/library/math/prime_factorization.hpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

#include "library/math/math.hpp"

namespace soryuusi {

namespace internal {

inline constexpr std::uint64_t pollard_batch_size = 1024;

inline std::uint64_t pollard_rho_wide(std::uint64_t n) {
    for (std::uint64_t c = 1;; ++c) {
        const auto f = [n, c](std::uint64_t x) {
            const std::uint64_t square = mul_mod64(x, x, n);
            const std::uint64_t add = c % n;
            return square >= n - add ? square - (n - add) : square + add;
        };
        std::uint64_t y = 2, x = 0, ys = 0;
        std::uint64_t r = 1, q = 1, g = 1;
        while (g == 1) {
            x = y;
            for (std::uint64_t i = 0; i < r; ++i) y = f(y);
            for (std::uint64_t k = 0; k < r && g == 1;
                 k += pollard_batch_size) {
                ys = y;
                const std::uint64_t limit =
                    std::min<std::uint64_t>(pollard_batch_size, r - k);
                for (std::uint64_t i = 0; i < limit; ++i) {
                    y = f(y);
                    const std::uint64_t diff = x > y ? x - y : y - x;
                    q = mul_mod64(q, diff, n);
                }
                g = std::gcd(q, n);
            }
            r <<= 1;
        }
        if (g == n) {
            do {
                ys = f(ys);
                const std::uint64_t diff = x > ys ? x - ys : ys - x;
                g = std::gcd(diff, n);
            } while (g == 1);
        }
        if (g != n) return g;
    }
}

inline std::uint64_t pollard_rho(std::uint64_t n) {
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;
    if (n >= (std::uint64_t{1} << 62)) return pollard_rho_wide(n);
    const montgomery64 reducer(n);
    for (std::uint64_t c = 1;; ++c) {
        const std::uint64_t montgomery_c = reducer.transform(c);
        const auto f = [&reducer, montgomery_c](std::uint64_t x) {
            return reducer.add(reducer.mul(x, x), montgomery_c);
        };
        std::uint64_t y = reducer.transform(2), x = 0, ys = 0;
        std::uint64_t r = 1, q = reducer.one(), g = 1;
        while (g == 1) {
            x = y;
            for (std::uint64_t i = 0; i < r; ++i) y = f(y);
            for (std::uint64_t k = 0; k < r && g == 1;
                 k += pollard_batch_size) {
                ys = y;
                const std::uint64_t limit =
                    std::min<std::uint64_t>(pollard_batch_size, r - k);
                for (std::uint64_t i = 0; i < limit; ++i) {
                    y = f(y);
                    q = reducer.mul(q, reducer.sub(x, y));
                }
                g = std::gcd(reducer.value(q), n);
            }
            r <<= 1;
        }
        if (g == n) {
            do {
                ys = f(ys);
                g = std::gcd(reducer.value(reducer.sub(x, ys)), n);
            } while (g == 1);
        }
        if (g != n) return g;
    }
}

inline void factorize(std::uint64_t n, std::vector<std::uint64_t>& factors) {
    if (n == 1) return;
    if (is_prime64_runtime(n)) {
        factors.push_back(n);
        return;
    }
    const std::uint64_t divisor = pollard_rho(n);
    factorize(divisor, factors);
    factorize(n / divisor, factors);
}

}  // namespace internal

inline long long Pollard_Rho(long long n) {
    assert(1 < n);
    if (is_prime(n)) return n;
    return static_cast<long long>(
        internal::pollard_rho(static_cast<std::uint64_t>(n)));
}

inline std::vector<long long> prime_factorization(long long n) {
    assert(0 < n);
    std::vector<std::uint64_t> factors;
    internal::factorize(static_cast<std::uint64_t>(n), factors);
    std::sort(factors.begin(), factors.end());
    return std::vector<long long>(factors.begin(), factors.end());
}

inline std::vector<std::pair<long long, long long>> prime_factorization_paired(
    long long n) {
    const auto factors = prime_factorization(n);
    std::vector<std::pair<long long, long long>> result;
    for (long long p : factors) {
        if (result.empty() || result.back().first != p) result.emplace_back(p, 1);
        else ++result.back().second;
    }
    return result;
}

inline std::vector<long long> divisors(long long n) {
    assert(0 < n);
    std::vector<long long> result = {1};
    for (const auto& [p, exponent] : prime_factorization_paired(n)) {
        const std::size_t size = result.size();
        long long power = 1;
        for (long long e = 1; e <= exponent; ++e) {
            power *= p;
            for (std::size_t i = 0; i < size; ++i) result.push_back(result[i] * power);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

}  // namespace soryuusi
