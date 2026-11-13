# mod_sqrt

[索引](../README.md) / [実装](../../../library/math/mod_sqrt.hpp)

## 機能・前提条件

素数modの平方根をTonelli–Shanksで求める。解なしなら整数版は `-1`、modint版は `Mint(-1)` を返す。modintでは番兵も剰余値になるので解の検査時に注意する。

## インターフェース

```cpp
#include "library/math/mod_sqrt.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
constexpr std::uint64_t mod_sqrt64(std::uint64_t a, std::uint64_t p);
inline long long mod_sqrt(long long a, long long p);
template <int Mod = 998244353>
constexpr long long mod_sqrt(long long a);
template <class Mint, internal::is_modint_like_t<Mint>* = nullptr>
constexpr Mint mod_sqrt(Mint a);
inline long long modsqrt(long long a, long long p);
template <long long Mod = 998244353>
constexpr long long modsqrt(long long a);
template <class Mint, internal::is_modint_like_t<Mint>* = nullptr>
constexpr Mint modsqrt(Mint a);
```

## 計算量

最悪 O(log² p) 回程度のmod乗算、O(1) 空間。pは素数。

## 依存関係

- [internal_math.hpp](internal_math.md)
- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

- [`library_checker/sqrt_mod`](../../../verify/library_checker/number_theory/sqrt_mod.cpp)
- [`library_checker/sqrt_of_formal_power_series`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series.cpp)
- [`library_checker/sqrt_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series_sparse.cpp)
