# maximal_weighted_independent_set

[索引](../README.md) / [実装](../../../library/graph/maximal_weighted_independent_set.hpp)

## 機能・前提条件

頂点重み付き最大独立集合。重み総和と選んだ頂点列を返す。

## インターフェース

```cpp
#include "library/graph/maximal_weighted_independent_set.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
pair<T, vector<long long>> maximal_weighted_independent_set(vector<vector<long long>>& G, vector<T> weight);
```

## 計算量

分枝探索による指数時間、上界 O(poly(V) 2^V)。空間はメモ化した探索状態数に依存する。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/independent_set`](../../../verify/unit/independent_set.cpp)
