# segtree_2d_sparse

[索引](../README.md) / [実装](../../../library/data_structure/segtree_2d_sparse.hpp)

## 機能・前提条件

事前登録座標上で点の値を結合し長方形を集約する疎二次元セグメント木。可換モノイドを使う。`add_point` で登録し、`build()` が重複座標をまとめて値を `e()` に初期化する。その後、登録済みの点に `add` できる。再度 `build()` すると値は初期化される。集約は半開区間。

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

登録数を `P`、異なる点数を `N` とすると、構築は `O(P log(P+1))`。葉のY座標を整列し、親では子の整列済み座標の和集合を取る。更新・集約は最悪 `O(log²(N+1))`、空間は `O(P+N log(N+1))`。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/rectangle_queries`](../../../verify/unit/rectangle_queries.cpp)
- [`library_checker/point_add_rectangle_sum_segtree_2d_sparse`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_segtree_2d_sparse.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
