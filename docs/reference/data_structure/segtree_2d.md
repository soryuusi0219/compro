# segtree_2d

[索引](../README.md) / [実装](../../../library/data_structure/segtree_2d.hpp)

## 機能・前提条件

密な二次元配列の一点更新・長方形集約。op は可換モノイドを想定する。

配列構築版には行の長さが揃った配列を渡す。空配列は `0 × 0`。各軸の葉は実際の長さ分だけ持ち、空区間の積は `e()`。

## インターフェース

```cpp
#include "library/data_structure/segtree_2d.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct Segtree2D;
Segtree2D();
Segtree2D(int h_, int w_);
Segtree2D(const vector<vector<S>>& v);
void set(int i, int j, S x);
S get(int i, int j) const;
S prod(int hl, int hr, int wl, int wr) const;
S all_prod();
```

## 計算量

正のサイズでは構築と空間は `O(HW)`。更新・集約は最悪 `O(log(H+1) log(W+1))`、`all_prod()` は `O(1)`。配列の要素数は `4*max(H,1)*max(W,1)`。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/rectangle_queries`](../../../verify/unit/rectangle_queries.cpp)
- [`aoj/range_sum_segtree_2d`](../../../verify/aoj/data_structure/range_sum_segtree_2d.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
