# euler_tour

[索引](../README.md) / [実装](../../../library/tree/euler_tour.hpp)

## 機能・前提条件

Euler tour順の部分木集約とLCA。型名は EularTour。build(root)後にset/get/prodを行う。

## インターフェース

```cpp
#include "library/tree/euler_tour.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct EularTour;
struct StaticRMQ;
StaticRMQ();
StaticRMQ(vector<pair<int,int>> v);
pair<int,int> naive_prod(int l, int r);
pair<int,int> prod(int l, int r);
EularTour(int n);
EularTour(const vector<S>& a);
void add_edge(int u, int v);
void build(int root = 0);
int get_depth(int u);
int lca(int u, int v);
int dist(int u, int v);
void set(int v, S x);
S get(int v);
S prod(int v, bool contains_root = true);
```

## 計算量

構築 O(N log N) を上界とし、LCA O(1)、一点更新・部分木積 O(log N)、空間 O(N log N) を上界とする。

## 依存関係

- [segtree.hpp](../data_structure/segtree.md)
- [static_graph.hpp](../graph/static_graph.md)

## Verify・利用例

- [`library_checker/vertex_add_subtree_sum_euler_tour`](../../../verify/library_checker/tree/vertex_add_subtree_sum_euler_tour.cpp)
- [`unit/recursive_tree`](../../../verify/unit/recursive_tree.cpp)
