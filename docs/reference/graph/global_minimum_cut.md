# global_minimum_cut

[索引](../README.md) / [実装](../../../library/graph/global_minimum_cut.hpp)

## 機能・前提条件

非負整数容量付き無向グラフの大域最小カットを Stoer–Wagner 法で求めます。平行辺はそれぞれ容量を数え、自己ループは無視します。正容量辺だけで非連結のときは、頂点 0 を含む成分を片側とする容量 0 のカットを返します。

全辺の容量和が `Cap` で表現できることを要求します。必要なら `__int128_t` を使用できます。頂点数と辺数は `INT_MAX/2` 以下を想定します。

## インターフェース

```cpp
enum class GlobalMinCutMethod { Auto, Dense, Sparse };

template <class Cap> struct GlobalMinCutResult {
  Cap value;
  std::vector<bool> side;
};

template <class Cap>
std::optional<GlobalMinCutResult<Cap>> global_minimum_cut(int n, const std::vector<UndirectedCutEdge<Cap>>& edges, GlobalMinCutMethod method = GlobalMinCutMethod::Auto);
```

頂点数が 2 未満なら `std::nullopt` です。それ以外では `side` の両側は非空で、異なる側にある端点を結ぶ辺の容量和が `value` です。等しい値のカットが複数ある場合は、そのうち 1 つを返します。

## 計算量

頂点数を N、入力辺数を M とすると、`Dense` は O(N³+M) 時間・O(N²) 追加空間、`Sparse` は O(N(N+M) log N) 時間・O(N+M) 追加空間です。`Sparse` は縮約と二分ヒープを使い、各ラウンドで存在する隣接辺だけを走査します。正容量辺が非連結なら、方式にかかわらず O((N+M) α(N)) 時間・O(N) 追加空間です。

`Auto` は頂点数 1024 以下かつ正容量辺でつながる頂点対の数が概ね N²/20 以上なら密版を選びます。`Sparse` を明示すると追加空間を O(N+M) に制限できます。

## 依存関係

- [undirected_cut_edge.hpp](undirected_cut_edge.md)
- [internal/cut_contraction.hpp](internal/cut_contraction.md)
- [dsu.hpp](../data_structure/dsu.md)
- [Mechthild Stoer and Frank Wagner, A Simple Min-Cut Algorithm](https://doi.org/10.1145/263867.263872)

## Verify・利用例

```cpp
std::vector<soryuusi::UndirectedCutEdge<long long>> edges = {{0, 1, 4}, {1, 2, 2}};
auto result = soryuusi::global_minimum_cut(3, edges);
```

- [密版・疎版の値と分割を全部分集合と照合](../../../verify/unit/undirected_cuts.cpp)
