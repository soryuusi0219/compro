# monotone_minima_dp

[索引](../README.md) / [実装](../../../library/utility/monotone_minima_dp.hpp)

## 機能・前提条件

遷移コスト `f(l,r)` を使い、`dp[0] = f(0,0)` から `dp[n]` まで求める。`r > 0` の `dp[r]` は、初期候補 `f(0,r)` と全ての `dp[l] + f(l,r)`（`0 <= l < r`）から `Compare` で選んだ最良値。

確定済みの左側から右側への遷移を長方形に分けて処理する。各長方形で最良の遷移元を同値の場合に最小の `l` とすると、その `l` が `r` に対して広義単調増加であること。例えば最小化では Monge な遷移コストが条件を満たす。`n == 0` では `f(0,0)` だけを返す。

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

$O((n+1)\log^2(n+2))$ 回のコスト評価・比較、$O(n+1)$ 空間。単一の遷移は直接評価し、区間を処理する作業配列は使い回す。

## 依存関係

- [monotone_minima.hpp](monotone_minima.md)

## Verify・利用例

- [`unit/monge`](../../../verify/unit/monge.cpp)
