# monotone_minima

[索引](../README.md) / [実装](../../../library/utility/monotone_minima.hpp)

## 機能・前提条件

行列の要素を `f(row,col)` で評価し、各行の `{最良列, 値}` を返す。行・列は0始まり。
`Compare` の優先順で選び、同値なら最も左の列を返す。この最良列が下の行へ向かって広義単調増加であること。行数・列数は非負で、行があれば列は1つ以上必要。

## インターフェース

```cpp
#include "library/utility/monotone_minima.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class F, class Compare = less<T>>
vector<pair<int,T>> monotoneminima(int h, int w, const F& f, const Compare &comp = Compare());
```

## 計算量

$H$ 行 $W$ 列に対して $O(H+W\log(H+1))$ 回の要素評価・比較。中央の行の最良列を境界として、上下の行が検索する列の範囲を絞る。出力を含め $O(H)$ 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_convex_arbitrary_monotone_minima_utility`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_monotone_minima_utility.cpp)
- [`unit/monge`](../../../verify/unit/monge.cpp)
