# lazy_kd_tree

[索引](../README.md) / [実装](../../../library/data_structure/lazy_kd_tree.hpp)

## 機能・前提条件

固定点集合に長方形作用と集約を行うKD木。mapping は集約に作用し composition は f∘g。

長方形は半開区間。`set` と `get` は根の作用も含めて葉への経路を伝播する。`all_apply(f)` は全点へ作用させた後の集約を返す。空の木の集約は `e()`。数値型の作用は恒等作用との比較で空タグを判定し、それ以外の型は保留フラグを使う。

ノード間の接続は配列添字で管理します。オブジェクトのコピーは独立した配列を持ち、ノードポインタの共有はありません。

## インターフェース

```cpp
#include "library/data_structure/lazy_kd_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class X, class Y, class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct LazyKDTree;
LazyKDTree();
LazyKDTree(int n_);
LazyKDTree(const vector<Point>& pts_);
int add_point(X x, Y y);
void build();
void build(const vector<S>& v);
void set(int i, S x);
S get(int i);
S prod(X xmn, X xmx, Y ymn, Y ymx);
void apply(X xmn, X xmx, Y ymn, Y ymx, F f);
S all_prod();
S all_apply(F f);
```

## 計算量

作用・結合を `O(1)` とすると、構築は平均 `O(N log(N+1))`、一点操作は `O(log(N+1))`、`all_prod` と `all_apply` は `O(1)`。長方形操作は訪問したノード数に比例し、最悪 `O(N)`。空間 `O(N)`。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)
- [kd_tree_builder.hpp](internal/kd_tree_builder.md)
- [segtree_ops.hpp](internal/segtree_ops.md)

## Verify・利用例

- [`unit/rectangle_queries`](../../../verify/unit/rectangle_queries.cpp)
- [`library_checker/dynamic_point_set_rectangle_affine_rectangle_sum`](../../../verify/library_checker/data_structure/dynamic_point_set_rectangle_affine_rectangle_sum.cpp)
- [`library_checker/rectangle_add_point_get_lazy_kd_tree`](../../../verify/library_checker/data_structure/rectangle_add_point_get_lazy_kd_tree.cpp)
