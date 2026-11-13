# dynamic_segtree_2d

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_segtree_2d.hpp)

## 機能・前提条件

巨大な整数座標上で一点 set/add と長方形 prod を扱う動的二次元木。二次元の集約には可換な op を用いる。

## インターフェース

```cpp
#include "library/data_structure/dynamic_segtree_2d.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct Dynamic_Segtree2D;
Dynamic_Segtree2D(int h_, int w_);
void set(int i, int j, S x);
void add(int i, int j, S x);
S get(int i, int j);
S prod(int hl, int hr, int wl, int wr);
S all_prod();
```

## 計算量

操作 O(log H log W)、Q 更新で最大 O(Q log H log W) ノード。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/point_add_rectangle_sum_dynamic_segtree_2d`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_dynamic_segtree_2d.cpp)
