# bounded_flow

[索引](../README.md) / [実装](../../../library/graph/bounded_flow.hpp)

## 機能・前提条件

各有向辺の上下限と各頂点の供給量を満たす整数フローを 1 つ求める。供給量は流出量 − 流入量で、正が供給、負が需要を表す。上下限は符号付きで `lower <= upper` とする。自己ループと平行辺を許す。頂点番号は `[0,n)`、辺 ID は追加順の 0 始まり。

`Cap` は 64 bit 以下の符号付き整数型。各辺の上下限と、指定する各頂点の供給量は `Cap` に収める。下限補正の合計や区間幅には `__int128_t` を使う。残余問題の各辺容量は総供給で打ち切り、総供給が `Cap` に収まる場合は `MaxFlow<Cap>` で解く。補助頂点を含む頂点数と、逆辺を含む補助グラフの辺数は `int` に収める。

## インターフェース

```cpp
#include "library/graph/bounded_flow.hpp"

soryuusi::BoundedFlow<long long> g(2);
g.reserve_edges(1);
int id = g.add_edge(0,1,2,5);
auto result = g.solve({3,-3});    // result->at(id) == 3
auto circulation = g.solve();     // nullopt
auto st = g.solve(0,1,4,8);        // st->first は 4 または 5
```

- `solve(supply)` の供給量は頂点順の `vector<Cap>` で渡す。省略または空ベクタの場合は全頂点を 0 として循環流を求める。戻り値は辺 ID 順の `optional<vector<Cap>>` で、実行不能なら `nullopt`。
- `solve(s,t,lower,upper)` は s-t フローの値も指定した閉区間に収める。戻り値は `optional<pair<Cap,vector<Cap>>>`。第 1 要素は s の流出量 − 流入量、第 2 要素は辺 ID 順の流量。負の値を含む閉区間も指定できる。実行可能な値を 1 つ返す。
- どちらの `solve` もグラフを変更しない。上下限の同じ辺や自己ループを含め、すべての登録辺の流量を返す。

## 計算量

頂点数 V、辺数 E に対して構築・復元 O(V+E)、最大流の最悪 O(V²(E+V))、空間 O(V+E)。各辺の下限を引いて残余容量を作り、補正後の供給・需要を超始点と超終点に接続し、Push-Relabel 法で解く。下限だけで実行可能な場合は最大流を省略する。

## 依存関係

- [max_flow.hpp](max_flow.md)

## Verify・利用例

- [`unit/flow`](../../../verify/unit/flow.cpp) で上下限制約付きの全流量を列挙し、実行可能性と辺流量を照合する。
