// https://github.com/soryuusi0219/compro/blob/main/library/math/modint.hpp
#pragma once

#ifndef SORYUUSI_MODINT_HPP
#define SORYUUSI_MODINT_HPP 1
#endif

#include <cassert>
#include <cstdint>
#include <istream>
#include <ostream>
#include <string>
#include <type_traits>

#include "library/math/internal_math.hpp"

namespace soryuusi {

namespace internal {

struct modint_base {};
struct static_modint_base : modint_base {};
struct modint64_base : modint_base {};
struct static_modint64_base : modint64_base {};

template <class T>
using is_signed_int = std::conjunction<std::is_integral<T>, std::is_signed<T>>;
template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;
template <class T>
using is_unsigned_int = std::conjunction<std::is_integral<T>, std::is_unsigned<T>>;
template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T>
using is_modint = std::is_base_of<modint_base, T>;
template <class T>
using is_modint_t = std::enable_if_t<is_modint<T>::value>;

template <class T>
using is_modint64 = std::is_base_of<modint64_base, T>;
template <class T>
using is_modint64_t = std::enable_if_t<is_modint64<T>::value>;

}  // namespace internal

// ACL-compatible 32-bit static modint.
template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base {
    using mint = static_modint;

    static constexpr int mod() { return m; }
    static constexpr mint raw(int v) {
        mint x;
        x._v = static_cast<unsigned int>(v);
        return x;
    }

    constexpr static_modint() : _v(0) {}

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    constexpr static_modint(T v) {
        if constexpr (sizeof(T) <= sizeof(long long)) {
            long long x = static_cast<long long>(v) % static_cast<long long>(umod());
            if (x < 0) x += umod();
            _v = static_cast<unsigned int>(x);
        } else {
            __int128_t x = static_cast<__int128_t>(v) % umod();
            if (x < 0) x += umod();
            _v = static_cast<unsigned int>(x);
        }
    }

    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    constexpr static_modint(T v) : _v(static_cast<unsigned int>(v % umod())) {}

    constexpr unsigned int val() const { return _v; }

    constexpr mint& operator++() {
        if (++_v == umod()) _v = 0;
        return *this;
    }
    constexpr mint& operator--() {
        if (_v == 0) _v = umod();
        --_v;
        return *this;
    }
    constexpr mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    constexpr mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    constexpr mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    constexpr mint& operator-=(const mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    constexpr mint& operator*=(const mint& rhs) {
        _v = static_cast<unsigned int>(
            static_cast<unsigned long long>(_v) * rhs._v % umod());
        return *this;
    }
    constexpr mint& operator/=(const mint& rhs) { return *this *= rhs.inv(); }

    constexpr mint operator+() const { return *this; }
    constexpr mint operator-() const { return mint() - *this; }

    constexpr mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    constexpr mint inv() const {
        if constexpr (prime) {
            assert(_v != 0);
            return pow(umod() - 2);
        } else {
            const auto eg = internal::inv_gcd(_v, m);
            assert(eg.first == 1);
            return eg.second;
        }
    }

    friend constexpr mint operator+(mint lhs, const mint& rhs) { return lhs += rhs; }
    friend constexpr mint operator-(mint lhs, const mint& rhs) { return lhs -= rhs; }
    friend constexpr mint operator*(mint lhs, const mint& rhs) { return lhs *= rhs; }
    friend constexpr mint operator/(mint lhs, const mint& rhs) { return lhs /= rhs; }
    friend constexpr bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend constexpr bool operator!=(const mint& lhs, const mint& rhs) {
        return !(lhs == rhs);
    }

  private:
    unsigned int _v;
    static constexpr unsigned int umod() { return static_cast<unsigned int>(m); }
    static constexpr bool prime = internal::is_prime<m>;
};

// ACL-compatible 32-bit dynamic modint. Multiplication uses Barrett reduction.
template <int id>
struct dynamic_modint : internal::modint_base {
    using mint = dynamic_modint;

    static int mod() { return static_cast<int>(bt.umod()); }
    static void set_mod(int m) {
        assert(1 <= m);
        bt = internal::barrett(static_cast<unsigned int>(m));
    }
    static mint raw(int v) {
        mint x;
        x._v = static_cast<unsigned int>(v);
        return x;
    }

    dynamic_modint() : _v(0) {}

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        if constexpr (sizeof(T) <= sizeof(long long)) {
            long long x = static_cast<long long>(v) % static_cast<long long>(umod());
            if (x < 0) x += umod();
            _v = static_cast<unsigned int>(x);
        } else {
            __int128_t x = static_cast<__int128_t>(v) % umod();
            if (x < 0) x += umod();
            _v = static_cast<unsigned int>(x);
        }
    }

    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    dynamic_modint(T v) : _v(static_cast<unsigned int>(v % umod())) {}

    unsigned int val() const { return _v; }

    mint& operator++() {
        if (++_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        --_v;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v += umod() - rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        _v = bt.mul(_v, rhs._v);
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this *= rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        const auto eg = internal::inv_gcd(_v, mod());
        assert(eg.first == 1);
        return eg.second;
    }

    friend mint operator+(mint lhs, const mint& rhs) { return lhs += rhs; }
    friend mint operator-(mint lhs, const mint& rhs) { return lhs -= rhs; }
    friend mint operator*(mint lhs, const mint& rhs) { return lhs *= rhs; }
    friend mint operator/(mint lhs, const mint& rhs) { return lhs /= rhs; }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) { return !(lhs == rhs); }

  private:
    unsigned int _v;
    static unsigned int umod() { return bt.umod(); }
    inline static internal::barrett bt = internal::barrett(998244353);
};

using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;

// 64-bit counterpart. The suffix deliberately prevents accidental mixing with
// ACL-compatible 32-bit modints.
template <std::uint64_t m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint64 : internal::static_modint64_base {
    using mint = static_modint64;

    static constexpr std::uint64_t mod() { return m; }
    static constexpr mint raw(std::uint64_t v) {
        mint x;
        x._v = v;
        return x;
    }

    constexpr static_modint64() : _v(0) {}

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    constexpr static_modint64(T v) {
        __int128_t x = static_cast<__int128_t>(v) % static_cast<__int128_t>(m);
        if (x < 0) x += static_cast<__int128_t>(m);
        _v = static_cast<std::uint64_t>(x);
    }

    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    constexpr static_modint64(T v)
        : _v(static_cast<std::uint64_t>(static_cast<__uint128_t>(v) % m)) {}

    constexpr std::uint64_t val() const { return _v; }

    constexpr mint& operator++() {
        if (++_v == m) _v = 0;
        return *this;
    }
    constexpr mint& operator--() {
        if (_v == 0) _v = m;
        --_v;
        return *this;
    }
    constexpr mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    constexpr mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    constexpr mint& operator+=(const mint& rhs) {
        const std::uint64_t sum = _v + rhs._v;
        _v = (sum < _v || sum >= m) ? sum - m : sum;
        return *this;
    }
    constexpr mint& operator-=(const mint& rhs) {
        if (_v < rhs._v) _v += m - rhs._v;
        else _v -= rhs._v;
        return *this;
    }
    constexpr mint& operator*=(const mint& rhs) {
        _v = internal::mul_mod64(_v, rhs._v, m);
        return *this;
    }
    constexpr mint& operator/=(const mint& rhs) { return *this *= rhs.inv(); }

    constexpr mint operator+() const { return *this; }
    constexpr mint operator-() const { return mint() - *this; }

    constexpr mint pow(long long n) const {
        assert(0 <= n);
        return pow_unsigned(static_cast<std::uint64_t>(n));
    }
    constexpr mint inv() const {
        if constexpr (prime) {
            assert(_v != 0);
            return pow_unsigned(m - 2);
        } else {
            const auto eg = internal::inv_gcd64(_v, m);
            assert(eg.first == 1);
            return eg.second;
        }
    }

    friend constexpr mint operator+(mint lhs, const mint& rhs) { return lhs += rhs; }
    friend constexpr mint operator-(mint lhs, const mint& rhs) { return lhs -= rhs; }
    friend constexpr mint operator*(mint lhs, const mint& rhs) { return lhs *= rhs; }
    friend constexpr mint operator/(mint lhs, const mint& rhs) { return lhs /= rhs; }
    friend constexpr bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend constexpr bool operator!=(const mint& lhs, const mint& rhs) {
        return !(lhs == rhs);
    }

  private:
    std::uint64_t _v;
    static constexpr bool prime = internal::is_prime64<m>;

    constexpr mint pow_unsigned(std::uint64_t n) const {
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
};

template <int id>
struct dynamic_modint64 : internal::modint64_base {
    using mint = dynamic_modint64;

    static std::uint64_t mod() { return modulus; }
    static void set_mod(std::uint64_t m) {
        assert(m != 0);
        modulus = m;
    }
    static mint raw(std::uint64_t v) {
        mint x;
        x._v = v;
        return x;
    }

    dynamic_modint64() : _v(0) {}

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    dynamic_modint64(T v) {
        __int128_t x = static_cast<__int128_t>(v) % static_cast<__int128_t>(modulus);
        if (x < 0) x += static_cast<__int128_t>(modulus);
        _v = static_cast<std::uint64_t>(x);
    }

    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    dynamic_modint64(T v)
        : _v(static_cast<std::uint64_t>(static_cast<__uint128_t>(v) % modulus)) {}

    std::uint64_t val() const { return _v; }

    mint& operator++() {
        if (++_v == modulus) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = modulus;
        --_v;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        const std::uint64_t sum = _v + rhs._v;
        _v = (sum < _v || sum >= modulus) ? sum - modulus : sum;
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        if (_v < rhs._v) _v += modulus - rhs._v;
        else _v -= rhs._v;
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        _v = internal::mul_mod64(_v, rhs._v, modulus);
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this *= rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        const auto eg = internal::inv_gcd64(_v, modulus);
        assert(eg.first == 1);
        return eg.second;
    }

    friend mint operator+(mint lhs, const mint& rhs) { return lhs += rhs; }
    friend mint operator-(mint lhs, const mint& rhs) { return lhs -= rhs; }
    friend mint operator*(mint lhs, const mint& rhs) { return lhs *= rhs; }
    friend mint operator/(mint lhs, const mint& rhs) { return lhs /= rhs; }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) { return !(lhs == rhs); }

  private:
    std::uint64_t _v;
    inline static std::uint64_t modulus = 998244353;
};

using modint64 = dynamic_modint64<-1>;

namespace internal {

template <class T>
using is_static_modint = std::is_base_of<static_modint_base, T>;
template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;

template <class>
struct is_dynamic_modint : std::false_type {};
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : std::true_type {};
template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;

template <class T>
using is_static_modint64 = std::is_base_of<static_modint64_base, T>;
template <class T>
using is_static_modint64_t = std::enable_if_t<is_static_modint64<T>::value>;

template <class>
struct is_dynamic_modint64 : std::false_type {};
template <int id>
struct is_dynamic_modint64<dynamic_modint64<id>> : std::true_type {};
template <class T>
using is_dynamic_modint64_t = std::enable_if_t<is_dynamic_modint64<T>::value>;

}  // namespace internal

template <class CharT, class Traits, class Mint,
          internal::is_modint_t<Mint>* = nullptr>
std::basic_istream<CharT, Traits>& operator>>(
    std::basic_istream<CharT, Traits>& stream, Mint& value) {
    std::basic_string<CharT> token;
    if (!(stream >> token)) return stream;
    bool negative = false;
    std::size_t position = 0;
    if (!token.empty() && (token[0] == CharT('-') || token[0] == CharT('+'))) {
        negative = token[0] == CharT('-');
        position = 1;
    }
    if (position == token.size()) {
        stream.setstate(std::ios_base::failbit);
        return stream;
    }
    Mint result = 0;
    for (; position < token.size(); ++position) {
        const CharT c = token[position];
        if (c < CharT('0') || CharT('9') < c) {
            stream.setstate(std::ios_base::failbit);
            return stream;
        }
        result = result * 10 + static_cast<int>(c - CharT('0'));
    }
    value = negative ? -result : result;
    return stream;
}

template <class CharT, class Traits, class Mint,
          internal::is_modint_t<Mint>* = nullptr>
std::basic_ostream<CharT, Traits>& operator<<(
    std::basic_ostream<CharT, Traits>& stream, const Mint& value) {
    return stream << value.val();
}

}  // namespace soryuusi
