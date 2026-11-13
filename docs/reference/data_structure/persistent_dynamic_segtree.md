# persistent_dynamic_segtree

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dynamic_segtree.hpp)

## 機能・前提条件

一点更新型の永続動的セグメント木。型名は Dynamic_Segtree で通常版と衝突する。rootを明示的に管理する。

## インターフェース

```cpp
#include "library/data_structure/persistent_dynamic_segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct Dynamic_Segtree;
using NP = node*;
Dynamic_Segtree(int n_);
NP instance() const;
NP set(NP p, int i, S x);
S get(NP p, int i) const;
S prod(NP p, int l, int r) const;
S all_prod(NP p) const;
template<class F> S max_right(NP p, int i, F f) const;
template<class F> S min_left(NP p, int i, F f) const;
```

## 計算量

更新・取得・集約 O(log U)、更新ごと O(log U) ノード追加。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/ordered_set_persistent_dynamic_segment_tree`](../../../verify/library_checker/data_structure/ordered_set_persistent_dynamic_segment_tree.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
