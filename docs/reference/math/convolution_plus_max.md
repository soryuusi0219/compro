# convolution_plus_max

[索引](../README.md) / [実装](../../../library/math/convolution_plus_max.hpp)

## 機能・前提条件

通常のmax-plusではなく c[k]=Σ(i+j=k) max(a[i],b[j]) を計算する。値でソートし畳み込みと列挙を併用する。

## インターフェース

```cpp
#include "library/math/convolution_plus_max.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> plus_max_convolution(vector<T> a, vector<T> b);
```

## 計算量

L=N+M、ブロックBに O(LB+L² log L/B) 時間、O(L) 作業空間。CRT中間値の範囲に注意。

## 依存関係

- [convolution.hpp](convolution.md)

## Verify・利用例

- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
