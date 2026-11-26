# bipartite_matching

[索引](../README.md) / [実装](../../../library/graph/bipartite_matching.hpp)

## 機能・前提条件

二部グラフの最大マッチングを Hopcroft–Karp 法で求める。左右の頂点は別々に 0 始まり。辺 ID は追加順の 0 始まりで、平行辺も個別の ID を持つ。左右の頂点数と辺数はそれぞれ `int` に収める。

## インターフェース

```cpp
#include "library/graph/bipartite_matching.hpp"

soryuusi::BipartiteMatching g(left_count,right_count);
g.reserve_edges(m);
int id = g.add_edge(u,v);
int k = g.solve();
auto cover = g.minimum_vertex_cover();
```

- `left_size()`、`right_size()`、`edge_count()` は頂点数と辺数。 `get_edge(id)` は端点の組、`edges()` は辺 ID 順の端点列を参照で返す。グラフの変更後に端点列のイテレータや要素の参照を使う場合は再取得する。
- `solve()` はマッチングの総本数を返す。辺を追加すると既存マッチングを初期解にして次の `solve()` で拡張する。
- `left_partner(u)`、`right_partner(v)` は相手頂点、`left_match_edges()[u]` と `right_match_edge(v)` は各頂点に接続したマッチング辺 ID。未マッチの値は −1。`matching_edges()` はマッチング辺 ID を左頂点順に返す。参照先のマッチングは `solve()` により更新される。
- `minimum_vertex_cover()` と `maximum_independent_set()` は `VertexSet{left,right}` を返す。各側の `vector<bool>` は頂点の採否。 `minimum_edge_cover()` は最小辺被覆の辺 ID、孤立頂点があれば `nullopt`。これらの前に現在のグラフで `solve()` を実行する。

## 計算量

V=L+R、E を辺数とする。構築は O(V)、辺追加は償却 O(1)。CSR の再構築は必要なときに O(V+E)。`solve()` は最悪 O((V+E)√V)、最大性が既知なら O(1)。空間 O(V+E)。必要に応じて次数 1 を優先する貪欲法と作業量を制限した再割当てで初期解を改善する。初回は次数 2 の右頂点が作る森を一段だけ縮約する場合がある。最終的な増加路探索は反復処理で行う。マッチング辺 ID 列の取得は O(L)、相手頂点・単一辺 ID の取得は O(1)。被覆・独立集合の復元は O(V+E)。

## 依存関係

標準ライブラリ。

## Verify・利用例

- [`library_checker/bipartitematching`](../../../verify/library_checker/graph/bipartitematching.cpp)
- [`unit/bipartite_matching`](../../../verify/unit/bipartite_matching.cpp)
