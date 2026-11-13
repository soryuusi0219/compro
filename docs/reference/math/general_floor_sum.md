# general_floor_sum

[索引](../README.md) / [実装](../../../library/math/general_floor_sum.hpp)

## 機能・前提条件

floorで定まる語の非可換モノイド積をEuclid再帰で求める。pow(S,k)とop/eを渡す。m>0、n,a,bは非負を想定。

## インターフェース

```cpp
#include "library/math/general_floor_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, auto pow>
S general_floor_sum(long long n, long long m, long long a, long long b, S x, S y);
```

## 計算量

Euclid再帰 O(log m) 段。各段のpowとopのコストを掛ける。再帰空間 O(log m)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/sum_of_floor_of_linear_general_floor_sum`](../../../verify/library_checker/number_theory/sum_of_floor_of_linear_general_floor_sum.cpp)
