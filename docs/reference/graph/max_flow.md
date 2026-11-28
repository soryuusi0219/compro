# max_flow

[索引](../README.md) / [実装](../../../library/graph/max_flow.hpp)

## 機能・前提条件

有向グラフの整数最大流と残余グラフ上の到達可能集合を求める。容量は非負で、自己ループ・平行辺・容量 0 を許す。逆向きに追加した辺も独立した辺 ID・容量・非負流量を持つ。無向のカット容量を表す場合は両方向に同容量を登録する。

`Cap` は `bool` を除く整数型。頂点番号は `[0,n)`、辺 ID は追加順の 0 始まりで、残余辺を含めた辺数は `int` に収める。

## インターフェース

```cpp
#include "library/graph/max_flow.hpp"

soryuusi::MaxFlow<long long> g(n);
g.reserve_edges(m);
int id = g.add_edge(u,v,capacity);
long long added = g.flow(s,t,limit);
added += g.flow(s,t);
auto [from,to,cap,flow] = g.get_edge(id);
auto side = g.min_cut(s);
```

- `flow(s,t[,limit])` は反復型 Dinic 法を使い、現在の辺流量を更新して今回の追加流量を返す。既定の上限は `numeric_limits<Cap>::max()`。頂点 `s,t` は相異なり、上限は非負とする。同じ端点で続けて呼び出せる。累積値が `Cap` の最大値を超える場合は呼び出しごとに加算先を広い型にする。
- `flow_push_relabel(s,t)` は highest-label Push-Relabel 法を使う。global relabel と gap を行い、余剰を始点に戻してから追加流量を返す。始点の使用可能な残余容量の合計が `Cap` に収まらない場合や高さの添字が `int` に収まらない場合は `flow(s,t)` に切り替える。
- `get_edge(id)` と `edges()` は `Edge`（`FlowEdge<Cap>`）のコピーを返す。各辺の正方向の残余容量は `cap-flow`、逆方向は `flow`。辺の追加・実行・リセット後も ID は同じ。
- `size()` は頂点数。
- `for_each_residual_arc(emit) const` は正残余容量の各有向辺を `emit(from,to,residual)` に渡す。元の辺 ID 順に正方向を先、逆方向を後に処理し、平行辺や自己ループも独立して渡す。呼び出し中のグラフ変更は不可。
- `change_edge(id,cap,flow)` は指定辺の容量と流量を置き換える。`0 <= flow <= cap` を満たし、同じ端点で実行を再開する前に中間頂点の流量保存則を呼び出し側で満たす。
- `reset()` は現在の各辺容量を保って流量を 0 にする。異なる端点で新たな最大流を求めるときは、先に `reset()` する。
- `min_cut(s)` は正の残余容量を持つ辺をたどって到達できる頂点を `vector<bool>` で返す。現在の流量が実行可能な s-t フローで、`min_cut(s)[t]` が偽なら、戻り値で `true` の頂点集合が最小カットの始点側となる。容量上限に達した呼び出しの後は到達可能性を確認する。

## 計算量

頂点数を V、辺数を E とする。隣接辺 ID の配列を初回の探索時と辺追加後の初回探索時に O(V+E) で作り、続く探索で再利用する。

| 操作 | 時間 |
| --- | --- |
| `add_edge` | 償却 O(1) |
| `flow` | 最悪 O(V²E)。二部マッチングなどの単位ネットワークでは O((V+E)√V) |
| `flow_push_relabel` | 最悪 O(V²E) の一般上界 |
| `get_edge`, `change_edge`, `size` | O(1) |
| `edges`, `reset`, `for_each_residual_arc` | O(E) |
| `min_cut` | O(V+E) |

空間 O(V+E)。探索に再帰を使わない。増加路長が多数の異なる値を取るグラフでは Push-Relabel が有効な場合がある。密な内部グラフに大量の余剰を送り返す入力では Dinic の方が速い場合がある。

## 依存関係

標準ライブラリ。

## Verify・利用例

- [`aoj/max_flow`](../../../verify/aoj/graph/max_flow.cpp)
- [`unit/flow`](../../../verify/unit/flow.cpp)
