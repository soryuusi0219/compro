# segtree_sparse

[索引](../README.md) / [実装](../../../library/data_structure/segtree_sparse.hpp)

## 機能・前提条件

更新する座標を事前登録するセグメント木。add_point → build → set/prod。

`build()` は重複座標を一つにまとめ、各座標の値を `e()` にする。`set` は登録座標に対して呼ぶ。`prod(l,r)` は `[l,r)` の登録座標を昇順に集約する。

## インターフェース

```cpp
#include "library/data_structure/segtree_sparse.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class X = int>
struct Segtree_Sparse;
Segtree_Sparse();
Segtree_Sparse(int sz);
void add_point(X x);
void build();
void set(X x, S a);
S prod(X x, X y);
S all_prod();
```

## 計算量

構築 O(N log N)、更新・取得 O(log N)、空間 O(N)。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/linear_segment_trees`](../../../verify/unit/linear_segment_trees.cpp)
- [`library_checker/point_add_range_sum_segtree_sparse`](../../../verify/library_checker/data_structure/point_add_range_sum_segtree_sparse.cpp)
- [`library_checker/point_set_range_composite_large_array_segtree_sparse`](../../../verify/library_checker/data_structure/point_set_range_composite_large_array_segtree_sparse.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
