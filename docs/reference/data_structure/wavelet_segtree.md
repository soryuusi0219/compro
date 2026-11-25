# wavelet_segtree

[索引](../README.md) / [実装](../../../library/data_structure/wavelet_segtree.hpp)

## 機能・前提条件

キー固定のwavelet matrixとセグメント木。付随値のsetと値域による可換モノイド集約。

キーは `[0,2^B)`、位置区間は `[l,r)`。整数型ではキーの範囲外の検索値も使える。値域の上下端は数値どおりに比較する。

## インターフェース

```cpp
#include "library/data_structure/wavelet_segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class U, auto op, auto e, int B = 31>
struct Wavelet_Segtree;
Wavelet_Segtree(vector<T> v, vector<U> w);
pair<T,U> get(int i);
void set(int i, U x);
int rank(int l, int r, T x);
U prod_with_val(int l, int r, T x);
T range_kth(int l, int r, int x);
int count_lt(int l, int r, T x);
U prod_lt(int l, int r, T x);
U prod_geq(int l, int r, T x);
int count_interval(int l, int r, T xl, T xu);
U prod_interval(int l, int r, T xl, T xu);
T next(int l, int r, T x, T default_value = -1);
T prev(int l, int r, T x, T default_value = -1);
```

## 計算量

構築は最悪 `O(NB)`、個数・順序統計は `O(B)`、値域集約は `O(B log(N+1))`。付随値の並びが同じ階層はセグメント木を共有する。保持する木の数を `D <= B` とすると、更新は `O(B + D log(N+1))`、空間は `O(ND + B(N/64+1))` ワード。

## 依存関係

- [segtree.hpp](segtree.md)
- [static_bit_vector.hpp](static_bit_vector.md)
- [wavelet_partition.hpp](internal/wavelet_partition.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../verify/unit/static_rank_and_order_statistics.cpp)
- [`library_checker/point_add_rectangle_sum_wavelet_segtree`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_wavelet_segtree.cpp)
- [`library_checker/rectangle_sum_wavelet_segtree`](../../../verify/library_checker/data_structure/rectangle_sum_wavelet_segtree.cpp)
