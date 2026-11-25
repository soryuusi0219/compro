# range_set_range_prod

[索引](../README.md) / [実装](../../../library/data_structure/range_set_range_prod.hpp)

## 機能・前提条件

同じ値の区間代入とモノイド集約。任意の結合的opと単位元eを使う。

区間は半開区間。内部の葉数は `max(1,n)` 以上の最小の2冪で、長さによる同値の積を遅延タグに対応づける。全区間を代入すると以前の積の履歴を破棄する。`max_right` と `min_left` の述語は単調で、`e()` に対して真を返す必要がある。

## インターフェース

```cpp
#include "library/data_structure/range_set_range_prod.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct RangeSetRangeProd;
RangeSetRangeProd(int n_ = 0);
RangeSetRangeProd(const vector<S>& v);
void set(int p, S x);
S get(int p);
S prod(int l, int r);
S all_prod();
void range_set(int l, int r, S x);
int max_right(int l, auto g);
int min_left(int r, auto g);
```

## 計算量

各モノイド演算を定数時間とすると、構築は `O(N)`、一点・区間問い合わせと境界探索は最悪 `O(log(N+1))`、`all_prod` は `O(1)`。区間代入は償却 `O(log(N+1))`、履歴をまとめて伝播する回は最悪 `O(N)`。空間は `O(N)`。

## 依存関係

- [segtree_ops.hpp](internal/segtree_ops.md)
- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/linear_segment_trees`](../../../verify/unit/linear_segment_trees.cpp)
- [`library_checker/range_set_range_composite`](../../../verify/library_checker/data_structure/range_set_range_composite.cpp)
