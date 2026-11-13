# wavelet_sum

[索引](../README.md) / [実装](../../../library/data_structure/wavelet_sum.hpp)

## 機能・前提条件

キーと重みが固定のwavelet matrix。累積和で値域に含まれる重みを集計する。

## インターフェース

```cpp
#include "library/data_structure/wavelet_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class U, int B = 31>
struct Wavelet_Sum;
Wavelet_Sum(vector<T> v, const vector<U>& w);
T get(int i);
int rank(int l, int r, T x);
U sum_with_val(int l, int r, T x);
T range_kth(int l, int r, int x);
int count_lt(int l, int r, T x);
U sum_lt(int l, int r, T x);
int count_interval(int l, int r, T xl, T xu);
U sum_interval(int l, int r, T xl, T xu);
T next(int l, int r, T x, T default_value = -1);
T prev(int l, int r, T x, T default_value = -1);
```

## 計算量

構築 O(NB)、問い合わせ O(B)、空間 O(NB)。

## 依存関係

- [static_bit_vector.hpp](static_bit_vector.md)

## Verify・利用例

- [`library_checker/rectangle_sum_wavelet_sum`](../../../verify/library_checker/data_structure/rectangle_sum_wavelet_sum.cpp)
- [`library_checker/static_range_frequency_wavelet_sum`](../../../verify/library_checker/data_structure/static_range_frequency_wavelet_sum.cpp)
- [`library_checker/static_range_sum_with_upper_bound_wavelet`](../../../verify/library_checker/data_structure/static_range_sum_with_upper_bound_wavelet.cpp)
