# monotone_minima_dp

[索引](../README.md) / [実装](../../../library/utility/monotone_minima_dp.hpp)

## 機能・前提条件

単調最小位置を持つDP遷移の高速化。f(i,j)は既に確定したDPを使うよう設計する。

## インターフェース

```cpp
#include "library/utility/monotone_minima_dp.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class F, class Compare = less<T>>
vector<T> monotoneminima_dp(int n, const F& f, const Compare &comp = Compare());
```

## 計算量

N状態に O(N log² N) 回の評価を保守的上界とする。空間 O(N)。

## 依存関係

- [monotone_minima.hpp](monotone_minima.md)

## Verify・利用例

- [`unit/monge`](../../../verify/unit/monge.cpp)
