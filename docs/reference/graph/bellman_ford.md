# bellman_ford

[索引](../README.md) / [実装](../../../library/graph/bellman_ford.hpp)

## 機能・前提条件

負辺を許す単一始点最短路。`distance(s)` は `{成功フラグ,距離列}` を返す。始点から到達できる負閉路がある場合にフラグがfalseになる。無限大・単位元・距離結合opを指定する。

## インターフェース

```cpp
#include "library/graph/bellman_ford.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, auto e, auto infty, auto op>
struct BellmanFord;
struct Edge;
BellmanFord(int n0);
void add_edge(int from, int to, T cost, bool undirected = false);
pair<bool,vector<T>> distance(int s);
long long  e();
long long  op(long long  x, long long  y);
long long  infty();
using BellmanFordll = BellmanFord<long long, e, infty, op>;
```

## 計算量

O(VE) 時間、O(V+E) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`aoj/bellman_ford`](../../../verify/aoj/graph/bellman_ford.cpp)
