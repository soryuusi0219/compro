# convolution_min_plus_convex_convex

[索引](../README.md) / [実装](../../../library/math/convolution_min_plus_convex_convex.hpp)

## 機能・前提条件

両方が凸列のmin-plus畳み込み。隣接差分をマージする。

## インターフェース

```cpp
#include "library/math/convolution_min_plus_convex_convex.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conv_conv(const vector<T> a, const vector<T> b, const Compare &comp = Compare());
```

## 計算量

O(N+M) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_convex_convex`](../../../verify/library_checker/convolution/min_plus_convolution_convex_convex.cpp)
