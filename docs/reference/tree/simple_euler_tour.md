# simple_euler_tour

[索引](../README.md) / [実装](../../../library/tree/simple_euler_tour.hpp)

## 機能・前提条件

Euler tourとRMQによるLCA・距離。型名は SimpleEularTour。無向木を登録しbuild(root)を呼ぶ。

## インターフェース

```cpp
#include "library/tree/simple_euler_tour.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct SimpleEularTour;
struct StaticRMQ;
StaticRMQ();
StaticRMQ(vector<pair<int,int>> v);
pair<int,int> naive_prod(int l, int r) const;
pair<int,int> prod(int l, int r) const;
SimpleEularTour(int n);
void add_edge(int u, int v);
void build(int root = 0);
inline int get_depth(int u) const;
inline int lca(int u, int v) const;
inline int dist(int u, int v) const;
```

## 計算量

前処理 O(N log N) 時間・空間を上界とし、LCA/距離 O(1)。

## 依存関係

- [static_graph.hpp](../graph/static_graph.md)

## Verify・利用例

- [`library_checker/lca_simple_euler_tour`](../../../verify/library_checker/tree/lca_simple_euler_tour.cpp)
- [`unit/recursive_tree`](../../../verify/unit/recursive_tree.cpp)
