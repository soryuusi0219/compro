# extreme_vertex_set

[索引](../README.md) / [実装](../../../library/graph/extreme_vertex_set.hpp)

## 機能・前提条件

非負整数容量付き無向グラフで、`d(X)` を集合 X の境界を横切る辺の容量和とします。Extreme Vertex Set は、任意の非空真部分集合 Y について `d(Y) > d(X)` が成立する非空集合 X です。互いに素または包含関係にあるので、全ての集合を包含木で返せます。

最大隣接順序に基づく pendant pair は大域最小カットに用います。ここでは最小次数順序の末尾の flat pair を繰り返し縮約し、全ての極点集合を含む候補木を作ります。各候補をその非空真部分集合に当たる候補の最小カット容量と比較し、同値を含めて除外します。

頂点は 0-indexed です。平行辺を許し、自己ループと容量 0 の辺は無視します。全辺の容量和が `Cap` に収まることを要求します。頂点数と辺数は `INT_MAX/2` 以下です。

## インターフェース

```cpp
template <class Cap> struct ExtremeVertexSetTree {
  std::vector<int> parent;
  std::vector<Cap> cut;
  int root;
  bool artificial_root;
};

template <class Cap>
ExtremeVertexSetTree<Cap> extreme_vertex_set(int n, const std::vector<UndirectedCutEdge<Cap>>& edges);
```

節点 `0 .. n-1` は元の頂点に対応する葉です。節点に対応する集合はその配下の葉全体で、`cut[v]` が元グラフでの境界容量です。親番号は必ず子番号より大きく、根の親は `-1` です。除外された候補を飛ばして最近接の包含先を親にします。

正容量辺でグラフが連結なら、根は全頂点集合であり、極点集合です。非連結の場合も `cut[root] == 0` の全頂点集合を人工根として返し、`artificial_root` を真にします。頂点数 0 のときは配列が空で、`root == -1`、`artificial_root == false` です。

## 計算量

正容量辺で分けた各連結成分が K 頂点・L 辺なら、二分ヒープによる候補生成は O(K(K+L) log K) 時間です。連結成分の計算は O((N+M) α(N)) 時間、候補判定と包含木の復元は O(N) 時間、全体の空間は O(N+M) です。包含木は最大 2N−1 節点です。全ての対応頂点集合を明示的に列挙すると、出力が二次の大きさになることがあります。

## 依存関係

- [internal/cut_contraction.hpp](internal/cut_contraction.md)
- [undirected_cut_edge.hpp](undirected_cut_edge.md)
- [dsu.hpp](../data_structure/dsu.md)
- [Hiroshi Nagamochi, Computing a Minimum Cut in a Graph with Dynamic Edges Incident to a Designated Vertex](https://doi.org/10.1093/ietisy/e90-d.2.428)

## Verify・利用例

- [全部分集合を調べ、包含木・厳密不等号・カット容量を照合](../../../verify/unit/undirected_cuts.cpp)
- [Dynamic Star の公式 verify](../../../verify/library_checker/graph/global_minimum_cut_of_dynamic_star_augmented_graph.cpp)
