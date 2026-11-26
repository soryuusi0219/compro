# gomory_hu_tree

[索引](../README.md) / [実装](../../../library/graph/gomory_hu_tree.hpp)

## 機能・前提条件

非負整数容量付き無向グラフの Gomory–Hu 木を構築します。異なる 2 頂点間の最小カット値は木のパス上の最小辺容量です。各木辺を取り除いたときの分割は、その木辺の両端点を分離する元グラフ上の最小カットです。従って、任意の点対のパスで最小の辺を 1 本選ぶと、その点対の最小カットも復元できます。

頂点は 0-indexed です。正容量辺の連結成分の間は容量 0 の木辺で結びます。平行辺は足し合わせて扱い、自己ループは無視します。全辺の容量和が `Cap` で表現できることを要求します。必要なら `__int128_t` を使用できます。入力辺数は `INT_MAX/4` 以下です。

## インターフェース

```cpp
enum class GomoryHuFlow { Dinic, PushRelabel };

template <class Cap = long long> class GomoryHuTree;
GomoryHuTree(int n, const std::vector<UndirectedCutEdge<Cap>>& edges, GomoryHuFlow method = GomoryHuFlow::Dinic);
int size() const;
int parent(int v) const;
Cap capacity(int child) const;
int min_cut_edge(int u, int v) const;
Cap min_cut_value(int u, int v) const;
std::vector<bool> cut_side(int child) const;
```

頂点 0 を根とし、`parent(0)` は `-1` です。頂点数 0 の場合は空の木です。`child` は根以外の頂点で、その頂点と親を結ぶ辺を指定します。`cut_side(child)` の `true` 側は `child` の部分木です。`min_cut_edge` は異なる頂点 `u, v` を分離する木辺の子頂点番号を返します。同値の場合はいずれか 1 辺を選びます。

## 計算量

正容量辺での連結成分ごとに構築します。木の連結成分には既存の辺を使用します。それ以外は、頂点数 K、辺数 L の連結成分について K−1 回の最大流を行い、同じ CSR をリセットして再利用します。最大流 1 回の最悪時間を F(K,L) とすると、成分ごとに O(K(F(K,L)+K+L)) 時間です。全体の前処理は O((N+M) α(N)) 時間、空間は O(N+M) です。Dinic は一般容量で F(K,L)=O(K²L) です。

`parent`, `capacity` は O(1)、`min_cut_edge`, `min_cut_value` は木の 2 頂点間のパス長に比例し、`cut_side` は O(N) です。一括で点対最小値を求めるときは、親と容量から作った木にパス最小値用のデータ構造を載せられます。

## 依存関係

- [max_flow.hpp](max_flow.md)
- [undirected_cut_edge.hpp](undirected_cut_edge.md)
- [dsu.hpp](../data_structure/dsu.md)
- [Dan Gusfield, Very Simple Methods for All Pairs Network Flow Analysis](https://doi.org/10.1137/0219009)

## Verify・利用例

```cpp
std::vector<soryuusi::UndirectedCutEdge<long long>> edges = {{0, 1, 3}, {1, 2, 7}};
soryuusi::GomoryHuTree tree(3, edges);
auto side = tree.cut_side(tree.min_cut_edge(0, 2));
```

- [全点対の値と各木辺の分割を全探索・最大流と照合](../../../verify/unit/undirected_cuts.cpp)
