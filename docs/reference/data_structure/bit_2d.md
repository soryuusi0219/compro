# bit_2d

[索引](../README.md) / [実装](../../../library/data_structure/bit_2d.hpp)

## 機能・前提条件

二次元Fenwick tree。一点加算と半開長方形の和。`sum(i,j)` は `[0,i) × [0,j)` を集計する。配列構築版には行の長さが揃った配列を渡す。空配列は `0 × 0`。

## インターフェース

```cpp
#include "library/data_structure/bit_2d.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct BIT2D;
BIT2D(int _h, int _w);
BIT2D(const vector<vector<T>>& v);
void add(int i, int j, T x);
T sum(int i, int j);
T sum(int hl, int hr, int wl, int wr);
```

## 計算量

配列構築は各軸で値を親へ一度ずつ伝え、正のサイズでは `O(HW)`。一点加算・和は最悪 `O(log(H+1) log(W+1))`、空間は `O((H+1)(W+1))`。一方の軸が空なら和は0。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/rectangle_queries`](../../../verify/unit/rectangle_queries.cpp)
- [`aoj/range_sum_bit_2d`](../../../verify/aoj/data_structure/range_sum_bit_2d.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
