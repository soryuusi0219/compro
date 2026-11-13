# dynamic_lazy_segtree

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_lazy_segtree.hpp)

## 機能・前提条件

巨大な整数添字区間の動的遅延セグメント木。default_prod(l,r) で未生成区間の集約を指定できる。get は現在 get(i,x) という2引数API。

## インターフェース

```cpp
#include "library/data_structure/dynamic_lazy_segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct Dynamic_LazySegtree;
Dynamic_LazySegtree(int n_);
template<class X> Dynamic_LazySegtree(int n_, X default_prod_);
void set(int i, S x);
S get(int i, S x);
S prod(int l, int r);
S all_prod();
void apply(int l, int r, F f);
```

## 計算量

更新・問い合わせ O(log U)、Q 操作で最大 O(Q log U) ノード。U は添字領域幅。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/range_affine_range_sum_large_array`](../../../verify/library_checker/data_structure/range_affine_range_sum_large_array.cpp)
