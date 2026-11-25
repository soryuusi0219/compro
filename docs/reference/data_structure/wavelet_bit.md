# wavelet_bit

[索引](../README.md) / [実装](../../../library/data_structure/wavelet_bit.hpp)

## 機能・前提条件

キー固定のwavelet matrixとFenwick tree。重みをaddし位置×値域の和を取得する。

キーは `[0,2^B)`、位置区間は `[l,r)`。整数型ではキーの範囲外の検索値も使える。値域の上下端は数値どおりに比較する。

## インターフェース

```cpp
#include "library/data_structure/wavelet_bit.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class S, int B = 31>
struct Wavelet_BIT;
Wavelet_BIT(vector<T> v, vector<S> w);
void add(int i, S x);
T get(int i);
int rank(int l, int r, T x);
S sum_with_val(int l, int r, T x);
T range_kth(int l, int r, int x);
int count_lt(int l, int r, T x);
S sum_lt(int l, int r, T x);
int count_interval(int l, int r, T xl, T xu);
S sum_interval(int l, int r, T xl, T xu);
T next(int l, int r, T x, T default_value = -1);
T prev(int l, int r, T x, T default_value = -1);
```

## 計算量

構築は最悪 `O(NB)`、個数・順序統計は `O(B)`、値域和は `O(B log(N+1))`。重みの並びが同じ階層はFenwick木を共有する。保持する木の数を `D <= B` とすると、更新は `O(B + D log(N+1))`、空間は `O(ND + B(N/64+1))` ワード。

## 依存関係

- [bit.hpp](bit.md)
- [static_bit_vector.hpp](static_bit_vector.md)
- [wavelet_partition.hpp](internal/wavelet_partition.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../verify/unit/static_rank_and_order_statistics.cpp)
- [`library_checker/point_add_rectangle_sum_wavelet_bit`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_wavelet_bit.cpp)
