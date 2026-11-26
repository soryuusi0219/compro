# general_weighted_matching

[索引](../README.md) / [実装](../../../library/graph/general_weighted_matching.hpp)

## 機能・前提条件

非二部グラフも含む無向グラフの重み付きマッチングを、Edmonds の主双対 blossom 法で求めます。重み型は `bool` を除く 64 bit 以下の整数で、負の値も扱えます。元の重みの総和は `__int128_t` で返します。

目的は次の三つです。

| `WeightedMatchingObjective` | 目的 |
| --- | --- |
| `Weight` | 重みの総和を最大化します。空集合を許し、負とゼロの辺は選びません。同じ総和での辺数には追加の条件を課しません。 |
| `CardinalityThenWeight` | 辺数を最大化し、その辺数の中で総和を最大化します。重みが負またはゼロの辺も使えます。 |
| `Perfect` | すべての頂点を被覆する解の中で総和を最大化します。存在しない場合は `std::nullopt` です。 |

頂点と辺 ID は 0 始まりです。自己ループにも ID は付きますが、解には入りません。選んだ端点間に平行辺があれば最大重みを使い、同じ最大重みの辺からは先に追加した ID を返します。N は `(INT_MAX-1)/2` 以下、M は `INT_MAX/2` 以下です。

## インターフェース

```cpp
#include "library/graph/general_weighted_matching.hpp"

soryuusi::GeneralWeightedMatching<long long> graph(n);
graph.reserve_edges(m);
int id = graph.add_edge(u,v,w);
auto answer = graph.solve(soryuusi::WeightedMatchingObjective::CardinalityThenWeight);
```

`solve()` の既定は `Weight` です。成功時は `std::optional<Result>` に `Result{cardinality,weight}` を格納します。頂点数 0 の完全マッチングは重み 0 で存在します。完全マッチングの探索に失敗した場合は公開する対応も空になります。

`size()`、`edge_count()`、`partner(v)`、`matched_edge(v)`、`matching_edges()` は [GeneralMatching](general_matching.md) と同じ規約です。`get_edge(id)` は `Edge{u,v,weight}` を返します。辺追加で最大性を無効化し、異なる目的での `solve()` でも解を再計算します。

## 計算量

使用可能な辺で分けた連結成分がそれぞれ K 頂点・L 辺のとき、前処理の O((N+M) α(N)) に加え、重みが異なる非木の成分ごとに O(K³+L) 時間、O(K²+L) の作業空間を使います。増加路探索の元の隣接辺は CSR で走査し、縮約間の最小余裕には行列を使います。木の成分は O(K+L) 時間・空間の木 DP に帰着します。全辺同重みの成分は [GeneralMatching](general_matching.md) に帰着します。構築後の辺追加は償却 O(1)、同じ目的での再 `solve()` は O(1) です。解の辺 ID を列挙する操作は O(N) です。

辺数優先では成分内で最小の重みを引き、取り得る重み差の総和を上回る定数を各辺に加えます。変換と総和を広い整数で計算し、内部双対値の上界に応じて 32 / 64 / 128 bit の演算を選びます。

## 依存関係

- [general_matching.hpp](general_matching.md)
- [internal/weighted_blossom.hpp](internal/weighted_blossom.md)
- [dsu.hpp](../data_structure/dsu.md)

## Verify・利用例

- [Library Checker / Weighted Matching on General Graph](../../../verify/library_checker/graph/general_weighted_matching.cpp)
- [目的別 bit DP、符号付き・巨大重み、完全マッチングの実行不能、二部ソルバとの照合](../../../verify/unit/general_matching.cpp)
