# steiner_tree

[索引](../README.md) / [実装](../../../library/tree/steiner_tree.hpp)

## 機能・前提条件

少数のterminalを結ぶ最小Steiner木。重み非負グラフに部分集合DP＋Dijkstraを使い辺を復元する。

## インターフェース

```cpp
#include "library/tree/steiner_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, T e = 0, T infty = 1ll << 60, class INT = int>
struct steiner_tree;
steiner_tree(int n_);
steiner_tree(const vector<vector<pair<INT,T>>>& G0);
void add_edge(int u, int v, T cost, bool undirected = true);
pair<T,vector<tuple<INT,INT,T>>> solve(const vector<INT>& terminals);
```

## 計算量

terminal数Kに O(3^K V+2^K(V+E) log V) 時間、O(2^K V+E) 空間。

## 依存関係

- [static_graph.hpp](../graph/static_graph.md)

## Verify・利用例

- [`library_checker/minimum_steiner_tree`](../../../verify/library_checker/graph/minimum_steiner_tree.cpp)
