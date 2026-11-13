# biconnected_graph

[索引](../README.md) / [実装](../../../library/graph/biconnected_graph.hpp)

## 機能・前提条件

無向グラフのlow-link、橋、二辺・二頂点連結成分、block-cut tree。構築用APIと成分の返却順は宣言・verifyを参照。

## インターフェース

```cpp
#include "library/graph/biconnected_graph.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct biconnected_graph;
biconnected_graph();
biconnected_graph(int n_);
biconnected_graph(int n_, int m);
void add_edge(int u, int v);
void low_link();
bool is_bridge(int u, int v);
vector<int> two_edge_connected_component(int v);
vector<vector<int>> two_edge_connected_components();
tuple<vector<vector<int>>,vector<int>,vector<vector<int>>> two_edge_connected_compressed();
vector<vector<int>> two_vertex_connected_components();
pair<vector<vector<int>>,vector<int>> block_cut_tree();
```

## 計算量

基本分解 O(V+E)。成分圧縮時のソート・重複除去を含む処理は O((V+E) log(V+E)) を上界とする。

## 依存関係

- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`library_checker/biconnected_components`](../../../verify/library_checker/graph/biconnected_components.cpp)
- [`library_checker/biconnected_components_block_cut_tree`](../../../verify/library_checker/graph/biconnected_components_block_cut_tree.cpp)
- [`library_checker/two_edge_connected_components`](../../../verify/library_checker/graph/two_edge_connected_components.cpp)
- [`library_checker/two_edge_connected_components_compressed`](../../../verify/library_checker/graph/two_edge_connected_components_compressed.cpp)
