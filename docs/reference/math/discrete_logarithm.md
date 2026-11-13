# discrete_logarithm

[索引](../README.md) / [実装](../../../library/math/discrete_logarithm.hpp)

## 機能・前提条件

作用・逆写像・冪・hashを指定する汎用baby-step giant-step。`DiscreteLogarithm` と逆元不要の `DiscreteLogarithm_Monoid` を提供する。上限MXを指定し、解なしの戻り値は `-1`。

## インターフェース

```cpp
#include "library/math/discrete_logarithm.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename F, typename S, auto pow, auto inv, auto apply, auto hash>
long long DiscreteLogarithm(F f, S s, S t, long long MX);
template<typename F, typename S, auto pow, auto apply, auto hash>
long long DiscreteLogarithm_Monoid(F f, S s, S t, long long MX);
```

## 計算量

O(√MX) 回程度の作用と期待hashアクセス、O(√MX) 空間。利用する冪callbackのコストを加える。

## 依存関係

- [hash_table.hpp](../data_structure/hash_table.md)

## Verify・利用例

- [`library_checker/discrete_logarithm_mod`](../../../verify/library_checker/number_theory/discrete_logarithm_mod.cpp)
