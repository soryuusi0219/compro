# bit_sparse

[索引](../README.md) / [実装](../../../library/data_structure/bit_sparse.hpp)

## 機能・前提条件

更新座標を先に登録する疎Fenwick tree。add_point → build → add/sum の順に利用する。

## インターフェース

```cpp
#include "library/data_structure/bit_sparse.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class X = int>
struct BIT_Sparse;
BIT_Sparse();
BIT_Sparse(int sz);
void add_point(X x);
void build();
void add(X i, T x);
T sum(X i) const;
T sum(X l, X r) const;
```

## 計算量

build O(N log N)、更新・和 O(log N)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/point_add_range_sum_bit_sparse`](../../../verify/library_checker/data_structure/point_add_range_sum_bit_sparse.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
