# segtree_dual

[索引](../README.md) / [実装](../../../library/data_structure/segtree_dual.hpp)

## 機能・前提条件

区間への作用と一点取得を行う双対セグメント木。mapping(f,x) と composition(f,g)=f∘g、恒等作用 id を渡す。

## インターフェース

```cpp
#include "library/data_structure/segtree_dual.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, class F, auto mapping, auto composition, auto id>
struct Segtree_dual;
Segtree_dual();
Segtree_dual(int n_);
Segtree_dual(int n_, S x);
Segtree_dual(const vector<S>& v);
void set(int i, S a);
S get(int i);
void apply(int l, int r, F f);
void all_apply(F f);
```

## 計算量

構築 `O(N)`、`set`・`get`・区間 `apply` は最悪 `O(log(N+1))`、`all_apply` は `O(1)`。内部ノードの保留フラグを含めて空間 `O(N)`。

## 依存関係

- [segtree_ops.hpp](internal/segtree_ops.md)
- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/linear_segment_trees`](../../../verify/unit/linear_segment_trees.cpp)
- [`library_checker/range_affine_point_get_dual_segtree`](../../../verify/library_checker/data_structure/range_affine_point_get_dual_segtree.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
