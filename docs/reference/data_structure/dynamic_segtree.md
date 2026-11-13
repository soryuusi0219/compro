# dynamic_segtree

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_segtree.hpp)

## 機能・前提条件

必要なノードだけ生成する一点更新・区間集約木。op は結合的、e は単位元。境界探索の述語は単調で e に対して true。

## インターフェース

```cpp
#include "library/data_structure/dynamic_segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct Dynamic_Segtree;
Dynamic_Segtree(int n_);
void set(int i, S x);
S get(int i);
S prod(int l, int r);
S all_prod();
template<class F> S max_right(int i, F f);
template<class F> S min_left(int i, F f);
```

## 計算量

更新・集約・境界探索 O(log U)、Q 回の更新で最大 O(Q log U) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/ordered_set_dynamic_segment_tree`](../../../verify/library_checker/data_structure/ordered_set_dynamic_segment_tree.cpp)
