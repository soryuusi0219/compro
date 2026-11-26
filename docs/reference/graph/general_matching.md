# general_matching

[索引](../README.md) / [実装](../../../library/graph/general_matching.hpp)

## 機能・前提条件

無向グラフで、端点を共有しない辺の集合のうち辺数最大のものを求めます。Gabow–Edmonds 法で奇閉路を縮約し、増加路が見つかったときにリンクから元グラフの辺を復元しながら対応を反転します。縮約と反転は反復処理です。

頂点と辺 ID は 0 始まりです。自己ループにも ID は付きますが、解には入りません。平行辺にもそれぞれ別の ID が付きます。N は `INT_MAX` 未満、M は `INT_MAX/2` 以下です。

## インターフェース

```cpp
#include "library/graph/general_matching.hpp"

soryuusi::GeneralMatching graph(n);
graph.reserve_edges(m);
int id = graph.add_edge(u,v);
int cardinality = graph.solve();
auto edges = graph.matching_edges();
```

- `size()` と `edge_count()` は頂点数と追加した辺数です。
- `get_edge(id)` は追加時の向きで `Edge{u,v}` を返します。
- `solve()` は最大マッチングの辺数を返します。
- `partner(v)` と `matched_edge(v)` は直近の解での相手頂点と辺 ID です。未マッチなら `-1` です。
- `matching_edges()` は辺 ID を一度ずつ返します。順序は各辺の小さい方の端点順です。
- 構築直後の解は空です。辺を追加したあとも取得 API は直近の解を参照し、再度 `solve()` すると新しいグラフの解に置き換わります。

## 計算量

N 頂点・M 辺。構築 O(N)、辺追加は償却 O(1)。粗い最悪評価は O(N+M+NM log(2+N)) 時間、O(N+M) 空間です。各増加路探索は隣接辺を走査し、縮約した頂点への探索には経路圧縮を使います。疎なグラフでは次数 1 の辺を先に確定し、成分ごとの上界で探索を打ち切ります。密なグラフでは貪欲法と長さ 3 の増加路で上界に届けば CSR 構築を省けます。

グラフが同じ場合の再 `solve()` は O(1)。`matching_edges()` は O(N)、各頂点・辺の取得は O(1) です。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [Library Checker / Matching on General Graph](../../../verify/library_checker/graph/general_matching.cpp)
- [入れ子の奇閉路、入力辺 ID、bit DP、二部マッチングとの照合](../../../verify/unit/general_matching.cpp)
