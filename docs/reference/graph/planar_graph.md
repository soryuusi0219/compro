# planar_graph

[索引](../README.md) / [実装](../../../library/graph/planar_graph.hpp)

## 機能・前提条件

交差のない直線埋め込みから面・面積を構築する。座標をsetし辺を登録後buildする。型名は planer_graph。

## インターフェース

```cpp
#include "library/graph/planar_graph.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct planer_graph;
planer_graph(int n);
planer_graph(vector<pair<T,T>> v);
void set(int i, pair<T,T> pt);
void add_edge(int i, int j);
int build();
int detect_domain(pair<T,T> pt);
T get_area(int id);
vector<T> get_areas();
```

## 計算量

隣接辺の角度ソートを含むbuild O(E log E+V)、点の面検出は最悪 O(E)、空間 O(V+E)。

## 依存関係

- [dsu.hpp](../data_structure/dsu.md)

## Verify・利用例

- [`unit/graph_misc`](../../../verify/unit/graph_misc.cpp)
