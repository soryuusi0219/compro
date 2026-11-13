# graph_general

[索引](../README.md) / [実装](../../../library/graph/graph_general.hpp)

## 機能・前提条件

木のパス、直径、重心などの補助関数。隣接配列とstatic_graphのオーバーロードを持つ。木を前提に親だけ除外する処理がある。

## インターフェース

```cpp
#include "library/graph/graph_general.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class INT>
vector<INT> get_path(const vector<vector<INT>>& G, int u, int v);
template<class T>
vector<int> get_path(const static_graph<T>& G, int u, int v);
template<class T, class INT>
vector<INT> get_path(const vector<vector<pair<INT, T>>>& G, int u, int v);
template<class INT>
tuple<int, int, int> diameter(const vector<vector<INT>>& G);
inline tuple<int, int, int> diameter(const static_graph<>& G);
template<class T, class INT>
tuple<int, int, T> diameter(const vector<vector<pair<INT, T>>>& G);
template<class T> requires (!is_void_v<T>);
tuple<int, int, T> diameter(const static_graph<T>& G);
template<class INT>
pair<int, int> tree_get_centroids(const vector<vector<INT>>& G);
template<class T>
pair<int, int> tree_get_centroids(const static_graph<T>& G);
template<class T, class INT>
pair<int, int> tree_get_centroids(const vector<vector<pair<INT, T>>>& G);
```

## 計算量

各走査 O(V+E)、木なら O(V)。復元・再帰・補助配列に O(V) 空間。

## 依存関係

- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`library_checker/tree_diameter_graph_general`](../../../verify/library_checker/tree/tree_diameter_graph_general.cpp)
- [`unit/graph_general`](../../../verify/unit/graph_general.cpp)
