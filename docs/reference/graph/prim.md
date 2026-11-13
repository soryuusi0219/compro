# prim

[索引](../README.md) / [実装](../../../library/graph/prim.hpp)

## 機能・前提条件

無向重み付きグラフの最小全域木。solve(x)は重み和と選んだ辺を返す。対象成分が全頂点を含む入力で利用する。

## インターフェース

```cpp
#include "library/graph/prim.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct Prim;
Prim(int n_);
void add_edge(int u, int v, T cost);
pair<T,vector<pair<long long,long long>>> solve(int x = 0);
```

## 計算量

O((V+E) log E) 時間、O(V+E) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/minimum_spanning_tree`](../../../verify/library_checker/graph/minimum_spanning_tree.cpp)
