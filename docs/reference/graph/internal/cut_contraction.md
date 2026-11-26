# cut_contraction

[索引](../../README.md) / [実装](../../../../library/graph/internal/cut_contraction.hpp)

## 機能・前提条件

Stoer–Wagner と Extreme Vertex Sets の縮約ラウンドで共用する内部処理です。無向辺を整数 ID の逆辺対で持ち、頂点の隣接リストを縮約時に移します。消える辺の逆辺は O(1) で除き、初期入力と縮約で生じる平行辺を加算します。

## インターフェース

`CutContraction` は `for_each(u, emit)` で現在の隣接辺を走査し、`contract(u, v)` で `v` を `u` に吸収します。吸収した頂点の番号は再利用しません。`degree[u]` は現時点での容量付き次数です。

`CutOrderingHeap<Cap, Minimum>` は有効頂点から構築する添字付き二分ヒープです。キーを改善する更新に対応し、`pop()` で取り除いた頂点への更新を無視します。最大隣接順序では加算、最小次数順序では減算が優先度の改善です。

## 計算量

縮約は 2 頂点の隣接辺数の和に比例し、隣接辺の総空間は O(N+M) です。ヒープは K 個の有効頂点から O(K) 時間で構築し、抽出と優先度更新は O(log K) です。空間は O(N) です。

## 依存関係

- [undirected_cut_edge.hpp](../undirected_cut_edge.md)

## Verify・利用例

- [無向カットの全部分集合・縮約検証](../../../../verify/unit/undirected_cuts.cpp)
