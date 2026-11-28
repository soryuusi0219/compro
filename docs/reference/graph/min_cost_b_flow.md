# min_cost_b_flow

[索引](../README.md) / [実装](../../../library/graph/min_cost_b_flow.hpp)

## 機能・前提条件

各有向辺の符号付き上下限と、各頂点の供給量を満たし、各辺の流量と単位費用の積の総和を最小化する。供給量は流出量 − 流入量。平行辺・自己ループ・固定流量の辺・孤立成分を許す。負費用閉路も最適化に含まれる。頂点は `[0,n)`、辺 ID は追加順の 0 始まり。

`Cap` と `Cost` は 64 bit 以下の符号付き整数型。各辺の流量と上下限、各頂点の入力供給量は `Cap` に収める。容量差・補正後の供給合計を広い型で扱う。総費用の途中和を符号付き 128 bit、最終値を `Total` に収める。既定の `Total` は `__int128_t`。オーバーフロー時は `std::overflow_error` を送出する。

## インターフェース

```cpp
#include "library/graph/min_cost_b_flow.hpp"

soryuusi::MinCostBFlow<> g(2);
g.reserve_edges(2);
int a = g.add_edge(0,1,0,4,-3);
int b = g.add_edge(1,0,0,2,1);
auto r = g.solve({1,-1});  // r->cost == -7、flow[a] == 3、flow[b] == 2
auto cycle = g.solve();   // 供給量がすべて 0 の場合も負費用循環を最適化
```

- `solve([supply]) const` は実行不能なら `nullopt`、実行可能なら `Result{cost,flow,potential}` を返す。`supply` は頂点順のベクタで、省略または空なら全頂点を 0 とする。固定量 k を s から t に送るには s の供給を k、t の供給を −k とする。
- `flow` は辺 ID 順の `vector<Cap>`、`potential` は頂点順の `vector<__int128_t>`。辺 u→v の換算費用を `r = cost + potential[u] - potential[v]` とすると、流量が上限未満なら `r >= 0`、下限を超えるなら `r <= 0` になる。
- `solve` は入力を保持する。異なる供給量で呼び出すと、それぞれの実行可能性判定と最適化を行う。

## 計算量

各辺を単独で最安にした流量が供給量を満たす場合は O(V+E)。その他の場合は [BoundedFlow](bounded_flow.md) で実行可能解を得てから、整数費用を V+1 倍した ε-cost scaling で最適化する。局所 push・relabel の一般上界は O(V²E log(2+VC))。C は単位費用の絶対値の最大。周期的な大域ポテンシャル更新を行う構成では更新の Dijkstra に最大 O(log(V+E+2)) の追加係数を見込む。実行可能性判定は最悪 O(V²(E+V))。空間は O(V+E)。

最後に全頂点のスケール済みポテンシャルを同じ値だけシフトし、V+1 で割る境界にどの頂点も乗らないようにする。これにより、換算費用がスケール後に −1 の残余辺も、整数のポテンシャルに戻した後は非負になる。

## 依存関係

- [bounded_flow.hpp](bounded_flow.md)

## Verify・利用例

- [`library_checker/min_cost_b_flow`](../../../verify/library_checker/graph/min_cost_b_flow.cpp) は [Minimum Cost b-flow](https://judge.yosupo.jp/problem/min_cost_b_flow) の最小費用・辺流量・双対ポテンシャルを照合する。
- [`unit/min_cost_b_flow`](../../../verify/unit/min_cost_b_flow.cpp) は微小グラフの全列挙、負費用循環、実行不能、64 bit 境界での流量と相補性を確認する。
