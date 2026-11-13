# static_rectangle_sum

[索引](../README.md) / [実装](../../../library/data_structure/static_rectangle_sum.hpp)

## 機能・前提条件

重み付き点と長方形質問を登録するオフライン二次元範囲和。solveは質問登録順の答えを返す。

## インターフェース

```cpp
#include "library/data_structure/static_rectangle_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, class X = int, class Y = int>
struct StaticRectangleSum;
StaticRectangleSum();
StaticRectangleSum(int n, int q_);
void add_point(X x, Y y, S w);
void add_query(X xl, X xr, Y yl, Y yr);
vector<S> solve();
```

## 計算量

点 N・質問 Q に O((N+Q) log(N+Q)) 時間、O(N+Q) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/rectangle_sum_static`](../../../verify/library_checker/data_structure/rectangle_sum_static.cpp)
