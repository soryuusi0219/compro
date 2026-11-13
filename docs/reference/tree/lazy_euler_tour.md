# lazy_euler_tour

[索引](../README.md) / [実装](../../../library/tree/lazy_euler_tour.hpp)

## 機能・前提条件

Euler tour上の部分木作用と集約、LCA。mappingが部分木の集約へ作用するようSを設計する。

## インターフェース

```cpp
#include "library/tree/lazy_euler_tour.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct lazy_EularTour;
struct StaticRMQ;
StaticRMQ();
StaticRMQ(vector<pair<int,int>> v);
pair<int,int> naive_prod(int l, int r);
pair<int,int> prod(int l, int r);
lazy_EularTour(int n);
lazy_EularTour(const vector<S>& a);
void add_edge(int u, int v);
void build(int root = 0);
int get_depth(int u);
int lca(int u, int v);
void set(int v, S x);
void apply(int v, F f, bool contains_root = true);
S get(int v);
S prod(int v, bool contains_root = true);
```

## 計算量

構築 O(N log N)、一点・部分木操作 O(log N)、LCA O(1)、空間 O(N log N) を上界とする。

## 依存関係

- [lazy_segtree.hpp](../data_structure/lazy_segtree.md)
- [static_graph.hpp](../graph/static_graph.md)

## Verify・利用例

- [`unit/recursive_tree`](../../../verify/unit/recursive_tree.cpp)
