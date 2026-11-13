# dijkstra_1

[索引](../README.md) / [実装](../../../library/graph/dijkstra_1.hpp)

## 機能・前提条件

全辺重み1の幅優先探索。distanceと経路復元を提供する。

## インターフェース

```cpp
#include "library/graph/dijkstra_1.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Dijkstra1;
Dijkstra1(int n_);
Dijkstra1(int n_, int m);
void add_edge(int u, int v, bool undirected = false);
vector<int> distance(int v);
vector<int> path(int v);
```

## 計算量

O(V+E) 時間・空間。

## 依存関係

- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`aoj/bfs_dijkstra_1`](../../../verify/aoj/graph/bfs_dijkstra_1.cpp)
- [`unit/csr_graph`](../../../verify/unit/csr_graph.cpp)
