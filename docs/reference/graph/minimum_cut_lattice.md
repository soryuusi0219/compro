# minimum_cut_lattice

[索引](../README.md) / [実装](../../../library/graph/minimum_cut_lattice.hpp)

## 機能・前提条件

有向グラフの固定された s,t 間の最小カット全体を、最大流の正残余容量辺から構成した SCC 縮約 DAG で表す。`MaxFlow` に保存されている流量は、s,t を端点とする流量保存則を満たす必要がある。最小カットが確定するまで最大流を実行してから使う。残余グラフに s から t へのパスがある場合は `nullopt`。

## インターフェース

```cpp
#include "library/graph/minimum_cut_lattice.hpp"

g.flow(s,t);
auto lattice = soryuusi::minimum_cut_lattice(g,s,t);
auto smallest = lattice->minimum_source_side();
auto largest = lattice->maximum_source_side();
```

- `component[v]` は元の頂点の SCC 番号。`dag` は重複辺と自己ループを除いた `static_graph<>`。成分番号はトポロジカル順。各出辺の順序は任意。
- `side[c]` は `Side::source`（s の成分から到達可能）、`Side::sink`（t の成分へ到達可能）、`Side::free` のいずれか。最初の二つはすべての最小カットで対応する側に固定される。自由成分の選択には DAG の含意が残る。
- `minimum_source_side()` と `maximum_source_side()` は包含関係で最小・最大の source 側を元頂点順の `vector<bool>` で返す。最小はすべての最小カットの source 側の共通部分、最大は和集合。
- s の SCC を含み、t の SCC を除き、DAG の各辺 `u→v` で `u` を選ぶとき `v` も選ぶような成分集合が、最小カットの source 側と 1 対 1 に対応する。
- 戻り値は独立したスナップショット。元の最大流グラフを更新・破棄した後も使用できる。

## 計算量

元の頂点数 V、辺数 E に対して構築 O(V+E) 時間・作業領域。正残余容量辺を直接走査し、縮約辺を成分単位で印付けして重複を線形時間で除く。縮約 DAG の頂点数を C、辺数を D とすると結果の領域は O(V+C+D)。source 側の復元は各 O(V)。

## 依存関係

- [max_flow.hpp](max_flow.md)
- [scc.hpp](scc.md)
- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`aoj/minimum_cut_lattice`](../../../verify/aoj/graph/minimum_cut_lattice.cpp) は GRL_6_A で最小・最大 source 側のカット容量を照合する。
- [`unit/minimum_cut_lattice`](../../../verify/unit/minimum_cut_lattice.cpp) は全カットを列挙し、縮約 DAG の閉集合との対応を検査する。
- [Picard–Queyranne の最小カット構造](https://publications.polymtl.ca/5993/)
