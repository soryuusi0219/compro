# static_range_inversions

[索引](../README.md) / [実装](../../../library/data_structure/static_range_inversions.hpp)

## 機能・前提条件

静的配列の区間転倒数。構築時のqで想定クエリ数を指定できる。

## インターフェース

```cpp
#include "library/data_structure/static_range_inversions.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
struct StaticRangeInversions;
using ll = long long;
StaticRangeInversions(const vector<T>& v, int q = -1);
long long query(int x, int y);
```

## 計算量

ブロック長 L に対し構築 O(N log N+N²/L)、query O(L)、空間 O(N²/L)。既定Lは概ね√N。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_inversions`](../../../verify/library_checker/data_structure/static_range_inversions.cpp)
