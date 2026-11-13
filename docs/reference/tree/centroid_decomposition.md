# centroid_decomposition

[索引](../README.md) / [実装](../../../library/tree/centroid_decomposition.hpp)

## 機能・前提条件

無向木の重心分解。addEdgeで木を作りbuildで重心の階層を構築する。

## インターフェース

```cpp
#include "library/tree/centroid_decomposition.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct CentroidDecomposition;
CentroidDecomposition();
CentroidDecomposition(int n_);
void addEdge(int u, int v);
void build();
```

## 計算量

O(N log N) 時間、O(N) 空間。

## 依存関係

- [static_graph.hpp](../graph/static_graph.md)

## Verify・利用例

- [`unit/csr_graph`](../../../verify/unit/csr_graph.cpp)
