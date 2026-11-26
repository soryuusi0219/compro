# min_cost_paths

[索引](../../README.md) / [実装](../../../../library/graph/internal/min_cost_paths.hpp)

## 機能・前提条件

線形費用流と凸な限界費用流で共用する、ポテンシャル付きの最短増加路探索。走査関数は正残余容量のある辺を列挙する。探索時には全残余辺の換算費用が非負であり、距離の加算と比較が `Price` で正確に行えることを要求する。

## インターフェース

`internal::MinCostPaths<Price>(n,inf[,zero])` は距離とポテンシャル用領域を用意する。`scan(v,emit)` は `emit(to,id,cost_callback)` を各残余辺について呼ぶ。費用は必要になったときに `cost_callback()` で取得する。

- `initialize(scan)` は仮の超始点から全頂点への距離でポテンシャルを設定する。負閉路を検出した場合は偽。
- `search(s,t,scan)` は到達できれば真を返す。増加路は `parent` に辺 ID で記録し、`price` を更新する。経路上の換算費用がすべて 0 になり、逆方向への残余辺を追加できる。

## 計算量

初期化は非負費用または DAG で O(V+E)、一般の負費用グラフで O(V(V+E))。1 回の探索は O((V+E) log(E+2))。距離 0 の遷移は作業用のスタック、それ以外は再利用する二分ヒープで処理する。空間 O(V+E)。

## 依存関係

標準ライブラリ。

## Verify・利用例

- [`unit/min_cost_flow`](../../../../verify/unit/min_cost_flow.cpp)
- [`unit/min_cost_flow_modified`](../../../../verify/unit/min_cost_flow_modified.cpp)
