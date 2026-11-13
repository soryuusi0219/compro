# static_top_tree

[索引](../README.md) / [実装](../../../library/tree/static_top_tree.hpp)

## 機能・前提条件

静的木をrake/compressで分解し一点変更に伴う木DPを更新する。演算が各clusterの意味に適合すること。

## インターフェース

```cpp
#include "library/tree/static_top_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto rake, auto compress>
struct StaticTopTree;
StaticTopTree();
StaticTopTree(int n_);
StaticTopTree(const vector<S>& v_);
void add_edge(int u, int v);
void build(int root);
inline int parent(int v) const;
void set_data(const vector<S>& v);
void set(int v, S x);
inline S get() const;
```

## 計算量

構築 O(N log N)、更新 O(log N) を想定する平衡化、全体取得 O(1)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/point_set_tree_path_composite_sum_fixed_root`](../../../verify/library_checker/tree/point_set_tree_path_composite_sum_fixed_root.cpp)
