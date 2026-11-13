# segtree_2d

[索引](../README.md) / [実装](../../../library/data_structure/segtree_2d.hpp)

## 機能・前提条件

密な二次元配列の一点更新・長方形集約。op は可換モノイドを想定する。

## インターフェース

```cpp
#include "library/data_structure/segtree_2d.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct Segtree2D;
Segtree2D();
Segtree2D(int h_, int w_);
Segtree2D(const vector<vector<S>>& v);
void set(int i, int j, S x);
S get(int i, int j) const;
S prod(int hl, int hr, int wl, int wr) const;
S all_prod();
```

## 計算量

構築 O(HW)、更新・集約 O(log H log W)、空間 O(HW)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`aoj/range_sum_segtree_2d`](../../../verify/aoj/data_structure/range_sum_segtree_2d.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
