# min_cost_flow_modified

[索引](../README.md) / [実装](../../../library/graph/min_cost_flow_modified.hpp)

## 機能・前提条件

有向辺に整数流量 x を流したときの費用を F(x) とし、単位増加の限界費用 F(x+1)−F(x) が x に対して非減少な最小費用流を扱う。同じ限界費用が続く区間をコールバックで渡す。負の初期限界費用を許す。正残余容量の辺に負閉路を検出した場合は `std::domain_error` を送出する。

## インターフェース

```cpp
#include "library/graph/min_cost_flow_modified.hpp"

template <class Cap, class Cost, class Costfct, auto FR, auto FL, auto MAXCOST, auto ZEROCOST> struct mcf_graph_modified;
```

- `FR(f,x)` は現在流量 x が容量未満のときに呼ばれ、`{F(x+1)-F(x),r}` を返す。x から r まで各単位を増やす費用が同じ。r は x より大きく、容量を超える場合は容量に切り詰める。
- `FL(f,x)` は x が正のときに呼ばれ、`{F(x)-F(x-1),l}` を返す。x から l まで各単位を減らす費用が同じ。l は x より小さく、負の場合は 0 に切り詰める。どちらも幅 0 以下の区間を返すと、その区間を使う際に `std::invalid_argument` を送出する。
- `MAXCOST()` は Dijkstra の距離の番兵を返す。到達する換算距離はこれより小さいことを要求する。`ZEROCOST()` は費用の 0 を返す。通常の 64 bit 以下の整数 `Cost` では内部の加減算を `__int128_t` で行う。返す費用は `Cost` に収める。
- `add_edge(from,to,cap,costfct)` は辺 ID を返す。容量は非負。登録した費用関数は実行の間も一貫した限界費用と区間を返す。`get_edge(i)` と `edges()` は `edge{from,to,cap,flow,costfct}` を返す。
- `flow(s,t[,limit])` は今回の追加流量と追加費用を返す。`slope(s,t[,limit])` は最初に `{0,0}` を含み、隣接区間の限界費用が異なる折点列を返す。続けて同じ端点で呼び出せる。異なる端点で呼ぶと現在の流出入に追加する。辺追加後の初回は正残余容量辺の費用でポテンシャルを再初期化する。
- `reset()` は辺 ID・容量・費用関数を保って流量を 0 に戻す。

## 計算量

初期化は非負費用または DAG で O(V+E)、一般の負費用グラフで O(V(V+E))。増加回数 A とし、FR と FL が O(1) の場合は O(A(V+E) log(E+2))、空間 O(V+E)。増加の際は経路上の最初の限界費用の変化点で止まる。単位幅の限界費用を持つ場合は A が総流量まで増える。区間数が少ない場合は、1 区間を 1 本の平行辺に展開し、線形費用版で解く方法も使える。

## 依存関係

- [internal/min_cost_paths.hpp](internal/min_cost_paths.md)

## Verify・利用例

- [`aoj/min_cost_flow_modified`](../../../verify/aoj/graph/min_cost_flow_modified.cpp) は一定の限界費用を使う。
- [`unit/min_cost_flow_modified`](../../../verify/unit/min_cost_flow_modified.cpp) は区分線形な凸費用、前後の限界区間、再開・残余閉路を検証する。
