# binomial_coefficient_prefix_sum

[索引](../README.md) / [実装](../../../library/math/binomial_coefficient_prefix_sum.hpp)

## 機能・前提条件

質問 $(n,k)$ ごとに二項係数のprefix和 $\sum_{i=0}^k\binom ni$ を一括計算する。$n\ge0$ を前提とし、$k<0$ なら0、$k\ge n$ なら $2^n$ を返す。階乗と2の逆元を使い、二項係数の対称性で中央より右の質問を左半分に移す。

## インターフェース

```cpp
#include "library/math/binomial_coefficient_prefix_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> BinomialCoefficientPrfixSum(const vector<pair<int,int>>& v);
```

## 計算量

N=max n、質問Qに O(N+Q log Q+N√Q) 時間程度、O(N+Q) 空間。

## 依存関係

- [modint_factorials.hpp](modint_factorials.md)
- [mo.hpp](../data_structure/mo.md)

## Verify・利用例

- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
- [`unit/modular_combinatorics`](../../../verify/unit/modular_combinatorics.cpp)
- [`library_checker/prefix_sum_of_binomial_coefficient`](../../../verify/library_checker/enumerative_combinatorics/prefix_sum_of_binomial_coefficient.cpp)
