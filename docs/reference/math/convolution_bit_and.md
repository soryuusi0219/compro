# convolution_bit_and

[索引](../README.md) / [実装](../../../library/math/convolution_bit_and.hpp)

## 機能・前提条件

bit AND畳み込み。長さを2冪へ揃える変換を行い、係数型には加減乗算が必要。

## インターフェース

```cpp
#include "library/math/convolution_bit_and.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> convolution_bitand(vector<T> a,vector<T> b);
```

## 計算量

長さ L=2^B に O(LB) 時間、O(L) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/bitwise_and_convolution`](../../../verify/library_checker/convolution/bitwise_and_convolution.cpp)
