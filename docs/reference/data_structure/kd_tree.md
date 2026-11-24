# kd_tree

[索引](../README.md) / [実装](../../../library/data_structure/kd_tree.hpp)

## 機能・前提条件

固定した二次元点集合の一点更新・長方形集約。add_point → build の順に構築する。空間内の集約順に依存しない可換演算を用いる。

`add_point(x,y,v)` は初期値と点を登録し、その点の0始まりの添字を返す。同じ座標も別々の点として扱う。`build()` は登録時の初期値を使い、`build(v)` は登録順の値を明示指定する。空の木や空の長方形に対する `prod` は `e()`。

ノード間の接続は配列添字で管理します。オブジェクトのコピーは独立した配列を持ち、ノードポインタの共有はありません。

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

構築は平均 `O(N log(N+1))`。登録順でXが昇順かつYが単調な場合は `O(N)`。一点更新は `O(log(N+1))`、長方形問い合わせは訪問したノード数に比例して最悪 `O(N)`。空間は `O(N)`。

## 依存関係

- [kd_tree_builder.hpp](internal/kd_tree_builder.md)

## Verify・利用例

- [`unit/rectangle_queries`](../../../verify/unit/rectangle_queries.cpp)
- [`library_checker/point_add_rectangle_sum_kd_tree`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_kd_tree.cpp)
