# complement_graph_connected_components

[索引](../README.md) / [実装](../../../library/graph/complement_graph_connected_components.hpp)

## 機能・前提条件

与えた単純無向グラフの補グラフの連結成分を求める。元グラフの辺だけ登録する。

## インターフェース

```cpp
#include "library/graph/complement_graph_connected_components.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct ComplementConnectedComponents;
ComplementConnectedComponents(int n_);
ComplementConnectedComponents(int n_, int m);
void add_edge(int u, int v);
vector<vector<int>> solve();
```

## 計算量

O(V+E) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/connected_components_of_complement_graph`](../../../verify/library_checker/graph/connected_components_of_complement_graph.cpp)
