# lazy_kd_tree

[索引](../README.md) / [実装](../../../library/data_structure/lazy_kd_tree.hpp)

## 機能・前提条件

固定点集合に長方形作用と集約を行うKD木。mapping は集約に作用し composition は f∘g。

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

構築 期待 O(N log N)、一点操作 O(log N)、長方形操作 O(√N)、空間 O(N)。作用・結合を O(1) とする。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/dynamic_point_set_rectangle_affine_rectangle_sum`](../../../verify/library_checker/data_structure/dynamic_point_set_rectangle_affine_rectangle_sum.cpp)
- [`library_checker/rectangle_add_point_get_lazy_kd_tree`](../../../verify/library_checker/data_structure/rectangle_add_point_get_lazy_kd_tree.cpp)
