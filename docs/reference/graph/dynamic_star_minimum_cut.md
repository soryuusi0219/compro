# dynamic_star_minimum_cut

[索引](../README.md) / [実装](../../../library/graph/dynamic_star_minimum_cut.hpp)

## 機能・前提条件

固定した無向グラフに新頂点を 1 個加え、元の頂点 v と新頂点の間に容量 `a[v]` の辺を置きます。`a[v]` の一点変更とグラフ全体の大域最小カット値の取得ができます。

元のグラフの全頂点集合を含む非空集合 X に対して、最小化する値は `d(X) + sum(a[v] for v in X)` です。極点集合でない X の中には境界容量が X 以下の真部分集合があり、星辺の容量が非負なので、その部分集合に移っても値は増えません。このため、極点集合の全候補を含む二分の縮約木で答えを維持できます。

元の頂点は 1 個以上必要です。全ての辺は非負整数容量です。元のグラフの容量和と現時点の星辺の容量和の合計が `Cap` で表現できることを要求します。頂点数は `INT_MAX/4` 以下、元の辺数は `INT_MAX/2` 以下です。

## インターフェース

```cpp
template <class Cap = long long> class DynamicStarMinimumCut;
DynamicStarMinimumCut(const std::vector<Cap>& weights, const std::vector<UndirectedCutEdge<Cap>>& edges);
int size() const;
Cap set(int v, Cap weight);
Cap minimum_cut() const;
```

`weights[v]` が星辺の初期容量です。`set(v, weight)` は星辺 1 本を指定容量で置き換え、更新後の大域最小カット値を返します。`minimum_cut()` は現在の値を返します。

## 計算量

候補木の構築は [Extreme Vertex Sets](extreme_vertex_set.md#計算量) の候補生成と同じです。静的トップ木への変換は O(N log N) 時間・O(N) 空間です。更新は O(log N) 時間、取得は O(1) 時間です。構築中の作業空間は O(N+M)、構築後の空間は O(N) です。

## 依存関係

- [extreme_vertex_set.hpp](extreme_vertex_set.md)
- [undirected_cut_edge.hpp](undirected_cut_edge.md)
- [static_top_tree.hpp](../tree/static_top_tree.md)

## Verify・利用例

```cpp
std::vector<long long> weights = {1, 3};
std::vector<soryuusi::UndirectedCutEdge<long long>> edges = {{0, 1, 4}};
soryuusi::DynamicStarMinimumCut graph(weights, edges);
long long answer = graph.set(0, 2);
```

- [Library Checker / Global Minimum Cut of Dynamic Star Augmented Graph](../../../verify/library_checker/graph/global_minimum_cut_of_dynamic_star_augmented_graph.cpp)
- [一点変更のたびに全部分集合の最小値と照合](../../../verify/unit/undirected_cuts.cpp)
