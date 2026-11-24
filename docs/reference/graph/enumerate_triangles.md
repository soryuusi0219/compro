# enumerate_triangles

[索引](../README.md) / [実装](../../../library/graph/enumerate_triangles.hpp)

## 機能・前提条件

次数に応じて辺を向き付けし、三角形の各頂点をcallbackへ渡す。無向単純グラフを登録する。疎な入力では頂点の印を使い、密な入力では隣接ビット列の積集合から列挙する。callbackの呼び出し順は任意。同じ入力で繰り返し `solve()` できる。

## インターフェース

```cpp
#include "library/graph/enumerate_triangles.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct EnumerateTriangles;
EnumerateTriangles(int n_);
EnumerateTriangles(int n_, int m_);
void add_edge(int u, int v);
template<class F>
void solve(F f);
```

## 計算量

O(V+E√E) 時間、O(V+E) 空間。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`library_checker/enumerate_triangles`](../../../verify/library_checker/graph/enumerate_triangles.cpp)
- [`unit/graph_algorithms`](../../../verify/unit/graph_algorithms.cpp)
