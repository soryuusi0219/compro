# wavelet

[索引](../README.md) / [実装](../../../library/data_structure/wavelet.hpp)

## 機能・前提条件

非負 B bit 値の静的wavelet matrix。rank、区間k番目、値域個数、前後の値を求める。

`range_kth(l,r,k)` のkは0始まり。キーは `[0,2^B)`、位置区間は `[l,r)` です。

## インターフェース

```cpp
#include "library/data_structure/wavelet.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, int B = 31>
struct Wavelet;
Wavelet(vector<T> v);
T get(int i);
int rank(int l, int r, T x);
T range_kth(int l, int r, int x);
int count_lt(int l, int r, T x);
int count_interval(int l, int r, T xl, T xu);
T next(int l, int r, T x, T default_value = -1);
T prev(int l, int r, T x, T default_value = -1);
```

## 計算量

構築 O(NB)、問い合わせ O(B)、空間 O(NB) bit とrank補助配列。

## 依存関係

- [static_bit_vector.hpp](static_bit_vector.md)

## Verify・利用例

- [`library_checker/range_kth_smallest`](../../../verify/library_checker/data_structure/range_kth_smallest.cpp)
- [`library_checker/range_kth_smallest_wavelet_legacy`](../../../verify/library_checker/data_structure/range_kth_smallest_wavelet_legacy.cpp)
- [`library_checker/static_range_frequency_wavelet`](../../../verify/library_checker/data_structure/static_range_frequency_wavelet.cpp)
