# selection_cut

[索引](../../README.md) / [実装](../../../../library/graph/internal/selection_cut.hpp)

## 機能・前提条件

Project Selection の二値モデルと多値モデルが共用する内部のカット構築。source を 0、sink を 1 として、変数と補助頂点を 2 から順に割り当てる。条件付き費用を定数、真のときの単項差分、有向辺に分けて保持する。

## インターフェース

- `add_delta(v,d)` は真のときの費用差を加える。`constant` には値によらない費用を加える。
- `add_arc(u,v,c)` は u が source 側、v が sink 側のときにかかる非負費用。`imply(u,v)` はその組合せを禁止する。
- `solve(push_relabel)` は最小費用と全頂点の source 側を返す。ハード制約に矛盾があれば `nullopt`。単項差分が負の場合は定数費用を補正して source から辺を張る。単項だけの場合は各変数の最適値を直接選ぶ。

## 計算量

頂点・辺追加は償却 O(1)。容量正規化・構築・復元は O(V+E)、最大流は最悪 O(V²E)。空間は O(V+E)。有限容量は通常 `uint64_t` の辺配列に保持し、同型を超える容量だけを別配列に保持する。ハード辺は端点だけを別に保持し、source から sink へのハード辺のみの経路がある場合は最大流の前に矛盾と判定する。有限容量の総和に 1 足した値が `uint64_t` を超える場合は符号付き 128 bit の最大流を使う。

## 依存関係

- [max_flow.hpp](../max_flow.md)

## Verify・利用例

- [`unit/project_selection`](../../../../verify/unit/project_selection.cpp)
