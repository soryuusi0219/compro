# bipartite_weighted_matching

[索引](../README.md) / [実装](../../../library/graph/bipartite_weighted_matching.hpp)

## 機能・前提条件

辺重みの総和を最大にする二部マッチング・整数 b-matching を求める。各頂点と各辺に非負の整数容量を指定できる。頂点・辺容量は `int`、重みは 64 bit 以下の符号付き整数型。負重み・平行辺を許す。左右の頂点はそれぞれ 0 始まり。

二部グラフ上の重み付き最小頂点被覆と最大独立集合にも対応する。頂点重みは負でもよい。

## インターフェース

```cpp
#include "library/graph/bipartite_weighted_matching.hpp"

soryuusi::BipartiteWeightedMatching<> matching(2,2);
int id = matching.add_edge(0,1,7);
auto best = matching.solve(); // best->weight == 7, best->flow[id] == 1
```

- `add_edge(left,right,weight[,capacity])` は辺 ID を返す。容量省略時は 1。
- `solve(left_capacity={},right_capacity={},required=-1) const` は頂点容量を `span<const int>` で受け取る。各側の空の span は全頂点容量 1。 `required >= 0` は総使用量をその値に固定し、不可能なら `nullopt`。既定値 −1 では総使用量を自由に選び、重みを最大にする。
- `Result{weight,size,flow}` の `weight` は `__int128_t` の最大重み、`size` は総使用量、`flow[id]` は各辺の整数使用量。モデルを保持して繰り返し解ける。
- `minimum_weight_bipartite_vertex_cover(graph,left_weight,right_weight)` と `maximum_weight_bipartite_independent_set(graph,left_weight,right_weight)` は `BipartiteMatching` の辺と頂点重みのベクタを読み、`WeightedBipartiteVertexSet{weight,vertices}` を返す。 `vertices.left` と `vertices.right` は頂点の採否。 `graph.solve()` は呼び出し不要。

## 計算量

V=L+R、E を辺数とする。b-matching は O(V+E) で循環流に変換し、[最小費用 b-flow](min_cost_b_flow.md) を解く。負の流量範囲を持つ逆向き辺で重みを表し、符号付き重みの最小値も扱う。頂点被覆は O(V+E) で [Project Selection](project_selection.md) に変換し、最大流を解く。最大独立集合の復元は頂点被覆の補集合を取る O(V)。還元グラフの頂点数と逆辺込みの辺数は `int` に収める。

## 依存関係

- [bipartite_matching.hpp](bipartite_matching.md)
- [min_cost_b_flow.hpp](min_cost_b_flow.md)
- [project_selection.hpp](project_selection.md)

## Verify・利用例

- [`unit/bipartite_weighted_matching`](../../../verify/unit/bipartite_weighted_matching.cpp) は b-matching の全流量と重み付き頂点集合を全列挙する。
