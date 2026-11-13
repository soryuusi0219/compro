# point_set_range_count

[索引](../README.md) / [実装](../../../library/data_structure/point_set_range_count.hpp)

## 機能・前提条件

一点の値を変更し、区間内で指定した値の個数を数える。値の種類数の容量とhashを指定する。

## インターフェース

```cpp
#include "library/data_structure/point_set_range_count.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, auto hash>
struct PointSetRangeCount;
PointSetRangeCount(int n_, int m);
PointSetRangeCount(const vector<T>& a, int m);
void set(int i, const T& k);
int count(int l, int r, const T& k) const;
template<class T>
uint64_t hash_int(T x);
```

## 計算量

hashの期待 O(1) を仮定して更新・個数 O(log N)、空間 O(N+容量)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/majority_voting`](../../../verify/library_checker/data_structure/majority_voting.cpp)
