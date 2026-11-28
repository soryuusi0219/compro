# dulmage_mendelsohn_decomposition

[索引](../README.md) / [実装](../../../library/graph/dulmage_mendelsohn_decomposition.hpp)

## 機能・前提条件

二部グラフの最大マッチングを基に、左右の不足領域と均衡領域の細分ブロックを求める。完全マッチングの有無を問わず、最大マッチングに含まれ得る辺と必ず含まれる辺を分類する。平行辺は別の辺 ID として分類する。

## インターフェース

```cpp
#include "library/graph/dulmage_mendelsohn_decomposition.hpp"

auto dm = soryuusi::dulmage_mendelsohn_decomposition(g);
bool essential = dm.essential[id];
bool unique = dm.unique();
```

- 引数は `BipartiteMatching&`。必要に応じて `solve()` で最大マッチングを完成させる。
- `left_block`、`right_block` は各頂点のブロック番号。 `region[block]` は `Region::right_deficient`、`balanced`、`left_deficient` のいずれか。ここで left/right deficient は、それぞれ左側/右側に未マッチ頂点を含む領域を指す。左右不足領域はそれぞれ非空なら 1 ブロック。均衡領域のブロックはマッチング辺で対応頂点を縮めた有向グラフの SCC。
- 並びは右不足、均衡領域のトポロジカル順、左不足。元の全辺 `(u,v)` で `left_block[u] <= right_block[v]`。同じブロック内の辺がちょうど許容辺。
- `allowed[id]` は少なくとも一つの最大マッチングに含まれる辺、`essential[id]` はすべての最大マッチングに含まれる辺。 `unique()` は辺 ID の集合として最大マッチングが唯一であることを判定する。
- 結果は独立したスナップショット。左右を加えた頂点数と、選ばれた辺の逆向きを加えた辺数は `int` に収める。

## 計算量

頂点・辺数を V、E とする。最大マッチングの時間に加えて O(V+E) 時間・領域。未マッチ左頂点からの到達可能性、未マッチ右頂点への到達可能性、均衡領域内の SCC を使う。 `unique()` は O(E)。

## 依存関係

- [bipartite_matching.hpp](bipartite_matching.md)
- [scc.hpp](scc.md)
- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`unit/bipartite_matching`](../../../verify/unit/bipartite_matching.cpp) は完全・非完全マッチングと平行辺を全列挙し、許容・必須の判定を照合する。
