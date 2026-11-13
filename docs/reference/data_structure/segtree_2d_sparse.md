# segtree_2d_sparse

[索引](../README.md) / [実装](../../../library/data_structure/segtree_2d_sparse.hpp)

## 機能・前提条件

事前登録座標上で点の値を結合し長方形を集約する疎二次元セグメント木。add_point → build。

## インターフェース

```cpp
#include "library/data_structure/segtree_2d_sparse.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class X = int, class Y = int>
struct Segtree2D_Sparse;
Segtree2D_Sparse();
Segtree2D_Sparse(int sz);
void add_point(X x, Y y);
void build();
void add(X x, Y y, S a);
S prod(X xl, X xr, Y yl, Y yr);
S all_prod();
```

## 計算量

構築 O(N log² N) を上界とし、更新・集約 O(log² N)、空間 O(N log N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/point_add_rectangle_sum_segtree_2d_sparse`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_segtree_2d_sparse.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
