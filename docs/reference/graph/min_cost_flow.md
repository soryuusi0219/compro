# min_cost_flow

[索引](../README.md) / [実装](../../../library/graph/min_cost_flow.hpp)

## 機能・前提条件

有向グラフで、指定量まで流せる最大の追加流量と、そのときの最小追加費用を求める。各辺は非負整数容量と符号付き整数の単位費用を持つ。平行辺・逆向きの独立した辺・自己ループを許す。頂点番号は `[0,n)`、辺 ID は追加順の 0 始まり。

初期流量は 0。実行開始時の正残余容量辺に負閉路がないことを要求する。負費用辺を含む場合は全頂点を始点とする最短路でポテンシャルを初期化する。負閉路を検出すると `std::domain_error` を送出する。負費用循環を含む最適化には [MinCostBFlow](min_cost_b_flow.md) を使う。

`Cap` は `bool` を除く 64 bit 以下の整数型、`Cost` は 64 bit 以下の符号付き整数型。内部ポテンシャルと単位費用の差は `__int128_t`。費用の増分積は符号付き 128 bit、各折点の累積費用は `Total` に収める。既定の `Total` は `__int128_t`。費用のオーバーフローでは `std::overflow_error` が送出される時点までの流量更新が残る。

## インターフェース

```cpp
#include "library/graph/min_cost_flow.hpp"

soryuusi::MinCostFlow<long long,long long> g(3);
g.reserve_edges(2);
int id = g.add_edge(0,1,4,-3);
g.add_edge(1,2,4,5);
auto [added,cost] = g.flow(0,2,1);  // 1, 2
auto points = g.slope(0,2);         // {{0,0}, {3,6}}
auto edge = g.get_edge(id);         // edge.flow == 4
g.reset();
```

- `flow(s,t[,limit])` は `pair<Cap,Total>` を返す。`s != t` かつ `limit >= 0`。既定上限は `numeric_limits<Cap>::max()`。現在の残余グラフに対して実行し、費用と流量は今回の差分を返す。
- `slope(s,t[,limit])` は `vector<pair<Cap,Total>>` で追加流量と追加費用の折点を返す。先頭は `{0,0}`。流量座標と区間の限界費用は狭義単調増加で、各区間内の整数流量の費用は線形補間できる。
- `get_edge(id)` と `edges()` は `Edge{from,to,cap,flow,cost}` のコピーを返す。`reset()` は容量・単位費用・辺 ID を保って流量を 0 にする。
- 実行後も残余容量とポテンシャルを保持する。同じ端点で再開でき、異なる端点を指定すると現在の流出入に対して新しい端点間の流量を加える。辺を追加した後の最初の実行ではポテンシャルを再計算する。新しい辺によって負残余閉路ができた場合は、流量 0 からの問題なら `reset()` して再実行する。

## 計算量

頂点数 V、辺数 E。構築 O(V+E)、辺追加は償却 O(1)、初回と追加後に隣接索引を O(V+E) で構築する。`get_edge` は O(1)、`edges` は O(E)、`reset` は O(V+E)、空間 O(V+E)。

初期化は非負費用または DAG で O(V+E)、一般の負費用初期化で最悪 O(V(V+E))。Dijkstra 1 回は O((V+E) log(E+2))。換算費用 0 の候補はヒープを経由しない。同じ限界費用が続く場合は換算費用 0 の残余グラフで反復型 blocking flow を行う。各増加経路は整数流量を 1 以上増やすため、追加流量を F とした一般上界は O(F(V+E)(V+log(E+2)))。異なる限界費用が続くケースでは Dijkstra と経路更新を 1 回ずつ行う。

## 依存関係

- [internal/min_cost_paths.hpp](internal/min_cost_paths.md)

## Verify・利用例

- [`aoj/min_cost_flow`](../../../verify/aoj/graph/min_cost_flow.cpp) は [AOJ GRL_6_B](https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_6_B) の固定量の最小費用を求める。
- [`unit/min_cost_flow`](../../../verify/unit/min_cost_flow.cpp) は全流量列挙、ACL、折点・累積流量・負費用・再開を照合する。
