# internal_math

[索引](../README.md) / [実装](../../../library/math/internal_math.hpp)

## 機能・前提条件

符号付き除算の丸め、剰余、Barrett/Montgomery、mod冪、素数判定などの内部数論。

## インターフェース

```cpp
#include "library/math/internal_math.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <class Int, std::enable_if_t<std::is_integral_v<Int> &&
constexpr Int divfloor(Int a, Int b);
template <class Int, std::enable_if_t<std::is_integral_v<Int> &&
constexpr Int divceil(Int a, Int b);
constexpr int ceil_pow2(int n);
constexpr int bsf(unsigned int n);
template<typename INT1, typename INT2>
constexpr INT2 safe_mod(INT1 x, INT2 m);
struct barrett;
explicit constexpr barrett(unsigned int m = 1);
constexpr unsigned int umod() const;
constexpr unsigned int mul(unsigned int a, unsigned int b) const;
struct montgomery64;
using u64 = std::uint64_t;
using u128 = __uint128_t;
explicit constexpr montgomery64(u64 m);
constexpr u64 reduce(u128 x) const;
constexpr u64 transform(u64 x) const;
constexpr u64 canonical(u64 x) const;
constexpr u64 value(u64 x) const;
constexpr u64 one() const;
constexpr u64 add(u64 x, u64 y) const;
constexpr u64 sub(u64 x, u64 y) const;
constexpr u64 mul(u64 x, u64 y) const;
constexpr u64 pow(u64 x, u64 n) const;
constexpr long long pow_mod_constexpr(long long x, long long n, int m);
template <class X, class N, class M,
constexpr long long mod_pow(X x, N n, M m);
template <class X, class N, class M,
constexpr std::uint64_t mod_pow(X x, N n, M m);
constexpr bool is_prime_constexpr(int n);
template <int n>
constexpr bool is_prime64_constexpr(std::uint64_t n);
inline bool is_prime64_runtime(std::uint64_t n);
template <std::uint64_t n>
template<typename INT1, typename INT2>
constexpr std::pair<INT1, INT1> inv_gcd(INT1 a, INT2 b);
constexpr int primitive_root_constexpr(int m);
template <int m>
```

## 計算量

基本演算 O(1)、冪 O(log k)、gcd系 O(log m)。固定幅と中間値の制約を満たすこと。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/primality_test_internal_math`](../../../verify/library_checker/number_theory/primality_test_internal_math.cpp)
