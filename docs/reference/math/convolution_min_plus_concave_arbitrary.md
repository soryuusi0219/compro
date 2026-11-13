# convolution_min_plus_concave_arbitrary

[索引](../README.md) / [実装](../../../library/math/convolution_min_plus_concave_arbitrary.hpp)

## 機能・前提条件

一方が凹列のmin-plus畳み込み。比較関数とINFを指定する。凸列用と条件が異なるので凹性を確認する。

## インターフェース

```cpp
#include "library/math/convolution_min_plus_concave_arbitrary.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conc_arb(const vector<T> A, const vector<T> B, const Compare &comp = Compare(), T INF = numeric_limits<T>::max());
```

## 計算量

入力長 N,M に対し保守的に O((N+M) log(N+M)) 時間、O(N+M) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_concave_arbitrary`](../../../verify/library_checker/convolution/min_plus_convolution_concave_arbitrary.cpp)
