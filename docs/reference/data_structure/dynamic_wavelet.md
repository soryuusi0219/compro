# dynamic_wavelet

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_wavelet.hpp)

## 機能・前提条件

挿入・削除できるwavelet matrix。非負 B bit 値を扱い、位置・値域を指定して順序統計を求める。内部bit列は乱択木。

## インターフェース

```cpp
#include "library/data_structure/dynamic_wavelet.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, int B = 31>
struct DynamicWavelet;
DynamicWavelet(vector<T> v);
int size();
T get(int i);
void insert(int i, T x);
void erase(int i);
void set(int i, T x);
int rank(int l, int r, T x);
T range_kth(int l, int r, int x);
int count_lt(int l, int r, T x);
int count_interval(int l, int r, T xl, T xu);
T next(int l, int r, T x, T default_value = -1);
T prev(int l, int r, T x, T default_value = -1);
```

## 計算量

操作は期待 O(B log N)、空間 O(NB) bit と木の管理領域。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/point_set_range_frequency_dynamic_wavelet`](../../../verify/library_checker/data_structure/point_set_range_frequency_dynamic_wavelet.cpp)
