# dijkstra_01

[索引](../README.md) / [実装](../../../library/graph/dijkstra_01.hpp)

## 機能・前提条件

辺重みが0または1の最短路。distanceの後にpathで経路復元。

## インターフェース

```cpp
#include "library/graph/dijkstra_01.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Dijkstra01;
Dijkstra01(int n_);
Dijkstra01(int n_, int m);
void add_edge(int u, int v, bool cost, bool undirected = false);
vector<int> distance(int v);
vector<int> path(int v);
```

## 計算量

O(V+E) 時間・空間。

## 依存関係

- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`unit/csr_graph`](../../../verify/unit/csr_graph.cpp)
