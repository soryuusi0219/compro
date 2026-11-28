# undirected_cut_edge

[索引](../README.md) / [実装](../../../library/graph/undirected_cut_edge.hpp)

## 機能・前提条件

無向カット系ライブラリに渡す辺です。頂点は 0-indexed で、無向辺 1 本につき 1 要素を渡します。容量は非負整数です。平行辺はそれぞれ数え、自己ループはカット容量に寄与しません。

## インターフェース

```cpp
template <class Cap = long long> struct UndirectedCutEdge {
  int from, to;
  Cap capacity;
};
```

## 計算量

辺 1 本につき空間 O(1)。

## 依存関係

標準ヘッダーへの依存なし。

## Verify・利用例

- [無向カットの全探索テスト](../../../verify/unit/undirected_cuts.cpp)
- [大域最小カットの利用例](global_minimum_cut.md#verify利用例)
