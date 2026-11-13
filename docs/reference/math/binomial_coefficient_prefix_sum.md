# binomial_coefficient_prefix_sum

[索引](../README.md) / [実装](../../../library/math/binomial_coefficient_prefix_sum.hpp)

## 機能・前提条件

質問 (n,k) ごとに二項係数のprefix和 Σ(i=0..k) C(n,i) を一括計算する。階乗と2の逆元を使う。

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
