# maximal_independent_set

[索引](../README.md) / [実装](../../../library/graph/maximal_independent_set.hpp)

## 機能・前提条件

名称はmaximalだが最大サイズの独立集合を探索し、頂点番号列を返す。bit表現の幅内の小グラフ向け。

## インターフェース

```cpp
#include "library/graph/maximal_independent_set.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
vector<long long> maximal_independent_set(vector<vector<long long>>& G);
```

## 計算量

分枝探索による指数時間。安全な上界 O(poly(V) 2^V)、メモ化領域も探索状態数に依存する。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/maximum_independent_set`](../../../verify/library_checker/graph/maximum_independent_set.cpp)
- [`unit/independent_set`](../../../verify/unit/independent_set.cpp)
