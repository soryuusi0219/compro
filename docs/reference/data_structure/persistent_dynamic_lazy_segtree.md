# persistent_dynamic_lazy_segtree

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dynamic_lazy_segtree.hpp)

## 機能・前提条件

rootで版を管理する永続遅延セグメント木。set/apply/copy/copy_with_apply は返したrootを保存する。

## インターフェース

```cpp
#include "library/data_structure/persistent_dynamic_lazy_segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct Persistent_Dynamic_LazySegtree;
using NP = node*;
Persistent_Dynamic_LazySegtree(int n_);
template<class X> Persistent_Dynamic_LazySegtree(int n_, X default_prod_);
NP instance();
NP set(NP p, int i, S x);
NP apply(NP p, int l, int r, F f);
NP copy(NP& from, NP to, int l, int r);
NP copy_with_apply(NP& from, NP to, int l, int r, F f);
S get(NP& p, int i);
S prod(NP& p, int l, int r);
S all_prod(NP p);
```

## 計算量

操作 O(log U)、通常の更新で O(log U) ノード追加。読み取りも遅延伝播でノードを作る場合がある。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/persistent_range_affine_range_sum`](../../../verify/library_checker/data_structure/persistent_range_affine_range_sum.cpp)
