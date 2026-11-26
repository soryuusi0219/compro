# weighted_blossom

[索引](../../README.md) / [実装](../../../../library/graph/internal/weighted_blossom.hpp)

## 機能・前提条件

固定した連結成分で正整数スコアの総和を最大化する主双対 blossom 法です。頂点を内部で 1 始まりに移し、奇閉路を表す節点は元の頂点より後ろに配置します。各縮約節点とほかの節点の間で、最小余裕の元の辺を行列に保持します。縮約解除した節点の ID を再利用します。

`N` 頂点、最大スコア `maximum` に対して、`4*(N+1)*maximum` が符号付き整数型 `Weight` の上限以下であることが前提です。デバッグビルドでは頂点・奇集合の双対値、全入力辺の双対制約、主目的値と双対目的値の一致を検査します。

## インターフェース

内部用 `WeightedBlossom<Weight>(n, edges, maximum)` の入力辺は、0 始まりの両端点と正のスコアを持ちます。平行辺のスコアは最大値を使います。`solve()` を一度呼ぶと、相手頂点を 0 始まりの配列で返します。未マッチは `-1` です。

## 計算量

K 頂点・L 辺に対し、構築 O(K²+L)、探索 O(K³)、作業空間 O(K²+L)。探索の各段階では交互森に現れた頂点の双対値と、余裕が登録された節点を更新します。非連結の分割、非正スコアの除外、元の辺 ID の復元は公開側が担当します。

## 依存関係

標準ライブラリのみ。密行列の設計は CC0-1.0 の [qandori/cp-library](https://github.com/qandori/cp-library/blob/97b50074b8c1613bc358316249dc38e54d08a331/components/weighted_blossom.hpp) を参照。

## Verify・利用例

- [公開ラッパーの unit](../../../../verify/unit/general_matching.cpp)
- [Library Checker](../../../../verify/library_checker/graph/general_weighted_matching.cpp)
