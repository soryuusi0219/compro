# min_mod_linear

[索引](../README.md) / [実装](../../../library/math/min_mod_linear.hpp)

## 機能・前提条件

一次式 (a*x+b) mod m のprefix最小値と区間最小値。m>0、指定区間と値域の整数条件を守る。

## インターフェース

```cpp
#include "library/math/min_mod_linear.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class INT = long long>
pair<vector<INT>,vector<INT>> ModLinearPrefixMinimum(INT a, INT b, INT m);
template<class INT = long long>
pair<INT,INT> MinModLinear(INT a, INT b, INT m, INT l, INT r);
```

## 計算量

Euclid型の O(log m) 時間・空間を基本とする。結果列挙は返却要素数を加える。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_of_mod_of_linear`](../../../verify/library_checker/number_theory/min_of_mod_of_linear.cpp)
