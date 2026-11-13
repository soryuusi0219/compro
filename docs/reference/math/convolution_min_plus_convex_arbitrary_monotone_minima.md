# convolution_min_plus_convex_arbitrary_monotone_minima

[索引](../README.md) / [実装](../../../library/math/convolution_min_plus_convex_arbitrary_monotone_minima.hpp)

## 機能・前提条件

一方が凸列のmin-plus畳み込みをmonotone minimaで計算する。同名のSMAWK版と同時includeを避ける。

## インターフェース

```cpp
#include "library/math/convolution_min_plus_convex_arbitrary_monotone_minima.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conv_arb(const vector<T> a, const vector<T> b, const Compare &comp = Compare(), T INF = numeric_limits<T>::max());
```

## 計算量

L=N+M に対し O(L log L) 時間、O(L) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_convex_arbitrary_monotone_minima`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_monotone_minima.cpp)
