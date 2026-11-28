# flow_decomposition

[索引](../README.md) / [実装](../../../library/graph/flow_decomposition.hpp)

## 機能・前提条件

実行可能な非負整数の s-t フロー、または循環流を、単純有向パス・単純有向サイクルに分解する。各成分は正の流量と、元の辺 ID を通過順に保持する。平行辺・自己ループを含め、各辺の流量を成分からちょうど復元できる。

入力の `edges[i]` は頂点番号 `[0,n)` と `0 <= flow <= cap` を満たす。`Cap` は `bool` を除く 64 bit 以下の整数型で、s-t フローの場合は s の流出量 − 流入量が非負であることを要求する。各成分の流量は `Cap` に収まり、合計流量は `Cap` の最大値を超えてよい。

## インターフェース

```cpp
#include "library/graph/flow_decomposition.hpp"

auto parts = soryuusi::decompose_flow(n,g.edges(),s,t);
for(const auto& [amount,ids,is_cycle] : parts) {
  // ids は edges() に対する 0 始まりの辺 ID
}
```

- `decompose_flow(n,edges,s,t)` の戻り値は `vector<FlowComponent<Cap>>`。各成分の `edge_ids` は辺の連続した順序で、`is_cycle` が偽なら始点 s から終点 t へのパス。真なら最後の辺の終点から最初の辺の始点に戻る。自己ループは 1 辺のサイクルになる。
- `decompose_flow(n,edges)` は循環流の分解。各頂点で流量保存則を要求し、全成分をサイクルとして返す。
- 入力の辺と流量は変更しない。成分の個数・出力順はグラフや辺追加順に依存する。

## 計算量

頂点数 V、辺数 E、出力する辺 ID の個数の合計 L に対し時間 O(V+E+L)、空間 O(V+E+L)。s-t フローは仮の t→s 辺を足して循環流に変換し、正の残余流量を持つ歩道のスタックからサイクルを切り出す。成分を取り出した後も残る正流量の接頭部を再利用する。仮の辺を含むサイクルからその辺を除くと s-t パスになる。

## 依存関係

- [max_flow.hpp](max_flow.md) の `FlowEdge`

## Verify・利用例

- [`unit/flow`](../../../verify/unit/flow.cpp) でサイクル・パスの向きと各辺の流量を照合する。
