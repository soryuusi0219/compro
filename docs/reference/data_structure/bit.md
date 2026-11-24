# bit

[索引](../README.md) / [実装](../../../library/data_structure/bit.hpp)

## 機能・前提条件

加法群上のFenwick tree。add(p,x)、sum(p)=[0,p)、sum(l,r)=[l,r)。lower_bound(k) は累積和が k 以下となる最大の区切り位置（非負要素が必要）。

`BIT(n)` は長さ `n` の零列を作る。`BIT(v)` は各ノードの区間和を添字の昇順で親へ加えて構築する。`lower_bound(k)` は `k < 0` なら `-1`、総和が `k` 以下なら `n` を返す。

## インターフェース

```cpp
#include "library/data_structure/bit.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct BIT;
BIT();
BIT(int _n);
BIT(const vector<T>& v);
void add(int p, T x);
T sum(int p) const;
T sum(int l, int r) const;
int lower_bound(T k) const;
```

## 計算量

構築 O(N)、加算・和・探索 O(log N)、空間 O(N)。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/linear_segment_trees`](../../../verify/unit/linear_segment_trees.cpp)
- [`library_checker/point_add_range_sum`](../../../verify/library_checker/data_structure/point_add_range_sum.cpp)
- [`library_checker/static_range_count_distinct_offline`](../../../verify/library_checker/data_structure/static_range_count_distinct_offline.cpp)
- [`library_checker/static_range_inversions_mo`](../../../verify/library_checker/data_structure/static_range_inversions_mo.cpp)
- [`library_checker/vertex_add_subtree_sum`](../../../verify/library_checker/tree/vertex_add_subtree_sum.cpp)
