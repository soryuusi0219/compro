# static_rectangle_add_rectangle_sum

[索引](../README.md) / [実装](../../../library/data_structure/static_rectangle_add_rectangle_sum.hpp)

## 機能・前提条件

長方形加算と長方形和を全て登録してsolveするオフライン処理。半開座標区間を使う。

## インターフェース

```cpp
#include "library/data_structure/static_rectangle_add_rectangle_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, class X = int, class Y = int>
struct StaticRectangleAddRectangleSum;
StaticRectangleAddRectangleSum();
StaticRectangleAddRectangleSum(int n, int q_);
void add_rectangle(X xl, X xr, Y yl, Y yr, S w);
void add_query(X xl, X xr, Y yl, Y yr);
vector<S> solve();
```

## 計算量

更新 N・質問 Q に O((N+Q) log(N+Q)) 時間、O(N+Q) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_rectangle_add_rectangle_sum`](../../../verify/library_checker/data_structure/static_rectangle_add_rectangle_sum.cpp)
