# dag_path_cover

[索引](../README.md) / [実装](../../../library/graph/dag_path_cover.hpp)

## 機能・前提条件

DAG の全頂点を頂点素な有向パスで被覆し、パス本数を最小化する。パスは登録した辺を通る。頂点は `[0,n)`、頂点 1 個のパスも許す。

## インターフェース

```cpp
#include "library/graph/dag_path_cover.hpp"

std::vector<std::pair<int,int>> edges{{0,1},{1,2}};
auto cover = soryuusi::dag_path_cover(4,edges);
```

`dag_path_cover(n,edges)` は各パスを頂点列で返す。 `edges` は `span<const pair<int,int>>`。同値解のうち一つを返す。

## 計算量

各頂点の出側・入側を二部グラフの左右に置く。最大マッチングを k とするとパス本数は n−k。時間 O((n+E)√n)、空間 O(n+E)。復元は O(n)。

## 依存関係

- [bipartite_matching.hpp](bipartite_matching.md)

## Verify・利用例

- [`unit/bipartite_matching`](../../../verify/unit/bipartite_matching.cpp) はパスの辺、重複頂点、パス本数を検査する。
