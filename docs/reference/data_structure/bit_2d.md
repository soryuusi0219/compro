# bit_2d

[索引](../README.md) / [実装](../../../library/data_structure/bit_2d.hpp)

## 機能・前提条件

二次元Fenwick tree。一点加算と半開長方形の和。配列構築版は空でない長方形配列を渡す。

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

一点操作・和 O(log H log W)、配列構築 O(HW log H log W)、空間 O(HW)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`aoj/range_sum_bit_2d`](../../../verify/aoj/data_structure/range_sum_bit_2d.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
