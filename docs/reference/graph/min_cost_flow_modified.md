# min_cost_flow_modified

[索引](../README.md) / [実装](../../../library/graph/min_cost_flow_modified.hpp)

## 機能・前提条件

限界費用関数を持つ最小費用流。FR/FLが流量変更時の費用を評価する。残余辺とポテンシャルを用いる。

## インターフェース

```cpp
#include "library/graph/min_cost_flow_modified.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <class Cap, class Cost, class Costfct, auto FR, auto FL, auto MAXCOST, auto ZEROCOST> struct mcf_graph_modified
mcf_graph_modified();
mcf_graph_modified(int n);
int add_edge(int from, int to, Cap cap, Costfct costfct);
struct edge;
edge get_edge(int i);
vector<edge> edges();
pair<Cap, Cost> flow(int s, int t);
pair<Cap, Cost> flow(int s, int t, Cap flow_limit);
vector<pair<Cap, Cost>> slope(int s, int t);
vector<pair<Cap, Cost>> slope(int s, int t, Cap flow_limit);
```

## 計算量

増加路回数 A に対し O(A(V+E) log V)、空間 O(V+E)。単位流量ずつ流す場合 A は総流量程度。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`aoj/min_cost_flow_modified`](../../../verify/aoj/graph/min_cost_flow_modified.cpp)
- [`unit/min_cost_flow_modified`](../../../verify/unit/min_cost_flow_modified.cpp)
