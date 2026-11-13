# convolution_min_plus_convex_arbitrary_smawk

[索引](../README.md) / [実装](../../../library/math/convolution_min_plus_convex_arbitrary_smawk.hpp)

## 機能・前提条件

一方が凸列のmin-plus畳み込みをSMAWKで計算する。同名関数の別実装ヘッダーとは選択して利用する。

## インターフェース

```cpp
#include "library/math/convolution_min_plus_convex_arbitrary_smawk.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
vector<pair<int,T>> SMAWK(int h, int w, const function<T(long long, long long)>& f, const Compare &comp = Compare());
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conv_arb(const vector<T> a, const vector<T> b, const Compare &comp = Compare());
```

## 計算量

L=N+M に対し O(L) 回の評価、O(L) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_convex_arbitrary_smawk`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_smawk.cpp)
