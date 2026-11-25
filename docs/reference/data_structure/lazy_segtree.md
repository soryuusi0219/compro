# lazy_segtree

[索引](../README.md) / [実装](../../../library/data_structure/lazy_segtree.hpp)

## 機能・前提条件

モノイドと作用を受け取る遅延セグメント木。区間は [l,r)。mapping と composition の法則を満たすこと。

`mapping(f,op(x,y))=op(mapping(f,x),mapping(f,y))`、`mapping(composition(f,g),x)=mapping(f,mapping(g,x))` を満たす必要があります。区間和への加算ではSに区間長を含めます。

`LazySegtree(n)` は各要素を `e()` に初期化する。内部の葉数は `max(1,n)` 以上の最小の2冪。`all_apply(f)` は根に作用させるので、余った葉を単位元に保つために `mapping(f,e()) == e()` を満たす作用を使う。境界探索の述語は単調で、`e()` に対して真を返す必要がある。

## インターフェース

```cpp
#include "library/data_structure/lazy_segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct LazySegtree;
LazySegtree(int n_ = 0);
LazySegtree(const vector<S>& v);
void set(int p, S x);
void reset(int i);
S get(int p);
S prod(int l, int r);
S all_prod();
void apply(int p, F f);
void apply(int l, int r, F f);
void all_apply(F f);
int max_right(int l, auto g);
int min_left(int r, auto g);
```

## 計算量

構築 `O(N)`、一点・区間・境界探索は最悪 `O(log(N+1))`、`all_prod` と `all_apply` は `O(1)`。空間は `O(N)`。

## 依存関係

- [segtree_ops.hpp](internal/segtree_ops.md)
- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/linear_segment_trees`](../../../verify/unit/linear_segment_trees.cpp)
- [`library_checker/range_add_range_min_lazy_segtree`](../../../verify/library_checker/data_structure/range_add_range_min_lazy_segtree.cpp)
- [`library_checker/range_set_range_composite_lazy_segtree`](../../../verify/library_checker/data_structure/range_set_range_composite_lazy_segtree.cpp)
