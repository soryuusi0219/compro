# scc

[索引](../README.md) / [実装](../../../library/graph/scc.hpp)

## 機能・前提条件

有向グラフの強連結成分分解と縮約DAG。scc_idsは成分数と各頂点ID。

## インターフェース

```cpp
#include "library/graph/scc.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct SCC;
SCC();
SCC(int n_);
SCC(int n_, int m);
void add_edge(int u, int v);
pair<int,vector<int>> scc_ids();
vector<vector<int>> scc();
tuple<int, vector<vector<int>>, vector<int>> scc_compressed(bool allow_self_edge = false, bool allow_double_edge = false);
```

## 計算量

分解 O(V+E)。縮約の重複除去はソート分を含め O(E log E) を上界とする。空間 O(V+E)。

## 依存関係

- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`library_checker/scc`](../../../verify/library_checker/graph/scc.cpp)
