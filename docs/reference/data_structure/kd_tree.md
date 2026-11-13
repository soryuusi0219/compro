# kd_tree

[索引](../README.md) / [実装](../../../library/data_structure/kd_tree.hpp)

## 機能・前提条件

固定した二次元点集合の一点更新・長方形集約。add_point → build の順に構築する。空間内の集約順に依存しない可換演算を用いる。

## インターフェース

```cpp
#include "library/data_structure/kd_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class X, class Y, class S, auto op, auto e>
struct KDTree;
KDTree();
KDTree(int n_);
KDTree(const vector<Point>& pts_);
int add_point(X x, Y y, S v = e());
void build();
void build(const vector<S>& v);
void set(int i, S x);
S get(int i) const;
S prod(X xmn, X xmx, Y ymn, Y ymx);
```

## 計算量

構築 期待 O(N log N)、一点更新 O(log N)、平衡な二次元木の長方形問い合わせ O(√N)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/point_add_rectangle_sum_kd_tree`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_kd_tree.cpp)
